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

\file       ch_init.c

\brief      Fonctions permettant la gestion de la phase d'initialisation du 
            cluster head.

\author     BURLOT Pascal

\date       29/08/08

   
\par     Historique:
        P.BURLOT 2009-01-20 
            + separation de la file de message CMM/RRM a envoyer en 2 files 
              distinctes ( file_send_cmm_msg, file_send_rrc_msg)
        L.IACOBELLI 2009-10-19
            + include

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
#include "ch_init.h"



//! Met un message dans la file des messages a envoyer
#define PUT_CMM_MSG(m)  put_msg(  &(rrm->file_send_cmm_msg), 0, rrm->cmm.s,m )  //mod_lor_10_01_25
#define PUT_PUSU_MSG(m) put_msg(  &(rrm->file_send_cmm_msg), 0, rrm->pusu.s,m)  //mod_lor_10_01_25
#define PUT_RRC_MSG(m)  put_msg(  &(rrm->file_send_rrc_msg), 0, rrm->rrc.s,m )  //mod_lor_10_01_25

/*!
*******************************************************************************
\brief  Request to initialize the Cluster Head with L3 Information 
*/
void cmm_init_ch_req(
	Instance_t inst      , //!< Identification de l'instance
	L3_INFO_T  L3_info_t , //!< Type of L3 Information
	void      *L3_info     //!< L3 addressing Information
	)
{
    rrm_t *rrm = &rrm_inst[inst] ; 

    if ( rrm->state == CLUSTERHEAD_INIT0 )
    {
        L2_ID src_dst[2] ;
        memcpy(&src_dst[0], &rrm->L2_id, sizeof(L2_ID)) ;
        memcpy(&src_dst[1], &rrm->L2_id, sizeof(L2_ID)) ;
        
        if ( L3_info != NULL ) 
        {
            rrm->L3_info_t = L3_info_t ;    
            if ( L3_info_t != NONE_L3 ) 
                memcpy( rrm->L3_info, L3_info, L3_info_t );
        }
        //fprintf(stderr,"L3_ch_init    ");//dbg
        //print_L3_id( IPv4_ADDR, rrm->L3_info   );
        //fprintf(stderr,"\n");//dbg

        pthread_mutex_lock( &( rrm->rrc.exclu ) ) ;
        rrm->rrc.trans_cnt++ ;

        PUT_RRC_MSG( 
                    msg_rrm_init_ch_req( inst,
                             rrm->rrc.trans_cnt,
                            &Lchan_desc[QOS_SRB0], 
                            &Lchan_desc[QOS_SRB1], 
                             rrm->L2_id
                             )
                        ) ;
         
        // On ne gere pas SRB0/1 car implicite
        // add_rb( &(rrm->rrc.pRbEntry), rrm->rrc.trans_cnt, QOS_SRB0, &src_dst[0] ) ;
        // add_rb( &(rrm->rrc.pRbEntry), rrm->rrc.trans_cnt, QOS_SRB1, &src_dst[0] ) ;
        
        pthread_mutex_unlock( &( rrm->rrc.exclu ) ) ;
        
        pthread_mutex_lock( &( rrm->pusu.exclu ) ) ;
        rrm->pusu.trans_cnt++ ;
        add_item_transact( &(rrm->pusu.transaction), rrm->pusu.trans_cnt,INT_PUSU,RRM_PUBLISH_IND,0,NO_PARENT);
        pthread_mutex_unlock( &( rrm->pusu.exclu ) ) ;
        PUT_PUSU_MSG(msg_rrm_publish_ind( inst, PUSU_RRM_SERVICE, rrm->pusu.trans_cnt  )) ;
        
    }
}

/*!
*******************************************************************************
\brief Mesh router PHY-Synch Indication
*/
void rrc_phy_synch_to_MR_ind(
	Instance_t inst      , //!< Identification de l'instance
	L2_ID      L2_id       //!< L2 ID of MR
	)
{
    rrm_t *rrm = &rrm_inst[inst] ; 

    if ( rrm->state == ISOLATEDNODE )
    {
        /* Memorisation du L2_id du noeud ( c'est le niveau RRC qui a l'info ) */
        memcpy( &rrm->L2_id,  &L2_id, sizeof(L2_ID));
      
        //--------------------------------------------
        pthread_mutex_lock(   &( rrm->rrc.exclu )  ) ;
        
        if ( rrm->rrc.pNeighborEntry  != NULL ) // Reset Neighborhood
            del_all_neighbor( &(rrm->rrc.pNeighborEntry) );
            
        pthread_mutex_unlock( &( rrm->rrc.exclu )  ) ;
        //mod_lor_10_03_01++
        //if (WSN){
        if (WSN && rrm->id == FC_ID){ 
            rrm->role = FUSIONCENTER; 
            fprintf(stderr,"[CRRM] FUSIONCENTER\n");
        }else if (rrm->id == BTS_ID && SCEN_1){
            rrm->role = BTS; 
            fprintf(stderr,"[CRRM] BTS\n");
        }
        //mod_lor_10_04_27++
        else if (rrm->id == FC_ID && SCEN_2_CENTR){
            rrm->role = FUSIONCENTER; 
            fprintf(stderr,"[CRRM] CH1\n");
        }
        else if (rrm->id == CH_COLL_ID &&SCEN_2_CENTR){
            rrm->role = CH_COLL; 
            fprintf(stderr,"[CRRM] CH2\n");
        }
        else 
            fprintf(stderr,"Warning!!! No role assigned\n");
        //mod_lor_10_04_27--
        //mod_lor_10_03_01--

        PUT_CMM_MSG( msg_router_is_CH_ind( inst,rrm->L2_id)) ;

        rrm->state = CLUSTERHEAD_INIT0 ; 
        fprintf(stderr,"[CRRM] CLUSTERHEAD_INIT0\n");
    }
    else
        fprintf(stderr,"[CRRM] RRC_PHY_SYNCH_TO_MR_IND/TIMEOUT_IN  is not allowed (Only IN):etat=%d\n",rrm->state);
}        


