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

\file       ip_msg.c

\brief      Fonctions permettant le formattage des donnees pour l'envoi d'un
            message sur le socket entre le  RRC et le RRM

\author     IACOBELLI Lorenzo

\date       27/04/10


\par     Historique:
        

*******************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/un.h>

#include "L3_rrc_defs.h"
#include "rrm_sock.h"
#include "L3_rrc_interface.h"
#include "ip_msg.h"
#include "rrm_util.h"

#ifdef TRACE
//! Macro creant la chaine a partir du nom de la variable
#define STRINGIZER(x) #x
//! Tableau pour le mode trace faisant la translation entre le numero et le nom du message
const char *Str_msg_ip[NB_MSG_IP] = {
    STRINGIZER(UPDATE_SENS_RESULTS_3    ), 
    STRINGIZER(OPEN_FREQ_QUERY_4        ),
    STRINGIZER(UPDATE_OPEN_FREQ_7       ),
    STRINGIZER(UPDATE_SN_OCC_FREQ_5     ),
    STRINGIZER(INIT_COLL_SENS_REQ       ),
    STRINGIZER(STOP_COLL_SENS           ),
    STRINGIZER(UP_CLUST_SENS_RESULTS    ),
    STRINGIZER(STOP_COLL_SENS_CONF      ),
    STRINGIZER(ASK_FREQ_TO_CH_3         ),
    STRINGIZER(USER_DISCONNECT_9        ),
    STRINGIZER(CLOSE_LINK               )/*,
    STRINGIZER(RRC_CLUST_SCAN_REQ       ),
    STRINGIZER(RRM_CLUST_SCAN_CONF      ),
    STRINGIZER(RRM_CLUST_MON_REQ        ),
    STRINGIZER(RRC_CLUST_MON_REQ        ),
    STRINGIZER(RRM_CLUST_MON_CONF       ),
    STRINGIZER(RRM_END_SCAN_CONF        ),
    STRINGIZER(RRM_INIT_CONN_REQ        ),
    STRINGIZER(RRC_INIT_CONN_CONF       ),
    STRINGIZER(RRM_FREQ_ALL_PROP        ),
    STRINGIZER(RRC_FREQ_ALL_PROP_CONF   ),
    STRINGIZER(RRM_REP_FREQ_ALL         ),
    STRINGIZER(RRC_REP_FREQ_ACK         ),
    STRINGIZER(RRC_INIT_CONN_REQ        ),
    STRINGIZER(RRM_CONN_SET             ),
    STRINGIZER(RRC_FREQ_ALL_PROP        ),
    STRINGIZER(RRM_FREQ_ALL_PROP_CONF   ),
    STRINGIZER(RRC_REP_FREQ_ALL         ),
    STRINGIZER(RRM_REP_FREQ_ACK         )  */

} ;
#endif


/*!
*******************************************************************************
\brief  This function initialize the message header 
\return any return value 
*/
static void init_ip_msg_head( 
    msg_head_t    *msg_head , //!< message header to initialize
    Instance_t     inst     , //!< Instance ID
    MSG_IP_T  msg_type , //!< type of message to initialize
    unsigned int   size     , //!< size of message
    Transaction_t  Trans_id   //!< transaction id associated to this message
    ) 
{
    if ( msg_head != NULL )
    {
        msg_head->start    = START_MSG ; 
        msg_head->msg_type = 0xFF & msg_type ;
        msg_head->inst     = inst  ;
        msg_head->Trans_id = Trans_id  ;
        msg_head->size     = size;
    }
}


/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        update_sens_results().
\return message formate
*/

msg_t *msg_update_sens_results_3( 
    Instance_t inst, 
    L2_ID L2_id,                //!< FC L2_id
    unsigned int NB_info,
    Sens_ch_t *Sens_meas, 
    Transaction_t Trans_id
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
        
    if ( msg != NULL )
    {
        unsigned int size = sizeof( rrm_update_sens_t );// + (NB_info-1) * sizeof(Sens_ch_t) ; //mod_lor_10_04_23
        
        rrm_update_sens_t *p = RRM_CALLOC2(rrm_update_sens_t , size ) ;

        if ( p != NULL )
        {
            //fprintf(stderr,"rrmUSR 1 \n");//dbg
            init_ip_msg_head(&(msg->head),inst, UPDATE_SENS_RESULTS_3, size ,Trans_id);

            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
        
            p->NB_info       = NB_info    ;
            p->info_time     = 0 ;
            
            if ( NB_info > 0 )
            {
                memcpy( p->Sens_meas , Sens_meas, NB_info * sizeof(Sens_ch_t) )  ;
            }
            //fprintf(stdout,"msg_rrm_update_sens()2 : NB_chan %d\n", p->NB_info);//dbg
            //fprintf(stdout,"NB_chan %d\n", p->NB_info);
              //  for (int i=0; i<NB_info; i++)
               // Sens_ch_t *ch = p->Sens_meas; ch!=NULL; ch=ch->next)
            //fprintf(stdout,"channel in msg arr: %d\n", ch->Ch_id); //dbg
            
        }
        //fprintf(stderr,"rrmUSR end \n");//dbg
        msg->data = (char *) p ;
    }
    
    return msg ;

}

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        open_freq_query_4().
\return message formate
*/
msg_t *msg_open_freq_query_4( 
    Instance_t    inst, 
    L2_ID         L2_id           ,
    QOS_CLASS_T   QoS             ,
    Transaction_t Trans_id 
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ;

    if ( msg != NULL )
    {
        open_freq_query_t *p = RRM_CALLOC(open_freq_query_t , 1 ) ;

        if ( p != NULL )
        {
            init_ip_msg_head(&(msg->head),inst,OPEN_FREQ_QUERY_4, sizeof( open_freq_query_t ) ,Trans_id);
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
            p->QoS = QoS;
        }
        msg->data = (char *) p ;
    }
    return msg ;
}

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        ask_freq_to_CH_3().
\return message formate
*/
msg_t *msg_ask_freq_to_CH_3( 
    Instance_t    inst, 
    L2_ID         L2_id           ,
    L2_ID         L2_id_dest      ,
    QOS_CLASS_T   QoS             ,
    Transaction_t Trans_id 
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ;

    if ( msg != NULL )
    {
        ask_freq_to_CH_t *p = RRM_CALLOC(ask_freq_to_CH_t , 1 ) ;

        if ( p != NULL )
        {
            init_ip_msg_head(&(msg->head),inst,ASK_FREQ_TO_CH_3, sizeof( ask_freq_to_CH_t ) ,Trans_id);
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
            memcpy( p->L2_id_dest.L2_id, L2_id_dest.L2_id, sizeof(L2_ID) )  ;
            p->QoS = QoS;
        }
        msg->data = (char *) p ;
    }
    return msg ;
}

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        rrm_open_freq().
\return message formate
*/
msg_t *msg_update_open_freq_7( 
    Instance_t    inst, 
    L2_ID         L2_id           ,
    unsigned int NB_ch,
    CHANNELS_DB_T *channels, //mod_lor_10_05_17
    Transaction_t Trans_id 
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ;

    if ( msg != NULL )
    {
        unsigned int size = sizeof( update_open_freq_t );// + (NB_free_ch-1) * sizeof(CHANNEL_T) ;//mod_lor_10_04_23
        
        update_open_freq_t *p = RRM_CALLOC2(update_open_freq_t , size ) ;

        if ( p != NULL )
        {
            init_ip_msg_head(&(msg->head),inst,UPDATE_OPEN_FREQ_7, size ,Trans_id);
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
            p->NB_chan       = NB_ch    ;
            
            if ( NB_ch > 0 )
            {
                memcpy( p->channels , channels, NB_ch * sizeof(CHANNELS_DB_T) )  ;
            }
        }
        msg->data = (char *) p ;
    }
    return msg ;
}

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        rrm_update_SN_freq().
\return message formate
*/

msg_t *msg_update_SN_occ_freq_5(
        Instance_t inst             , //!< instance ID 
        L2_ID L2_id                 , //!< Layer 2 (MAC) ID of FC
        unsigned int NB_chan        ,
        unsigned int *occ_channels      , 
        Transaction_t Trans_id        //!< Transaction ID
        )
            
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ;
    //fprintf(stdout,"rrc_end_scan_ord() cp1\n"); //dbg

    if ( msg != NULL )
    {
        unsigned int size = sizeof( update_SN_occ_freq_t );// + (NB_chan-1) * sizeof(unsigned int) ;//mod_lor_10_04_23
        update_SN_occ_freq_t *p = RRM_CALLOC2(update_SN_occ_freq_t , size ) ;

        if ( p != NULL )
        {
        
            
            init_ip_msg_head(&(msg->head),inst,UPDATE_SN_OCC_FREQ_5, size,Trans_id);
            
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
            
        
            p->NB_chan = NB_chan;
            if ( NB_chan != 0 ){
                
                memcpy( p->occ_channels, occ_channels, NB_chan*sizeof(unsigned int) );
                
            }
  
       
        }
        msg->data = (char *) p ;
        
    }
    return msg ;
}

//mod_lor_10_05_05++
/*!
*******************************************************************************
\brief  Message to be sent to a CH that will collaborate in sensing operation.
\return message formate
*/
msg_t *msg_init_coll_sens_req( 
    Instance_t       inst,        //!< instance ID of asking CH
    L2_ID L2_id                 , //!< Layer 2 (MAC) ID of asking CH
    unsigned int     Start_fr,
    unsigned int     Stop_fr,
    unsigned int     Meas_band,
    unsigned int     Meas_tpf,
    unsigned int     Nb_channels,
    unsigned int     Overlap,
    unsigned int     Sampl_freq,
    Transaction_t    Trans_id        //!< Transaction ID
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        init_coll_sens_req_t *p = RRM_CALLOC(init_coll_sens_req_t , 1 ) ;

        if ( p != NULL )
        {
            init_ip_msg_head(&(msg->head),inst, INIT_COLL_SENS_REQ, sizeof( init_coll_sens_req_t) ,0);
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
            p->Start_fr     = Start_fr;
            p->Stop_fr      = Stop_fr;
            p->Meas_band    = Meas_band;
            p->Meas_tpf     = Meas_tpf;
            p->Nb_channels  = Nb_channels;
            p->Overlap      = Overlap;
            p->Sampl_freq   = Sampl_freq;

        }       
        msg->data = (char *) p ;
    }
    return msg ;
}

//mod_lor_10_05_05--
//mod_lor_10_05_06++
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        cmm_stop_sensing.
\return message formate
*/
msg_t *msg_stop_coll_sens( 
    Instance_t inst        //!< identification de l'instance
    )
{
    msg_t *msg = RRM_CALLOC(msg_t ,1 ) ; 
    
    if ( msg != NULL )
    {
        init_ip_msg_head(&(msg->head),inst,STOP_COLL_SENS, 0 ,0);            
        msg->data = NULL ;
    }
    return msg  ;
}
//mod_lor_10_05_06--
//mod_lor_10_05_12++
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        cmm_stop_sensing.
\return message formate
*/
msg_t *msg_stop_coll_sens_conf( 
    Instance_t inst,        //!< identification de l'instance
    L2_ID L2_id
    )
{
    msg_t *msg = RRM_CALLOC(msg_t ,1 ) ; 
    
    if ( msg != NULL )
    {
        stop_coll_sens_conf_t *p = RRM_CALLOC(stop_coll_sens_conf_t , 1 ) ;

        if ( p != NULL )
        {
            init_ip_msg_head(&(msg->head),inst,STOP_COLL_SENS_CONF, sizeof( stop_coll_sens_conf_t) ,0);            
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
        }
        msg->data = (char *) p ;
    }
    return msg  ;
}
//mod_lor_10_05_12--

//mod_lor_10_05_07++
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        up_clust_sens_results().
\return message formate
*/

msg_t *msg_up_clust_sens_results( 
    Instance_t inst, 
    L2_ID L2_id,                //!< FC L2_id
    unsigned int NB_info,
    unsigned int info_value,
    Sens_ch_t *Sens_meas, 
    Transaction_t Trans_id
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
        
    if ( msg != NULL )
    {
        unsigned int size = sizeof( update_coll_sens_t );// + (NB_info-1) * sizeof(Sens_ch_t) ; //mod_lor_10_04_23
        
        update_coll_sens_t *p = RRM_CALLOC2(update_coll_sens_t , size ) ;

        if ( p != NULL )
        {
            init_ip_msg_head(&(msg->head),inst, UP_CLUST_SENS_RESULTS, size ,Trans_id);

            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
        
            p->NB_info       = NB_info    ;
            p->info_time     = 0 ;
            p->info_value    = info_value;
            
            if ( NB_info > 0 )
            {
                memcpy( p->Sens_meas , Sens_meas, NB_info * sizeof(Sens_ch_t) )  ;
            }
        }
        msg->data = (char *) p ;
    }
    
    return msg ;

}

//mod_lor_10_05_07--

/*!add_lor_10_11_09
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        user_disconnect.
\return message formate
*/
msg_t *msg_user_disconnect_9( 
    Instance_t inst,        //!< identification de l'instance
    L2_ID L2_id,            //!< L2_id of the user that wants to disconnect
    Transaction_t Trans_id
    )
{
    msg_t *msg = RRM_CALLOC(msg_t ,1 ) ; 
    
    if ( msg != NULL )
    {
        user_disconnect_t *p = RRM_CALLOC(user_disconnect_t , 1 ) ;

        if ( p != NULL )
        {
            init_ip_msg_head(&(msg->head),inst,USER_DISCONNECT_9, sizeof( user_disconnect_t) ,Trans_id);            
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
        }
        msg->data = (char *) p ;
    }  
    return msg  ;

}

/*!add_lor_10_11_09
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        user_disconnect.
\return message formate
*/
msg_t *msg_close_link( 
    Instance_t inst,        //!< identification de l'instance
    L2_ID L2_id,            //!< L2_id of the user that wants to stop the link
    L2_ID L2_id_dest,       //!< L2_id of the destination
    Transaction_t Trans_id
    )
{
    msg_t *msg = RRM_CALLOC(msg_t ,1 ) ; 
    
    if ( msg != NULL )
    {
        close_link_t *p = RRM_CALLOC(close_link_t , 1 ) ;

        if ( p != NULL )
        {
            init_ip_msg_head(&(msg->head),inst,CLOSE_LINK, sizeof( close_link_t) ,Trans_id);            
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
            memcpy( p->L2_id_dest.L2_id, L2_id_dest.L2_id, sizeof(L2_ID) )  ;
        }
        msg->data = (char *) p ;
    }  
    return msg  ;

}
