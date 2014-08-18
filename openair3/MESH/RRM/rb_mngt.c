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

\file       rb_mngt.c

\brief      Fonctions permettant la gestion des radio bearers du cluster par 
            le cluster head

\author     BURLOT Pascal

\date       29/08/08

   
\par     Historique:
        P.BURLOT 2009-01-20 
            + separation de la file de message CMM/RRM a envoyer en 2 files 
              distinctes ( file_send_cmm_msg, file_send_rrc_msg)
        L. IACOBELLI 2010-01-05
            + inclusions 

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
#include "cmm_msg.h"
#include "pusu_msg.h"
#include "msg_mngt.h"
#include "rb_db.h"
#include "neighbor_db.h"
#include "sens_db.h"
#include "channels_db.h"
#include "rrm_util.h"
#include "transact.h"
#include "rrm_constant.h"
#include "rrm.h"
#include "rb_mngt.h"

//! Met un message dans la file des messages a envoyer
#define PUT_CMM_MSG(m)  put_msg(  &(rrm->file_send_cmm_msg), 0, rrm->cmm.s,m )  //mod_lor_10_01_25
#define PUT_PUSU_MSG(m) put_msg(  &(rrm->file_send_cmm_msg), 0, rrm->pusu.s,m)  //mod_lor_10_01_25
#define PUT_RRC_MSG(m)  put_msg(  &(rrm->file_send_rrc_msg), 0, rrm->rrc.s,m )  //mod_lor_10_01_25

/*
 *  =========================================================================
 *  OUVERTURE D'UN RADIO BEARER
 *  =========================================================================
 */

/*!
*******************************************************************************
 \brief CMM connection setup request.  Only in CH. It asks to create a 
 * connection p2p between CH and MR
*/
int cmm_cx_setup_req(         
	Instance_t    inst      , //!< Identification de l'instance
	L2_ID         Src       , //!< L2 source MAC address
	L2_ID         Dst       , //!< L2 destination MAC address
	QOS_CLASS_T   QoS_class , //!< QOS class index
	Transaction_t Trans_id    //!< Transaction ID
	)
{
    int ret = -1 ;
    rrm_t *rrm = &rrm_inst[inst] ; 
    
    if ( (rrm->state == CLUSTERHEAD_INIT1 ) || (rrm->state == CLUSTERHEAD ) )
    {
        L2_ID src_dst[2] ;
        memcpy(&src_dst[0], &Src, sizeof(L2_ID)) ;
        memcpy(&src_dst[1], &Dst, sizeof(L2_ID)) ;
        
        pthread_mutex_lock( &( rrm->cmm.exclu ) ) ;
        //fprintf(stderr,"item_trans in CMM_CX_SETUP_REQ=%d node %d\n",rrm->cmm.trans_cnt,rrm->id);//dbg
        add_item_transact( &(rrm->cmm.transaction), Trans_id,INT_CMM,CMM_CX_SETUP_REQ, 0,NO_PARENT);
        pthread_mutex_unlock( &( rrm->cmm.exclu ) ) ;
        
        /** \todo Evaluer si le RB peut etre cree avant d'envoyer la commande au RRC */
        pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
        rrm->rrc.trans_cnt++ ;
//fprintf(stderr,"item_trans in RRM_RB_ESTABLISH_REQ=%d node %d\n",rrm->rrc.trans_cnt,rrm->id);//dbg
        add_item_transact( &(rrm->rrc.transaction), rrm->rrc.trans_cnt ,INT_RRC,RRM_RB_ESTABLISH_REQ,Trans_id,PARENT);
        add_rb( &(rrm->rrc.pRbEntry), rrm->rrc.trans_cnt, QoS_class, &src_dst[0] ) ;
        pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
                
        PUT_RRC_MSG(    
                    msg_rrm_rb_establish_req(inst,               
                        &Lchan_desc[QoS_class], 
                        &Mac_rlc_meas_desc[QoS_class],
                        &src_dst[0] ,
                        rrm->rrc.trans_cnt,
                        NULL,NONE_L3)   
                    ) ;

        if(QoS_class == QOS_DTCH_D){//faire le srb2 seulement a l'attachement (ouverture du DTCH IP par Defaut)
          pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
          rrm->rrc.trans_cnt++ ;
          //fprintf(stderr,"item_trans in RRM_RB_ESTABLISH_REQ 2=%d node %d\n",rrm->rrc.trans_cnt,rrm->id);//dbg
          add_item_transact( &(rrm->rrc.transaction), rrm->rrc.trans_cnt ,INT_RRC,RRM_RB_ESTABLISH_REQ,0,NO_PARENT);
          add_rb( &(rrm->rrc.pRbEntry), rrm->rrc.trans_cnt, QOS_SRB2, &src_dst[0] ) ;
          pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
          
          
          PUT_RRC_MSG(   
                msg_rrm_rb_establish_req(inst,               
                             &Lchan_desc[QOS_SRB2], 
                             &Mac_rlc_meas_desc[QOS_SRB2],
                             &src_dst[0] ,
                             rrm->rrc.trans_cnt,
                             rrm->L3_info,rrm->L3_info_t)   
                ) ;
        }
        ret = 0 ;
    }
    else
    {
        if ( rrm->state == CLUSTERHEAD_INIT0 )    
        {
            L2_ID src_dst[2] ;
            
            pthread_mutex_lock( &( rrm->cmm.exclu ) ) ;
            //fprintf(stderr,"item_trans in CMM_CX_SETUP_REQ 2=%d node %d\n",rrm->cmm.trans_cnt,rrm->id);//dbg
            add_item_transact( &(rrm->cmm.transaction), Trans_id, INT_CMM, CMM_CX_SETUP_REQ,0,NO_PARENT);
            pthread_mutex_unlock( &( rrm->cmm.exclu ) ) ;
            
            /** \todo Evaluer si le RB peut etre cree avant de solliciter le RRC */
            pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;

            memcpy(&src_dst[0], &Src, sizeof(L2_ID)) ;
            memcpy(&src_dst[1], &Dst, sizeof(L2_ID)) ;

            rrm->rrc.trans_cnt++ ;
            PUT_RRC_MSG(     
                        msg_rrm_rb_establish_req(inst,                   
                            &Lchan_desc[QOS_DTCH_B], 
                            &Mac_rlc_meas_desc[QOS_DTCH_B],
                            &src_dst[0] ,
                            rrm->rrc.trans_cnt,
                            rrm->L3_info,rrm->L3_info_t)
                        ) ;
            //fprintf(stderr,"item_trans in RRM_RB_ESTABLISH_REQ 3=%d node %d\n",rrm->rrc.trans_cnt,rrm->id);//dbg    
            add_item_transact( &(rrm->rrc.transaction), rrm->rrc.trans_cnt ,INT_RRC,RRM_RB_ESTABLISH_REQ,Trans_id,PARENT);

            add_rb( &(rrm->rrc.pRbEntry), rrm->rrc.trans_cnt, QOS_DTCH_B, &src_dst[0] ) ;

            pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
            
            rrm->state = CLUSTERHEAD_INIT1 ;
            fprintf(stderr,"[RRM] CLUSTERHEAD_INIT1\n" );
            ret = 0 ;
        }
        else
        fprintf(stderr,"[RRM] CMM_CX_SETUP_REQ (%d) is not allowed (Only CH):etat=%d\n", Trans_id, rrm->state); 
    }
        
    return ret ;
}

/*!
*******************************************************************************
\brief RRC response to rb_establish_req.  RRC Acknowledgement of reception of 
       rrc_rb_establishment_req.
*/
void rrc_rb_establish_resp(
	Instance_t    inst     , //!< Identification de l'instance
	Transaction_t Trans_id   //!< Transaction ID
	)
{
    rrm_t *rrm = &rrm_inst[inst] ; 
    
    if ( (rrm->state == CLUSTERHEAD) 
        || (rrm->state == CLUSTERHEAD_INIT0)
        || (rrm->state == CLUSTERHEAD_INIT1)
        )
    {
        pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
        set_ttl_transact(rrm->rrc.transaction, Trans_id, TTL_DEFAULT_VALUE ) ;
        pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;       
    }
    else
        fprintf(stderr,"[RRM] RRC_RB_ESTABLISH_RESP (%d) is not allowed (Only CH):etat=%d\n",Trans_id,rrm->state);  
}
/*!
*******************************************************************************
\brief Liberation d'un radio bearer
 */
static void rb_release_req( 
    Instance_t    inst           , ///< Identification de l'instance
    rrm_t        *rrm            , ///< pointeur sur l'instance du RRM
    RB_ID         Rb_id          , ///< Id du RB a liberer
    unsigned int  parentTransact , ///< Transaction parent  ( a l'origne de la requete )
    unsigned int  status_parent    ///< Status du parent
    )
{
    rrm->rrc.trans_cnt++ ;                  
    PUT_RRC_MSG( msg_rrm_rb_release_req(inst,Rb_id, rrm->rrc.trans_cnt) );
    add_item_transact( &(rrm->rrc.transaction), rrm->rrc.trans_cnt, INT_RRC, RRM_RB_RELEASE_REQ, parentTransact,status_parent);
}

/*!
*******************************************************************************
\brief RRC confirmation of rb_establish_req.  RRC confirmation of 
        rrc_rb_establishment_req after transactions are complete.  
        Essentially for CH only (except SRB0/1)
*/
void rrc_rb_establish_cfm(
    Instance_t    inst     , //!< Identification de l'instance
    RB_ID         Rb_id    , //!< Radio Bearer ID used by RRC
    RB_TYPE       RB_type  , //!< Radio Bearer Type
    Transaction_t Trans_id   //!< Transaction ID
    )
{
    rrm_t *rrm = &rrm_inst[inst] ; 
    
    if ( (rrm->state == CLUSTERHEAD) 
        || (rrm->state == CLUSTERHEAD_INIT0)
        || (rrm->state == CLUSTERHEAD_INIT1)
        )
    {
        transact_t *pTransact ;

        pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
        pTransact = get_item_transact(rrm->rrc.transaction,Trans_id ) ;
        if ( pTransact == NULL )
        {
            fprintf(stderr,"[RRM] rrc_rb_establish_cfm (%d) unknown transaction\n",Trans_id);
            // comme la transaction est inconnue, on libere immediatement le 
            // tuyau nouvellement  cree (au niveau du RRC uniquement)
            rb_release_req( inst, rrm,Rb_id,0,NO_PARENT );
            pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
        }
        else
        {
            unsigned int parent_id = pTransact->parent_id ;
            unsigned int status_parent = pTransact->parent_status ;
            
            update_rb_desc(rrm->rrc.pRbEntry, Trans_id, Rb_id, RB_type );
            del_item_transact( &(rrm->rrc.transaction),Trans_id ) ;
            pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
                    
            if ( status_parent )
            {
                transact_t *pTransactParent;
                
                pthread_mutex_lock( &( rrm->cmm.exclu ) ) ;
                pTransactParent = get_item_transact(rrm->cmm.transaction,parent_id ) ;
                if ( pTransactParent != NULL )
                {
                    PUT_CMM_MSG( msg_rrm_cx_setup_cnf(inst,Rb_id,pTransactParent->id ));
                    
                    if ( rrm->state == CLUSTERHEAD_INIT1 ) 
                        rrm->state = CLUSTERHEAD ;
                    
                    del_item_transact( &(rrm->cmm.transaction),pTransactParent->id ) ;
                    pthread_mutex_unlock( &( rrm->cmm.exclu ) ) ;
                }
                else
                {   // comme la transaction parent est inconnue, on libere 
                    // immediatement le  tuyau nouvellement  cree (au niveau 
                    // du RRC uniquement)
                    pthread_mutex_unlock( &( rrm->cmm.exclu ) ) ;
                    
                    pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
                    rb_release_req( inst, rrm,Rb_id, 0, NO_PARENT );
                    del_rb_by_rbid( &(rrm->rrc.pRbEntry), Rb_id ) ;
                    pthread_mutex_unlock( &( rrm->rrc.exclu ) );
                }
            }
            pthread_mutex_unlock( &( rrm->cmm.exclu ) ) ;
        }   
    }
    else
        fprintf(stderr,"[RRM] RRC_RB_ESTABLISH_CFM (%d) is not allowed (Only CH):etat=%d\n",Trans_id,rrm->state);       
}

/*
 *  =========================================================================
 *  MODICATION D'UN RADIO BEARER
 *  =========================================================================
 */

/*!
*******************************************************************************
\brief CMM connection modify request.  Only in CH.

*/
int cmm_cx_modify_req(
	Instance_t    inst      , //!< Identification de l'instance
	RB_ID         Rb_id     , //!< L2 Rb_id
	QOS_CLASS_T   QoS_class , //!< QOS class index
	Transaction_t Trans_id    //!< Transaction ID
	)
{
    int ret = -1 ;
    rrm_t *rrm = &rrm_inst[inst] ; 
    
    if ( rrm->state == CLUSTERHEAD )
    {
        RB_desc_t *pRb ;
        
        pthread_mutex_lock( &( rrm->cmm.exclu ) ) ;
        add_item_transact( &(rrm->cmm.transaction), Trans_id,INT_CMM,CMM_CX_MODIFY_REQ,0,NO_PARENT);
        pthread_mutex_unlock( &( rrm->cmm.exclu ) ) ;
        
        /** \todo Evaluer si le RB peut etre modifier avant de solliciter le RRC */
        pRb = (RB_desc_t *) get_rb_desc_by_rbid( rrm->rrc.pRbEntry, Rb_id ) ;
        if ( pRb != NULL )
        {
            pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
            rrm->rrc.trans_cnt++ ;
                    
            PUT_RRC_MSG( 
                        msg_rrm_rb_modify_req( inst,
                            &Lchan_desc[QoS_class], 
                            &Mac_rlc_meas_desc[QoS_class],
                            Rb_id,
                            rrm->rrc.trans_cnt)
                        ) ;
                
            add_item_transact( &(rrm->rrc.transaction), rrm->rrc.trans_cnt ,INT_RRC,RRM_RB_MODIFY_REQ,Trans_id,PARENT);
            add_rb( &(rrm->rrc.pRbEntry), rrm->rrc.trans_cnt, QoS_class, &pRb->L2_id[0] ) ;
            
            pRb->QoS_class =  QoS_class ; 
            pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
        }   
        
        ret = 0 ;
    }
    else
        fprintf(stderr,"[RRM] CMM_CX_MODIFY_REQ (%d) is not allowed (Only CH):etat=%d\n",Trans_id,rrm->state);  
        
    return ret ;
}
/*!
*******************************************************************************
\brief RRC response to rb_modify_req
*/
void rrc_rb_modify_resp(
	Instance_t    inst     , //!< Identification de l'instance
	Transaction_t Trans_id   //!< Transaction ID
	)
{
    rrm_t *rrm = &rrm_inst[inst] ; 
    
    if ( (rrm->state == CLUSTERHEAD) 
            || (rrm->state == CLUSTERHEAD_INIT0) 
            || (rrm->state == CLUSTERHEAD_INIT1) 
            )
    {
        pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
        set_ttl_transact(rrm->rrc.transaction,Trans_id, TTL_DEFAULT_VALUE) ;
        pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;       
    }
    else
        fprintf(stderr,"[RRM] RRC_RB_MODIFY_RESP (%d) is not allowed (Only CH):etat=%d\n",Trans_id,rrm->state); 
}
/*!
*******************************************************************************
\brief RRC confirmation of rb_modify_req
*/
void rrc_rb_modify_cfm(
    Instance_t    inst     , //!< Identification de l'instance
    RB_ID         Rb_id    , //!< Radio Bearer ID used by RRC
    Transaction_t Trans_id   //!< Transaction ID
    )
{
    rrm_t *rrm = &rrm_inst[inst] ; 
    
    if ( rrm->state == CLUSTERHEAD )
    {
        transact_t *pTransact ;
        pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
        pTransact = get_item_transact(rrm->rrc.transaction,Trans_id ) ;
        if ( pTransact == NULL )
        {
            fprintf(stderr,"[RRM] rrc_rb_modify_cfm (%d) unknown transaction\n",Trans_id);
            pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
        }
        else
        {
            unsigned int parent_id = pTransact->parent_id ;
            unsigned int status_parent = pTransact->parent_status ;
            
            RB_desc_t * pRb = (RB_desc_t *) get_rb_desc_by_rbid( rrm->rrc.pRbEntry, Rb_id ) ;
            if ( pRb != NULL )
            {
                RB_TYPE RB_type = pRb->RB_type ;
                // détruit l'ancienne description
                del_rb_by_rbid( &(rrm->rrc.pRbEntry), Rb_id ) ;
                // mise à jour de la nouvelle
                update_rb_desc(rrm->rrc.pRbEntry, Trans_id, Rb_id, RB_type );
            }
            
            del_item_transact( &(rrm->rrc.transaction),Trans_id ) ;
            pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
            
            pthread_mutex_lock( &( rrm->cmm.exclu ) ) ;
            if ( status_parent )
            {
                transact_t *pTransactParent = get_item_transact(rrm->cmm.transaction,parent_id ) ;
                if ( pTransactParent != NULL )
                {
                    PUT_CMM_MSG( msg_rrm_cx_modify_cnf(inst,pTransactParent->id ));
                    del_item_transact( &(rrm->cmm.transaction),pTransactParent->id ) ;
                }
                else // la transaction parent est inconnue, on ne fait rien
                    fprintf(stderr, "[RRM] RRC_RB_MODIFY_CFM (%d) : the parent transaction (%d) is unknown\n",Trans_id,parent_id);  
            }
            pthread_mutex_unlock( &( rrm->cmm.exclu ) ) ;
        }           
    }
    else
        fprintf(stderr,"[RRM] RRC_RB_MODIFY_CFM (%d) is not allowed (Only CH):etat=%d\n",Trans_id,rrm->state);  
}

/*
 *  =========================================================================
 *  LIBERATION D'UN RADIO BEARER
 *  =========================================================================
 */

/*!
*******************************************************************************
\brief CMM connection release request.  Only in CH.
 */
int cmm_cx_release_req(
	Instance_t    inst     , //!< Identification de l'instance
	RB_ID         Rb_id    , //!< L2 Rb_id
	Transaction_t Trans_id   //!< Transaction ID
	)
{
    int ret = -1 ;
    rrm_t *rrm = &rrm_inst[inst] ; 
    
    if ( rrm->state == CLUSTERHEAD )
    {
        pthread_mutex_lock( &( rrm->cmm.exclu ) ) ;
        add_item_transact( &(rrm->cmm.transaction), Trans_id,INT_CMM,CMM_CX_RELEASE_REQ,0,NO_PARENT);
        pthread_mutex_unlock( &( rrm->cmm.exclu ) ) ;
        
        pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
        rb_release_req( inst, rrm,Rb_id,Trans_id, PARENT );
        del_rb_by_rbid( &(rrm->rrc.pRbEntry), Rb_id ) ;
        pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
        
        ret = 0 ;
    }
    else
        fprintf(stderr,"[RRM] CMM_CX_RELEASE_REQ (%d) is not allowed (Only CH):etat=%d\n",Trans_id,rrm->state); 
        
    return ret ;
}

/*!
*******************************************************************************
\brief RRC response to rb_release_req
*/
void rrc_rb_release_resp(
    Instance_t    inst     , //!< Identification de l'instance
    Transaction_t Trans_id   //!< Transaction ID
    )
{
    rrm_t *rrm = &rrm_inst[inst] ; 
    
    if ( rrm->state == CLUSTERHEAD )
    {
        transact_t *pTransact ;

        pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
        pTransact = get_item_transact(rrm->rrc.transaction,Trans_id ) ;
        if ( pTransact == NULL )
        {
            fprintf(stderr,"[RRM] rrc_rb_release_resp (%d) unknown transaction\n",Trans_id);
            pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
        }
        else
        {
            unsigned int parent_id = pTransact->parent_id ;
            unsigned int status_parent = pTransact->parent_status ;

            del_item_transact( &(rrm->rrc.transaction),Trans_id ) ;
            pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
            
            pthread_mutex_lock( &( rrm->cmm.exclu ) ) ;
            if ( status_parent )
            {
                transact_t *pTransactParent = get_item_transact(rrm->cmm.transaction,parent_id ) ;
                if ( pTransactParent != NULL )
                {
                    PUT_CMM_MSG( msg_rrm_cx_release_cnf(inst,pTransactParent->id ) );
                    del_item_transact( &(rrm->cmm.transaction),pTransactParent->id ) ;
                }   
                else // la transaction parent est inconnue, on ne fait rien
                    fprintf(stderr,"[RRM] RRC_RB_RELEASE_RESP (%d) : the parent transaction (%d) is unknown\n",Trans_id,parent_id); 
            }
            pthread_mutex_unlock( &( rrm->cmm.exclu ) ) ;
        }       
    }
    else
        fprintf(stderr,"[RRM] RRC_RB_RELEASE_RESP (%d) is not allowed (Only CH):etat=%d\n",Trans_id,rrm->state);    
}

/*
 *  =========================================================================
 *  MESURES SUR UN RADIO BEARER
 *  =========================================================================
 */
/*!
*******************************************************************************
\brief RRC measurement indication 
 */
void rrc_rb_meas_ind(
	 Instance_t     inst         , //!< Identification de l'instance
	 RB_ID          Rb_id        , //!< Radio Bearer ID
	 L2_ID          L2_id        , //!< Layer 2 (MAC) IDs for link
	 MEAS_MODE      Meas_mode    , //!< Measurement mode (periodic or event-driven)
	 MAC_RLC_MEAS_T Mac_rlc_meas , //!< MAC/RLC measurements
	 Transaction_t  Trans_id       //!< Transaction ID
	 )
{
    rrm_t *rrm = &rrm_inst[inst] ; 
    
    if ( rrm->state == CLUSTERHEAD )
    {
        PUT_RRC_MSG( msg_rrm_rb_meas_resp(inst,Trans_id) );
        
        if ( Meas_mode == PERIODIC )
        {
            RB_desc_t *pRb ;
            int ii_rb = 1 ;
            int transaction_link_info ;
            
            // update database of RB
            pthread_mutex_lock(   &( rrm->rrc.exclu )  ) ;
            update_rb_meas( rrm->rrc.pRbEntry, Rb_id, &L2_id, &Mac_rlc_meas );  
            pthread_mutex_unlock( &( rrm->rrc.exclu )  ) ;
        
            pthread_mutex_lock( &( rrm->pusu.exclu ) ) ;
            rrm->pusu.trans_cnt++ ;
            transaction_link_info = rrm->pusu.trans_cnt ;
            add_item_transact( &(rrm->pusu.transaction), transaction_link_info,INT_PUSU,RRM_LINK_INFO_IND,0,NO_PARENT);
            pthread_mutex_unlock( &( rrm->pusu.exclu ) ) ;  
            
            pRb = get_rb_desc_by_rbid( rrm->rrc.pRbEntry, Rb_id ) ;
            if ( memcmp(&L2_id, &pRb->L2_id[1], sizeof(L2_ID ) )  == 0 )
                ii_rb=0 ;
                
            PUT_PUSU_MSG(  
                        msg_rrm_link_info_ind(inst, L2_id , pRb->L2_id[ii_rb] , Rb_id, 
                                            Mac_rlc_meas.Rssi ,Mac_rlc_meas.Spec_eff, transaction_link_info ) 
                    );
        }
    }
    else
        fprintf(stderr,"[RRM] RRC_RB_MEAS_IND (%d) is not allowed (Only CH):etat=%d\n",Trans_id,rrm->state);    
}
