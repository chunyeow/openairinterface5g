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

\file       rrm.c

\brief      RRM (Radio Ressource Manager )

            Cette application a pour objet
                - de gérer la ressource radio du cluster
                - de commander le RRC pour l'ouverture de RB
                - de recevoir des commandes du CMM
                - de gérer le voisinage

\author     BURLOT Pascal

\date       10/07/08


\par     Historique:
        P.BURLOT 2009-01-20 
            + separation de la file de message CMM/RRM a envoyer en 2 files 
              distinctes ( file_send_cmm_msg, file_send_rrc_msg)
            + l'envoi de message via la fifo:
                - envoi du header
                - puis des donnees s'il y en a
            + reception des donnees de la fifo:
                - copie du message dans la file d'attente des messages
                - traitement du cas du message n'ayant pas de data (ex: response )
        L.IACOBELLI 2009-10-19
            + sensing database
            + channels database
            + new cases 

*******************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>
#include <fcntl.h>
#include <pthread.h>

#include "debug.h"

#include "L3_rrc_defs.h"
#include "L3_rrc_interface.h"
#include "cmm_rrm_interface.h"
#include "rrm_sock.h"
#include "rrc_rrm_msg.h"
#include "ip_msg.h"                 //mod_lor_10_04_27
#include "sensing_rrm_msg.h"
#include "cmm_msg.h"
#include "pusu_msg.h"
#include "msg_mngt.h"
#include "neighbor_db.h"
#include "rb_db.h"
#include "sens_db.h"
#include "channels_db.h"
#include "transact.h"
#include "rrm_constant.h"
#include "rrm_util.h"
#include "rrm.h"

#include "forms.h"                  //mod_eure_lor
#include "sensing_form.h"           //mod_eure_lor
#include "SN_freq_form.h"           //mod_lor_10_06_01
#include "sens_scen_2_form.h"       //mod_lor_10_11_04
#include "all_freq_to_users_form.h" //mod_lor_10_11_04
#include "sens_CH1_scen_2.h"        //mod_lor_10_11_04
#include "sens_sensor.h"        //mod_lor_11_02_18


/*
** ----------------------------------------------------------------------------
** DEFINE LOCAL
** ----------------------------------------------------------------------------
*/
//mod_lor_10_05_18++
/*!
*******************************************************************************
\brief Definition of IP @ in main entities. i.e. they have to correspond 
        to the ones in node_info vector in emul_interface.c
*/
 static unsigned char FC_L3id [4]={0x0A,0x00,0x01,0x01};
 static unsigned char BTS_L3id [4]={0x0A,0x00,0x02,0x02};
 static unsigned char CH_COLL_L3id [4]={0x0A,0x00,0x02,0x02};
 FD_sensing_form *form;
 FD_Secondary_Network_frequencies *SN_form; //mod_lor_10_06_01
 FD_sens_scen_2 *Sens_form_CH2;             //mod_lor_10_11_04
 FD_all_freq_to_users *Chann_form;          //mod_lor_10_11_04
 FD_sens_CH1_scen_2 *Sens_form_CH1;         //mod_lor_10_11_04
 FD_sens_sensor *Sens_sensor_form;          //mod_lor_11_02_18
 static int SN_waiting = 0;                 //mod_lor_10_06_02
//mod_lor_10_05_18--
/*
** ----------------------------------------------------------------------------
** DECLARATION DE NOUVEAU TYPE
** ----------------------------------------------------------------------------
*/

/*!
*******************************************************************************
\brief Structure de data passe en parametre au threads
*/
struct data_thread {
    char *name              ; ///< Nom du thread
    char *sock_path_local   ; ///< fichier du "rrm->..." pour le socket Unix
    char *sock_path_dest    ; ///< fichier du "...->rrm " pour le socket Unix
    sock_rrm_t  s           ; ///< Descripteur du socket
}  ;

//mod_lor_10_01_25++
struct data_thread_int {
    char *name              ; ///< Nom du thread
    unsigned char *sock_path_local  ; ///< local IP address for internet socket
    int local_port          ; ///< local IP port for internet socket
    unsigned char *sock_path_dest   ; ///< dest IP address for internet socket
    int dest_port           ; ///< dest IP port for internet socket
    sock_rrm_int_t  s       ; ///< Descripteur du socket
    int instance            ; ///<instance rrm 
}  ;
//mod_lor_10_01_25--

#ifdef RRC_KERNEL_MODE

#define RRC2RRM_FIFO 14
#define RRM2RRC_FIFO 15

#define RX_MSG_STARTED 0; //mod_lor_10_01_25
/*!
*******************************************************************************
\brief Structure regroupant les handles des fifos pour la communication en
       mode KERNEL
*/
typedef struct{
    int rrc_2_rrm_fifo;
    int rrm_2_rrc_fifo;
}RRM_FIFOS; 

#endif /* RRC_KERNEL_MODE */

/*
** ----------------------------------------------------------------------------
** DECLARATION DES VARIABLES GLOBALES PUBLIQUES
** ----------------------------------------------------------------------------
*/
rrm_t rrm_inst[MAX_RRM] ;
int   nb_inst = -1 ;

/*
** ----------------------------------------------------------------------------
** DECLARATION DES VARIABLES GLOBALES PRIVEES
** ----------------------------------------------------------------------------
*/
#ifdef RRC_KERNEL_MODE
static RRM_FIFOS Rrm_fifos;
#endif

static int flag_not_exit = 1 ;
static pthread_t pthread_recv_rrc_msg_hnd,
                 pthread_recv_cmm_msg_hnd ,

                 pthread_send_rrc_msg_hnd ,

                 pthread_send_cmm_msg_hnd ,

                 pthread_recv_pusu_msg_hnd ,
                 pthread_recv_sensing_msg_hnd ,
                 pthread_send_sensing_msg_hnd ,
                 
                 pthread_recv_int_msg_hnd ,
                 pthread_send_ip_msg_hnd ,

                 pthread_ttl_hnd ;
static unsigned int cnt_timer = 0;
static float st_fr = 2.42e6; //mod_lor_10_06_01
static float end_fr = 2.46e6; //mod_lor_10_06_01

#ifdef TRACE
static FILE *cmm2rrm_fd  = NULL ;
static FILE *rrc2rrm_fd  = NULL ;
static FILE *pusu2rrm_fd = NULL ;
static FILE *sensing2rrm_fd = NULL ;
static FILE *ip2rrm_fd = NULL ;
#endif
static FILE *output_2 = NULL; //mod_lor_10_04_20

/*
** ----------------------------------------------------------------------------
** DECLARATION DES FONCTIONS
** ----------------------------------------------------------------------------
*/
//mod_eure_lor++
/*!
*******************************************************************************
\brief  function to plot the spectrum sensing results

\return NULL
*/

void plot_spectra(Sens_ch_t *S, unsigned int NB_info, /*FD_sensing_form *form,*/ unsigned int sensor) {
    
    float f[MAX_NUM_SB*NB_info],spec_dBm[MAX_NUM_SB*NB_info];
    //float f[100],spec_dBm[100];
    unsigned int tot_sub_bands = MAX_NUM_SB*NB_info;
    unsigned int SB_BW;
    int i, j, k=0;
    //printf("nb_info %d tot sub: %d \n",NB_info, tot_sub_bands);//dbg
    // Compute frequencies and store in f 
    for (i=0;i<NB_info ;i++) {
        SB_BW = (S[i].Final_f-S[i].Start_f)/MAX_NUM_SB;
        for (j=0; j< MAX_NUM_SB;j++){
            f[k]=S[i].Start_f+(SB_BW*j)+(SB_BW/2);
            // Transfer power measurements to spec_dBm (float)
            spec_dBm[k] = S[i].mu0[j];
            //printf("S[i].Start_f %d S[i].mu0[j] %d freq: %f spec_dBm %f \n",S[i].Start_f,  S[i].mu0[j], f[k],  spec_dBm[k]); //dbg
             k++;
        }
    }
   
    if (sensor == 1){
        fl_set_xyplot_xbounds(form->spec_SN1,(float)S[0].Start_f,(float)S[NB_info-1].Final_f);
        fl_set_xyplot_ybounds(form->spec_SN1,-115,-70);

        fl_set_xyplot_data(form->spec_SN1,f,spec_dBm,tot_sub_bands,"","","");
    }else if (sensor == 2){
        fl_set_xyplot_xbounds(form->spec_SN2,(float)S[0].Start_f,(float)S[NB_info-1].Final_f);
        fl_set_xyplot_ybounds(form->spec_SN2,-115,-70);

        fl_set_xyplot_data(form->spec_SN2,f,spec_dBm,tot_sub_bands,"","","");
    }else if(sensor == 3){
        fl_set_xyplot_xbounds(form->spec_SN3,(float)S[0].Start_f,(float)S[NB_info-1].Final_f);
        fl_set_xyplot_ybounds(form->spec_SN3,-115,-70);

        fl_set_xyplot_data(form->spec_SN3,f,spec_dBm,tot_sub_bands,"","","");
    }else 
        printf("Error! Sensor %d not considered",sensor);
    fl_check_forms();
}//mod_eure_lor--

//mod_lor_10_06_01++
/*!
*******************************************************************************
\brief  function to plot the selected channels

\return NULL
*/

void plot_SN_channels(CHANNELS_DB_T *channels_db, unsigned int NB_info, unsigned int *selected, /*FD_Secondary_Network_frequencies *SN_form,*/ unsigned int rrm_id) {
    
    float f[SB_NEEDED_FOR_SN*NB_info],spec_dBm[SB_NEEDED_FOR_SN*NB_info];
    CHANNELS_DB_T *pCurrent;
    //float f[100],spec_dBm[100];
    unsigned int tot_sub_bands = SB_NEEDED_FOR_SN*NB_info;
    unsigned int SB_BW;
    float Start_fr, Final_fr;
    int i, j, k=0;
    //printf("nb_info %d tot sub: %d \n",NB_info, tot_sub_bands);//dbg
    // Compute frequencies and store in f 
    for (i=0;i<NB_info ;i++) {
        pCurrent = get_chann_db_info(channels_db,selected[i]);
        SB_BW = (pCurrent->channel.Final_f-pCurrent->channel.Start_f)/SB_NEEDED_FOR_SN;
        for (j=0; j< SB_NEEDED_FOR_SN;j++){
            f[k]=pCurrent->channel.Start_f+(SB_BW*j)+(SB_BW/2);
            // Transfer power measurements to spec_dBm (float)
            spec_dBm[k] = 1;
           //printf ("for k = %d f: %f; spec_dBm %f\n",k,f[k], spec_dBm[k]);//dbg
            //printf("S[i].Start_f %d S[i].mu0[j] %d freq: %f spec_dBm %f \n",S[i].Start_f,  S[i].mu0[j], f[k],  spec_dBm[k]); //dbg
             k++;
        }
    }
    Start_fr = st_fr;
    Final_fr = end_fr;
    //printf ("start: %f; end %f\n",f[0], f[k-1]);//dbg
   
    if (rrm_id == BTS_ID){
        fl_set_xyplot_xbounds(SN_form->Selected_frequencies,Start_fr,Final_fr);
        fl_set_xyplot_ybounds(SN_form->Selected_frequencies,0,2);

        fl_set_xyplot_data(SN_form->Selected_frequencies,f,spec_dBm,tot_sub_bands,"","","");
    }else if (rrm_id == FC_ID){
        fl_set_xyplot_xbounds(form->Secondary_Network_frequencies,Start_fr,Final_fr);
        fl_set_xyplot_ybounds(form->Secondary_Network_frequencies,0,2);

        fl_set_xyplot_data(form->Secondary_Network_frequencies,f,spec_dBm,tot_sub_bands,"","","");
    }
    fl_check_forms();
}
//mod_lor_10_06_01--

//add_lor_10_11_04++
/*!
*******************************************************************************
\brief  function to plot the spectrum sensing results in cluster 1

\return NULL
*/
void plot_spectra_CH1(Sens_ch_t *S, unsigned int NB_info, /*FD_sens_CH1_scen_2 *Sens_form_CH1,*/ int sensor) {
    
    float f[MAX_NUM_SB*NB_info],spec_dBm[MAX_NUM_SB*NB_info];
    //float f[100],spec_dBm[100];
    unsigned int tot_sub_bands = MAX_NUM_SB*NB_info;
    unsigned int SB_BW;
    int i, j, k=0;
    //printf("nb_info %d tot sub: %d \n",NB_info, tot_sub_bands);//dbg
    // Compute frequencies and store in f 
    if (sensor!=-1){
        for (i=0;i<NB_info ;i++) {
            SB_BW = (S[i].Final_f-S[i].Start_f)/MAX_NUM_SB;
            for (j=0; j< MAX_NUM_SB;j++){
                f[k]=S[i].Start_f+(SB_BW*j)+(SB_BW/2);
                // Transfer power measurements to spec_dBm (float)
                spec_dBm[k] = S[i].mu0[j];
                //printf("S[i].Start_f %d S[i].mu0[j] %d freq: %f spec_dBm %f \n",S[i].Start_f,  S[i].mu0[j], f[k],  spec_dBm[k]); //dbg
                 k++;
            }
        }
    }else{
        for (i=0;i<NB_info ;i++) {
            SB_BW = (S[i].Final_f-S[i].Start_f)/MAX_NUM_SB;
            for (j=0; j< MAX_NUM_SB;j++){
                f[k]=S[i].Start_f+(SB_BW*j)+(SB_BW/2);
                // Transfer power measurements to spec_dBm (float)
                spec_dBm[k] = (S[i].is_free[j]+1)%2;
                //printf("S[i].Start_f %d S[i].mu0[j] %d freq: %f spec_dBm %f \n",S[i].Start_f,  S[i].mu0[j], f[k],  spec_dBm[k]); //dbg
                 k++;
            }
        }
    }
   
    if (sensor == 1){
        fl_set_xyplot_xbounds(Sens_form_CH1->User_1_sens,(float)S[0].Start_f,(float)S[NB_info-1].Final_f);
        fl_set_xyplot_ybounds(Sens_form_CH1->User_1_sens,-115,-70);

        fl_set_xyplot_data(Sens_form_CH1->User_1_sens,f,spec_dBm,tot_sub_bands,"","","");
        
    }else if (sensor == 2){
        fl_set_xyplot_xbounds(Sens_form_CH1->User_2_sens,(float)S[0].Start_f,(float)S[NB_info-1].Final_f);
        fl_set_xyplot_ybounds(Sens_form_CH1->User_2_sens,-115,-70);

        fl_set_xyplot_data(Sens_form_CH1->User_2_sens,f,spec_dBm,tot_sub_bands,"","","");
    }else if(sensor == 3){
        fl_set_xyplot_xbounds(Sens_form_CH1->User_3_sens,(float)S[0].Start_f,(float)S[NB_info-1].Final_f);
        fl_set_xyplot_ybounds(Sens_form_CH1->User_3_sens,-115,-70);

        fl_set_xyplot_data(Sens_form_CH1->User_3_sens,f,spec_dBm,tot_sub_bands,"","","");
    }else if(sensor == 4){
        fl_set_xyplot_xbounds(Sens_form_CH1->User_4_sens,(float)S[0].Start_f,(float)S[NB_info-1].Final_f);
        fl_set_xyplot_ybounds(Sens_form_CH1->User_4_sens,-115,-70);

        fl_set_xyplot_data(Sens_form_CH1->User_4_sens,f,spec_dBm,tot_sub_bands,"","","");
    }else if(sensor == -1){
        fl_set_xyplot_xbounds(Sens_form_CH1->Cluster_2_sensing,(float)S[0].Start_f,(float)S[NB_info-1].Final_f);
        fl_set_xyplot_ybounds(Sens_form_CH1->Cluster_2_sensing,0,2);

        fl_set_xyplot_data(Sens_form_CH1->Cluster_2_sensing,f,spec_dBm,tot_sub_bands,"","","");
    }else
        printf("Error! Sensor %d not considered",sensor);
    fl_check_forms();
}

/*!
*******************************************************************************
\brief  function to plot the spectrum sensing results of collaborative cluster 2

\return NULL
*/
void plot_spectra_CH2(Sens_ch_t *S, unsigned int NB_info, /*FD_sensing_form *form,*/ unsigned int sensor) {
    
    float f[MAX_NUM_SB*NB_info],spec_dBm[MAX_NUM_SB*NB_info];
    //float f[100],spec_dBm[100];
    unsigned int tot_sub_bands = MAX_NUM_SB*NB_info;
    unsigned int SB_BW;
    int i, j, k=0;

    for (i=0;i<NB_info ;i++) {
        SB_BW = (S[i].Final_f-S[i].Start_f)/MAX_NUM_SB;
        for (j=0; j< MAX_NUM_SB;j++){
            f[k]=S[i].Start_f+(SB_BW*j)+(SB_BW/2);
            // Transfer power measurements to spec_dBm (float)
            spec_dBm[k] = S[i].mu0[j];
            //printf("S[i].Start_f %d S[i].mu0[j] %d freq: %f spec_dBm %f \n",S[i].Start_f,  S[i].mu0[j], f[k],  spec_dBm[k]); //dbg
             k++;
        }
    }
   
    if (sensor == 1){
        fl_set_xyplot_xbounds(Sens_form_CH2->User_1,(float)S[0].Start_f,(float)S[NB_info-1].Final_f);
        fl_set_xyplot_ybounds(Sens_form_CH2->User_1,-115,-70);

        fl_set_xyplot_data(Sens_form_CH2->User_1,f,spec_dBm,tot_sub_bands,"","","");
    }else if (sensor == 2){
        fl_set_xyplot_xbounds(Sens_form_CH2->User_2,(float)S[0].Start_f,(float)S[NB_info-1].Final_f);
        fl_set_xyplot_ybounds(Sens_form_CH2->User_2,-115,-70);

        fl_set_xyplot_data(Sens_form_CH2->User_2,f,spec_dBm,tot_sub_bands,"","","");
    }else if(sensor == 3){
        fl_set_xyplot_xbounds(Sens_form_CH2->User_3,(float)S[0].Start_f,(float)S[NB_info-1].Final_f);
        fl_set_xyplot_ybounds(Sens_form_CH2->User_3,-115,-70);

        fl_set_xyplot_data(Sens_form_CH2->User_3,f,spec_dBm,tot_sub_bands,"","","");
    }else if(sensor == 4){
        fl_set_xyplot_xbounds(Sens_form_CH2->User_4,(float)S[0].Start_f,(float)S[NB_info-1].Final_f);
        fl_set_xyplot_ybounds(Sens_form_CH2->User_4,-115,-70);

        fl_set_xyplot_data(Sens_form_CH2->User_4,f,spec_dBm,tot_sub_bands,"","","");
    }else 
        printf("Error! Sensor %d not considered",sensor);
    fl_check_forms();
}

/*!
*******************************************************************************
\brief  function to plot the attributed channel

\return NULL
*/

void plot_all_chann_scen_2(unsigned int *tx, unsigned int *chann_start, unsigned int *chann_end, unsigned int NB_info, unsigned int rrm_id) { 
    
    float Start_fr, Final_fr;
    
    float f[SB_NEEDED_FOR_SN*NB_info], spec_dBm[SB_NEEDED_FOR_SN*NB_info];
    Start_fr = st_fr;
    Final_fr = end_fr;

    unsigned int tot_sub_bands = SB_NEEDED_FOR_SN*NB_info;
    unsigned int SB_BW ;

    int i, j, k=0;

    // Compute frequencies and store in f 
    for (i=0;i<NB_info ;i++) {
        SB_BW = (chann_end[i]-chann_start[i])/SB_NEEDED_FOR_SN;
        for (j=0; j< SB_NEEDED_FOR_SN;j++){
            f[k]=chann_start[i]+(SB_BW*j)+(SB_BW/2);
            if (tx[i]==1)
                spec_dBm[k] = 2;
            else
                spec_dBm[k] = 1;
            k++;
        }
    }
    
    //printf ("start: %f; end %f\n",f[0], f[k-1]);//dbg
   
    if (rrm_id == 1){
        fl_set_xyplot_xbounds(Chann_form->User_1_channels,Start_fr,Final_fr);
        fl_set_xyplot_ybounds(Chann_form->User_1_channels,0,3);
        fl_set_xyplot_data(Chann_form->User_1_channels,f,spec_dBm,tot_sub_bands,"","","");
        
    }else if (rrm_id == 2){
        fl_set_xyplot_xbounds(Chann_form->User_2_channels,Start_fr,Final_fr);
        fl_set_xyplot_ybounds(Chann_form->User_2_channels,0,3);
        fl_set_xyplot_data(Chann_form->User_2_channels,f,spec_dBm,tot_sub_bands,"","","");
    
    }else if (rrm_id == 3){
        fl_set_xyplot_xbounds(Chann_form->User_3_channels,Start_fr,Final_fr);
        fl_set_xyplot_ybounds(Chann_form->User_3_channels,0,3);
        fl_set_xyplot_data(Chann_form->User_3_channels,f,spec_dBm,tot_sub_bands,"","","");
   
    }else if (rrm_id == 4){
        fl_set_xyplot_xbounds(Chann_form->User_4_channels,Start_fr,Final_fr);
        fl_set_xyplot_ybounds(Chann_form->User_4_channels,0,3);
        fl_set_xyplot_data(Chann_form->User_4_channels,f,spec_dBm,tot_sub_bands,"","","");
    
    }else
        printf("Error! User %d not considered",rrm_id);
    fl_check_forms();
}
//add_lor_10_11_04--


/*!
*******************************************************************************
\brief  thread de traitement des ttl des transactions (rrc ou cmm).

\return NULL
*/
static void * thread_processing_ttl (
    void * p_data /**< parametre du pthread */
    )
{
    int ii ;
    fprintf(stderr,"TTL :starting ... \n"); fflush(stdout);

    while ( flag_not_exit)
    {
        for ( ii = 0 ; ii<nb_inst ; ii++ )
        {
            rrm_t *rrm = &rrm_inst[ii] ;

            pthread_mutex_lock(   &( rrm->cmm.exclu )  ) ;
            dec_all_ttl_transact( rrm->cmm.transaction ) ;
            // Trop simpliste et pas fonctionnel , il faut faire une gestion des erreurs de transaction
            //if (rrm->cmm.transaction!=NULL)
              //  fprintf(stderr,"delete on cmm of %d\n", ii); //dbg
            del_all_obseleted_transact( &(rrm->cmm.transaction));
            pthread_mutex_unlock( &( rrm->cmm.exclu )  ) ;

            pthread_mutex_lock(   &( rrm->rrc.exclu )  ) ;
            dec_all_ttl_transact( rrm->rrc.transaction ) ;
            // idem :commentaire ci-dessus
            //if (rrm->rrc.transaction!=NULL)
              //  fprintf(stderr,"delete on rrc of %d\n", ii); //dbg
            del_all_obseleted_transact( &(rrm->rrc.transaction));
            pthread_mutex_unlock( &( rrm->rrc.exclu )  ) ;

            pthread_mutex_lock(   &( rrm->pusu.exclu )  ) ;
            dec_all_ttl_transact( rrm->pusu.transaction ) ;
            // idem :commentaire ci-dessus
            del_all_obseleted_transact( &(rrm->pusu.transaction));
            pthread_mutex_unlock( &( rrm->pusu.exclu )  ) ;

            pthread_mutex_lock(   &( rrm->sensing.exclu )  ) ;
            dec_all_ttl_transact( rrm->sensing.transaction ) ;
            // idem :commentaire ci-dessus
            del_all_obseleted_transact( &(rrm->sensing.transaction));
            pthread_mutex_unlock( &( rrm->sensing.exclu )  ) ;
            
            //mod_lor_10_01_25++
            pthread_mutex_lock(   &( rrm->ip.exclu )  ) ;
            dec_all_ttl_transact( rrm->ip.transaction ) ;
            // idem :commentaire ci-dessus
            del_all_obseleted_transact( &(rrm->ip.transaction));
            pthread_mutex_unlock( &( rrm->ip.exclu )  ) ;
            //mod_lor_10_01_25--*/
            
        }
        cnt_timer++;
        usleep( 2000*1000 ) ;//mod_lor_10_03_01: incrementing timeout
    }
    fprintf(stderr,"... stopped TTL\n"); fflush(stdout);
    return NULL;

}

/*!
*******************************************************************************
\brief  thread de traitement des messages sortants sur les sockets (rrc ou cmm).

\return NULL
*/
static void * thread_send_msg_cmm (
    void * p_data /**< parametre du pthread */
    )
{
    int ii ;
    int no_msg ;
    fprintf(stderr,"Thread Send Message: starting ... \n");
    fflush(stderr);
    file_msg_t *pItem ;

    while ( flag_not_exit)
    {
        no_msg = 0  ;
        for ( ii = 0 ; ii<nb_inst ; ii++ )
        {
            rrm_t      *rrm = &rrm_inst[ii] ;

            pItem = get_msg( &(rrm->file_send_cmm_msg) ) ;
            

            if ( pItem == NULL )
                no_msg++;
            else
            {
                int r =  send_msg( pItem->s, pItem->msg );
                WARNING(r!=0);
            }
            RRM_FREE( pItem ) ;
        }

        if ( no_msg==nb_inst ) // Pas de message
            usleep(1000);
    }
    fprintf(stderr,"... stopped Thread Send Message\n"); fflush(stderr);
    return NULL;
}

/*!
*******************************************************************************
\brief  thread de traitement des messages sortants sur les sockets (rrc ou cmm).

\return NULL
*/
static void * thread_send_msg_rrc (
    void * p_data /**< parametre du pthread */
    )
{
    int ii ;

    int no_msg ;
    fprintf(stderr,"Thread Send Message To RRC: starting ... \n");
    fflush(stderr);
    file_msg_t *pItem ;
    while ( flag_not_exit)
    {
        no_msg = 0  ;
        for ( ii = 0 ; ii<nb_inst ; ii++ )
        {
            rrm_t      *rrm = &rrm_inst[ii] ;

            pItem = get_msg( &(rrm->file_send_rrc_msg) ) ;
            

            if ( pItem == NULL )
                no_msg++;
            else
            {
            
#ifdef RRC_KERNEL_MODE
                // envoi du header
                status = write (Rrm_fifos.rrm_2_rrc_fifo,(char*) pItem->msg, sizeof(msg_head_t) );
                if ( pItem->msg->head.size >0)
                    status = write (Rrm_fifos.rrm_2_rrc_fifo,(pItem->msg)->data, pItem->msg->head.size);
                //printf( "status write: %d \n",status);
                //printf("send msg to rrc [%d] id msg: %d \n", (pItem->msg)->head.inst, (pItem->msg)->head.msg_type );
#else
                int r =  send_msg( pItem->s, pItem->msg );
                WARNING(r!=0);
#endif
            }
            RRM_FREE( pItem ) ;
        }

        if ( no_msg==nb_inst ) // Pas de message
            usleep(1000);
    }
    fprintf(stderr,"... stopped Thread Send Message\n"); fflush(stderr);
    return NULL;
}

//mod_lor_10_01_25++
/*!
*******************************************************************************
\brief  thread de traitement des messages sortants sur les sockets (rrc ou cmm).

\return NULL
*/
static void * thread_send_msg_ip (
    void * p_data /**< parametre du pthread */
    )
{
    int ii ;
    int no_msg ;
    fprintf(stderr,"Thread Send Message IP: starting ... \n");
    fflush(stderr);
    file_msg_t *pItem ;
    
    while ( flag_not_exit)
    {
        no_msg = 0  ;
        for ( ii = 0 ; ii<nb_inst ; ii++ )
        {
            rrm_t      *rrm = &rrm_inst[ii] ;

            pItem = get_msg( &(rrm->file_send_ip_msg) ) ;


            if ( pItem == NULL )
                no_msg++;
            else
            {
                // fprintf(stderr,"Thread Send Message inst %d socket %d msg_type = %d dest %X\n", ii, rrm->ip.s->s,pItem->msg->head.msg_type, rrm->ip.s->in_dest_addr.sin_addr.s_addr); //dbg
                //fprintf(stderr,"dest in msg%X\n\n\n\n",  pItem->s_int->in_dest_addr.sin_addr.s_addr); //dbg
               
               
               // if (pItem->msg->head.msg_type == 26)
                 //   msg_fct( "IP -> UPDATE_SENSING_RESULTS_3 inst: %d sockid %d\n", ii, rrm->ip.s->s);//dbg
                //printf("T4b:ok user %d msg %d\n", rrm->id,pItem->msg->head.msg_type);//dbg
                int r =  send_msg_int( rrm->ip.s, pItem->msg );
                //printf("T5b:ok r: %d\n",r);//dbg
           
                WARNING(r!=0);
            }
            RRM_FREE( pItem ) ;
        }

        if ( no_msg==nb_inst ) // Pas de message
            usleep(1000);
    }
    fprintf(stderr,"... stopped Thread Send Message\n"); fflush(stderr);
    return NULL;
}

/*!
*******************************************************************************
\brief  thread de traitement des messages sortants sur les sockets (rrc ou cmm).

\return NULL
*/
static void * thread_send_msg_sensing (
				       void * p_data /**< parametre du pthread */
				       )
{
  int ii ;
  
  int no_msg ;
  fprintf(stderr,"Thread Send Message To Sensing Unit: starting ... \n");
  fflush(stderr);
  file_msg_t *pItem ;
  while ( flag_not_exit)
    {
      no_msg = 0  ;
      for ( ii = 0 ; ii<nb_inst ; ii++ )
        {
	  rrm_t      *rrm = &rrm_inst[ii] ;
	  
	  pItem = get_msg( &(rrm->file_send_sensing_msg) ) ;
	  
	  
	  if ( pItem == NULL )
	    no_msg++;
	  else
            {
                int r =  send_msg( pItem->s, pItem->msg );
                WARNING(r!=0);
	      
            }
	  RRM_FREE( pItem ) ;
        }
      
      if ( no_msg==nb_inst ) // Pas de message
	usleep(1000);
    }
  fprintf(stderr,"... stopped Thread Send Message\n"); fflush(stderr);
  return NULL;
}

//mod_lor_10_01_25--*/



/*!
*******************************************************************************
\brief  thread de traitement des messages entrant sur une interface (rrc, cmm ou sensing).

\return NULL
*/

static void * thread_recv_msg (
    void * p_data /**< parametre du pthread */
    )
{
    msg_t *msg ;
    struct data_thread *data = (struct data_thread *) p_data;
    int sock ;

    fprintf(stderr,"%s interfaces :starting ... %s %s\n",data->name , data->sock_path_local, data->sock_path_dest);
    fflush(stderr);

    /* ouverture des liens de communications */
    sock = open_socket( &data->s,  data->sock_path_local, data->sock_path_dest ,0 );

    if ( sock != -1 )
    {
        fprintf(stderr,"   %s -> socket =  %d\n",data->name , sock );
        fflush(stderr);

        while (flag_not_exit)
        {
            msg = (msg_t *) recv_msg(&data->s) ;
            if (msg == NULL )
            {
                fprintf(stderr,"Server closed connection\n");
                flag_not_exit = 0;
            }
            else
            {
                int inst = msg->head.inst ;
                rrm_t      *rrm = &rrm_inst[inst];

                put_msg( &(rrm->file_recv_msg), 0, &data->s, msg) ;//mod_lor_10_01_25
            }
        }
        close_socket(&data->s) ;
    }

    fprintf(stderr,"... stopped %s interfaces\n",data->name);
    return NULL;
}

//mod_lor_10_01_25++
/*!
*******************************************************************************
\brief  thread de traitement des messages entrant via ip.

\return NULL
*/

static void * thread_recv_msg_int (
    void * p_data /**< parametre du pthread */
    )
{
    msg_t *msg ;
    struct data_thread_int *data = (struct data_thread_int *) p_data; 
    rrm_t      *rrm = &rrm_inst[data->instance]; 
    int sock ;

    fprintf(stderr,"%s interfaces :starting on inst. %d ... ",data->name, data->instance  );
    fprintf(stderr,"\n");
               
    fflush(stderr);

    /* ouverture des liens de communications */
    sock = open_socket_int( &data->s,  data->sock_path_local, data->local_port, data->sock_path_dest, data->dest_port,0 );
    data->s.s = sock;
    memcpy(rrm->ip.s, &(data->s), sizeof(sock_rrm_int_t));
    //fprintf(stderr,"IP dest %X \n", rrm->ip.s->in_dest_addr.sin_addr.s_addr);//dbg
 
    if ( sock != -1 )
    {
        fprintf(stderr,"   %s -> socket =  %d\n",data->name , sock );
        fflush(stderr);

        while (flag_not_exit)
        {
            
            msg = (msg_t *) recv_msg_int(rrm->ip.s) ;

            if (msg == NULL )
            {
                fprintf(stderr,"Server closed connection\n");
                flag_not_exit = 0;
            }
            else
            {
                //fprintf(stdout,"Ip dest in FC  when received msg %X\n",rrm_inst[0].ip.s->in_dest_addr.sin_addr.s_addr); //dbg
                //printf(stderr,"msg received from %X \n", rrm->ip.s->in_dest_addr.sin_addr.s_addr);//dbg
                put_msg( &(rrm->file_recv_msg), 1, &data->s, msg) ;
                //fprintf(stdout,"Ip dest in FC after put_msg %X\n",rrm_inst[0].ip.s->in_dest_addr.sin_addr.s_addr); //dbg
                
            }
        }
        close_socket_int(&data->s) ;
    }

    fprintf(stderr,"... stopped %s interfaces\n",data->name);
    return NULL;
}
//mod_lor_10_01_25--

/*******************************************************************************/
#ifdef RRC_KERNEL_MODE
char Header_buf[sizeof(msg_head_t)];
char Data[2400];
unsigned short  Header_read_idx=0,
                Data_read_idx=0, 
                Data_to_read=0,
                Header_size=sizeof(msg_head_t),
                READ_OK=1;
  
/*!
*******************************************************************************
\brief  thread de traitement des messages entrant sur l'interface fifo du RRC
        en mode KERNEL

\return NULL
*/
static void * thread_recv_msg_fifo (void * p_data )
{
    msg_t      *msg_cpy   ;
    rrm_t      *rrm       ;
    msg_head_t *Header    ;
    int         taille    ;
    int         inst      ;
    int         bytes_read;
    
    msg_fifo("[RRM]: RX MSG_FIFOS %d handler starting....\n",RRC2RRM_FIFO);
    
    while (flag_not_exit)
    {
        if(Header_read_idx < Header_size)
        {
            bytes_read = read(Rrm_fifos.rrc_2_rrm_fifo,&Data[Header_read_idx],Header_size-Header_read_idx);
            if(bytes_read <0) 
                continue;
            Header_read_idx+=bytes_read;
            msg_fifo("[RRM]: RX MSG ON FIFOS %d: Header size %d, bytes_read %d\n",RRC2RRM_FIFO,Header_read_idx,bytes_read);
            if(Header_read_idx == Header_size)
            {
                Header=(msg_head_t*)Data;
                Data_to_read=Header->size;
                Data_read_idx=Header_read_idx;
                msg_fifo("[RRM]: RX MSG ON FIFOS %d: Header read completed, Data size %d\n",RRC2RRM_FIFO,Data_to_read);
                Header = (msg_head_t *) Data;
            }
            else
                continue;
        }
        
        if (Data_to_read > 0 )
        {
            bytes_read = read (Rrm_fifos.rrc_2_rrm_fifo,&Data[Data_read_idx],Data_to_read);
            if(bytes_read <0) 
                continue;
            Data_to_read-=bytes_read;
            Data_read_idx+=bytes_read;
            msg_fifo("[RRM]: RX MSG ON FIFOS %d: data size %d\n",RRC2RRM_FIFO,Data_read_idx-Header_read_idx);
            if(Data_to_read > 0 )
                continue;
            Header_read_idx= 0 ;
            Data_read_idx  = 0 ;
            Data_to_read   = 0 ;
            taille         = Header->size + sizeof(msg_head_t) ;
            msg_cpy        = RRM_MALLOC( msg_t, taille ) ;
            inst           = Header->inst ;
            rrm            = &rrm_inst[inst];
 
            memcpy( msg_cpy, Data , taille ) ;
            msg_fifo("[RRM]: RX MSG ON FIFOS %d: data read completed, Proccess on inst .... %d\n",RRC2RRM_FIFO,inst);
            put_msg( &(rrm->file_recv_msg), 0, rrm->rrc.s, msg_cpy) ; //mod_lor_10_01_25
            msg_fifo("[RRM]: RX MSG ON FIFOS %d: data read completed, Proccess on inst done %d\n",RRC2RRM_FIFO,inst);
        }
        else
        { /* Seulement le header */
            Header_read_idx= 0 ;
            Data_read_idx  = 0 ;
            Data_to_read   = 0 ;
            taille         = sizeof(msg_head_t) ;
            msg_cpy        = RRM_MALLOC( msg_t, taille ) ;
            inst           = Header->inst ;
            rrm            = &rrm_inst[inst];

            memcpy( msg_cpy, Data , taille ) ;
            put_msg( &(rrm->file_recv_msg), 0, rrm->rrc.s, msg_cpy) ;//mod_lor_10_01_25
        }
    }
    return NULL;
}
#endif
/*!
*******************************************************************************
\brief  traitement des messages entrant sur l'interface CMM

\return Auncune valeur
*/
static void processing_msg_cmm(
    rrm_t       *rrm        , ///< Donnee relative a une instance du RRM
    msg_head_t  *header     , ///< Entete du message
    char        *msg        , ///< Message recu
    int         len_msg       ///< Longueur du message
    )
{
#ifdef TRACE
    if ( header->msg_type < NB_MSG_CMM_RRM )
    fprintf(cmm2rrm_fd,"%lf CMM->RRM %d %-30s %d %d\n",get_currentclock(),header->inst,Str_msg_cmm_rrm[header->msg_type], header->msg_type,header->Trans_id);
    else
    fprintf(cmm2rrm_fd,"%lf CMM->RRM %-30s %d %d\n",get_currentclock(),"inconnu", header->msg_type,header->Trans_id);
    fflush(cmm2rrm_fd);
#endif

    switch ( header->msg_type )
    {
        case CMM_CX_SETUP_REQ:
            {
                cmm_cx_setup_req_t *p = (cmm_cx_setup_req_t *) msg ;
                msg_fct( "[CMM]>[RRM]:%d:CMM_CX_SETUP_REQ\n",header->inst);
                if ( cmm_cx_setup_req(header->inst,p->Src,p->Dst,p->QoS_class,header->Trans_id ) )
                { /* RB_ID = 0xFFFF => RB error */
                    put_msg( &(rrm->file_send_cmm_msg), 0,
                                rrm->cmm.s, msg_rrm_cx_setup_cnf(header->inst,0xFFFF , header->Trans_id )) ;//mod_lor_10_01_25
                }
            }
            break ;
        case CMM_CX_MODIFY_REQ:
            {
                cmm_cx_modify_req_t *p = (cmm_cx_modify_req_t *) msg ;
                msg_fct( "[CMM]>[RRM]:%d:CMM_CX_MODIFY_REQ\n",header->inst);
                cmm_cx_modify_req(header->inst,p->Rb_id,p->QoS_class,header->Trans_id )  ;
            }
            break ;
        case CMM_CX_RELEASE_REQ :
            {
                cmm_cx_release_req_t *p = (cmm_cx_release_req_t *) msg ;
                msg_fct( "[CMM]>[RRM]:%d:CMM_CX_RELEASE_REQ\n",header->inst);
                cmm_cx_release_req(header->inst,p->Rb_id,header->Trans_id )  ;
            }
            break ;
        case CMM_CX_RELEASE_ALL_REQ :
            {
                //cmm_cx_release_all_req_t *p = (cmm_cx_release_all_req_t *) msg ;
                msg_fct( "[CMM]>[RRM]:%d:CMM_CX_RELEASE_ALL_REQ\n",header->inst);
            }
            break ;
        case CMM_ATTACH_CNF : ///< The thread that allows 
            {
                cmm_attach_cnf_t *p = (cmm_attach_cnf_t *) msg ;
                msg_fct( "[CMM]>[RRM]:%d:CMM_ATTACH_CNF\n",header->inst);
              
               //mod_lor_10_01_25++
#ifndef    RRC_EMUL          
                
                if (rrm->ip.s->s == -1){ 
                    //mod_lor_10_05_06++
                    unsigned char tmp [4];
                    /*for (int i=0; i<10;i++ )
                        if (memcmp( &(node_info[i].L2_id), &(p->L2_id), sizeof(L2_ID) )){
                            fprintf(stderr,"Inst. to connect with %d\n",i);
                            break;
                        }//memcpy()*/
                       
                    //mod_lor_10_05_18++: destination addresses depends on 
                    //scenario and on role, they are declared at beginning of file
                    if (SCEN_2_CENTR && rrm->id >= FIRST_SECOND_CLUSTER_USER_ID){ //mod_lor_11_01_06
                        tmp[0]=CH_COLL_L3id[0];
                        tmp[1]=CH_COLL_L3id[1];
                        tmp[2]=CH_COLL_L3id[2];
                        tmp[3]=CH_COLL_L3id[3];
                    }
                    else if (rrm->id == 5){
                        tmp[0]=BTS_L3id[0];
                        tmp[1]=BTS_L3id[1];
                        tmp[2]=BTS_L3id[2];
                        tmp[3]=BTS_L3id[3];
                    }
                   else {
                        tmp[0]=FC_L3id[0];
                        tmp[1]=FC_L3id[1];
                        tmp[2]=FC_L3id[2];
                        tmp[3]=FC_L3id[3];
                    }//mod_lor_10_05_18--
                       //unsigned char tmp [4]={0x0A,0x00,0x01,0x01};
                    /*fprintf(stderr,"IP_addr :");//dbg //mod_lor_10_05_06
                    for (int i=0;i<4;i++)//dbg
                        fprintf(stderr," %X",tmp[i]);//dbg
                    fprintf(stderr,"\n");//dbg*/
                    
                    //mod_lor_10_05_06--
                    fprintf(stderr,"IP interface starting inst. %d\n",rrm->id); 
                    if (rrm->id != 5){
                        int sock = open_socket_int(rrm->ip.s, p->L3_info, 0, tmp, 0, header->inst);
                        if ( sock != -1 )
                        {
                            fprintf(stderr,"   Ip -> socket =  %d\n", rrm->ip.s->s );
                            fflush(stderr);
                        }else
                            fprintf(stderr," Error in IP socket opening \n");
                    }
                }else
                        fprintf(stderr," Socket IP for inst %d already opened %d \n",rrm->id,rrm->ip.s->s);
#endif            //mod_lor_10_01_25--*/
                
                cmm_attach_cnf( header->inst, p->L2_id, p->L3_info_t, p->L3_info, header->Trans_id ) ;
            }
            break ;
        case CMM_INIT_MR_REQ :
            {
                msg_fct( "[CMM]>[RRM]:%d:CMM_INIT_MR_REQ ????\n",header->inst);
            }
            break ;
        case CMM_INIT_CH_REQ :
            {
                cmm_init_ch_req_t *p = (cmm_init_ch_req_t *) msg ;
                //mod_lor_10_03_01++
            
                struct data_thread_int DataIp;
                
                DataIp.name = "IP"             ; ///< Nom du thread
                DataIp.sock_path_local=p->L3_info;///< local IP address for internet socket
                DataIp.local_port = 7000          ; ///< local IP port for internet socket
                //mod_lor_10_03_01++: setting for topology with FC and BTS on instances 0 and 1
                //mod_lor_10_05_18++
                if (rrm->role == FUSIONCENTER){
                    if (SCEN_1)
                        DataIp.sock_path_dest = BTS_L3id ; ///< dest IP address for internet socket
                    else if (SCEN_2_CENTR)
                        DataIp.sock_path_dest = CH_COLL_L3id ; ///< dest IP address for internet socket
                }else if (rrm->role == BTS ||rrm->role == CH_COLL){ //mod_lor_10_04_27
                    DataIp.sock_path_dest = FC_L3id  ; ///< dest IP address for internet socket
                }else 
                    fprintf (stderr, "wrong node role %d \n", rrm->role);
                //mod_lor_10_05_18--
                //mod_lor_10_03_01--
                DataIp.dest_port = 0          ; ///< dest IP port for internet socket
                DataIp.s.s = -1      ; 
                DataIp.instance = rrm->id;
                //fprintf(stderr,"L3_local ");//dbg
                //print_L3_id( IPv4_ADDR,  rrm->L3_info );
                //fprintf(stderr,"\n");//dbg
                
                int ret = pthread_create ( &pthread_recv_int_msg_hnd, NULL, thread_recv_msg_int , &DataIp );
                if (ret)
                {
                    fprintf (stderr, "%s", strerror (ret));
                    exit(-1) ;
                }
                    
                sleep(5);
                //mod_lor_10_03_01--
                cmm_init_ch_req(header->inst,p->L3_info_t,&(p->L3_info[0]));
                msg_fct( "[CMM]>[RRM]:%d:CMM_INIT_CH_REQ\n",header->inst);
                
            }
            break ;

        case CMM_INIT_SENSING :
            {
                cmm_init_sensing_t *p = (cmm_init_sensing_t *) msg ;                
                msg_fct( "[CMM]>[RRM]:%d:CMM_INIT_SENSING\n",header->inst);
                rrm->sensing.sens_active=1;//mod_lor_10_05_07
                //st_fr = p->Start_fr; //mod_lor_10_06_01
                //end_fr = p->Stop_fr; //mod_lor_10_06_01
                cmm_init_sensing(header->inst,p->Start_fr ,p->Stop_fr,p->Meas_band,p->Meas_tpf,
                        p->Nb_channels, p->Overlap,p->Sampl_freq);
                
            }
            break ;
        case CMM_INIT_COLL_SENSING : //add_lor_10_11_08
            {
                cmm_init_coll_sensing_t *p = (cmm_init_coll_sensing_t *) msg ;                
                msg_fct( "[CMM]>[RRM]:%d:CMM_INIT_COLL_SENSING\n",header->inst);
                rrm->sensing.sens_active=1;
                cmm_init_coll_sensing(header->inst,p->Start_fr ,p->Stop_fr,p->Meas_band,p->Meas_tpf,
                        p->Nb_channels, p->Overlap,p->Sampl_freq);
                
            }
            break ;
        case CMM_STOP_SENSING :
            {
                msg_fct( "[CMM]>[RRM]:%d:CMM_STOP_SENSING\n",rrm->id);
                rrm->sensing.sens_active=0;//mod_lor_10_05_07
                //print_sens_db(rrm->rrc.pSensEntry);//dbg
                cmm_stop_sensing(header->inst);
            }
            break ;
        case CMM_ASK_FREQ :
            {
                msg_fct( "[CMM]>[RRM]:%d:CMM_ASK_FREQ\n",header->inst);
                cmm_ask_freq(header->inst);
            }
            break ;
        case CMM_NEED_TO_TX : //add_lor_10_10_28
            {
                cmm_need_to_tx_t *p = (cmm_need_to_tx_t *) msg ;  
                msg_fct( "[CMM]>[RRM]:%d:CMM_NEED_TO_TX\n",header->inst);
                cmm_need_to_tx(header->inst, p->dest,p->QoS_class);
            }
            break ;
        case CMM_USER_DISC : //add_lor_10_11_08
            {
                msg_fct( "[CMM]>[RRM]:%d:CMM_USER_DISC\n",header->inst);
                cmm_user_disc(header->inst);
            }
            break ;
        case CMM_LINK_DISC : //add_lor_10_11_09
            {
                cmm_link_disk_t *p = (cmm_link_disk_t *) msg ;
                msg_fct( "[CMM]>[RRM]:%d:CMM_LINK_DISC with user %d\n",header->inst, p->dest);
                cmm_link_disc(header->inst, p->dest);
            }
            break ;

        default :
            fprintf(stderr,"CMM:\n") ;
            printHex(msg,len_msg,1) ;
    }
}
/*!
*******************************************************************************
\brief  traitement des messages entrant sur l'interface RRC

\return Aucune valeur
*/
static void processing_msg_rrc(
    rrm_t *rrm          , ///< Donnee relative a une instance du RRM
    msg_head_t *header  , ///< Entete du message
    char *msg           , ///< Message recu
    int len_msg           ///< Longueur du message
    )
{
#ifdef TRACE
    if ( header->msg_type < NB_MSG_RRC_RRM )
      fprintf(rrc2rrm_fd,"%lf RRC->RRM %d %-30s %d %d\n",get_currentclock(),header->inst,Str_msg_rrc_rrm[header->msg_type],header->msg_type,header->Trans_id);
    else
    fprintf(rrc2rrm_fd,"%lf RRC->RRM %-30s %d %d\n",get_currentclock(),"inconnu",header->msg_type,header->Trans_id);
    fflush(rrc2rrm_fd);
#endif

    switch ( header->msg_type )
    {
        case RRC_RB_ESTABLISH_RESP:
            {
                msg_fct( "[RRC]>[RRM]:%d:RRC_RB_ESTABLISH_RESP %d \n",header->inst, header->Trans_id );
                rrc_rb_establish_resp(header->inst,header->Trans_id) ;
            }
            break ;
        case RRC_RB_ESTABLISH_CFM:
            {
                rrc_rb_establish_cfm_t *p = (rrc_rb_establish_cfm_t *) msg ;
                msg_fct( "[RRC]>[RRM]:%d:RRC_RB_ESTABLISH_CFM (%d)  %d \n",header->inst,p->Rb_id, header->Trans_id);
                rrc_rb_establish_cfm(header->inst,p->Rb_id,p->RB_type,header->Trans_id) ;
            }
            break ;

        case RRC_RB_MODIFY_RESP:
            {
                msg_fct( "[RRC]>[RRM]:%d:RRC_RB_MODIFY_RESP  %d \n",header->inst, header->Trans_id);
                rrc_rb_modify_resp(header->inst,header->Trans_id) ;
            }
            break ;
        case RRC_RB_MODIFY_CFM:
            {
                rrc_rb_modify_cfm_t *p = (rrc_rb_modify_cfm_t *) msg ;
                msg_fct( "[RRC]>[RRM]:%d:RRC_RB_MODIFY_CFM (%d) %d \n",header->inst,p->Rb_id, header->Trans_id);
                rrc_rb_modify_cfm(header->inst,p->Rb_id,header->Trans_id) ;
            }
            break ;

        case RRC_RB_RELEASE_RESP:
            {
                msg_fct( "[RRC]>[RRM]:%d:RRC_RB_RELEASE_RESP %d \n",header->inst, header->Trans_id);
                rrc_rb_release_resp(header->inst,header->Trans_id) ;
            }
            break ;
        case RRC_MR_ATTACH_IND :
            {
                rrc_MR_attach_ind_t *p = (rrc_MR_attach_ind_t *) msg ;
                msg_fct( "[RRC]>[RRM]:%d:RRC_MR_ATTACH_IND (Node %02d) %d \n",header->inst, p->L2_id.L2_id[0], header->Trans_id);
                rrc_MR_attach_ind(header->inst,p->L2_id) ;
            }
            break ;
        case RRC_SENSING_MEAS_RESP:
            {
                msg_fct( "[RRC]>[RRM]:%d:RRC_SENSING_MEAS_RESP %d \n",header->inst, header->Trans_id);
                rrc_sensing_meas_resp(header->inst,header->Trans_id) ;
            }
            break ;
        case RRC_CX_ESTABLISH_IND:
            {
                rrc_cx_establish_ind_t *p = (rrc_cx_establish_ind_t *) msg ;
                msg_fct( "[RRC]>[RRM]:%d:RRC_CX_ESTABLISH_IND (Node %02d) %d \n",header->inst, p->L2_id.L2_id[0], header->Trans_id);

                rrc_cx_establish_ind(header->inst,p->L2_id,header->Trans_id,
                                    p->L3_info,p->L3_info_t,
                                    p->DTCH_B_id,p->DTCH_id) ;
                //mod_lor_10_01_25++
                if (rrm->state == MESHROUTER){
                    memcpy(rrm->L3_info_corr,p->L3_info, IPv4_ADDR);
                    
               }
                    //mod_lor_10_01_25--*/
            }
            break ;
        case RRC_PHY_SYNCH_TO_MR_IND :
            {
                rrc_phy_synch_to_MR_ind_t *p = (rrc_phy_synch_to_MR_ind_t *) msg ;
                msg_fct( "[RRC]>[RRM]:%d:RRC_PHY_SYNCH_TO_MR_IND.... (Node %02d) %d \n",header->inst, p->L2_id.L2_id[0], header->Trans_id);
                rrc_phy_synch_to_MR_ind(header->inst,p->L2_id) ;
                //msg_fct( "[RRC]>[RRM]:%d:RRC_PHY_SYNCH_TO_MR_IND Done\n",header->inst);
            }
            break ;
        case RRC_PHY_SYNCH_TO_CH_IND :
            {
                rrc_phy_synch_to_CH_ind_t *p = (rrc_phy_synch_to_CH_ind_t *) msg ;
                msg_fct( "[RRC]>[RRM]:%d:RRC_PHY_SYNCH_TO_CH_IND.... %d (Node %02d) %d \n",header->inst, p->Ch_index, p->L2_id.L2_id[0], header->Trans_id);
                rrc_phy_synch_to_CH_ind(header->inst,p->Ch_index,p->L2_id ) ;
                //msg_fct( "[RRC]>[RRM]:%d:RRC_PHY_SYNCH_TO_CH_IND Done\n",header->inst);

            }
            break ;
        case RRC_SENSING_MEAS_IND :
            {
                rrc_sensing_meas_ind_t *p  = (rrc_sensing_meas_ind_t *) msg ;
                msg_fct( "[RRC]>[RRM]:%d:RRC_SENSING_MEAS_IND (Node %02d) %d \n",header->inst, p->L2_id.L2_id[0], header->Trans_id);
                rrc_sensing_meas_ind( header->inst,p->L2_id, p->NB_meas, p->Sensing_meas, header->Trans_id );
            }
            break ;
        case RRC_RB_MEAS_IND :
            {
                rrc_rb_meas_ind_t *p  = (rrc_rb_meas_ind_t *) msg ;
                msg_fct( "[RRC]>[RRM]:%d:RRC_RB_MEAS_IND (Noede %02d) %d \n",header->inst, p->L2_id.L2_id[0], header->Trans_id);
                rrc_rb_meas_ind( header->inst, p->Rb_id, p->L2_id, p->Meas_mode, p->Mac_rlc_meas, header->Trans_id );
            }
            break ;


        case RRC_INIT_SCAN_REQ :
            {
                rrc_init_scan_req_t *p  = (rrc_init_scan_req_t *) msg ;
              //  fprintf(stdout,"sens_database before:\n");//dbg
              //  print_sens_db( rrm->rrc.pSensEntry );//dbg
                msg_fct( "[RRC]>[RRM]:%d:RRC_INIT_SCAN_REQ  %d \n",header->inst, header->Trans_id);
                rrm->sensing.sens_active=1;//mod_lor_10_04_21

                rrc_init_scan_req( header->inst, p->L2_id, p->Start_fr ,p->Stop_fr,p->Meas_band,p->Meas_tpf,
                        p->Nb_channels, p->Overlap,p->Sampl_freq, header->Trans_id ); 
                
            //    fprintf(stdout,"sens_database:\n");//dbg
            //    print_sens_db( rrm->rrc.pSensEntry );//dbg
                
            }
            break ;
        case RRC_END_SCAN_CONF :
            {
                rrc_end_scan_conf_t *p  = (rrc_end_scan_conf_t *) msg ;
                msg_fct( "[RRC]>[RRM]:%d:RRC_END_SCAN_CONF  %d (Node ",header->inst, header->Trans_id);
                for ( int i=0;i<8;i++)
                    msg_fct("%02X", p->L2_id.L2_id[i]);
                msg_fct( ")\n");
                rrc_end_scan_conf( header->inst, p->L2_id, header->Trans_id );  
            }
            break ;
        case RRC_END_SCAN_REQ :
            {
                rrc_end_scan_req_t *p  = (rrc_end_scan_req_t *) msg ;
                msg_fct( "[RRC]>[RRM]:%d:RRC_END_SCAN_REQ %d \n",header->inst, header->Trans_id);
                rrm->sensing.sens_active=0;//mod_lor_10_04_21
                rrc_end_scan_req( header->inst, p->L2_id, header->Trans_id );  
            }
            break ;
        case RRC_INIT_MON_REQ :
            {
                rrc_init_mon_req_t *p  = (rrc_init_mon_req_t *) msg ;
                msg_fct( "[RRC]>[RRM]:%d:RRC_INIT_MON_REQ %d (Node ",header->inst, header->Trans_id);
                for ( int i=0;i<8;i++)
                    msg_fct("%02X", p->L2_id.L2_id[i]);
                msg_fct( ")\n");
                rrc_init_mon_req( header->inst, p->L2_id, p->ch_to_scan, p->NB_chan, p->interval, header->Trans_id );  
            }
            break ;
        //mod_lor_10_06_04++
        case RRC_UP_FREQ_ASS :
            {
                //rrm_up_freq_ass_t *p  = (rrm_up_freq_ass_t *) msg ;
                //int i;
                msg_fct( "[RRC]>[RRM]:%d:RRC_UP_FREQ_ASS \n",header->inst);
                 
            }
            break;
        //mod_lor_10_06_04--
        //mod_lor_10_11_05++
        case RRC_UP_FREQ_ASS_SEC :
            {
                rrm_up_freq_ass_sec_t *p  = (rrm_up_freq_ass_sec_t *) msg ;
                int i;
                unsigned int chann_start[NB_SENS_MAX];
                unsigned int chann_end[NB_SENS_MAX];
                unsigned int tx[NB_SENS_MAX];
                unsigned int tot_ch = 0;
                msg_fct( "[RRC]>[RRM]:%d:RRC_UP_FREQ_ASS_SEC ch %d\n",header->inst,p->NB_all);
    
                for (i=0;i<p->NB_all;i++){
                    if (memcmp( &(p->L2_id[i]), &(rrm->L2_id), sizeof(L2_ID) )==0){
                        printf("USER:%d:Transmission on channel %d from %d to %d\n\n",header->inst,p->ass_channels[i].Ch_id, p->ass_channels[i].Start_f, p->ass_channels[i].Final_f);
                        chann_start[tot_ch] = p->ass_channels[i].Start_f;
                        chann_end[tot_ch]   = p->ass_channels[i].Final_f;
                        tx[tot_ch]          = 1;
                        tot_ch++;
                    }
                    if (memcmp( &(p->L2_id_dest[i]), &(rrm->L2_id), sizeof(L2_ID) )==0){
                        printf("USER:%d:Ready to receive on channel %d from %d to %d\n\n",header->inst,p->ass_channels[i].Ch_id, p->ass_channels[i].Start_f, p->ass_channels[i].Final_f);
                        chann_start[tot_ch] = p->ass_channels[i].Start_f;
                        chann_end[tot_ch]   = p->ass_channels[i].Final_f;
                        tx[tot_ch]          = 0;
                        tot_ch++;
                    }
                }
                plot_all_chann_scen_2(tx, chann_start, chann_end, tot_ch, header->inst-FIRST_SENSOR_ID+1);
                
                 
            }
            break;
        //mod_lor_10_11_05--

        default :
            fprintf(stderr,"RRC:\n") ;
            printHex(msg,len_msg,1) ;
    }

}

static void processing_msg_sensing(
    rrm_t       *rrm        , ///< Donnee relative a une instance du RRM
    msg_head_t  *header     , ///< Entete du message
    char        *msg        , ///< Message recu
    int         len_msg       ///< Longueur du message
    )
{

#ifdef TRACE
  if ( header->msg_type < NB_MSG_SENSING_RRM )
    fprintf(sensing2rrm_fd,"%lf SENSING->RRM %d %-30s %d %d\n",get_currentclock(),header->inst,Str_msg_sensing_rrm[header->msg_type],header->msg_type,header->Trans_id);
  else
    fprintf(sensing2rrm_fd,"%lf SENSING->RRM %-30s %d %d\n",get_currentclock(),"inconnu",header->msg_type,header->Trans_id);
  fflush(sensing2rrm_fd);
#endif
  
  switch ( (MSG_SENSING_RRM_T)header->msg_type )
    {

        case SNS_UPDATE_SENS :
            {
                rrc_update_sens_t *p  = (rrc_update_sens_t *) msg ;
                if (rrm->sensing.sens_active) {//mod_lor_10_05_07
                    msg_fct( "[SENSING]>[RRM]:%d:SNS_UPDATE_SENS \n",header->inst);
                    if (SCEN_1)
                        plot_spectra(p->Sens_meas, p->NB_info,  header->inst-FIRST_SENSOR_ID+1);
                    //mod_lor_10_11_04++
                    else if (SCEN_2_CENTR && !COLL_CLUST)
                        plot_spectra_CH1(p->Sens_meas, p->NB_info,  header->inst-FIRST_SENSOR_ID+1);
                    else if (SCEN_2_CENTR && COLL_CLUST)
                        plot_spectra_CH2(p->Sens_meas, p->NB_info,  header->inst-FIRST_SENSOR_ID+1);
                    
                    //mod_lor_10_11_04--
   
                    rrc_update_sens( header->inst, rrm->L2_id, p->NB_info, p->Sens_meas, p->info_time );  //fix info_time & understand trans_id
                }//mod_lor_10_05_07
            }
            break ;
    //mod_lor_10_04_14++
         case SNS_END_SCAN_CONF :
            {
                
                msg_fct( "[SENSING]>[RRM]:%d:SNS_END_SCAN_CONF\n",header->inst);
                
                sns_end_scan_conf( header->inst );  
            }
            break ;
        default :
            fprintf(stderr,"SENSING:%d:\n",header->msg_type) ;
            printHex(msg,len_msg,1) ;
    }//mod_lor_10_04_14--
}


/*!
*******************************************************************************
\brief  traitement des messages entrant sur l'interface PUSU

\return Aucune valeur
*/
static void processing_msg_pusu(
    rrm_t *rrm          , ///< Donnee relative a une instance du RRM
    msg_head_t *header  , ///< Entete du message
    char *msg           , ///< Message recu
    int len_msg           ///< Longueur du message
    )
{
    transact_t *pTransact ;

    pthread_mutex_lock( &( rrm->pusu.exclu ) ) ;
    pTransact = get_item_transact(rrm->pusu.transaction,header->Trans_id ) ;
    if ( pTransact == NULL )
    {
        fprintf(stderr,"[RRM] %d PUSU Response (%d): unknown transaction\n",header->msg_type,header->Trans_id);
    }
    else
    {
        del_item_transact( &(rrm->pusu.transaction),header->Trans_id ) ;
    }
    pthread_mutex_unlock( &( rrm->pusu.exclu ) ) ;

#ifdef TRACE
    if ( header->msg_type < NB_MSG_RRM_PUSU )
      fprintf(pusu2rrm_fd,"%lf PUSU->RRM %d %-30s %d %d\n",get_currentclock(),header->inst,Str_msg_pusu_rrm[header->msg_type],header->msg_type,header->Trans_id);
    else
    fprintf(pusu2rrm_fd,"%lf PUSU->RRM %-30s %d %d\n",get_currentclock(),"inconnu",header->msg_type,header->Trans_id);
    fflush(pusu2rrm_fd);
#endif

    switch ( header->msg_type )
    {
        case PUSU_PUBLISH_RESP:
            {
                msg_fct( "[PUSU]>[RRM]:%d:PUSU_PUBLISH_RESP\n",header->inst );
            }
            break ;
        case PUSU_UNPUBLISH_RESP:
            {
                msg_fct( "[PUSU]>[RRM]:%d:PUSU_UNPUBLISH_RESP\n",header->inst );
            }
            break ;
        case PUSU_LINK_INFO_RESP:
            {
                msg_fct( "[PUSU]>[RRM]:%d:PUSU_LINK_INFO_RESP\n",header->inst );
            }
            break ;
        case PUSU_SENSING_INFO_RESP:
            {
                msg_fct( "[PUSU]>[RRM]:%d:PUSU_SENSING_INFO_RESP\n",header->inst );
            }
            break ;
        case PUSU_CH_LOAD_RESP:
            {
                msg_fct( "[PUSU]>[RRM]:%d:PUSU_CH_LOAD_RESP\n",header->inst );
            }
            break ;
        default :
            fprintf(stderr,"PUSU:%d:\n",header->msg_type) ;
            printHex(msg,len_msg,1) ;
    }

}

//mod_lor_10_01_25++
/*!
*******************************************************************************
\brief  traitement des messages entrant via IP

\return Auncune valeur
*/
static void processing_msg_ip(
    rrm_t       *rrm        , ///< Donnee relative a une instance du RRM
    msg_head_t  *header     , ///< Entete du message
    char        *msg        , ///< Message recu
    int         len_msg       ///< Longueur du message
    )
{
#ifdef TRACE
    //mod_lor_10_04_27++
    /*if ( header->msg_type < NB_MSG_RRC_RRM )
    fprintf(ip2rrm_fd,"%lf IP->RRM %d %-30s %d %d\n",get_currentclock(),header->inst,Str_msg_rrc_rrm[header->msg_type], header->msg_type,header->Trans_id);
    else
    fprintf(ip2rrm_fd,"%lf CMM->RRM %-30s %d %d\n",get_currentclock(),"inconnu", header->msg_type,header->Trans_id);
    fflush(ip2rrm_fd);*/
    if ( header->msg_type < NB_MSG_IP )//mod_lor_10_04_27
    fprintf(ip2rrm_fd,"%lf IP->RRM %d %-30s %d %d\n",get_currentclock(),header->inst,Str_msg_ip[header->msg_type], header->msg_type,header->Trans_id);
    else
    fprintf(ip2rrm_fd,"%lf IP->RRM %-30s %d %d\n",get_currentclock(),"inconnu", header->msg_type,header->Trans_id);
    fflush(ip2rrm_fd);
    //mod_lor_10_04_27--
#endif

    switch ( header->msg_type )
    {
        case UPDATE_SENS_RESULTS_3 :
            {
                //fprintf(stderr,"1node entry  @%p \n", rrm->rrc.pSensEntry);//dbg
                rrm_update_sens_t *p = (rrm_update_sens_t *) msg ;
                //fprintf(stdout,"Ip dest in FC before update%X\n",rrm_inst[0].ip.s->in_dest_addr.sin_addr.s_addr); //dbg
                if (rrm->sensing.sens_active){ //mod_lor_10_05_07
                    msg_fct( "[IP]>[RRM]:%d:UPDATE_SENS_RESULTS_3 from %d \n",rrm->id, header->inst);
                    if (SCEN_1){
                        if(update_sens_results( rrm->id, p->L2_id, p->NB_info, p->Sens_meas, p->info_time)||SN_waiting == 1) //mod_lor_10_06_02
                            if ((open_freq_query(rrm->id, rrm->L2_id, 0, 1)>0) && SN_waiting)   //mod_lor_10_06_02
                                SN_waiting=0; //mod_lor_10_06_02
                    }else if (SCEN_2_CENTR){ //add_lor_10_11_03
                        update_sens_results( rrm->id, p->L2_id, p->NB_info, p->Sens_meas, p->info_time);  
                    }
                    
                } //mod_lor_10_05_07
                //fprintf(stdout,"Ip dest in FC after update%X\n",rrm_inst[0].ip.s->in_dest_addr.sin_addr.s_addr); //dbg
                //fprintf(stderr,"2node entry  @%p \n", rrm->rrc.pSensEntry);//dbg
                
            }
            break ;
        //mod_lor_10_05_07++    
        case UP_CLUST_SENS_RESULTS :
            {
                update_coll_sens_t *p = (update_coll_sens_t *) msg ;
                if (rrm->sensing.sens_active){ 
                    msg_fct( "[IP]>[RRM]:%d:UP_CLUST_SENS_RESULTS from %d \n",rrm->id, header->inst);
                    if (SCEN_2_CENTR && !COLL_CLUST)
                        plot_spectra_CH1(p->Sens_meas, p->NB_info,  -1);
                    else
                        printf("Error!!! this message should be received by Cluster 1\n");
                    up_coll_sens_results( rrm->id, p->L2_id, p->NB_info, p->Sens_meas, p->info_time); 
                } 
            }
            break ;
        //mod_lor_10_05_07--
        case OPEN_FREQ_QUERY_4 :
            {
                open_freq_query_t *p = (open_freq_query_t *) msg ;
                msg_fct( "[IP]>[RRM]:%d:OPEN_FREQ_QUERY_4 from %d\n",rrm->id, header->inst);
                open_freq_query( rrm->id, p->L2_id, p->QoS, header->Trans_id ); 
                
            }
            break ;
        case ASK_FREQ_TO_CH_3 : //add_lor_10_11_03
            {
                ask_freq_to_CH_t *p = (ask_freq_to_CH_t *) msg ;
                msg_fct( "[IP]>[RRM]:%d:ASK_FREQ_TO_CH_3 from %d\n",rrm->id, header->inst);
                if (ask_freq_to_CH( rrm->id, &(p->L2_id), &(p->L2_id_dest), 1, header->Trans_id )!=0){
                    printf ("Not available channels... Waiting for a channel...\n");
                    pthread_mutex_lock( &( rrm->ip.exclu ) ) ;
                    memcpy(rrm->ip.L2_id_wait_users[rrm->ip.users_waiting_update][0].L2_id,p->L2_id.L2_id, sizeof(L2_ID));
                    memcpy(rrm->ip.L2_id_wait_users[rrm->ip.users_waiting_update][1].L2_id,p->L2_id_dest.L2_id, sizeof(L2_ID));
                    rrm->ip.users_waiting_update++;
                    printf ("N4 tot waiting: %d\n",rrm->ip.users_waiting_update);//db
                    pthread_mutex_unlock( &( rrm->ip.exclu ) ) ;
                } 
            }
            break ;
        case UPDATE_OPEN_FREQ_7 :
            {
                update_open_freq_t *p = (update_open_freq_t *) msg ;
                unsigned int occ_channels[p->NB_chan]; //mod_lor_10_05_18: occ_channels passed as parameter to update open freq.
                msg_fct( "[IP]>[RRM]:%d:UPDATE_OPEN_FREQ_7 from %d\n",rrm->id, header->inst);
                unsigned int NB_occ = update_open_freq( rrm->id, p->L2_id, p->NB_chan, occ_channels, p->channels, header->Trans_id ); 
                
                if (SCEN_1)     
                    plot_SN_channels(rrm->rrc.pChannelsEntry,NB_occ,occ_channels,rrm->id);//mod_lor_10_06_01
            }
            break ;
        case UPDATE_SN_OCC_FREQ_5 :
            {
                sleep (2); //mod_lor_10_06_02
                update_SN_occ_freq_t *p = (update_SN_occ_freq_t *) msg ;
                if (p->NB_chan<CH_NEEDED_FOR_SN)    //mod_lor_10_06_02
                    SN_waiting = 1;                 //mod_lor_10_06_02
                else                                //mod_lor_10_06_02
                    SN_waiting = 0;                 //mod_lor_10_06_02
                    
                msg_fct( "[IP]>[RRM]:%d:UPDATE_SN_OCC_FREQ_5 from %d \n",rrm->id, header->inst);
                if (SCEN_1)
                    plot_SN_channels(rrm->rrc.pChannelsEntry, p->NB_chan, p->occ_channels, rrm->id);//mod_lor_10_06_01
                if(update_SN_occ_freq( rrm->id, p->L2_id, p->NB_chan, p->occ_channels, header->Trans_id )) //mod_lor_10_05_18
                    open_freq_query( rrm->id, p->L2_id, 0, header->Trans_id ); //mod_lor_10_05_18
                
            }
            break ;
        //mod_lor_10_05_05++
        case INIT_COLL_SENS_REQ :
            {
                init_coll_sens_req_t *p = (init_coll_sens_req_t *) msg ;
                msg_fct( "[IP]>[RRM]:%d:INIT_COLL_SENS_REQ from %d\n",rrm->id, header->inst);
                rrm->sensing.sens_active=1;//mod_lor_10_05_07
                memcpy( rrm->L2_id_FC.L2_id, p->L2_id.L2_id, sizeof(L2_ID) );
                cmm_init_sensing(rrm->id,p->Start_fr ,p->Stop_fr,p->Meas_band,p->Meas_tpf,
                        p->Nb_channels, p->Overlap,p->Sampl_freq);
                
            }
            break ;
        //mod_lor_10_05_05--
        //mod_lor_10_05_06++
        case STOP_COLL_SENS :
            {
                //init_coll_sens_req_t *p = (init_coll_sens_req_t *) msg ;
                msg_fct( "[IP]>[RRM]:%d:STOP_COLL_SENS from %d\n",rrm->id, header->inst);
                //memcpy( rrm->L2_id_FC.L2_id, p->L2_id.L2_id, sizeof(L2_ID) );
                rrm->sensing.sens_active=0;//mod_lor_10_05_07
                cmm_stop_sensing(rrm->id);
                
            }
            break ;
        //mod_lor_10_05_06--
        //mod_lor_10_05_12++
        case STOP_COLL_SENS_CONF :
            {
                stop_coll_sens_conf_t *p = (stop_coll_sens_conf_t *) msg ;
                msg_fct( "[IP]>[RRM]:%d:STOP_COLL_SENS_CONF from %d\n",rrm->id, header->inst);
                rrc_end_scan_conf( header->inst, p->L2_id, header->Trans_id ); //AAA:to_check
                
            }
            break ;
        //mod_lor_10_05_12--
        case USER_DISCONNECT_9 : //add_lor_10_11_09
            {
                user_disconnect_t *p = (user_disconnect_t *) msg ;
                msg_fct( "[IP]>[RRM]:%d:USER_DISCONNECT from %d\n",rrm->id, header->inst);
                disconnect_user( rrm->id, p->L2_id );
                
            }
            break ;
        case CLOSE_LINK : //add_lor_10_11_09
            {
                close_link_t *p = (close_link_t *) msg ;
                msg_fct( "[IP]>[RRM]:%d:CLOSE_LINK from %d\n",rrm->id, header->inst);
                close_active_link( rrm->id, p->L2_id, p->L2_id_dest );
                
            }
            break ;
       
        default :
            fprintf(stderr,"IP:\n") ;
            printHex(msg,len_msg,1) ;
    }
}
//mod_lor_10_01_25--



/*!
*******************************************************************************
\brief  thread de traitement des messages sortants sur les sockets
        (rrc ou cmm).
\return none
*/
static void rrm_scheduler ( )
{
    int ii ;
    int no_msg ;
    fprintf(stderr,"RRM Scheduler: starting ... \n");
    fflush(stderr);
    file_msg_t *pItem ;
    //mod_lor_10_04_22++
    unsigned int priority = 0; ///< to guarantee priority to one rrm (fusion center) during sensing period
    unsigned int pr_ii = 0; ///< id of the rrm with priority
    rrm_t      *rrm ;
    //mod_lor_10_04_22--
    while ( flag_not_exit)
    {
        no_msg = 0  ;
        for ( ii = 0 ; ii<nb_inst ; ii++ )
        {
            //mod_lor_10_04_22++
            if (priority){
                rrm = &rrm_inst[pr_ii] ;
                ii--;
                priority = 0;
            }
            
            else{
                rrm = &rrm_inst[ii] ;
                if (ii > 0 && rrm->sensing.sens_active)
                    priority = 1;
            }
            //mod_lor_10_04_22--
           // rrm = &rrm_inst[ii] ;
            pItem=NULL;

            pItem = get_msg( &(rrm->file_recv_msg)) ;

            if ( pItem == NULL )
            no_msg++;
            else
            {
                msg_head_t *header = (msg_head_t *) pItem->msg;
                char *msg = NULL ;
                if ( header != NULL )
                {
                    if ( header->size > 0 )
                    {
                        msg = (char *) (header +1) ;
                    }
                     //mod_lor_10_01_25
                    if (pItem->s_type==0){
                        if ( pItem->s->s == rrm->cmm.s->s )
                            processing_msg_cmm( rrm , header , msg , header->size ) ;
                        else if ( pItem->s->s == rrm->rrc.s->s )
                        {
                            processing_msg_rrc( rrm , header , msg , header->size ) ;
                        }
                        else if ( pItem->s->s == rrm->sensing.s->s) {
                          processing_msg_sensing( rrm , header , msg , header->size ) ;
                          //fprintf(stderr,"RRM Scheduler: sensing message ... \n"); //dbg
                        }
                        else
                            processing_msg_pusu( rrm , header , msg , header->size ) ;
                    }
                    else{
                        //fprintf(stderr,"RRM Scheduler: ip message ... \n"); //dbg
                        processing_msg_ip( rrm , header , msg , header->size ) ;
                        
                    }

                RRM_FREE( pItem->msg) ;
            }
            RRM_FREE( pItem ) ;
          }
      }
        if ( no_msg == nb_inst )
      usleep(1000);
      
     
      
      }
    fprintf(stderr,"... stopped RRM Scheduler\n"); fflush(stderr);
}
/*!
*******************************************************************************
\brief This function reads the configuration node file
*/
static void get_config_file(char *filename )
{
    FILE *fd = fopen( filename , "r" );
    char buf_line[128] ;
    int adresse[LENGTH_L2_ID] ;
    int ii = 0 ;


    if ( fd == NULL )
        return ;

    while ( 1 )
    {
        fgets( buf_line, 127, fd ) ;
        if (feof(fd))
            break ;

        if ( buf_line[0] == '#' )
            continue ;
        if ( buf_line[0] == ' ' )
            continue ;
        if ( buf_line[0] == '\t' )
            continue ;
        if ( buf_line[0] == '\n' )
            continue ;

        sscanf( buf_line, "%x %x %x %x %x %x %x %x",
                          &adresse[0],&adresse[1],&adresse[2],&adresse[3],
                          &adresse[4],&adresse[5],&adresse[6],&adresse[7]);

        rrm_inst[ii].id                 = ii ;
        rrm_inst[ii].L2_id.L2_id[0]     = adresse[0] &  0xFF ;
        rrm_inst[ii].L2_id.L2_id[1]     = adresse[1] &  0xFF;
        rrm_inst[ii].L2_id.L2_id[2]     = adresse[2] &  0xFF;
        rrm_inst[ii].L2_id.L2_id[3]     = adresse[3] &  0xFF;
        rrm_inst[ii].L2_id.L2_id[4]     = adresse[4] &  0xFF;
        rrm_inst[ii].L2_id.L2_id[5]     = adresse[5] &  0xFF;
        rrm_inst[ii].L2_id.L2_id[6]     = adresse[6] &  0xFF;
        rrm_inst[ii].L2_id.L2_id[7]     = adresse[7] &  0xFF;

        print_L2_id( &rrm_inst[ii].L2_id ) ;
        fprintf(stderr," (%d) %d \n", ii, buf_line[0] );

        ii++ ;

        adresse[0]=adresse[1]=adresse[2]=adresse[3]=adresse[4]=adresse[5]=adresse[6]=adresse[7]=0;
        buf_line[0] = 0;
    }

    nb_inst = ii ;

    fclose(fd) ;
}
/*!
*******************************************************************************
\brief This function displays the help message (syntax)
*/
static void help()
{
    fprintf(stderr,"syntax: rrm -i <nombre instance> \n" );
    fprintf(stderr,"        rrm -f <config file> \n" );
}

/*!
*******************************************************************************
\brief programme principale du RRM
*/
int main( int argc , char **argv )
{

    int ii;
    int c           =  0;
    int ret         =  0;
    int flag_cfg    =  0 ;
    struct data_thread DataRrc;
    struct data_thread DataCmm;
    struct data_thread DataPusu;
    struct data_thread DataSensing;

    sock_rrm_int_t  DataIpS[MAX_RRM]; //mod_lor_10_01_25
    pthread_attr_t attr ;

    
    
    /* Vérification des arguments */
    while ((c = getopt(argc,argv,"i:f:h")) != -1)
        switch (c)
        {
            case 'i':
                nb_inst=atoi(optarg);
            break;
            case 'f':
                get_config_file(optarg);
                flag_cfg = 1 ;
            break;
            case 'h':
                help();
                exit(0);
            break;
            default:
                help();
                exit(0);
        }

    if (nb_inst <= 0 )
    {
        fprintf(stderr,"[RRM] Provide a node id\n");
        exit(-1);
    }
    if (nb_inst >= MAX_RRM)
    {
        fprintf(stderr,"[RRM] the instance number (%d) is upper than MAX_RRM (%d)\n", nb_inst, MAX_RRM);
        exit(-1);
    }

#ifdef RRC_KERNEL_MODE
    msg("RRM INIT :open fifos\n");
    while (( Rrm_fifos.rrc_2_rrm_fifo= open ("/dev/rtf14", O_RDONLY )) < 0) 
    {
        printf("[RRM][INIT] open fifo  /dev/rtf14 returned %d\n", Rrm_fifos.rrc_2_rrm_fifo);
        usleep(100);
    }
    printf ("[RRM][INIT] open fifo  /dev/rtf14 returned %d\n", Rrm_fifos.rrc_2_rrm_fifo);
    
    while (( Rrm_fifos.rrm_2_rrc_fifo= open ("/dev/rtf15", O_WRONLY |O_NONBLOCK  | O_NDELAY)) < 0) 
    {//| O_BLOCK
        printf("[RRM][INIT] open fifo  /dev/rtf15 returned %d\n", Rrm_fifos.rrm_2_rrc_fifo);
        usleep(100);
    }
    printf("[RRM][INIT] open fifo  /dev/rtf15 returned %d\n", Rrm_fifos.rrm_2_rrc_fifo);
#endif /* RRC_KERNEL_MODE */

    /* ***** MUTEX ***** */
    // initialise les attributs des threads
    pthread_attr_init( &attr ) ;
    pthread_attr_setschedpolicy( &attr, SCHED_RR ) ;

    DataRrc.name        = "RRC" ;
    DataRrc.sock_path_local = RRM_RRC_SOCK_PATH ;
    DataRrc.sock_path_dest  = RRC_RRM_SOCK_PATH ;
    DataRrc.s.s             = -1 ;

    DataCmm.name        = "CMM" ;
    DataCmm.sock_path_local = RRM_CMM_SOCK_PATH ;
    DataCmm.sock_path_dest  = CMM_RRM_SOCK_PATH ;
    DataCmm.s.s             = -1 ;

    DataPusu.name           = "PUSU" ;
    DataPusu.sock_path_local= RRM_PUSU_SOCK_PATH ;
    DataPusu.sock_path_dest = PUSU_RRM_SOCK_PATH ;
    DataPusu.s.s            = -1 ;
    
    DataSensing.name           = "SENSING" ;
    DataSensing.sock_path_local= RRM_SENSING_SOCK_PATH ;
    DataSensing.sock_path_dest = SENSING_RRM_SOCK_PATH ;
    DataSensing.s.s            = -1 ;    

#ifdef TRACE
    cmm2rrm_fd  = fopen( "VCD/cmm2rrm.txt" , "w") ;
    PNULL(cmm2rrm_fd) ;

    rrc2rrm_fd  = fopen( "VCD/rrc2rrm.txt", "w") ;
    PNULL(rrc2rrm_fd) ;

    pusu2rrm_fd = fopen( "VCD/pusu2rrm.txt", "w") ;
    PNULL(pusu2rrm_fd) ;
    
    ip2rrm_fd = fopen( "VCD/ip2rrm.txt", "w") ;
    PNULL(ip2rrm_fd) ;

    sensing2rrm_fd = fopen( "VCD/sensing2rrm.txt", "w") ;
    PNULL(sensing2rrm_fd) ;    
    
#endif
    output_2 = fopen( "VCD/output_2.txt", "w") ; //mod_lor_10_04_20
    PNULL(output_2) ; //mod_lor_10_04_20

    for ( ii = 0 ; ii < nb_inst ; ii++ ){
        DataIpS[ii].s               = -1 ;    //mod_lor_10_01_25
        if ( !flag_cfg )
        {
            rrm_inst[ii].id                 = ii ;
            rrm_inst[ii].L2_id.L2_id[0]     = ii;
            rrm_inst[ii].L2_id.L2_id[1]     = 0x00;
            rrm_inst[ii].L2_id.L2_id[2]     = 0x00;
            rrm_inst[ii].L2_id.L2_id[3]     = 0xDE;
            rrm_inst[ii].L2_id.L2_id[4]     = 0xAD;
            rrm_inst[ii].L2_id.L2_id[5]     = 0xBE;
            rrm_inst[ii].L2_id.L2_id[6]     = 0xAF;
            rrm_inst[ii].L2_id.L2_id[7]     = 0x00;
        }

        pthread_mutex_init( &( rrm_inst[ii].rrc.exclu ), NULL ) ;
        pthread_mutex_init( &( rrm_inst[ii].cmm.exclu ), NULL ) ;
        pthread_mutex_init( &( rrm_inst[ii].pusu.exclu ), NULL ) ;
        pthread_mutex_init( &( rrm_inst[ii].sensing.exclu ), NULL ) ;
        pthread_mutex_init( &( rrm_inst[ii].ip.exclu ), NULL ) ; //mod_lor_10_01_25

        init_file_msg( &(rrm_inst[ii].file_recv_msg), 1 ) ;
        init_file_msg( &(rrm_inst[ii].file_send_cmm_msg), 2 ) ;
        init_file_msg( &(rrm_inst[ii].file_send_rrc_msg), 3 ) ;
        init_file_msg( &(rrm_inst[ii].file_send_ip_msg), 4 ) ; //mod_lor_10_01_25
        init_file_msg( &(rrm_inst[ii].file_send_sensing_msg), 5 ) ;


        rrm_inst[ii].state              = ISOLATEDNODE ;
        rrm_inst[ii].role               = NOROLE ;
        rrm_inst[ii].cmm.trans_cnt      =  1024;
        rrm_inst[ii].rrc.trans_cnt      =  2048;
        rrm_inst[ii].pusu.trans_cnt     =  3072;
        rrm_inst[ii].ip.trans_cnt       =  4096; //mod_lor_10_01_25
        rrm_inst[ii].sensing.trans_cnt  =  5120; //mod_lor_10_01_25

        rrm_inst[ii].rrc.s              = &DataRrc.s;
        rrm_inst[ii].cmm.s              = &DataCmm.s;
        rrm_inst[ii].pusu.s             = &DataPusu.s;
        rrm_inst[ii].sensing.s          = &DataSensing.s;
        rrm_inst[ii].ip.s               = &DataIpS[ii]; //mod_lor_10_01_25

        rrm_inst[ii].rrc.transaction    = NULL ;
        rrm_inst[ii].cmm.transaction    = NULL ;
        rrm_inst[ii].pusu.transaction   = NULL ;
        rrm_inst[ii].sensing.transaction= NULL ;
        
        rrm_inst[ii].sensing.sens_active= 0;        //mod_lor_10_04_21
        rrm_inst[ii].ip.users_waiting_update=0;     //add_lor_10_11_08
        rrm_inst[ii].rrc.pNeighborEntry = NULL ;
        rrm_inst[ii].rrc.pRbEntry       = NULL ;
        rrm_inst[ii].rrc.pSensEntry     = NULL ;
        rrm_inst[ii].rrc.pChannelsEntry = NULL ;
        
    }
    
   
    
    //open_socket( &DataRrc.s,  DataRrc.sock_path_local, DataRrc.sock_path_dest ,0 );

    /* Creation du thread de reception des messages RRC*/
    fprintf(stderr,"Creation du thread RRC : %d\n", nb_inst);
#ifdef RRC_KERNEL_MODE
    ret = pthread_create ( &pthread_recv_rrc_msg_hnd, NULL, thread_recv_msg_fifo , &DataRrc );
    if (ret)
    {
        fprintf (stderr, "%s", strerror (ret));
        exit(-1) ;
    }
#else
    ret = pthread_create ( &pthread_recv_rrc_msg_hnd, NULL, thread_recv_msg , &DataRrc );
    if (ret)
    {
        fprintf (stderr, "%s", strerror (ret));
        exit(-1) ;
    }
#endif
    /* Creation du thread de reception des messages CMM */
    ret = pthread_create (&pthread_recv_cmm_msg_hnd , NULL, thread_recv_msg, &DataCmm );
    if (ret)
    {
        fprintf (stderr, "%s", strerror (ret));
        exit(-1) ;
    }

    /* Creation du thread CMM d'envoi des messages */
    ret = pthread_create (&pthread_send_cmm_msg_hnd, NULL, thread_send_msg_cmm, NULL );
    if (ret)
    {
        fprintf (stderr, "%s", strerror (ret));
        exit(-1) ;
    }

    /* Creation du thread de reception des messages PUSU */
    ret = pthread_create (&pthread_recv_pusu_msg_hnd , NULL, thread_recv_msg, &DataPusu );
    if (ret)
      {
        fprintf (stderr, "%s", strerror (ret));
        exit(-1) ;
      }

    /* Creation du thread de reception des messages SENSING */
    ret = pthread_create (&pthread_recv_sensing_msg_hnd , NULL, thread_recv_msg, &DataSensing );
    if (ret)
      {
        fprintf (stderr, "%s", strerror (ret));
        exit(-1) ;
      }
    /* Creation du thread SENSING d'envoi des messages */
    ret = pthread_create (&pthread_send_sensing_msg_hnd, NULL, thread_send_msg_sensing, NULL );
    if (ret)
    {
        fprintf (stderr, "%s", strerror (ret));
        exit(-1) ;
    }
 
    /* Creation du thread RRC d'envoi des messages */
    ret = pthread_create (&pthread_send_rrc_msg_hnd, NULL, thread_send_msg_rrc, NULL );
    if (ret)
    {
        fprintf (stderr, "%s", strerror (ret));
        exit(-1) ;
    }
    
    //mod_lor_10_01_25++
    
    /* Creation du thread IP d'envoi des messages */
    ret = pthread_create (&pthread_send_ip_msg_hnd, NULL, thread_send_msg_ip, NULL );
    if (ret)
    {
        fprintf (stderr, "%s", strerror (ret));
        exit(-1) ;
    }
    //mod_lor_10_01_25--*/

    /* Creation du thread TTL */
    ret = pthread_create (&pthread_ttl_hnd , NULL, thread_processing_ttl, NULL);
    if (ret)
    {
        fprintf (stderr, "%s", strerror (ret));
        exit(-1) ;
    }
    //mod_eure_lor++
    if (FC_ID>=0 && SCEN_1){ //mod_lor_10_11_04
         fl_initialize(&argc, argv, "SWN Spectral Measurements", 0, 0);  
         form = create_form_sensing_form(); 
         fl_show_form(form->sensing_form,FL_PLACE_HOTSPOT,FL_FULLBORDER,"Spectral Measurements");     
         fl_check_forms();   
    }
    //mod_eure_lor--  
    
    //mod_lor_10_06_01++
    if (BTS_ID>=0 && SCEN_1){ //mod_lor_10_11_04
         //fl_initialize(&argc, argv, "Secondary Network Frequencies", 0, 0);  
         SN_form = create_form_Secondary_Network_frequencies(); 
         fl_show_form(SN_form->Secondary_Network_frequencies,FL_PLACE_HOTSPOT,FL_FULLBORDER,"Secondary Network Frequencies");     
         fl_check_forms();   
     }
     //mod_lor_10_06_01--
     //mod_lor_10_11_04++
     if (SCEN_2_CENTR && COLL_CLUST){ 
         fl_initialize(&argc, argv, "Cluster 2", 0, 0);  
         Sens_form_CH2 = create_form_sens_scen_2(); 
         fl_show_form(Sens_form_CH2->sens_scen_2,FL_PLACE_HOTSPOT,FL_FULLBORDER,"Cluster 2: Sensing");     
         fl_check_forms();   
     }
     if (SCEN_2_CENTR && !COLL_CLUST){ 
         fl_initialize(&argc, argv, "Cluster 1", 0, 0);  
         Sens_form_CH1 = create_form_sens_CH1_scen_2(); 
         fl_show_form(Sens_form_CH1->sens_CH1_scen_2,FL_PLACE_HOTSPOT,FL_FULLBORDER,"Cluster 1: Sensing");     
         fl_check_forms();   
         
         Chann_form = create_form_all_freq_to_users();
         fl_show_form(Chann_form->all_freq_to_users,FL_PLACE_HOTSPOT,FL_FULLBORDER,"Cluster 1: Used channels");     
         fl_check_forms();
     }
     //mod_lor_10_11_04--
    
    /* main loop */
    rrm_scheduler( ) ;

    /* Attente de la fin des threads. */
    pthread_join (pthread_recv_cmm_msg_hnd, NULL);
    pthread_join (pthread_recv_rrc_msg_hnd, NULL);
    pthread_join (pthread_recv_pusu_msg_hnd, NULL);
    pthread_join (pthread_recv_sensing_msg_hnd, NULL);
    pthread_join (pthread_send_cmm_msg_hnd, NULL);
    pthread_join (pthread_send_rrc_msg_hnd, NULL);
    pthread_join (pthread_send_sensing_msg_hnd, NULL);
    pthread_join (pthread_send_ip_msg_hnd, NULL);
    pthread_join (pthread_ttl_hnd, NULL);
    
 
#ifdef TRACE
    fclose(cmm2rrm_fd ) ;
    fclose(rrc2rrm_fd ) ;
    fclose(pusu2rrm_fd ) ;
    fclose(sensing2rrm_fd ) ;

#endif
    fclose(output_2 ) ; //mod_lor_10_04_20

    return 0 ;
}

