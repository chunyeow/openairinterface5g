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

\file       cmm_msg.c

\brief      Fonctions permettant le formattage des donnees pour l'envoi d'un
            message sur le socket entre le CMM et le RRM 

\author     BURLOT Pascal

\date       16/07/08

   
\par     Historique:
            L.IACOBELLI 2009-10-19
                + new messages

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

#include "L3_rrc_defs.h"
#include "rrm_sock.h"
#include "cmm_rrm_interface.h"
#include "cmm_msg.h"
#include "rrm_util.h"

#ifdef TRACE
//! Macro creant la chaine a partir du nom de la variable
#define STRINGIZER(x) #x
//! Tableau pour le mode trace faisant la translation entre le numero et le nom du message
const char *Str_msg_cmm_rrm[NB_MSG_CMM_RRM] =  { 
    STRINGIZER(CMM_CX_SETUP_REQ       ),
    STRINGIZER(RRM_CX_SETUP_CNF       ),
    STRINGIZER(CMM_CX_MODIFY_REQ      ),
    STRINGIZER(RRM_CX_MODIFY_CNF      ),
    STRINGIZER(CMM_CX_RELEASE_REQ     ),
    STRINGIZER(RRM_CX_RELEASE_CNF     ),
    STRINGIZER(CMM_CX_RELEASE_ALL_REQ ),
    STRINGIZER(RRM_CX_RELEASE_ALL_CNF ),
    STRINGIZER(RRCI_ATTACH_REQ        ),
    STRINGIZER(RRM_ATTACH_IND         ),
    STRINGIZER(CMM_ATTACH_CNF         ),
    STRINGIZER(RRM_MR_ATTACH_IND      ),
    STRINGIZER(ROUTER_IS_CH_IND       ),
    STRINGIZER(RRCI_CH_SYNCH_IND      ),
    STRINGIZER(CMM_INIT_MR_REQ        ),
    STRINGIZER(RRM_MR_SYNCH_IND       ),
    STRINGIZER(RRM_NO_SYNCH_IND       ),
    STRINGIZER(CMM_INIT_CH_REQ        ),
    STRINGIZER(CMM_INIT_SENSING       ),
    STRINGIZER(CMM_STOP_SENSING       ), 
    STRINGIZER(CMM_ASK_FREQ           ), 
    STRINGIZER(CMM_NEED_TO_TX         ),
    STRINGIZER(CMM_INIT_COLL_SENSING  ),//add_lor_10_11_08
    STRINGIZER(CMM_USER_DISC          ),//add_lor_10_11_08
    STRINGIZER(CMM_LINK_DISC          )//,//add_lor_10_11_08
    //STRINGIZER(CMM_INIT_TRANS_REQ     ),
    //STRINGIZER(RRM_INIT_TRANS_CONF    )
};
#endif

/*!
*******************************************************************************
\brief  This function initialize the message header 
\return any return value 
*/
static void init_cmm_msg_head( 
    msg_head_t   *msg_head , //!< message header to initialize 
    Instance_t    inst     , //!< identification de l'instance
    MSG_CMM_RRM_T msg_type , //!< type of message to initialize
    unsigned int  size     , //!< size of message
    Transaction_t Trans_id   //!< transaction id associated to this message
    ) 
{
    if ( msg_head != NULL )
    {
        msg_head->start    = START_MSG ; 
        msg_head->msg_type = 0xFF & msg_type ;
        msg_head->inst     = inst  ;
        msg_head->Trans_id = Trans_id  ;
        msg_head->size     = size ;
    }
}

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        cmm_cx_setup_req().
\return message formate
*/
msg_t * msg_cmm_cx_setup_req(
    Instance_t    inst      , //!< identification de l'instance 
    L2_ID         Src       , //!< L2 source MAC address
    L2_ID         Dst       , //!< L2 destination MAC address
    QOS_CLASS_T   QoS_class , //!< QOS class index
    Transaction_t Trans_id    //!< Transaction ID
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        cmm_cx_setup_req_t *p = RRM_CALLOC(cmm_cx_setup_req_t , 1 ) ;

        if ( p != NULL )
        {
            init_cmm_msg_head(&(msg->head),inst,CMM_CX_SETUP_REQ, sizeof( cmm_cx_setup_req_t) ,Trans_id);

            memcpy( p->Src.L2_id, Src.L2_id, sizeof(L2_ID) )  ;
            memcpy( p->Dst.L2_id, Dst.L2_id, sizeof(L2_ID) )  ;
            p->QoS_class    = QoS_class;
        }
        
        msg->data = (char *) p ;
    }
    return msg ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        rrm_cx_setup_cnf().
\return message formate
*/
msg_t * msg_rrm_cx_setup_cnf(
    Instance_t    inst      , //!< identification de l'instance 
    RB_ID         Rb_id     , //!< L2 Rb_id
    Transaction_t Trans_id    //!< Transaction ID
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        rrm_cx_setup_cnf_t *p = RRM_CALLOC(rrm_cx_setup_cnf_t , 1 ) ;

        if ( p != NULL )
        {
            init_cmm_msg_head(&(msg->head),inst,RRM_CX_SETUP_CNF, sizeof( rrm_cx_setup_cnf_t) ,Trans_id);
            
            p->Rb_id        = Rb_id ;
        }   
        msg->data = (char *) p ;
    }
    return msg ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        cmm_cx_modify_req().
\return message formate
*/
msg_t * msg_cmm_cx_modify_req(
    Instance_t    inst      , //!< identification de l'instance 
    RB_ID         Rb_id     , //!< L2 Rb_id
    QOS_CLASS_T   QoS_class , //!< QOS class index
    Transaction_t Trans_id    //!< Transaction ID
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {  
        cmm_cx_modify_req_t *p = RRM_CALLOC(cmm_cx_modify_req_t, 1 ) ;

        if ( p != NULL )
        {
            init_cmm_msg_head(&(msg->head),inst,CMM_CX_MODIFY_REQ, sizeof( cmm_cx_modify_req_t ) ,Trans_id);
            p->Rb_id        = Rb_id  ;
            p->QoS_class    = QoS_class  ;
        }
        msg->data = (char *) p ;
    }
    return msg ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        rrm_cx_modify_cnf().
\return message formate
*/
msg_t * msg_rrm_cx_modify_cnf(
    Instance_t    inst      , //!< identification de l'instance  
    Transaction_t Trans_id    //!< Transaction ID
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        init_cmm_msg_head(&(msg->head),inst,RRM_CX_MODIFY_CNF, 0 ,Trans_id);            
        msg->data = NULL ;
    }
    return msg ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
         cmm_cx_release_req().
\return message formate
*/
msg_t * msg_cmm_cx_release_req(
    Instance_t    inst      , //!< identification de l'instance  
    RB_ID          Rb_id    , //!< L2 Rb_id
    Transaction_t Trans_id    //!< Transaction ID
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        cmm_cx_release_req_t *p = RRM_CALLOC(cmm_cx_release_req_t , 1) ;

        if ( p != NULL )
        {
            init_cmm_msg_head(&(msg->head),inst,CMM_CX_RELEASE_REQ, sizeof( cmm_cx_release_req_t ) ,Trans_id);
            p->Rb_id        = Rb_id ;
        }
        
        msg->data = (char *) p ;
    }
    return msg ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        rrm_cx_release_cnf().
\return message formate
*/
msg_t * msg_rrm_cx_release_cnf(
    Instance_t    inst      , //!< identification de l'instance  
    Transaction_t Trans_id    //!< Transaction ID
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        init_cmm_msg_head(&(msg->head),inst,RRM_CX_RELEASE_CNF, 0 ,Trans_id);
        msg->data = NULL ;
    }
    return msg ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        cmm_cx_release_all_req().
\return message formate
*/
msg_t * msg_cmm_cx_release_all_req(
    Instance_t    inst      , //!< identification de l'instance 
    L2_ID         L2_id     , //!< L2 Rb_id
    Transaction_t Trans_id    //!< Transaction ID
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        cmm_cx_release_all_req_t *p = RRM_CALLOC(cmm_cx_release_all_req_t , 1 ) ;

        if ( p != NULL )
        {
            init_cmm_msg_head(&(msg->head),inst,CMM_CX_RELEASE_ALL_REQ, sizeof( cmm_cx_release_all_req_t) ,Trans_id);           
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
        }   
        msg->data = (char *) p ;
    }
    return msg ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        rrm_cx_release_all_cnf().
\return message formate
*/
msg_t * msg_rrm_cx_release_all_cnf(
    Instance_t    inst      , //!< identification de l'instance 
    Transaction_t Trans_id    //!< Transaction ID
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        init_cmm_msg_head(&(msg->head),inst,RRM_CX_RELEASE_ALL_CNF, 0 ,Trans_id);
        msg->data = NULL ;
    }
    return msg ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        rrci_attach_req().
\return message formate
*/
msg_t * msg_rrci_attach_req(
    Instance_t     inst      , //!< identification de l'instance 
    L2_ID          L2_id     , //!< Layer 2 (MAC) ID
    L3_INFO_T      L3_info_t , //!< Type of L3 Information
    unsigned char *L3_info   , //!< L3 addressing Information
    RB_ID          DTCH_B_id , //!< RBID of broadcast IP service (MR only)
    RB_ID          DTCH_id   , //!< RBID of default IP service (MR only)
    Transaction_t  Trans_id    //!< Transaction ID
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        rrci_attach_req_t *p = RRM_CALLOC(rrci_attach_req_t , 1 ) ;

        if ( p != NULL )
        {
            init_cmm_msg_head(&(msg->head),inst,RRCI_ATTACH_REQ, sizeof( rrci_attach_req_t) ,Trans_id);
            
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
            p->L3_info_t    = L3_info_t ;           
            if ( L3_info_t != NONE_L3 ) 
                memcpy( p->L3_info, L3_info, L3_info_t );
                    
            p->DTCH_B_id    = DTCH_B_id ;
            p->DTCH_id      = DTCH_id ;
        }   
        msg->data = (char *) p ;
    }
    return msg ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        rrm_attach_ind().
\return message formate
*/
msg_t * msg_rrm_attach_ind(
    Instance_t     inst      , //!< identification de l'instance 
    L2_ID          L2_id     , //!< Layer 2 (MAC) ID
    L3_INFO_T      L3_info_t , //!< Type of L3 Information
    unsigned char *L3_info   , //!< L3 addressing Information
    RB_ID DTCH_id              //!< RBID of default IP service (MR only)
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        rrm_attach_ind_t *p = RRM_CALLOC(rrm_attach_ind_t , 1) ;

        if ( p != NULL )
        {
            init_cmm_msg_head(&(msg->head),inst,RRM_ATTACH_IND, sizeof( rrm_attach_ind_t) ,0);
            
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
            p->L3_info_t    = L3_info_t ;
            
            if ( L3_info_t != NONE_L3 ) 
                memcpy( p->L3_info, L3_info, L3_info_t );
                    
            p->DTCH_id      = DTCH_id ;
        }
        msg->data = (char *) p ;
    }
    return msg ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        cmm_attach_cnf().
\return message formate
*/
msg_t * msg_cmm_attach_cnf(
    Instance_t     inst      , //!< identification de l'instance
    L2_ID          L2_id     , //!< Layer 2 (MAC) ID
    L3_INFO_T      L3_info_t , //!< Type of L3 Information
    unsigned char *L3_info   , //!< L3 addressing Information
    Transaction_t  Trans_id    //!< Transaction ID
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        cmm_attach_cnf_t *p = RRM_CALLOC(cmm_attach_cnf_t, 1 ) ;

        if ( p != NULL )
        {
            init_cmm_msg_head(&(msg->head),inst,CMM_ATTACH_CNF, sizeof( cmm_attach_cnf_t) ,Trans_id);
            
            p->L3_info_t    = L3_info_t ;
            
            if ( L3_info_t != NONE_L3 ) 
                memcpy( p->L3_info, L3_info, L3_info_t );

            memcpy( &p->L2_id, &L2_id, sizeof(L2_ID) );
        }   
        msg->data = (char *) p ;
    }
    return msg ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        rrm_MR_attach_ind().
\return message formate
*/
msg_t * msg_rrm_MR_attach_ind(
    Instance_t     inst      , //!< identification de l'instance 
    L2_ID          L2_id       //!< MR Layer 2 (MAC) ID
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        rrm_MR_attach_ind_t *p = RRM_CALLOC( rrm_MR_attach_ind_t , 1 ) ;

        if ( p != NULL )
        {
            init_cmm_msg_head(&(msg->head),inst,RRM_MR_ATTACH_IND, sizeof( rrm_MR_attach_ind_t) ,0);            
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
        }               
        msg->data = (char *) p ;
    }
    return msg ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        router_is_CH_ind().
\return message formate
*/
msg_t * msg_router_is_CH_ind(
    Instance_t     inst      , //!< identification de l'instance 
    L2_ID          L2_id       //!< CH Layer 2 (MAC) ID
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        router_is_CH_ind_t *p = RRM_CALLOC(router_is_CH_ind_t , 1 ) ;

        if ( p != NULL )
        {
            init_cmm_msg_head(&(msg->head),inst,ROUTER_IS_CH_IND, sizeof( router_is_CH_ind_t) ,0);  
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
        }
        msg->data = (char *) p ;
    }
    return msg ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        rrci_CH_synch_ind().
\return message formate
*/
msg_t * msg_rrci_CH_synch_ind(
    Instance_t     inst        //!< identification de l'instance 
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        init_cmm_msg_head(&(msg->head),inst,RRCI_CH_SYNCH_IND, 0 ,0);           
        msg->data = NULL ;
    }
    return msg ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        cmm_init_mr_req().
\return message formate 
*/
msg_t * msg_cmm_init_mr_req(
    Instance_t     inst        //!< identification de l'instance 
    )
{
    msg_t *msg = RRM_CALLOC( msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        init_cmm_msg_head(&(msg->head),inst,RRCI_CH_SYNCH_IND, 0 ,0);
            
        msg->data = NULL ;
    }
    return msg ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        rrm_MR_synch_ind().
\return message formate
*/
msg_t * msg_rrm_MR_synch_ind(
    Instance_t     inst        //!< identification de l'instance 
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        init_cmm_msg_head(&(msg->head),inst, RRM_MR_SYNCH_IND, 0 ,0);   
        msg->data = NULL ;
    }
    return msg ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        rm_no_synch_ind().
\return message formate
*/
msg_t * msg_rrm_no_synch_ind(
    Instance_t     inst        //!< identification de l'instance 
    )
{
    msg_t *msg = RRM_CALLOC(msg_t ,1 ) ; 
    
    if ( msg != NULL )
    {
        init_cmm_msg_head(&(msg->head),inst,RRM_NO_SYNCH_IND, 0 ,0);            
        msg->data = NULL ;
    }
    return msg  ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        cmm_init_ch_req.
\return message formate
*/
msg_t * msg_cmm_init_ch_req(
    Instance_t     inst      , //!< identification de l'instance 
    L3_INFO_T      L3_info_t , //!< Type of L3 Information
    void          *L3_info     //!< L3 addressing Information
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        cmm_init_ch_req_t *p = RRM_CALLOC(cmm_init_ch_req_t , 1 ) ;

        if ( p != NULL )
        {
            init_cmm_msg_head(&(msg->head),inst, CMM_INIT_CH_REQ, sizeof( cmm_init_ch_req_t) ,0);

            p->L3_info_t    = L3_info_t ;
            if ( L3_info_t != NONE_L3 ) 
                memcpy( p->L3_info, L3_info, L3_info_t );
        }       
        msg->data = (char *) p ;
    }
    return msg ;
}

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        cmm_init_sensing.
\return message formate
*/
msg_t *msg_cmm_init_sensing( //mod_lor_10_03_12: change of parameters
    Instance_t       inst,        //!< identification de l'instance
    unsigned int     Start_fr,
    unsigned int     Stop_fr,
    unsigned int     Meas_band,
    unsigned int     Meas_tpf,
    unsigned int     Nb_channels,
    unsigned int     Overlap,
    unsigned int     Sampl_freq
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        cmm_init_sensing_t *p = RRM_CALLOC(cmm_init_sensing_t , 1 ) ;

        if ( p != NULL )
        {
            init_cmm_msg_head(&(msg->head),inst, CMM_INIT_SENSING, sizeof( cmm_init_sensing_t) ,0);
//mod_lor_10_03_12++
            p->Start_fr     = Start_fr;
            p->Stop_fr      = Stop_fr;
            p->Meas_band    = Meas_band;
            p->Meas_tpf     = Meas_tpf;
            p->Nb_channels  = Nb_channels;
            p->Overlap      = Overlap;
            p->Sampl_freq   = Sampl_freq;
//mod_lor_10_03_12--
        }       
        msg->data = (char *) p ;
    }
    return msg ;
}


/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        cmm_stop_sensing.
\return message formate
*/
msg_t *msg_cmm_stop_sensing( 
    Instance_t inst        //!< identification de l'instance
    )
{
    msg_t *msg = RRM_CALLOC(msg_t ,1 ) ; 
    
    if ( msg != NULL )
    {
        init_cmm_msg_head(&(msg->head),inst,CMM_STOP_SENSING, 0 ,0);            
        msg->data = NULL ;
    }
    return msg  ;
}

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        cmm_ask_freq.
\return message formate
*/
msg_t *msg_cmm_ask_freq( 
    Instance_t inst        //!< identification de l'instance
    )
{
    msg_t *msg = RRM_CALLOC(msg_t ,1 ) ; 
    
    if ( msg != NULL )
    {
        init_cmm_msg_head(&(msg->head),inst,CMM_ASK_FREQ, 0 ,0);            
        msg->data = NULL ;
    }
    return msg  ;
}

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        cmm_need_to_tx. //add: mod_lor_10_10_28
\return message formate
*/
msg_t *msg_cmm_need_to_tx( 
    Instance_t inst        ,//!< identification de l'instance du noeud qui veut transmettre
    Instance_t dest        ,//!< identification de l'instance du noeud destinataire
    QOS_CLASS_T QoS_class   //!< Required quality of service (i.e. number of channels)
    )
{
    msg_t *msg = RRM_CALLOC(msg_t ,1 ) ; 
    if ( msg != NULL )
    {
        cmm_need_to_tx_t *p = RRM_CALLOC(cmm_need_to_tx_t , 1 ) ;
        if ( p != NULL )
        {
            init_cmm_msg_head(&(msg->head),inst, CMM_NEED_TO_TX, sizeof( cmm_need_to_tx_t) ,0);
            p->dest           = dest;
            p->QoS_class      = QoS_class;
        }       
        msg->data = (char *) p ;
    }
    return msg  ;
}

/*! //add_lor_10_11_08
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        cmm_init_coll_sensing.
\return message formate
*/
msg_t *msg_cmm_init_coll_sensing( 
    Instance_t       inst,        //!< identification de l'instance
    unsigned int     Start_fr,
    unsigned int     Stop_fr,
    unsigned int     Meas_band,
    unsigned int     Meas_tpf,
    unsigned int     Nb_channels,
    unsigned int     Overlap,
    unsigned int     Sampl_freq
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ; 
    
    if ( msg != NULL )
    {
        cmm_init_coll_sensing_t *p = RRM_CALLOC(cmm_init_coll_sensing_t , 1 ) ;

        if ( p != NULL )
        {
            init_cmm_msg_head(&(msg->head),inst, CMM_INIT_COLL_SENSING, sizeof( cmm_init_coll_sensing_t) ,0);
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

/*!//add_lor_10_11_09
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        cmm_user_disc. msg received by a secondary user that wants to disconnect
\return message formate
*/
msg_t *msg_cmm_user_disc( 
    Instance_t inst        //!< identification de l'instance
    )
{
    msg_t *msg = RRM_CALLOC(msg_t ,1 ) ; 
    
    if ( msg != NULL )
    {
        init_cmm_msg_head(&(msg->head),inst,CMM_USER_DISC, 0 ,0);            
        msg->data = NULL ;
    }
    return msg  ;
}

/*!//add_lor_10_11_09
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        cmm_linkr_disc. msg received by a secondary user that wants to stop a link
\return message formate
*/
msg_t *msg_cmm_link_disc( 
    Instance_t inst       ,//!< identification de l'instance
    Instance_t dest        //!< identification du destinataire
    )
{
    msg_t *msg = RRM_CALLOC(msg_t ,1 ) ; 
    
    if ( msg != NULL )
    {
        cmm_link_disk_t *p = RRM_CALLOC(cmm_link_disk_t , 1 ) ;

        if ( p != NULL )
        {
            init_cmm_msg_head(&(msg->head),inst, CMM_LINK_DISC, sizeof( cmm_link_disk_t) ,0);
            p->dest = dest;
        }       
        msg->data = (char *) p ;

    }
    return msg  ;
}
