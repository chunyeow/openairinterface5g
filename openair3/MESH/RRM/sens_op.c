/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
   included in this distribution in the file called "COPYING". If not, 
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06410 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

/*!
*******************************************************************************

\file       sens_op.c

\brief      Fonctions permettant la gestion des informations de sensing des 
            differents noeuds

\author     IACOBELLI Lorenzo

\date       21/10/09

   
\par     Historique:
            L.IACOBELLI 2010-03-19
            + "take_decision" function added

*******************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <pthread.h>

#include "debug.h"
#include "L3_rrc_defs.h"
#include "L3_rrc_interface.h"
#include "cmm_rrm_interface.h"
#include "rrm_sock.h"
#include "rrc_rrm_msg.h"
#include "ip_msg.h" //mod_lor_10_04_27
#include "sensing_rrm_msg.h"
#include "cmm_msg.h"
#include "msg_mngt.h"
#include "pusu_msg.h"
#include "rb_db.h"
#include "neighbor_db.h"
#include "sens_db.h"
#include "channels_db.h"
#include "rrm_util.h"
#include "transact.h"
#include "rrm_constant.h"
#include "rrm.h"
#include "sens_op.h"


//! Met un message dans la file des messages a envoyer
#define PUT_CMM_MSG(m)  put_msg(  &(rrm->file_send_cmm_msg), 0, rrm->cmm.s,m )  //mod_lor_10_01_25
#define PUT_PUSU_MSG(m) put_msg(  &(rrm->file_send_cmm_msg), 0, rrm->pusu.s,m)  //mod_lor_10_01_25
#define PUT_RRC_MSG(m)  put_msg(  &(rrm->file_send_rrc_msg), 0, rrm->rrc.s,m )  //mod_lor_10_01_25
#define PUT_IP_MSG(m)   put_msg(  &(rrm->file_send_ip_msg) , 1, rrm->ip.s,m  )  //mod_lor_10_01_25
#define PUT_SENS_MSG(m) put_msg(  &(rrm->file_send_sensing_msg), 0, rrm->sensing.s,m )  //mod_lor_10_04_01

/*!
*******************************************************************************
\brief  Comparaison de deux ID de niveau 2

\return si 0 alors les IDs sont identiques
*/
static int L2_ID_cmp(
    L2_ID *L2_id1, ///< ID de niveau 2
    L2_ID *L2_id2  ///< ID de niveau 2
    )
{
    return memcmp( L2_id1, L2_id2, sizeof(L2_ID) ) ;
}



//mod_lor_10_05_26++
/*!
*******************************************************************************
\brief  Function that decides locally if a channel is free using mu0 and mu1 datas: 
        
\return 
*/
void take_local_decision(
    Sens_ch_t *Sens_info
    )
{
    unsigned int i;
    for (i=0; i < NUM_SB; i++){
        if (Sens_info->mu0[i]>LAMBDA0 && Sens_info->mu1[i]>LAMBDA1)
            Sens_info->is_free[i]=0; // primary system is present
        else
            Sens_info->is_free[i]=1; // primary system is not present
    }
} 
//mod_lor_10_05_26--

/*!
*******************************************************************************
\brief  Updating of the sensing measures received by the rrm from the sensing unit
* of the node. If the node is a mesh router it reports the information to its clusterhead
*/
void rrc_update_sens( 
	Instance_t inst         , //!< Identification de l'instance
	L2_ID L2_id             , //!< Adresse L2 of the source of information 
	unsigned int NB_info    , //!< Number of channel info
	Sens_ch_t *Sens_meas    , //!< Pointer to the sensing information
	double info_time
	)
{
    rrm_t *rrm = &rrm_inst[inst] ; 

   
    int i,j;
    
    //fprintf(stderr,"rrc_update_sens NB_info = %d\n",NB_info);//dbg
   // Sens_ch_t *p;//dbg
    /*for ( i=0; i<NB_info; i++){//dbg
        fprintf(stderr," Ch_id %d     \n",Sens_meas[i].Ch_id);//dbg
        for (j=0;j<MAX_NUM_SB;j++)
            fprintf(stderr,"    SB %d  is %d   \n",j,Sens_meas[i].is_free[j]);//dbg
    }*/
    /*fprintf(stderr," \nrrm_database     ");//dbg
    if (rrm->rrc.pSensEntry != NULL)//dbg
        for ( p=rrm->rrc.pSensEntry->info_hd; p!=NULL; p=p->next)//dbg
            fprintf(stderr," %d     ",p->Ch_id);//dbg
    else//dbg
        fprintf(stderr," empty     ");//dbg
    fprintf(stderr,"\n1 update\n");//dbg*/
    for (i=0; i<NB_info;i++){
        take_local_decision(&Sens_meas[i]); //mod_lor_10_05_26
        //for (int j=0; j<NUM_SB; j++)//dbg
        //    fprintf(stderr,"sns_update: channel %d is %d\n",Sens_meas[i].Ch_id, Sens_meas[i].is_free[j]);//dbg*/
    }
    
    pthread_mutex_lock( &( rrm->rrc.exclu ) ) ; //mod_lor_10_03_08
    update_node_info( &(rrm->rrc.pSensEntry), &L2_id, NB_info, Sens_meas, info_time);
    pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ; //mod_lor_10_03_08

    
    //AAA: for the moment the channel db is reserved for CHs and SUs only in SCEN_2_DISTR 
    
    
    if ( SCEN_2_DISTR) 
    {
        
        //fprintf(stderr,"cluster_head\n");//dbg
        
        CHANNEL_T channel ;
        CHANNELS_DB_T *canal;
        unsigned int *is_free; //mod_eure_lor //mod_lor_10_05_28 ->char instead of int
        for (i=0; i<NB_info; i++){
            
            channel.Start_f = Sens_meas[i].Start_f;
            channel.Final_f = Sens_meas[i].Final_f;
            channel.Ch_id   = Sens_meas[i].Ch_id;
            channel.QoS     = 0;
            is_free     = Sens_meas[i].is_free;
            pthread_mutex_lock( &( rrm->rrc.exclu ) ) ; //mod_lor_10_03_08
            canal = up_chann_db( &(rrm->rrc.pChannelsEntry), channel, is_free[0], info_time);//TO DO SCEN2 DISTR fix it!
            pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ; //mod_lor_10_03_08
            //fprintf(stderr,"inst %d, channel %d, is_free %d\n", inst,Sens_meas[i].Ch_id,Sens_meas[i].is_free);//dbg
            //fprintf(stderr,"chann %d updated\n", Sens_meas[i].Ch_id);//dbg 
            
        }
       
        //AAA: just to save the right L2_id in SCEN_2_DISTR
        if ( rrm->state != CLUSTERHEAD && SCEN_2_DISTR)
            memcpy( rrm->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
    
    }
    
    // mod_lor_10_01_25: monitoring will be launched after an Update SN occupied frequencies message from BTS
    /*if ( rrm->state == CLUSTERHEAD && !SCEN_2_DISTR)
    {
        unsigned int ch_to_scan[NB_info];
        for (i=0; i<NB_info; i++)
            ch_to_scan[i]=Sens_meas[i].Ch_id;
        if (SCEN_2_CENTR && (L2_ID_cmp(&(rrm->L2_id_FC),  &L2_id))==0){
            pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
            rrm->rrc.trans_cnt++ ;
            //To send via IP: PUT_RRC_MSG(msg_rrm_clust_mon_req( inst, L2_id, ch_to_scan, NB_info, 0.5, rrm->rrc.trans_cnt));
            pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
        }
        else{
            pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
            rrm->rrc.trans_cnt++ ;
            PUT_RRC_MSG(msg_rrm_init_mon_req( inst, L2_id,  NB_info, 0.5, ch_to_scan, rrm->rrc.trans_cnt));
            pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
        }
    }*/
#ifndef    RRC_EMUL   
    else if (!SCEN_2_DISTR && rrm->state != CLUSTERHEAD) ///< Case in which a sensor have to inform the FC via IP about its sensing results
    {
        //fprintf (stdout,"msg IP to send from inst %d\n",rrm->id);//dbg
        pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
        rrm->ip.trans_cnt++ ;
        
        //mod_lor_10_04_22++
        /*int r =  send_msg_int( rrm->ip.s, msg_update_sens_results_3( inst, rrm->L2_id, NB_info, Sens_meas, rrm->ip.trans_cnt));
                    WARNING(r!=0);*/
        fprintf(stderr,"before msg_update_sens %d\n",rrm->L2_id.L2_id[0]);//dbg mod_lor_11_02_17
        PUT_IP_MSG(msg_update_sens_results_3( (rrm->L2_id.L2_id[0]-FIRST_MR_ID+1), rrm->L2_id, NB_info, Sens_meas, rrm->ip.trans_cnt)); 
       
        //mod_lor_10_04_22--
        pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;
        
        //mod_lor_10_04_21++ TO DO -> to remove when sensing remont automatically info
        //sleep(10);
        /*if(rrm->sensing.sens_active){
            sleep(10);
            pthread_mutex_lock( &( rrm->sensing.exclu ) ) ;
            rrm->sensing.trans_cnt++ ;
            //fprintf(stderr,"sensing counter %d in msg_rrm_scan_ord on socket %d \n",rrm->sensing.trans_cnt,rrm->sensing.s->s);//dbg
            PUT_SENS_MSG(msg_rrm_scan_ord( inst,  NB_info, 0, 0, 0, Sens_meas, rrm->sensing.trans_cnt )); //mod_lor_10_04_01: Sampl_nb instead of Sampl_freq
            pthread_mutex_unlock( &( rrm->sensing.exclu ) ) ;
        }*/
        //mod_lor_10_04_21--

    }
#endif
    //fprintf(stderr,"end funct rrc_update_sens\n");//dbg
}


/*!
*******************************************************************************
 \brief CMM init sensing request.  Only in CH/FC. 
*/
void cmm_init_sensing( 
    Instance_t       inst,            //!< identification de l'instance
    unsigned int     Start_fr,
    unsigned int     Stop_fr,
    unsigned int     Meas_band,
    unsigned int     Meas_tpf,
    unsigned int     Nb_channels,
    unsigned int     Overlap,
    unsigned int     Sampl_freq
    )
{
    rrm_t *rrm = &rrm_inst[inst] ; 
    
    //if ( (rrm->state == CLUSTERHEAD_INIT1 ) || (rrm->state == CLUSTERHEAD ) )
    if (rrm->role == FUSIONCENTER || rrm->role == CH_COLL) //mod_lor_10_05_05
    {
        
        pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
        rrm->rrc.trans_cnt++ ;
        //fprintf(stderr,"rrc counter %d in rrm_init_scan_req  \n",rrm->rrc.trans_cnt);//dbg
        PUT_RRC_MSG(msg_rrm_init_scan_req( inst, Start_fr ,Stop_fr,Meas_band, Meas_tpf,
                         Nb_channels,  Overlap, Sampl_freq, rrm->rrc.trans_cnt));
        pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
        
       //mod_lor_10_05_05++ 
       /*if (SCEN_2_CENTR && rrm->role == FUSIONCENTER){
            pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
            rrm->ip.trans_cnt++ ;
            PUT_IP_MSG(msg_init_coll_sens_req( inst, rrm->L2_id, Start_fr, Stop_fr,Meas_band, Meas_tpf,
                         Nb_channels,  Overlap, Sampl_freq, rrm->ip.trans_cnt));
            pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;
        }*/
        //mod_lor_10_05_05--
        
    }
    else
    {
        fprintf(stderr,"It is not a cluster head!!!"); 
    }
        
    
    
}

/*!
*******************************************************************************
 \brief rrc transmits order to start sensing received from the clusterhead.
 * The node will than activate its sensing unit sending a scan_ord message
*/
//mod_lor_10_03_13++
void rrc_init_scan_req( 
    Instance_t inst           , //!< identification de l'instance
    L2_ID     L2_id           , //!< FC/CH address
    unsigned int     Start_fr,
    unsigned int     Stop_fr,
    unsigned int     Meas_band,
    unsigned int     Meas_tpf,
    unsigned int     Nb_channels,
    unsigned int     Overlap,
    unsigned int     Sampl_freq,
    Transaction_t  Trans_id     //!< Transaction ID
    )
{
    rrm_t *rrm = &rrm_inst[inst] ;
    Sens_ch_t ch_info_init[Nb_channels];
    unsigned int Sampl_nb = 0;//((Start_fr - Stop_fr)/Sampl_freq)/Nb_channels; //mod_lor_10_04_01: number of samples per sub-band
    unsigned int     act_start_fr = Start_fr;
    for (int i = 0; i<Nb_channels; i++){
        ch_info_init[i].Start_f = act_start_fr   ; 
        act_start_fr+=Meas_band;
        ch_info_init[i].Final_f = act_start_fr  ; ///< frequence final du canal
        ch_info_init[i].Ch_id  = i + 1    ; ///< ID du canal
        //ch_info_init[i].meas   = 0    ; ///< Sensing results 
        //mod_eure_lor++
        for (int j=0;j<MAX_NUM_SB;j++)
            ch_info_init[i].is_free[j]  = 2  ; ///< Decision about the channel
        //mod_eure_lor--
		printf("rrc_init i :%d, start %d, end %d MAX %d band %d\n",i,ch_info_init[i].Start_f,ch_info_init[i].Final_f,Sampl_nb,Meas_band);
    }
    
    

    pthread_mutex_lock( &( rrm->rrc.exclu ) ) ; //mod_lor_10_03_13
    update_node_par( &(rrm->rrc.pSensEntry), &(rrm->L2_id), Nb_channels, ch_info_init, 0,Meas_tpf,Overlap,Sampl_freq); //mod_lor_10_02_19 
    //update_node_info( &(rrm->rrc.pSensEntry), &(rrm->L2_id), Nb_channels, ch_info_init, 0); 
    pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ; //mod_lor_10_03_13


    memcpy( rrm->L2_id_FC.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;

    pthread_mutex_lock( &( rrm->sensing.exclu ) ) ;
    rrm->sensing.trans_cnt++ ;
    //fprintf(stderr,"sensing counter %d in msg_rrm_scan_ord on socket %d \n",rrm->sensing.trans_cnt,rrm->sensing.s->s);//dbg
    PUT_SENS_MSG(msg_rrm_scan_ord( inst,  Nb_channels, Meas_tpf, Overlap, Sampl_nb, ch_info_init, Trans_id )); //mod_lor_10_04_01: Sampl_nb instead of Sampl_freq
    pthread_mutex_unlock( &( rrm->sensing.exclu ) ) ;

}//mod_lor_10_03_13--

/*!
*******************************************************************************
 \brief CMM stop sensing request.  Only in CH/FC. With this function the node
        sends an order to stop sensing to all sensing nodes that were regeistered
        in sensing database. 
*/
void cmm_stop_sensing( 
    Instance_t inst            //!< identification de l'instance
    )
{
    rrm_t *rrm = &rrm_inst[inst] ; 
 //   fprintf(stderr,"stop sensing %d role %d\n\n\n\n\n\n\n\n\n", rrm->id, rrm->role);//dbg
   // if ( (rrm->state == CLUSTERHEAD_INIT1 ) || (rrm->state == CLUSTERHEAD ) )
    if ( (rrm->role == FUSIONCENTER ) || (rrm->role == CH_COLL ) ) //mod_lor_10_05_06
    {
        //fprintf(stderr,"2 end\n");//dbg
        Sens_node_t     *p = rrm->rrc.pSensEntry;
        if (p == NULL)
            fprintf(stderr,"no sensor node information saved\n");
        while (p!=NULL){
            //for ( int i=0;i<8;i++)
            //    fprintf(stderr,"cmm_stop_sens: to send on %X end\n",p->L2_id.L2_id[i]);//dbg
            pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
            rrm->rrc.trans_cnt++ ;
            //fprintf(stderr,"rrc counter %d in msg_rrm_end_scan_req  \n",rrm->rrc.trans_cnt);//dbg
            PUT_RRC_MSG(msg_rrm_end_scan_req( inst, p->L2_id, rrm->rrc.trans_cnt));
            pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
            p = p->next;
        }
        //mod_lor_10_05_06++
        if (SCEN_2_CENTR){ //TO DO: need to add control to know if collaboration is active
            if (rrm->role == FUSIONCENTER){
                sleep(2);
                pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
                rrm->ip.trans_cnt++ ;
                //fprintf(stderr,"rrc counter %d in msg_rrm_end_scan_req  \n",rrm->rrc.trans_cnt);//dbg
                PUT_IP_MSG(msg_stop_coll_sens( inst));
                pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;
            }else{
                pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
                rrm->ip.trans_cnt++ ;
                //fprintf(stderr,"rrc counter %d in msg_rrm_end_scan_req  \n",rrm->rrc.trans_cnt);//dbg
                //PUT_IP_MSG(msg_stop_coll_sens_conf( inst));
                pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;
            }
        }
        
        /*if (SCEN_2_CENTR ){ //!< To inform the CH that is collaborating in sensing to stop sensing
            pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
            rrm->rrc.trans_cnt++ ;
            //fprintf(stderr,"rrc counter %d in msg_rrm_end_scan_req  \n",rrm->rrc.trans_cnt);//dbg
            PUT_RRC_MSG(msg_rrm_end_scan_req( inst, rrm->L2_id_FC, rrm->rrc.trans_cnt));
            pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
        }*/
        //mod_lor_10_05_06--
        
    }
    
        
    else
    {
        fprintf(stderr,"It is not a cluster head!!!"); 
    }
        
    
    
}


/*!
\brief RRC ending sensing confirmation 
 */
void rrc_end_scan_conf(
    Instance_t        inst            , //!< instance ID
    L2_ID             L2_id           , //!< FC/CH address
    Transaction_t     Trans_id          //!< Transaction ID
    
    )
{
    rrm_t *rrm = &rrm_inst[inst] ;     
    pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
    del_node( &(rrm->rrc.pSensEntry), &L2_id ) ;
    pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
    
    if ((rrm->role == CH_COLL) && (rrm->rrc.pSensEntry == NULL)){
        //printf("before IP message\n");//dbg
        pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
        PUT_IP_MSG(msg_stop_coll_sens_conf( inst, rrm->L2_id)); //mod_lor_10_05_12
        pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;
        //printf("after IP message\n");//dbg
    }
        
    
}

/*!
\brief RRC ending sensing request 
 */
void rrc_end_scan_req(
    Instance_t        inst            , //!< instance ID
    L2_ID             L2_id           , //!< FC/CH address
    Transaction_t     Trans_id          //!< Transaction ID
    
    )
{
   
    //fprintf(stdout,"rrc_end_scan_req() cp1 %d\n",inst); //dbg
    rrm_t *rrm = &rrm_inst[inst] ; 
    Sens_node_t *pNode = rrm->rrc.pSensEntry;
    if (pNode==NULL)
        fprintf(stderr,"Database empty \n");
        
    else{
        if (rrm->role == CH_COLL && (L2_ID_cmp(&(rrm->L2_id_FC),  &L2_id))==0 ) ///< case SCEN_2_CENTR
        {
            pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;//mod_lor_10_03_08
            pNode = rrm->rrc.pSensEntry;//mod_lor_10_03_08
            while (pNode!=NULL){
                
                rrm->rrc.trans_cnt++ ;
                //fprintf(stderr,"rrc counter %d msg_rrm_end_scan_req  \n",rrm->rrc.trans_cnt);//dbg
                PUT_RRC_MSG(msg_rrm_end_scan_req( inst, pNode->L2_id, rrm->rrc.trans_cnt));
                
                pNode = pNode->next;
            }
            pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;    //mod_lor_10_03_08
            
        }
        else 
        {
            pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;//mod_lor_10_03_08
            pNode = rrm->rrc.pSensEntry;//mod_lor_10_03_08
            unsigned int i=0, Nb_chan = rrm->rrc.pSensEntry->Nb_chan;
            Sens_ch_t *ch_point = rrm->rrc.pSensEntry->info_hd;
            unsigned int channels[Nb_chan];

            ch_point = rrm->rrc.pSensEntry->info_hd;
            while  (ch_point!=NULL){
                channels[i]=ch_point->Ch_id;
                ch_point = ch_point->next;
                i++;
            }
            pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;//mod_lor_10_03_08
            
            if ((L2_ID_cmp(&(rrm->L2_id_FC),  &L2_id))==0) {
                
                pthread_mutex_lock( &( rrm->sensing.exclu ) ) ;
                rrm->sensing.trans_cnt++ ;
                PUT_SENS_MSG(msg_rrm_end_scan_ord(inst, Nb_chan, channels, Trans_id ));
                pthread_mutex_unlock( &( rrm->sensing.exclu ) ) ;
            }
            else {
                fprintf(stderr,"The message received is not from the right FC \n");
            }
        }
    }
    //dbg: test end_scan_ack
    /*
    pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
    rrm->rrc.trans_cnt++ ;
    
    PUT_RRC_MSG(msg_rrm_end_scan_ord(inst, rrm->L2_id_FC, 0, NULL, Trans_id ));
    pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;*/
    
    
}

/*!
*******************************************************************************
 \brief rrc transmits order to monitor channels
*/
void rrc_init_mon_req( 
    Instance_t inst           , //!< identification de l'instance
    L2_ID     L2_id           , //!< FC address
    unsigned int  *ch_to_scan , //!< vector of identifiers of the channels to monitor
    unsigned int  NB_chan     , //!< Number of channels to monitor
    unsigned int     interv          , //!< time between two sensing operation
    Transaction_t  Trans_id     //!< Transaction ID
    )
{
    rrm_t *rrm = &rrm_inst[inst] ; 
    
    if ((L2_ID_cmp(&(rrm->L2_id_FC),  &L2_id))==0) {
   
        pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
        rrm->rrc.trans_cnt++ ;
        //memcpy( rrm->L2_id_FC.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
        //fprintf(stderr,"rrc counter %d msg_rrm_scan_ord  \n",rrm->rrc.trans_cnt);//dbg
        //PUT_RRC_MSG(msg_rrm_scan_ord( inst,  NB_chan, ch_to_scan, Trans_id ));
        pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
    }
    else {
        fprintf(stderr,"The message received is not from the right FC \n");
    }
    
}

//To send via IP: 

/*!
*******************************************************************************
 \brief rrc transmits order to monitor channels
*/
/*
void rrc_clust_scan_req( 
    Instance_t inst             , //!< instance ID 
    L2_ID L2_id                 , //!< Layer 2 (MAC) ID of CH1
    float interv                , //!< time between two sensing operation
    COOPERATION_T coop          , //!< Cooperation mode
    Transaction_t Trans_id        //!< Transaction ID
    )
{
    rrm_t *rrm = &rrm_inst[inst] ; 

    memcpy( rrm->L2_id_FC.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ; 
    rrm->role = CH_COLL; 
    
    pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
    rrm->rrc.trans_cnt++ ;
    PUT_RRC_MSG(msg_rrm_init_scan_req( inst, interv, rrm->rrc.trans_cnt));
    pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
    
    pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
    rrm->rrc.trans_cnt++ ;
    PUT_RRC_MSG(msg_rrm_clust_scan_conf( inst, L2_id, coop, Trans_id));
    pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
    

    
}*/

/*!
*******************************************************************************
 \brief rrc transmits order from CH1 to monitor channels
*/
/*
void rrc_clust_mon_req( 
    Instance_t inst           , //!< identification de l'instance
    L2_ID     L2_id           , //!< Layer 2 (MAC) ID of CH1
    unsigned int  *ch_to_scan , //!< vector of identifiers of the channels to monitor
    unsigned int  NB_chan     , //!< Number of channels to monitor
    float     interv          , //!< time between two sensing operation
    Transaction_t  Trans_id     //!< Transaction ID
    )
{
    rrm_t *rrm = &rrm_inst[inst] ; 
    
    if ((L2_ID_cmp(&(rrm->L2_id_FC),  &L2_id))==0) {
   
        Sens_node_t *pNode = rrm->rrc.pSensEntry;
        while (pNode!=NULL){
             pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
            rrm->rrc.trans_cnt++ ;
            PUT_RRC_MSG(msg_rrm_init_mon_req( inst,  pNode->L2_id, NB_chan, interv, ch_to_scan, Trans_id ));
            pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
            pNode = pNode->next;
        }
       
        
        pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
        rrm->rrc.trans_cnt++ ;
        PUT_RRC_MSG(msg_rrm_clust_mon_conf( inst,  L2_id, Trans_id ));
        pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
        
        
    }
    else {
        fprintf(stderr,"The message received is not from the right CH \n");
    }
}*/

/*!mod_lor_10_11_03 -> part about SCEN_2 and CH_COLL
*******************************************************************************
\brief  Updating of the sensing measures received via IP from another node
*/
unsigned int update_sens_results( 
	Instance_t inst         , //!< Identification de l'instance
	L2_ID L2_id             , //!< Adresse L2 of the source of information 
	unsigned int NB_info    , //!< Number of channel info
	Sens_ch_t *Sens_meas    , //!< Pointer to the sensing information
	double info_time
	)
{
    rrm_t *rrm = &rrm_inst[inst] ; 

    CHANNELS_DB_T *channel;
    int i,j,k, send_up_to_SN=0;
    unsigned int all_result;
    int tot_ch;
    //L2_ID User_active_L2_id;
    
    pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
    update_node_info( &(rrm->rrc.pSensEntry), &L2_id, NB_info, Sens_meas, info_time);
    pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
    
    //mod_lor_10_10_27++
    if(rrm->role == CH_COLL){
        //AAA: add procedure to take decision from the database -> send a probability that the channel is busy to primary
       
        Sens_ch_t coll_measures[NB_SENS_MAX];
        Sens_node_t *pNode=rrm->rrc.pSensEntry;
        unsigned int weight=0; ///number of sensors of CH_COLL
        while (pNode!=NULL){
            weight++;
            pNode=pNode->next;
        }
        //printf ("nodes %d\n",weight);//dbg 
            
        pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
        for (i=0; take_ch_coll_decision( rrm->rrc.pSensEntry, &coll_measures[i],(i+1))==0; i++);
        pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
	
      //  pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
       // take_ch_coll_decision(rrm->rrc.pSensEntry, channel.Ch_id,is_free);
       // pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
        
        pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
        rrm->ip.trans_cnt++ ;
        PUT_IP_MSG(msg_up_clust_sens_results( inst, rrm->L2_id, i, weight, coll_measures, rrm->ip.trans_cnt));
        pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;

    }
    //mod_lor_10_10_27++
    
    //mod_lor_10_05_28
    ///case of scenario 1
    if (SCEN_1){
        if (!(rrm->ip.waiting_SN_update) ){
            pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
            channel = rrm->rrc.pChannelsEntry;
            while (channel!=NULL){
                //printf("Channel!=NULL\n");
                if (channel->is_ass)
                    if((send_up_to_SN = evalaute_sens_info(rrm->rrc.pSensEntry,channel->channel.Start_f,channel->channel.Final_f)))
                        break;
                channel = channel->next;
            }
            pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
            if (send_up_to_SN && rrm->role == FUSIONCENTER){
                return 1;
                //open_freq_query(inst, L2_id, 0, 0);
            }
        }
    }
    ///case of scenario 2 add_lor_10_11_03
    else if (SCEN_2_CENTR){
        int free_av=0;
        int N_chan=0;
        int chan_in_use=0;
        unsigned int reallocate = 0;
        L2_ID User_active_L2_id[MAX_USER_NB];
        L2_ID User_dest_L2_id[MAX_USER_NB];
        CHANNELS_DB_T *t_channels_db = NULL;
        
        ///check part: it evaluates if a channel in use is not free anymore 
        send_up_to_SN = check_allocated_channels( inst, User_active_L2_id ,User_dest_L2_id ,&free_av);
        
        ///Attribute new channels to users that have to change or that are waiting
        if (send_up_to_SN>0 || rrm->ip.users_waiting_update>0){
            /// case in which there are enough free channels
            if (free_av>=(send_up_to_SN + rrm->ip.users_waiting_update)){ 
                pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
                j = rrm->ip.users_waiting_update-1;
                for (k = send_up_to_SN; k<(send_up_to_SN + rrm->ip.users_waiting_update);k++){
                    memcpy(User_active_L2_id[k].L2_id , rrm->ip.L2_id_wait_users[j][0].L2_id, sizeof(L2_ID));
                    memcpy(User_dest_L2_id[k].L2_id , rrm->ip.L2_id_wait_users[j][1].L2_id, sizeof(L2_ID));
                    j--;
                }
                all_result = ask_freq_to_CH( inst, User_active_L2_id, User_dest_L2_id,k, 0 );  ///Update of channels for both busy and waiting users
                if (all_result == 0){
                    rrm->ip.users_waiting_update=0;
                } else {
                    printf ("ERROR!!! in if free not enough free!!! all_result =%d\n",all_result);//dbg
                }
                pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;
            }
            
            ///no channels to change, but users waiting channels //add_lor_10_11_08++
            ///reallocate only if there are more available channels, otherwise don't do anything
            else if (send_up_to_SN==0 && rrm->ip.users_waiting_update>0){
                chan_in_use = count_free_channels (inst, &free_av);
                tot_ch = chan_in_use + free_av;
                N_chan = find_available_channels(rrm->rrc.pSensEntry,&(t_channels_db));
                del_all_channels( &(t_channels_db));
                if (N_chan>tot_ch){
                    reallocate = 1;
                }else if (free_av>0){
                    pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
                    for (k=0; rrm->ip.users_waiting_update>0 && free_av>0; k++) { /// allocate frequencies to waiting users
                        memcpy(User_active_L2_id[k].L2_id , rrm->ip.L2_id_wait_users[rrm->ip.users_waiting_update-1][0].L2_id, sizeof(L2_ID));
                        memcpy(User_dest_L2_id[k].L2_id , rrm->ip.L2_id_wait_users[rrm->ip.users_waiting_update-1][1].L2_id, sizeof(L2_ID));
                        free_av--;
                        rrm->ip.users_waiting_update--;
                        printf ("N7 tot waiting: %d\n",rrm->ip.users_waiting_update);//db
                    }
                    all_result = ask_freq_to_CH( inst, User_active_L2_id, User_dest_L2_id,k, 0 );  ///Update of channels for waiting users
                    pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;
                }
            }//add_lor_10_11_08--
            
            ///all channels to reallocate
            if (reallocate || (free_av<=(send_up_to_SN + rrm->ip.users_waiting_update) && send_up_to_SN>0)){
                chan_in_use = count_free_channels (inst, &free_av);
                tot_ch = chan_in_use + send_up_to_SN;
                if (chan_in_use!=0){
                    ///add all channels in use to channels to change
                    pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
                    t_channels_db = rrm->rrc.pChannelsEntry;
                    while (t_channels_db!=NULL){
                        if (t_channels_db->is_ass && t_channels_db->is_free){
                            memcpy(User_active_L2_id[send_up_to_SN].L2_id , t_channels_db->source_id.L2_id, sizeof(L2_ID));
                            memcpy(User_dest_L2_id[send_up_to_SN].L2_id , t_channels_db->dest_id.L2_id, sizeof(L2_ID));
                            send_up_to_SN++;
                        }
                        t_channels_db = t_channels_db->next;
                    }
                    pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
                }
                pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
                del_all_channels( & (rrm->rrc.pChannelsEntry) ); 
                pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
                pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
                for (k = send_up_to_SN, j = rrm->ip.users_waiting_update-1; j>=0 ;j--,k++){
                    memcpy(User_active_L2_id[k].L2_id , rrm->ip.L2_id_wait_users[j][0].L2_id, sizeof(L2_ID));
                    memcpy(User_dest_L2_id[k].L2_id , rrm->ip.L2_id_wait_users[j][1].L2_id, sizeof(L2_ID));
                }
                all_result = ask_freq_to_CH( inst, User_active_L2_id, User_dest_L2_id,k, 0 );  ///Update of channels for waiting users
                pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;
                if (all_result == 0){
                    pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
                    rrm->ip.users_waiting_update=0;
                    pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;
                } else {
                    pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
                    rrm->ip.users_waiting_update=0;
                    for (j=1; j<=all_result;j++){
                        memcpy(rrm->ip.L2_id_wait_users[rrm->ip.users_waiting_update][0].L2_id,User_active_L2_id[k-j].L2_id, sizeof(L2_ID));
                        memcpy(rrm->ip.L2_id_wait_users[rrm->ip.users_waiting_update][1].L2_id,User_dest_L2_id[k-j].L2_id, sizeof(L2_ID));
                        rrm->ip.users_waiting_update++;
                        printf ("N8 tot waiting: %d\n",rrm->ip.users_waiting_update);//db
                    }
                    pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;
                }
            }
        }
    }
    return 0;
    
    
    //fprintf(stderr,"node entry  @%p \n", rrm->rrc.pSensEntry);//dbg
    //fprintf(stderr,"2 cluster_head\n");//dbg
    //AAA: for the moment the channel db is reserved for CHs and SUs only in SCEN_2_DISTR
    /*Sens_node_t *pn = rrm->rrc.pSensEntry;
    Sens_ch_t *pc;
    while (pn!=NULL){
        pc = pn->info_hd;
        while (pc!=NULL){
            for (int j=0; j<NUM_SB; j++)//dbg
                fprintf(stderr,"sns_update: channel %d sb %d is %d\n",pc->Ch_id, j, pc->is_free[j]);//dbg
            pc=pc->next;
        }
        pn=pn->next;
    }*/
    
  
    /*if ( rrm->role == FUSIONCENTER || SCEN_2_DISTR || rrm->role == CH_COLL ) //mod_lor_10_03_08: role instead of status -> to check
    //mod_lor_10_05_06 -> 2nd option of if changed (before SCEN_2_DISTR)
    {
        
        //fprintf(stderr,"cluster_head\n");//dbg
        CHANNEL_T channel ;
        CHANNELS_DB_T *canal;
        unsigned int is_free[MAX_NUM_SB];//mod_lor_10_05_28 ->char instead of int
        for (i=0;i<MAX_NUM_SB;i++)
            is_free[i]=0;
        int decision;
        unsigned int send_up_to_SN =0; //mod_lor_10_05_12
        for (i=0; i<NB_info; i++){
            
            channel.Start_f = Sens_meas[i].Start_f;
            channel.Final_f = Sens_meas[i].Final_f;
            channel.Ch_id   = Sens_meas[i].Ch_id;
            channel.QoS     = 0;
            printf(stdout,"Channel in msg %d : \n", channel.Ch_id); //dbg ou LOG
            for (decision = 0; decision<MAX_NUM_SB; decision++)
                fprintf(stdout,"SB %d : is %d\n", decision,Sens_meas[i].is_free[decision]); //dbg ou LOG
            */
            //mod_lor_10_03_19++
       /*     pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
            take_decision(rrm->rrc.pSensEntry, channel.Ch_id,is_free);//mod_eure_lor
         //   pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
            //mod_lor_10_03_19--
            
            //mod_lor_10_05_07++
            if (decision>0)
                is_free = 1;
            else
                is_free = 0;*/
                
            
           
                //Sens_meas[i].meas = decision;
            
       /*     if(rrm->role == CH_COLL){
                memcpy(Sens_meas[i].is_free, is_free, MAX_NUM_SB*sizeof(unsigned int));//mod_lor_10_05_28 ->char instead of int
                //Sens_meas[i].meas = decision;
            }
            //mod_lor_10_05_07--
            
            fprintf(stdout,"Channel %d : \n", channel.Ch_id); //dbg ou LOG
            for (decision = 0; decision<MAX_NUM_SB; decision++)
                fprintf(stdout,"SB %d : is %d\n", decision,is_free[decision]); //dbg ou LOG
            */
         //   pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
    /*        canal = up_chann_db( &(rrm->rrc.pChannelsEntry), channel, is_free[0], info_time);//TO DO: fix it!
            pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
            //mod_lor_10_05_12++
            if (!(rrm->ip.waiting_SN_update) && canal->is_ass && !(canal->is_free)){//mod_lor_10_05_18
                //fprintf(stderr,"send_up_to_SN =1\n");//dbg
                send_up_to_SN =1;//mod_lor_10_05_12--
            }
            //fprintf(stderr,"chann %d updated\n", Sens_meas[i].Ch_id);//dbg
            
        }
        //mod_lor_10_05_07++
        if(rrm->role == CH_COLL){
            pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
            rrm->ip.trans_cnt++ ;
            PUT_IP_MSG(msg_up_clust_sens_results( inst, rrm->L2_id, NB_info, decision, Sens_meas, rrm->ip.trans_cnt));
            pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;

        }//mod_lor_10_05_07--
        //mod_lor_10_05_12++
        if (send_up_to_SN && rrm->role == FUSIONCENTER){
            open_freq_query(inst, L2_id, 0, 0);
        }

    
    }else   
        fprintf(stderr,"error!!! Cannot update channels \n");*/

}

//mod_lor_10_04_14++
/*!
*******************************************************************************
\brief  SENSING unit end scan confirmation 
*/
void sns_end_scan_conf( 
	Instance_t inst          //!< Identification de l'instance
	)
{
    rrm_t *rrm = &rrm_inst[inst] ; 

    ///< Next three lines delete the local sensing information database
    pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
    del_node( &(rrm->rrc.pSensEntry), &(rrm->L2_id));
    pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
    
    // AAA TO DO: Confirmation sent via RRC to the fusion centre in case FC id != 0
    pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
    rrm->rrc.trans_cnt++ ;
    //fprintf(stderr, "before put RRM_end_scan_confirm\n");//dbg
    PUT_RRC_MSG(msg_rrm_end_scan_conf( inst, rrm->rrc.trans_cnt));
    //fprintf(stderr, "after put RRM_end_scan_confirm\n"); //dbg
    pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;

}
//mod_lor_10_04_14--

/*!//mod_lor_10_11_03
*******************************************************************************
\brief  Updating of the sensing measures received via IP from another node
*/
void up_coll_sens_results( 
	Instance_t inst         , //!< Identification de l'instance
	L2_ID L2_id             , //!< Adresse L2 of the source of information 
	unsigned int NB_info    , //!< Number of channel info
	Sens_ch_t *Sens_meas    , //!< Pointer to the sensing information
	double info_time
	)
{
    rrm_t *rrm = &rrm_inst[inst] ; 
    int free_av=0;
    int N_chan=0;
    int chan_in_use=0;
    unsigned int reallocate = 0;
    L2_ID User_active_L2_id[MAX_USER_NB];
    L2_ID User_dest_L2_id[MAX_USER_NB];
    CHANNELS_DB_T *t_channels_db = NULL;
    int i,j,k, send_up_to_SN=0;
    unsigned int all_result;
    int tot_ch;

    pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
    update_node_info( &(rrm->rrc.pSensEntry), &L2_id, NB_info, Sens_meas, info_time);
    pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;

    if (SCEN_2_CENTR && rrm->role == FUSIONCENTER){
        ///check part: it evaluates if a channel in use is not free anymore 
        send_up_to_SN = check_allocated_channels( inst, User_active_L2_id ,User_dest_L2_id ,&free_av);
        
        ///Attribute new channels to users that have to change or that are waiting
        if (send_up_to_SN>0 || rrm->ip.users_waiting_update>0){
            /// case in which there are enough free channels
            if (free_av>=(send_up_to_SN + rrm->ip.users_waiting_update)){ 
                pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
                j = rrm->ip.users_waiting_update-1;
                for (k = send_up_to_SN; k<(send_up_to_SN + rrm->ip.users_waiting_update);k++){
                    memcpy(User_active_L2_id[k].L2_id , rrm->ip.L2_id_wait_users[j][0].L2_id, sizeof(L2_ID));
                    memcpy(User_dest_L2_id[k].L2_id , rrm->ip.L2_id_wait_users[j][1].L2_id, sizeof(L2_ID));
                    j--;
                }
                all_result = ask_freq_to_CH( inst, User_active_L2_id, User_dest_L2_id,k, 0 );  ///Update of channels for both busy and waiting users
                if (all_result == 0){
                    rrm->ip.users_waiting_update=0;
                } else {
                    printf ("ERROR!!! in if free not enough free!!! all_result =%d\n",all_result);//dbg
                }
                pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;
            }
            
            ///no channels to change, but users waiting channels //add_lor_10_11_08++
            ///reallocate only if there are more available channels, otherwise don't do anything
            else if (send_up_to_SN==0 && rrm->ip.users_waiting_update>0){
                chan_in_use = count_free_channels (inst, &free_av);
                tot_ch = chan_in_use + free_av;
                N_chan = find_available_channels(rrm->rrc.pSensEntry,&(t_channels_db));
                del_all_channels( &(t_channels_db));
                if (N_chan>tot_ch){
                    reallocate = 1;
                }else if (free_av>0){
                    pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
                    for (k=0; rrm->ip.users_waiting_update>0 && free_av>0; k++) { /// allocate frequencies to waiting users
                        memcpy(User_active_L2_id[k].L2_id , rrm->ip.L2_id_wait_users[rrm->ip.users_waiting_update-1][0].L2_id, sizeof(L2_ID));
                        memcpy(User_dest_L2_id[k].L2_id , rrm->ip.L2_id_wait_users[rrm->ip.users_waiting_update-1][1].L2_id, sizeof(L2_ID));
                        free_av--;
                        rrm->ip.users_waiting_update--;
                        printf ("N9 tot waiting: %d\n",rrm->ip.users_waiting_update);//db
                    }
                    all_result = ask_freq_to_CH( inst, User_active_L2_id, User_dest_L2_id,k, 0 );  ///Update of channels for waiting users
                    pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;
                }
            }//add_lor_10_11_08--
            
            ///all channels to reallocate
            if (reallocate || (free_av<=(send_up_to_SN + rrm->ip.users_waiting_update) && send_up_to_SN>0)){
                chan_in_use = count_free_channels (inst, &free_av);
                tot_ch = chan_in_use + send_up_to_SN;
                if (chan_in_use!=0){
                    ///add all channels in use to channels to change
                    pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
                    t_channels_db = rrm->rrc.pChannelsEntry;
                    while (t_channels_db!=NULL){
                        if (t_channels_db->is_ass && t_channels_db->is_free){
                            memcpy(User_active_L2_id[send_up_to_SN].L2_id , t_channels_db->source_id.L2_id, sizeof(L2_ID));
                            memcpy(User_dest_L2_id[send_up_to_SN].L2_id , t_channels_db->dest_id.L2_id, sizeof(L2_ID));
                            send_up_to_SN++;
                        }
                        t_channels_db = t_channels_db->next;
                    }
                    pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
                }
                pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
                del_all_channels( & (rrm->rrc.pChannelsEntry) ); 
                pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
                pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
                for (k = send_up_to_SN, j = rrm->ip.users_waiting_update-1; j>=0 ;j--,k++){
                    memcpy(User_active_L2_id[k].L2_id , rrm->ip.L2_id_wait_users[j][0].L2_id, sizeof(L2_ID));
                    memcpy(User_dest_L2_id[k].L2_id , rrm->ip.L2_id_wait_users[j][1].L2_id, sizeof(L2_ID));
                }
                all_result = ask_freq_to_CH( inst, User_active_L2_id, User_dest_L2_id,k, 0 );  ///Update of channels for waiting users
                pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;
                if (all_result == 0){
                    pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
                    rrm->ip.users_waiting_update=0;
                    pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;
                } else {
                    pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
                    rrm->ip.users_waiting_update=0;
                    for (j=1; j<=all_result;j++){
                        memcpy(rrm->ip.L2_id_wait_users[rrm->ip.users_waiting_update][0].L2_id,User_active_L2_id[k-j].L2_id, sizeof(L2_ID));
                        memcpy(rrm->ip.L2_id_wait_users[rrm->ip.users_waiting_update][1].L2_id,User_dest_L2_id[k-j].L2_id, sizeof(L2_ID));
                        rrm->ip.users_waiting_update++;
                        printf ("N10 tot waiting: %d\n",rrm->ip.users_waiting_update);//db
                    }
                    pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;
                }
            }
        }
    }

        ///check part
//        send_up_to_SN = check_allocated_channels( inst, User_active_L2_id ,User_dest_L2_id ,&free_av);
        
        /*pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
        channel = rrm->rrc.pChannelsEntry;
        tot_ch = 0;
        send_up_to_SN=0;
        k=0;
        free_av=0;
        while (channel!=NULL){
            if(evalaute_sens_info(rrm->rrc.pSensEntry,channel->channel.Start_f,channel->channel.Final_f)){
                if (channel->is_ass){
                    ///save the address of user active on the channel
                    memcpy( User_active_L2_id[send_up_to_SN].L2_id, channel->source_id.L2_id, sizeof(L2_ID) )  ;
                    memcpy( User_dest_L2_id[send_up_to_SN].L2_id, channel->dest_id.L2_id, sizeof(L2_ID) )  ;
                    send_up_to_SN++;
                }
                channel->is_free=0;
                channels_to_change[tot_ch]= channel->channel.Ch_id;
            }else{
                if (!(channel->is_ass))
                    free_av++;
                channels_to_change[tot_ch]=-1;
            }
            channel = channel->next;
            tot_ch++;
        }
        pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;*/
        
        ///Attribute new channels to users that have to change
/*      if (send_up_to_SN>0){
            //pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
            /// case in which there are enough free channels
            if (free_av>=send_up_to_SN){
                for (k = 0; k < send_up_to_SN; k++) {
                    ask_freq_to_CH( inst, User_active_L2_id[k], User_dest_L2_id[k],1, 0 );  ///QoS fixed to 1
                }
                /*k = 0;                              ///check on channels to attribute
                channel = rrm->rrc.pChannelsEntry;
                while (channel!=NULL && k< send_up_to_SN){
                    if (channel->is_free && !(channel->is_ass)){
                        channel->is_ass=1;
                        memcpy( channel->source_id.L2_id, User_active_L2_id[k].L2_id, sizeof(L2_ID) )  ;
                        memcpy( channel->dest_id.L2_id, User_dest_L2_id[k].L2_id, sizeof(L2_ID) )  ;
                        k++;
                    }
                    
                }*/
//            }
            ///all channels to reallocate
/*            else{
                del_all_channels( & (rrm->rrc.pChannelsEntry) ); 
                for (k = 0; k < send_up_to_SN; k++) {
                    all_result = ask_freq_to_CH( inst, User_active_L2_id[k], User_dest_L2_id[k],1, 0 );  ///QoS fixed to 1
                    if (all_result!=0)
                        break;
                }
                //add_lor_10_11_08++
                if (k!=send_up_to_SN){
                    printf("Not enough available channels for all users!\n%d links not assigned\n",send_up_to_SN-k);//dbg
                    pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
                    while (k < send_up_to_SN){
                        memcpy(L2_id_wait_users[users_waiting_update][1].L2_id,User_active_L2_id[k].L2_id, sizeof(L2_ID));
                        memcpy(L2_id_wait_users[users_waiting_update][2].L2_id,User_dest_L2_id[k].L2_id, sizeof(L2_ID));
                        users_waiting_update++;
                        k++;
                    }
                    pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;
                }
                //add_lor_10_11_08--
                
                
                /*k = 0;                              ///check on channels to attribute
                channel = rrm->rrc.pChannelsEntry;
                while (channel!=NULL){
                    if (channel->is_ass){
                        memcpy( User_active_L2_id[k].L2_id, channel->source_id.L2_id, sizeof(L2_ID) )  ;
                        memcpy( User_dest_L2_id[k].L2_id, channel->dest_id.L2_id, sizeof(L2_ID) )  ;
                        k++;
                    }
                }
                
                ///new identification of available channels and allocation to users that need to communicate
                NB_chan = find_available_channels(rrm->rrc.pSensEntry,&(rrm->rrc.pChannelsEntry));
                printf ("found channels: %d \n", NB_chan);//dbg
                
                ///Analysing the list of identified channels
                channel = rrm->rrc.pChannelsEntry;
                j=0;
                
                while (channel!=NULL && j<k){
                    if (channel->is_free && !channel->is_ass){
                        memcpy(&(ass_channels[j]) , &(channel->channel), sizeof(CHANNEL_T));
                        up_chann_ass( rrm->rrc.pChannelsEntry  , ass_channels[j].Ch_id, 1, User_active_L2_id[j], User_dest_L2_id[j] );
                        printf ("copied channel: %d start %d end %d\n",ass_channels[j].Ch_id,ass_channels[j].Start_f,ass_channels[j].Final_f);//dbg
                        j++;
                    }
                    channel = channel->next;
                }*/
    
//            }
            //pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
//        }
        
    
            //ask_freq_to_CH( inst, User_active_L2_id, send_up_to_SN, 0 ); /// Update sent to user active 
        
    
    else  
        fprintf(stderr,"Error!!! Not a fusion center or not in Scenario 2 \n");

}


/*!//mod_lor_10_11_03
*******************************************************************************
\brief  check channels in use in second scenario
*/
int check_allocated_channels( 
	Instance_t inst         , //!< Identification de l'instance
	L2_ID *User_active_L2_id ,
	L2_ID *User_dest_L2_id ,
	int *free_av
	)
{
    rrm_t *rrm = &rrm_inst[inst] ; 
    CHANNELS_DB_T *channel;
    int send_up_to_SN=0;
    
    pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
    channel = rrm->rrc.pChannelsEntry;
    send_up_to_SN=0;
    *free_av=0;
    while (channel!=NULL){
        if(evalaute_sens_info(rrm->rrc.pSensEntry,channel->channel.Start_f,channel->channel.Final_f)){
            if (channel->is_ass){
                ///save the address of user active on the channel
                memcpy( User_active_L2_id[send_up_to_SN].L2_id, channel->source_id.L2_id, sizeof(L2_ID) )  ;
                memcpy( User_dest_L2_id[send_up_to_SN].L2_id, channel->dest_id.L2_id, sizeof(L2_ID) )  ;
                send_up_to_SN++;
            }
            channel->is_free=0;
        }else{
            if (!(channel->is_ass))
                *free_av++;
        }
        channel = channel->next;
    }
    pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
    return (send_up_to_SN);
}


/*!//mod_lor_10_11_08
*******************************************************************************
\brief  start_coll_sensing
*/
void cmm_init_coll_sensing( 
    Instance_t       inst,            //!< identification de l'instance
    unsigned int     Start_fr,
    unsigned int     Stop_fr,
    unsigned int     Meas_band,
    unsigned int     Meas_tpf,
    unsigned int     Nb_channels,
    unsigned int     Overlap,
    unsigned int     Sampl_freq
    )
{
    rrm_t *rrm = &rrm_inst[inst] ;
    if (SCEN_2_CENTR && rrm->role == FUSIONCENTER){
        pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
        rrm->ip.trans_cnt++ ;
        PUT_IP_MSG(msg_init_coll_sens_req( inst, rrm->L2_id, Start_fr, Stop_fr,Meas_band, Meas_tpf,
                     Nb_channels,  Overlap, Sampl_freq, rrm->ip.trans_cnt));
        pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;
    }else {
        printf ("ERROR! Node is not the main CH or not in the right scenario.\n");
    }
}

/*!//mod_lor_10_11_08
*******************************************************************************
\brief  count channels in use and free channels in second scenario
*/
int count_free_channels( 
	Instance_t inst         , //!< Identification de l'instance
	int *free_av
	)
{
    rrm_t *rrm = &rrm_inst[inst] ; 
    CHANNELS_DB_T *channel;
    int used_ch=0;
    
    pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
    channel = rrm->rrc.pChannelsEntry;
    *free_av=0;
    while (channel!=NULL){
        if(!evalaute_sens_info(rrm->rrc.pSensEntry,channel->channel.Start_f,channel->channel.Final_f)){
            if (!(channel->is_ass))
                *free_av++;
            else
                used_ch++;
        }
        channel = channel->next;
    }
    pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
    return (used_ch);
}
