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

\file       rrm_2_rrc_msg.c

\brief      Fonctions permettant le formattage des donnees pour l'envoi d'un
            message sur le socket entre le  RRC et le RRM

\author     BURLOT Pascal

\date       16/07/08


\par     Historique:
        P.BURLOT 2009-01-20 
            + L2_id: ajout de memcpy() dans msg_rrci_cx_establish_resp()

        L. IACOBELLI 2010-01-05
            + new messages for sensing
            + messages sent via ip

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
#include "rrc_rrm_msg.h"
#include "rrm_util.h"

#ifdef TRACE
//! Macro creant la chaine a partir du nom de la variable
#define STRINGIZER(x) #x
//! Tableau pour le mode trace faisant la translation entre le numero et le nom du message
const char *Str_msg_rrc_rrm[NB_MSG_RRC_RRM] = {
    STRINGIZER(RRM_RB_ESTABLISH_REQ     ),
    STRINGIZER(RRC_RB_ESTABLISH_RESP    ),
    STRINGIZER(RRC_RB_ESTABLISH_CFM     ),
    STRINGIZER(RRM_RB_MODIFY_REQ        ),
    STRINGIZER(RRC_RB_MODIFY_RESP       ),
    STRINGIZER(RRC_RB_MODIFY_CFM        ),
    STRINGIZER(RRM_RB_RELEASE_REQ       ),
    STRINGIZER(RRC_RB_RELEASE_RESP      ),
    STRINGIZER(RRC_MR_ATTACH_IND        ),
    STRINGIZER(RRM_SENSING_MEAS_REQ     ),
    STRINGIZER(RRC_SENSING_MEAS_RESP    ),
    STRINGIZER(RRC_CX_ESTABLISH_IND     ),
    STRINGIZER(RRC_PHY_SYNCH_TO_MR_IND  ),
    STRINGIZER(RRC_PHY_SYNCH_TO_CH_IND  ),
    STRINGIZER(RRCI_CX_ESTABLISH_RESP   ),
    STRINGIZER(RRC_SENSING_MEAS_IND     ),
    STRINGIZER(RRM_SENSING_MEAS_RESP    ),
    STRINGIZER(RRC_RB_MEAS_IND          ),
    STRINGIZER(RRM_RB_MEAS_RESP         ),
    STRINGIZER(RRM_INIT_CH_REQ          ),
    STRINGIZER(RRM_INIT_MR_REQ          ),
    STRINGIZER(RRM_INIT_MON_REQ         ),
    STRINGIZER(RRM_INIT_SCAN_REQ        ),
    STRINGIZER(RRC_INIT_SCAN_REQ        ),
    STRINGIZER(UPDATE_SENS_RESULTS_3    ), //mod_lor_10_01_25
    STRINGIZER(RRM_END_SCAN_REQ         ),
    STRINGIZER(RRC_END_SCAN_REQ         ),
    STRINGIZER(RRC_END_SCAN_CONF        ),
    STRINGIZER(RRC_INIT_MON_REQ         ),
    STRINGIZER(OPEN_FREQ_QUERY_4        ),
    STRINGIZER(UPDATE_OPEN_FREQ_7       ),
    STRINGIZER(UPDATE_SN_OCC_FREQ_5     ),
    STRINGIZER(RRM_UP_FREQ_ASS          ),
    STRINGIZER(RRM_END_SCAN_CONF        ),
    STRINGIZER(RRC_UP_FREQ_ASS          ),//add_lor_10_11_05
    STRINGIZER(RRM_UP_FREQ_ASS_SEC      ),//add_lor_10_11_05
    STRINGIZER(RRC_UP_FREQ_ASS_SEC      )/*,//add_lor_10_11_05
    STRINGIZER(RRM_OPEN_FREQ            ),
    STRINGIZER(RRM_UPDATE_SN_FREQ       ),
    STRINGIZER(RRC_UPDATE_SN_FREQ       ),
    STRINGIZER(RRM_CLUST_SCAN_REQ       ),
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
static void init_rrc_msg_head(
    msg_head_t    *msg_head , //!< message header to initialize
    Instance_t     inst     , //!< instance ID
    MSG_RRC_RRM_T  msg_type , //!< type of message to initialize
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
        rrm_rb_establish_req().
\return message formate
*/
msg_t *msg_rrm_rb_establish_req(
    Instance_t               inst              , //!< instance ID
    const LCHAN_DESC        *Lchan_desc        , //!< Logical Channel Descriptor Array
    const MAC_RLC_MEAS_DESC *Mac_rlc_meas_desc , //!< MAC/RLC Measurement descriptors for RB
    L2_ID                   *L2_id             , //!< Layer 2 (MAC) IDs for link
    Transaction_t            Trans_id          , //!< Transaction ID
    unsigned char           *L3_info           , //!< Optional L3 Information
    L3_INFO_T                L3_info_t           //!< Type of L3 Information
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ;

    if ( msg != NULL )
    {
        rrm_rb_establish_req_t *p = RRM_CALLOC(rrm_rb_establish_req_t ,1 ) ;

        if ( p != NULL )
        {
            init_rrc_msg_head(&(msg->head),inst,RRM_RB_ESTABLISH_REQ, sizeof( rrm_rb_establish_req_t) ,Trans_id);

            memcpy(&(p->Lchan_desc), Lchan_desc, sizeof(LCHAN_DESC) );
            memcpy(&(p->Mac_rlc_meas_desc), Mac_rlc_meas_desc, sizeof(MAC_RLC_MEAS_DESC) );
            memcpy(&(p->L2_id[0]), L2_id, 2*sizeof(L2_ID) ); // SRC+DST
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
        rrm_rb_modify_req().
\return message formate
*/
msg_t *msg_rrm_rb_modify_req(
    Instance_t               inst          , //!< instance ID
	const LCHAN_DESC        *Lchan_desc    , //!< Logical Channel Descriptor Array
	const MAC_RLC_MEAS_DESC *Mac_meas_desc , //!< MAC/RLC Measurement descriptors for RB
	RB_ID                    Rb_id         , //!< Radio Bearer ID
	Transaction_t            Trans_id        //!< Transaction ID
	)
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ;

    if ( msg != NULL )
    {
        rrm_rb_modify_req_t *p = RRM_CALLOC(rrm_rb_modify_req_t , 1) ;

        if ( p != NULL )
        {
            init_rrc_msg_head(&(msg->head),inst,RRM_RB_MODIFY_REQ, sizeof( rrm_rb_modify_req_t) ,Trans_id);

            memcpy(&(p->Lchan_desc), Lchan_desc, sizeof(LCHAN_DESC) );
            memcpy(&(p->Mac_meas_desc), Mac_meas_desc, sizeof(MAC_RLC_MEAS_DESC) );

            p->Rb_id    = Rb_id ;
        }
        msg->data = (char *) p ;
    }
    return msg ;
}

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction
        rrm_rb_release_req().
\return message formate
*/
msg_t *msg_rrm_rb_release_req(
    Instance_t    inst     , //!< instance ID
	RB_ID         Rb_id    , //!< Radio Bearer ID
	Transaction_t Trans_id   //!< Transaction ID
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ;

    if ( msg != NULL )
    {
        rrm_rb_release_req_t *p = RRM_CALLOC(rrm_rb_release_req_t , 1 ) ;

        if ( p != NULL )
        {
            init_rrc_msg_head(&(msg->head),inst,RRM_RB_RELEASE_REQ, sizeof( rrm_rb_release_req_t) ,Trans_id);

            p->Rb_id    = Rb_id ;
        }
        msg->data = (char *) p ;
    }
    return msg ;
}

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction
        rrm_sensing_meas_req().
\return message formate
*/
msg_t *msg_rrm_sensing_meas_req(
    Instance_t          inst              , //!< instance ID
    L2_ID               L2_id             , //!< Layer 2 (MAC) ID
    SENSING_MEAS_DESC   Sensing_meas_desc , //!< Sensing Measurement Descriptor
    Transaction_t       Trans_id            //!< Transaction ID
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ;

    if ( msg != NULL )
    {
        rrm_sensing_meas_req_t *p = RRM_CALLOC(rrm_sensing_meas_req_t , 1 ) ;

        if ( p != NULL )
        {
            init_rrc_msg_head(&(msg->head),inst,RRM_SENSING_MEAS_REQ, sizeof( rrm_sensing_meas_req_t) ,Trans_id);

            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
            memcpy(&(p->Sensing_meas_desc), &Sensing_meas_desc, sizeof(SENSING_MEAS_DESC)) ;
        }
        msg->data = (char *) p ;
    }
    return msg ;

}

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction
          rrci_cx_establish_resp().
\return message formate
*/
msg_t * msg_rrci_cx_establish_resp(
    Instance_t     inst      , //!< instance ID
	Transaction_t  Trans_id  , //!< Transaction ID
	L2_ID          L2_id     , //!< Layer 2 (MAC) ID
	unsigned char *L3_info   , //!< Optional L3 Information
	L3_INFO_T      L3_info_t   //!< Type of L3 Information
        )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ;

    if ( msg != NULL )
    {
        rrci_cx_establish_resp_t *p = RRM_CALLOC(rrci_cx_establish_resp_t , 1 ) ;

        if ( p != NULL )
        {
            init_rrc_msg_head(&(msg->head),inst,RRCI_CX_ESTABLISH_RESP, sizeof( rrci_cx_establish_resp_t ) ,Trans_id);

            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
            p->L3_info_t    = L3_info_t     ;
            if ( L3_info_t != NONE_L3 )
                memcpy( p->L3_info, L3_info, L3_info_t );
        }
        msg->data = (char *) p ;
    }
    return msg ;
}
/*!
*******************************************************************************
\brief  La fonction formate en un message generique de reponse pour les
          fonctions :
            - msg_rrm_sensing_meas_resp(),  msg_rrm_rb_meas_resp().
\return message formate
*/
static msg_t *msg_rrm_generic_resp(
    Instance_t    inst     , //!< instance ID
    MSG_RRC_RRM_T msg_type , //!< type of message
    Transaction_t Trans_id   //!< Transaction ID
        )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ;

    if ( msg != NULL )
    {
        init_rrc_msg_head(&(msg->head),inst,msg_type, 0, Trans_id);
        msg->data = NULL ;
    }
    return msg ;
}

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction
        rrm_sensing_meas_resp().
\return message formate
*/
msg_t * msg_rrm_sensing_meas_resp(
    Instance_t    inst     , //!< instance ID
    Transaction_t Trans_id   //!< Transaction ID
    )
{
    return msg_rrm_generic_resp( inst,RRM_SENSING_MEAS_RESP, Trans_id) ;
}

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction
        rrm_rb_meas_resp().
\return message formate
*/
msg_t * msg_rrm_rb_meas_resp(
    Instance_t    inst     , //!< instance ID
    Transaction_t Trans_id   //!< Transaction ID
    )
{
    return msg_rrm_generic_resp( inst,RRM_RB_MEAS_RESP, Trans_id) ;
}

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction
        rrm_init_ch_req().
\return message formate
*/
msg_t *msg_rrm_init_ch_req(
    Instance_t        inst            , //!< instance ID
    Transaction_t     Trans_id        , //!< Transaction ID
    const LCHAN_DESC *Lchan_desc_srb0 , //!< Logical Channel Descriptor - SRB0
    const LCHAN_DESC *Lchan_desc_srb1 , //!< Logical Channel Descriptor - SRB1
    L2_ID             L2_id
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ;

    if ( msg != NULL )
    {
        rrm_init_ch_req_t *p = RRM_CALLOC(rrm_init_ch_req_t , 1 ) ;

        if ( p != NULL )
        {
            init_rrc_msg_head(&(msg->head),inst,RRM_INIT_CH_REQ, sizeof( rrm_init_ch_req_t ) ,Trans_id);

            memcpy(&(p->Lchan_desc_srb0), Lchan_desc_srb0, sizeof(LCHAN_DESC) );
            memcpy(&(p->Lchan_desc_srb1), Lchan_desc_srb1, sizeof(LCHAN_DESC) );
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
        }
        msg->data = (char *) p ;
    }
    return msg ;

}

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction
        rrm_init_mr_req().
\return message formate
*/

msg_t *msg_rrci_init_mr_req(
    Instance_t        inst            , //!< instance ID
    Transaction_t     Trans_id        , //!< Transaction ID
    const LCHAN_DESC *Lchan_desc_srb0 , //!< Logical Channel Descriptor - SRB0
    const LCHAN_DESC *Lchan_desc_srb1 , //!< Logical Channel Descriptor - SRB1
    unsigned char     CH_index          //!< index to identify the CH
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ;

    if ( msg != NULL )
    {
        rrci_init_mr_req_t *p = RRM_CALLOC(rrci_init_mr_req_t , 1 ) ;

        if ( p != NULL )
        {
            init_rrc_msg_head(&(msg->head),inst,RRCI_INIT_MR_REQ, sizeof( rrci_init_mr_req_t ) ,Trans_id);

            memcpy(&(p->Lchan_desc_srb0), Lchan_desc_srb0, sizeof(LCHAN_DESC) );
            memcpy(&(p->Lchan_desc_srb1), Lchan_desc_srb1, sizeof(LCHAN_DESC) );
            p->CH_index = CH_index ;
        }
        msg->data = (char *) p ;
    }
    return msg ;
}

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction
        rrm_init_mon_req().
\return message formate
*/

msg_t *msg_rrm_init_mon_req(
    Instance_t        inst            , //!< instance ID
    L2_ID             L2_id           ,
    unsigned int      NB_chan         , 
    unsigned int      interval        ,
    unsigned int      *ch_to_scan     ,
    Transaction_t     Trans_id          //!< Transaction ID
    
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ;

    if ( msg != NULL )
    {
        unsigned int size = sizeof( rrm_init_mon_req_t ) + (NB_chan-1) * sizeof(unsigned int) ;

        rrm_init_mon_req_t *p = RRM_CALLOC2(rrm_init_mon_req_t , size ) ;

        if ( p != NULL )
        {
            
            //for (int i=0; i<NB_chan; i++)
              //  fprintf(stdout,"msg_rrm_init_mon_req(), chan: %d\n", ch_to_scan[i]); //dbg
            init_rrc_msg_head(&(msg->head),inst,RRM_INIT_MON_REQ, size ,Trans_id);
            
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
            p->NB_chan = NB_chan;
            p->interval = interval;
            
            if ( NB_chan != 0 )
                memcpy( p->ch_to_scan, ch_to_scan, NB_chan*sizeof(unsigned int) );
                
           
        }
        msg->data = (char *) p ;
    }
    return msg ;
}

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction
        rrm_init_scan_req().
\return message formate
*/

msg_t *msg_rrm_init_scan_req(
    Instance_t       inst            , //!< instance ID
    unsigned int     Start_fr,
    unsigned int     Stop_fr,
    unsigned int     Meas_band,
    unsigned int     Meas_tpf,
    unsigned int     Nb_channels,
    unsigned int     Overlap,
    unsigned int     Sampl_freq,
    Transaction_t    Trans_id          //!< Transaction ID
    
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ;

    if ( msg != NULL )
    {
        rrm_init_scan_req_t *p = RRM_CALLOC(rrm_init_scan_req_t , 1 ) ;

        if ( p != NULL )
        {
            init_rrc_msg_head(&(msg->head),inst,RRM_INIT_SCAN_REQ, sizeof( rrm_init_scan_req_t ) ,Trans_id);

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
        rrm_end_scan_req().
\return message formate
*/

msg_t *msg_rrm_end_scan_req( 
    Instance_t    inst, 
    L2_ID         L2_id           ,
    Transaction_t Trans_id 
    )
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ;

    if ( msg != NULL )
    {
        rrm_end_scan_req_t *p = RRM_CALLOC(rrm_end_scan_req_t , 1 ) ;

        if ( p != NULL )
        {
            init_rrc_msg_head(&(msg->head),inst,RRM_END_SCAN_REQ, sizeof( rrm_end_scan_req_t ) ,Trans_id);
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
        }
        msg->data = (char *) p ;
    }
    return msg ;
}

/*!//add_lor_10_11_05
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        rrm_up_freq_ass_sec().
\return message formate
*/

msg_t *msg_rrm_up_freq_ass_sec(
        Instance_t inst             , //!< instance ID 
        L2_ID *L2_id                , //!< Layer 2 (MAC) ID vector of SUs starting tx
        L2_ID *L2_id_dest           , //!< Layer 2 (MAC) ID vector of SUs receiving tx
        unsigned int NB_all         , //!< Number of allocated links
        CHANNEL_T *ass_channels       //!< Vector of allocated links
        )
            
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ;
    int i;

    if ( msg != NULL )
    {
        unsigned int size = sizeof(  rrm_up_freq_ass_sec_t );
                //printf ("size rrm_up_freq_ass_t %d\n", size);//dbg
        rrm_up_freq_ass_sec_t *p = RRM_CALLOC2( rrm_up_freq_ass_sec_t , size ) ;


        if ( p != NULL )
        {
        
            
            init_rrc_msg_head(&(msg->head),inst,RRM_UP_FREQ_ASS_SEC, size,0);
            p->NB_all = NB_all;
            for (i=0;i<NB_all;i++){
                memcpy( p->L2_id[i].L2_id, L2_id[i].L2_id, sizeof(L2_ID) )  ;
                memcpy( p->L2_id_dest[i].L2_id, L2_id_dest[i].L2_id, sizeof(L2_ID) )  ;
            }
            memcpy( p->ass_channels, ass_channels, NB_all*sizeof(CHANNEL_T) );

        }
        msg->data = (char *) p ;
        
    }
    return msg ;
}


/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        rrm_up_freq_ass().
\return message formate
*/

msg_t *msg_rrm_up_freq_ass(
        Instance_t inst             , //!< instance ID 
        L2_ID L2_id                 , //!< Layer 2 (MAC) ID of SU
        unsigned int NB_chan        , 
        CHANNEL_T *ass_channels
        )
            
{
    msg_t *msg = RRM_CALLOC(msg_t , 1 ) ;

    if ( msg != NULL )
    {
        unsigned int size = sizeof(  rrm_up_freq_ass_t );// + (NB_chan-1) * sizeof(CHANNEL_T) ;//mod_lor_10_04_23
        rrm_up_freq_ass_t *p = RRM_CALLOC2( rrm_up_freq_ass_t , size ) ;


        if ( p != NULL )
        {
        
            
            init_rrc_msg_head(&(msg->head),inst,RRM_UP_FREQ_ASS, size,0);
            p->NB_chan = NB_chan;
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
            memcpy( p->ass_channels, ass_channels, NB_chan*sizeof(CHANNEL_T) );

        }
        msg->data = (char *) p ;
        
    }
    return msg ;
}


///! MESSAGES SENT VIA IP
//mod_lor_10_04_27++
/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        update_sens_results_3().
\return message formate
*/
/*msg_t *msg_update_sens_results_3( 
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
            init_rrc_msg_head(&(msg->head),inst, UPDATE_SENS_RESULTS_3, size ,Trans_id);

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

}*/

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        open_freq_query_4().
\return message formate
*//*
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
            init_rrc_msg_head(&(msg->head),inst,OPEN_FREQ_QUERY_4, sizeof( open_freq_query_t ) ,Trans_id);
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
            p->QoS = QoS;
        }
        msg->data = (char *) p ;
    }
    return msg ;
}*/

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        rrm_open_freq().
\return message formate
*//*
msg_t *msg_update_open_freq_7( 
    Instance_t    inst, 
    L2_ID         L2_id           ,
    unsigned int NB_free_ch,
    CHANNEL_T *fr_channels,
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
            init_rrc_msg_head(&(msg->head),inst,UPDATE_OPEN_FREQ_7, size ,Trans_id);
            memcpy( p->L2_id.L2_id, L2_id.L2_id, sizeof(L2_ID) )  ;
            p->NB_chan       = NB_free_ch    ;
            
            if ( NB_free_ch > 0 )
            {
                memcpy( p->fr_channels , fr_channels, NB_free_ch * sizeof(CHANNEL_T) )  ;
            }
        }
        msg->data = (char *) p ;
    }
    return msg ;
}*/

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction 
        rrm_update_SN_freq().
\return message formate
*/
/*
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
        
            
            init_rrc_msg_head(&(msg->head),inst,UPDATE_SN_OCC_FREQ_5, size,Trans_id);
            
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
//mod_lor_10_04_27--*/

/*!
*******************************************************************************
\brief  La fonction formate en un message les parametres de la fonction
        sns_end_scan_conf(). It sends confirmation of sensing interruprion to 
        the RRC that will informe the fusion center about it 
\return message formate
*/
msg_t * msg_rrm_end_scan_conf(
    Instance_t    inst     , //!< instance ID
    Transaction_t Trans_id   //!< Transaction ID
    )
{
    return msg_rrm_generic_resp( inst,RRM_END_SCAN_CONF, Trans_id) ;
}
