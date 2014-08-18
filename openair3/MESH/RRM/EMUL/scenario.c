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

\file       scenario.c

\brief      Emulation d'un scenario de test  sur les interfaces du RRM 

            Cette application d'envoyer des stimuli sur les interfaces RRM:
                - RRC -> RRM
                - CMM -> RRM

\author     BURLOT Pascal

\date       10/07/08

   
\par     Historique:
            IACOBELLI Lorenzo 15/04/2010
            - scenarios from 14 that include sensing unit emulation


*******************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/socket.h>
#include <sys/un.h>

#include <pthread.h>
#include <time.h>

#include "debug.h"

#include "L3_rrc_defs.h"
#include "cmm_rrm_interface.h"

#include "rrm_sock.h"
#include "cmm_msg.h"
#include "rrc_rrm_msg.h"
#include "ip_msg.h" //mod_lor_10_04_27
#include "pusu_msg.h"
#include "sensing_rrm_msg.h" //mod_lor_10_04_15

#include "transact.h"
#include "actdiff.h"
#include "rrm_util.h"
#include "rrm_constant.h"
#include "emul_interface.h"

extern msg_t *msg_rrc_rb_meas_ind(Instance_t inst, RB_ID Rb_id, L2_ID L2_id, MEAS_MODE Meas_mode, MAC_RLC_MEAS_T *Mac_rlc_meas_t, Transaction_t Trans_id );
extern msg_t *msg_rrc_sensing_meas_ind( Instance_t inst, L2_ID L2_id, unsigned int NB_meas, SENSING_MEAS_T *Sensing_meas, Transaction_t Trans_id );
extern msg_t *msg_rrc_sensing_meas_resp( Instance_t inst, Transaction_t Trans_id )  ;
extern msg_t *msg_rrc_cx_establish_ind( Instance_t inst, L2_ID L2_id, Transaction_t Trans_id,unsigned char *L3_info, L3_INFO_T L3_info_t,
                                    RB_ID DTCH_B_id, RB_ID DTCH_id );
extern msg_t *msg_rrc_phy_synch_to_MR_ind( Instance_t inst, L2_ID L2_id);
extern msg_t *msg_rrc_phy_synch_to_CH_ind( Instance_t inst, unsigned int Ch_index,L2_ID L2_id );
extern msg_t *msg_rrc_rb_establish_resp( Instance_t inst, Transaction_t Trans_id  );
extern msg_t *msg_rrc_rb_establish_cfm( Instance_t inst, RB_ID Rb_id, RB_TYPE RB_type, Transaction_t Trans_id );
extern msg_t *msg_rrc_rb_modify_resp( Instance_t inst, Transaction_t Trans_id );
extern msg_t *msg_rrc_rb_modify_cfm(Instance_t inst, RB_ID Rb_id, Transaction_t Trans_id  );
extern msg_t *msg_rrc_rb_release_resp( Instance_t inst, Transaction_t Trans_id );
extern msg_t *msg_rrc_MR_attach_ind( Instance_t inst, L2_ID L2_id );
extern msg_t *msg_rrc_update_sens( Instance_t inst, L2_ID L2_id, unsigned int NB_info, Sens_ch_t *Sens_meas, Transaction_t Trans_id);
extern msg_t *msg_rrc_init_scan_req(Instance_t inst, L2_ID L2_id, unsigned int  Start_fr, unsigned int  Stop_fr,unsigned int Meas_band,
        unsigned int Meas_tpf, unsigned int Nb_channels,unsigned int Overlap, unsigned int Sampl_freq, Transaction_t Trans_id );
extern msg_t *msg_rrc_end_scan_conf(Instance_t inst, L2_ID L2_id, Transaction_t Trans_id );
extern msg_t *msg_rrc_end_scan_req( Instance_t inst, L2_ID L2_id, Transaction_t Trans_id );
extern msg_t *msg_rrc_init_mon_req(Instance_t inst, L2_ID L2_id, unsigned int *ch_to_scan, unsigned int NB_chan, float interval, Transaction_t Trans_id );
extern msg_t *msg_rrc_open_freq( Instance_t inst, L2_ID L2_id,unsigned int NB_chan, CHANNEL_T *fr_channels, Transaction_t Trans_id);
extern msg_t *msg_rrc_ask_for_freq( Instance_t inst, L2_ID L2_id, QOS_CLASS_T QoS, Transaction_t Trans_id );
extern msg_t *msg_rrc_update_SN_freq( Instance_t inst, L2_ID L2_id, unsigned int NB_chan, unsigned int *occ_channels, Transaction_t Trans_id);
extern msg_t *msg_rrc_clust_scan_req( Instance_t inst, L2_ID L2_id, float interv, COOPERATION_T coop, Transaction_t Trans_id);
extern msg_t *msg_rrc_clust_mon_req(Instance_t inst, L2_ID L2_id, unsigned int *ch_to_scan, unsigned int NB_chan, float interval, Transaction_t Trans_id );
extern msg_t *msg_rrc_init_conn_conf( Instance_t inst, L2_ID L2_id, unsigned int Session_id, Transaction_t Trans_id );
extern msg_t *msg_rrc_freq_all_prop_conf( Instance_t inst, L2_ID L2_id, unsigned int Session_id, unsigned int NB_free_ch, CHANNEL_T *fr_channels, Transaction_t Trans_id);
extern msg_t *msg_rrc_rep_freq_ack( Instance_t inst, L2_ID L2_id_ch, L2_ID L2_id_source, L2_ID L2_id_dest, unsigned int Session_id, CHANNEL_T all_channel, Transaction_t Trans_id);
extern msg_t *msg_rrc_init_conn_req( Instance_t inst, L2_ID L2_id , unsigned int Session_id, QOS_CLASS_T QoS_class, Transaction_t Trans_id );
extern msg_t *msg_rrc_freq_all_prop( Instance_t inst, L2_ID L2_id, unsigned int Session_id, unsigned int NB_free_ch, CHANNEL_T *fr_channels, Transaction_t Trans_id);
extern msg_t *msg_rrc_rep_freq_all( Instance_t inst, L2_ID L2_id_source,L2_ID L2_id_dest, unsigned int Session_id, unsigned int NB_prop_ch, CHANNEL_T *pr_channels, Transaction_t Trans_id);


L2_ID L2_id_ch  ={{0xAA,0xCC,0x33,0x55,0x00,0x11,0x00,0x00}};
L2_ID L2_id_fc  ={{0xAA,0xCC,0x33,0x55,0x00,0x11,0x00,0x00}};
L2_ID L2_id_mr  ={{0xAA,0xCC,0x33,0x55,0x00,0x00,0x22,0x00}};
L2_ID L2_id_mr2 ={{0xAA,0xCC,0x33,0x55,0x00,0x00,0x33,0x00}};
L2_ID L2_id_mr3 ={{0xAA,0xCC,0x33,0x55,0x00,0x00,0x44,0x00}};
L2_ID L2_id_bts ={{0xAA,0xCC,0x33,0x55,0x00,0x00,0x00,0x11}};
L2_ID L2_id_ch2 ={{0xAA,0xCC,0x33,0x55,0x00,0x22,0x00,0x00}};

unsigned char L3_info_mr3[MAX_L3_INFO] = { 0x0A, 0x00, 3, 3 } ; 
unsigned char L3_info_mr2[MAX_L3_INFO] = { 0x0A, 0x00, 2, 2 } ; 
unsigned char L3_info_mr[MAX_L3_INFO]  = { 0x0A, 0x00, 1, 1 } ; 
unsigned char L3_info_ch[MAX_L3_INFO]  = { 0x0A, 0x00, 0, 0 } ; 

//mod_lor_10_03_12++ : init sensing global parameters
unsigned int     Start_fr   = 1000;
unsigned int     Stop_fr    = 2000;
unsigned int     Meas_band  = 200;
unsigned int     Meas_tpf   = 2;
unsigned int     Nb_channels= 5; 
unsigned int     Overlap    = 5;
unsigned int     Sampl_freq = 10;
//mod_lor_10_03_12--

static void prg_opening_RB( sock_rrm_t *s_cmm, double date, L2_ID *src, L2_ID *dst, QOS_CLASS_T qos )
{
    cmm_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_cmm,
                msg_cmm_cx_setup_req(0,*src,*dst, qos, cmm_transaction ) ) ;
    pthread_mutex_unlock( &actdiff_exclu ) ;
                
    pthread_mutex_lock( &cmm_transact_exclu ) ;
    add_item_transact( &cmm_transact_list, cmm_transaction, INT_CMM,CMM_CX_SETUP_REQ,0,NO_PARENT);
    pthread_mutex_unlock( &cmm_transact_exclu ) ;
}

static void prg_modifying_RB( sock_rrm_t *s_cmm, double date, RB_ID Rb_id, QOS_CLASS_T qos )
{
    cmm_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_cmm,
                msg_cmm_cx_modify_req(0,Rb_id, qos, cmm_transaction ) ) ;
    pthread_mutex_unlock( &actdiff_exclu ) ;
                
    pthread_mutex_lock( &cmm_transact_exclu ) ;
    add_item_transact( &cmm_transact_list, cmm_transaction, INT_CMM,CMM_CX_MODIFY_REQ,0,NO_PARENT);
    pthread_mutex_unlock( &cmm_transact_exclu ) ;
}

static void prg_releasing_RB( sock_rrm_t *s_cmm, double date, RB_ID Rb_id )
{
    cmm_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_cmm,
                msg_cmm_cx_release_req(0,Rb_id, cmm_transaction ) ) ;
    pthread_mutex_unlock( &actdiff_exclu ) ;
                
    pthread_mutex_lock( &cmm_transact_exclu ) ;
    add_item_transact( &cmm_transact_list, cmm_transaction, INT_CMM,CMM_CX_RELEASE_REQ,0,NO_PARENT);
    pthread_mutex_unlock( &cmm_transact_exclu ) ;
}

static void prg_phy_synch_to_MR( sock_rrm_t *s_rrc, double date )
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc, msg_rrc_phy_synch_to_MR_ind(0, L2_id_ch) ) ;
    pthread_mutex_unlock( &actdiff_exclu ) ;
                
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC,RRC_PHY_SYNCH_TO_MR_IND,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
}

static void prg_rrc_MR_attach_ind( sock_rrm_t *s_rrc, double date, L2_ID *L2_id_mr  )
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc,
                msg_rrc_MR_attach_ind(0,*L2_id_mr ) ) ;
    pthread_mutex_unlock( &actdiff_exclu ) ;
                
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC,RRC_MR_ATTACH_IND,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
}

static void prg_rrc_cx_establish_ind( 
        sock_rrm_t *s_rrc, double date, 
        L2_ID *L2_id,
        unsigned char *L3_info,
        L3_INFO_T L3_info_t,
        RB_ID dtch_b_id,
        RB_ID dtch_id  
        )
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc,
                msg_rrc_cx_establish_ind(0,*L2_id,rrc_transaction, L3_info, L3_info_t, dtch_b_id, dtch_id ) ) ;
    pthread_mutex_unlock( &actdiff_exclu ) ;
                
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC,RRC_CX_ESTABLISH_IND,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
}

static void prg_rrc_sensing_meas_ind( sock_rrm_t *s_rrc, double date, L2_ID *L2_id_mr,
                                    int nb_meas, SENSING_MEAS_T *Sensing_meas )
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc,
                msg_rrc_sensing_meas_ind(0,*L2_id_mr,nb_meas,Sensing_meas, rrc_transaction ) ) ;
    pthread_mutex_unlock( &actdiff_exclu ) ;
                
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC,RRC_SENSING_MEAS_IND,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
}

static void prg_rrc_rb_meas_ind( sock_rrm_t *s_rrc, double date, RB_ID Rb_id, L2_ID *L2_id,MEAS_MODE Meas_mode,
                                    MAC_RLC_MEAS_T *Mac_rlc_meas )
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc,
                msg_rrc_rb_meas_ind(0, Rb_id, *L2_id, Meas_mode, Mac_rlc_meas,  rrc_transaction ) ) ;
    pthread_mutex_unlock( &actdiff_exclu ) ;
                
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC,RRC_RB_MEAS_IND,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
}   
 
static void prg_cmm_init_sensing( sock_rrm_t *s_cmm, double date, unsigned int Start_fr,unsigned int Stop_fr,unsigned int Meas_band,unsigned int Meas_tpf,
                                    unsigned int Nb_channels,unsigned int Overlap,unsigned int Sampl_freq )
{
    cmm_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_cmm, msg_cmm_init_sensing ( 0, Start_fr,Stop_fr,Meas_band,Meas_tpf,Nb_channels,Overlap,Sampl_freq));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &cmm_transact_exclu ) ;
    add_item_transact( &cmm_transact_list, cmm_transaction, INT_CMM, CMM_INIT_SENSING,0,NO_PARENT);
    pthread_mutex_unlock( &cmm_transact_exclu ) ;
                
}
static void prg_rrc_update_sens( sock_rrm_t *s_rrc, double date, L2_ID *L2_id_mr,
                                    unsigned int NB_info, Sens_ch_t *Sens_meas )
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc, msg_rrc_update_sens ( 0, *L2_id_mr, NB_info, Sens_meas, rrc_transaction));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC, RRC_UPDATE_SENS,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
                
}
static void prg_rrc_init_scan_req( sock_rrm_t *s_rrc, double date, L2_ID *L2_id_fc,
                                    unsigned int Start_fr,unsigned int Stop_fr,unsigned int Meas_band,unsigned int Meas_tpf,
                                    unsigned int Nb_channels,unsigned int Overlap,unsigned int Sampl_freq )
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc, msg_rrc_init_scan_req ( 0, *L2_id_fc, Start_fr,Stop_fr,Meas_band,Meas_tpf,Nb_channels,Overlap,Sampl_freq, rrc_transaction));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC, RRC_INIT_SCAN_REQ,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
                
}

static void prg_cmm_stop_sensing( sock_rrm_t *s_cmm, double date)
{
    cmm_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_cmm, msg_cmm_stop_sensing ( 0 ));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &cmm_transact_exclu ) ;
    add_item_transact( &cmm_transact_list, cmm_transaction, INT_CMM, CMM_STOP_SENSING,0,NO_PARENT);
    pthread_mutex_unlock( &cmm_transact_exclu ) ;
                
}

static void prg_rrc_end_scan_conf( sock_rrm_t *s_rrc, double date, L2_ID *L2_id_mr )
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc, msg_rrc_end_scan_conf ( 0, *L2_id_mr, rrc_transaction));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC, RRC_END_SCAN_CONF,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
                
}

static void prg_rrc_end_scan_req( sock_rrm_t *s_rrc, double date, L2_ID *L2_id_fc )
{
     rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc, msg_rrc_end_scan_req ( 0, *L2_id_fc, rrc_transaction));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC, RRC_END_SCAN_REQ,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
                
}

static void prg_rrc_init_mon_req( sock_rrm_t *s_rrc, double date, L2_ID *L2_id_fc, unsigned int *ch_to_scan, unsigned int NB_chan, float interv )
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc, msg_rrc_init_mon_req ( 0, *L2_id_fc, ch_to_scan, NB_chan, interv, rrc_transaction));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC, RRC_INIT_MON_REQ,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
                
}


/*
static void prg_cmm_ask_freq( sock_rrm_t *s_cmm, double date)
{
    cmm_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_cmm, msg_cmm_ask_freq ( 0 ));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &cmm_transact_exclu ) ;
    add_item_transact( &cmm_transact_list, cmm_transaction, INT_CMM, CMM_ASK_FREQ,0,NO_PARENT);
    pthread_mutex_unlock( &cmm_transact_exclu ) ;
                
}

static void prg_rrc_open_freq( sock_rrm_t *s_rrc, double date, L2_ID *L2_id_fc,
                                    unsigned int NB_chan, CHANNEL_T *fr_channels)
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc, msg_rrc_open_freq ( 0, *L2_id_fc, NB_chan, fr_channels, rrc_transaction ));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC, RRC_OPEN_FREQ,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
                
}


static void prg_rrc_ask_for_freq( sock_rrm_t *s_rrc, double date, L2_ID *L2_id_bts,
                                   QOS_CLASS_T QoS)
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc, msg_rrc_ask_for_freq ( 0, *L2_id_bts, QoS, rrc_transaction ));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC, RRC_ASK_FOR_FREQ,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
                
}


static void prg_rrc_update_SN_freq( sock_rrm_t *s_rrc, double date, L2_ID *L2_id_bts, 
                                   unsigned int NB_chan, unsigned int *occ_channels)
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc, msg_rrc_update_SN_freq ( 0, *L2_id_bts, NB_chan, occ_channels, rrc_transaction ));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC, RRC_UPDATE_SN_FREQ,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
                
}

static void prg_cmm_need_to_tx( sock_rrm_t *s_cmm, double date, QOS_CLASS_T QoS_class)
{
    cmm_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_cmm, msg_cmm_need_to_tx ( 0, QoS_class ));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &cmm_transact_exclu ) ;
    add_item_transact( &cmm_transact_list, cmm_transaction, INT_CMM, CMM_NEED_TO_TX,0,NO_PARENT);
    pthread_mutex_unlock( &cmm_transact_exclu ) ;
                
}

static void prg_rrc_clust_scan_req( sock_rrm_t *s_rrc, double date, L2_ID *L2_id_ch2, 
                                   float interv, COOPERATION_T coop)
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc, msg_rrc_clust_scan_req ( 0, *L2_id_ch2, interv, coop, rrc_transaction ));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC, RRC_CLUST_SCAN_REQ,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
                
}

static void prg_rrc_clust_mon_req( sock_rrm_t *s_rrc, double date, L2_ID *L2_id_fc, unsigned int *ch_to_scan, unsigned int NB_chan, float interv )
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc, msg_rrc_clust_mon_req ( 0, *L2_id_fc, ch_to_scan, NB_chan, interv, rrc_transaction));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC, RRC_CLUST_MON_REQ,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
                
}

static void prg_cmm_init_trans_req( sock_rrm_t *s_cmm, double date, L2_ID *L2_id_mr2, unsigned int Session_id, QOS_CLASS_T QoS_class)
{
    cmm_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_cmm, msg_cmm_init_trans_req ( 0, *L2_id_mr2, Session_id,  QoS_class, cmm_transaction));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &cmm_transact_exclu ) ;
    add_item_transact( &cmm_transact_list, cmm_transaction, INT_CMM, CMM_INIT_TRANS_REQ,0,NO_PARENT);
    pthread_mutex_unlock( &cmm_transact_exclu ) ;
                
}

static void prg_rrc_init_conn_conf ( sock_rrm_t *s_rrc, double date, L2_ID *L2_id_mr2, unsigned int Session_id )
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc, msg_rrc_init_conn_conf ( 0, *L2_id_mr2, Session_id, rrc_transaction));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC, RRC_INIT_CONN_CONF,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
                
}

static void prg_rrc_freq_all_prop_conf ( sock_rrm_t *s_rrc, double date, L2_ID *L2_id_mr2, 
            unsigned int Session_id, unsigned int NB_free_ch, CHANNEL_T *fr_channels   )
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc, msg_rrc_freq_all_prop_conf ( 0, *L2_id_mr2, Session_id, NB_free_ch, fr_channels, rrc_transaction));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC, RRC_FREQ_ALL_PROP_CONF,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
                
}

static void prg_rrc_init_conn_req( sock_rrm_t *s_rrc, double date, L2_ID *L2_id_mr2, unsigned int Session_id, QOS_CLASS_T QoS_class)
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc, msg_rrc_init_conn_req ( 0, *L2_id_mr2, Session_id, QoS_class, rrc_transaction));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC, RRC_INIT_CONN_REQ,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
                
}

static void prg_rrc_freq_all_prop ( sock_rrm_t *s_rrc, double date, L2_ID *L2_id_mr2, 
            unsigned int Session_id, unsigned int NB_free_ch, CHANNEL_T *fr_channels   )
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc, msg_rrc_freq_all_prop ( 0, *L2_id_mr2, Session_id, NB_free_ch, fr_channels, rrc_transaction));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC, RRC_FREQ_ALL_PROP,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
                
}

static void prg_rrc_rep_freq_all ( sock_rrm_t *s_rrc, double date, L2_ID *L2_id_mr, L2_ID *L2_id_mr2,
            unsigned int Session_id, unsigned int NB_prop_ch, CHANNEL_T *pr_channels   )
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc, msg_rrc_rep_freq_all ( 0,  *L2_id_mr, *L2_id_mr2, Session_id, NB_prop_ch, pr_channels, rrc_transaction));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC, RRC_REP_FREQ_ALL,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
                
}

static void prg_rrc_rep_freq_ack ( sock_rrm_t *s_rrc, double date,  L2_ID *L2_id_ch, L2_ID *L2_id_mr, L2_ID *L2_id_mr2,
            unsigned int Session_id, CHANNEL_T all_channel   )
{
    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,date, cnt_actdiff++, s_rrc, msg_rrc_rep_freq_ack ( 0,  *L2_id_ch, *L2_id_mr, *L2_id_mr2, Session_id, all_channel, rrc_transaction));
    pthread_mutex_unlock( &actdiff_exclu ) ;
    
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC, RRC_REP_FREQ_ACK,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;
                
}*/



/* =========================================================================== *
 *                              SCENARII                                       *
 * =========================================================================== */

/**
 * \brief Cette fonction simule le passage de IN en CH (TIMEOUT) ,
 *        ensuite  l'ouverture d'un RB, la modification et finalement la
 *        libération.
 */
static void scenario0(sock_rrm_t *s_rrc,  sock_rrm_t *s_cmm )
{
    printf("\nSCENARIO 0: ...\n\n" ) ;
// ========================= ISOLATED NODE to CLUSTERHEAD 
    prg_phy_synch_to_MR( s_rrc, 0.1 );

// ========================= Ouverture d'un RB
    prg_opening_RB( s_cmm, 2.0, &L2_id_ch,&L2_id_mr,QOS_DTCH_USER1 );
// ========================= Modification d'un RB
    prg_modifying_RB( s_cmm, 2.2 , 5, QOS_DTCH_USER2 );
// ========================= Fermeture d'un RB
    prg_releasing_RB( s_cmm, 2.5, 5 );
}

/**
 * \brief Cette fonction simule le passage de IN en CH par la reception du 
 *        SYNCH d'un MR, et ensuite  l'ouverture d'un RB
 */
static void scenario1(sock_rrm_t *s_rrc,  sock_rrm_t *s_cmm )
{
    printf("\nSCENARIO 1: ...\n\n" ) ;
// ========================= ISOLATED NODE to CLUSTERHEAD : RRC_PHY_SYNCH_TO_MR_IND
    prg_phy_synch_to_MR( s_rrc, 0.2 );

// ========================= Ouverture d'un RB
    prg_opening_RB( s_cmm, 2.0, &L2_id_ch,&L2_id_mr,QOS_DTCH_USER1 );
}

/**
 * \brief Cette fonction simule le passage de IN en CH ,
 *        puis l'attachement d'un MR et finalement l'ouverture d'un RB.
 */
static void scenario2(sock_rrm_t *s_rrc,  sock_rrm_t *s_cmm)
{
    printf("\nSCENARIO 2: ...\n\n" ) ;
// ========================= ISOLATED NODE to CLUSTERHEAD 
    prg_phy_synch_to_MR( s_rrc, 0.1 );

// ========================= Attachement d'un MR
    prg_rrc_MR_attach_ind( s_rrc, 2.0, &L2_id_mr );
    prg_rrc_MR_attach_ind( s_rrc, 3.0, &L2_id_mr2 );
    prg_rrc_MR_attach_ind( s_rrc, 4.0, &L2_id_mr3 );

// ========================= Indicateur d'une connexion établie
    prg_rrc_cx_establish_ind( s_rrc, 5.0, &L2_id_mr, L3_info_mr,IPv4_ADDR,0,0);
    prg_rrc_cx_establish_ind( s_rrc, 6.0, &L2_id_mr2, L3_info_mr2,IPv4_ADDR,0,0);
    prg_rrc_cx_establish_ind( s_rrc, 7.0, &L2_id_mr3, L3_info_mr3,IPv4_ADDR,0,0);
}

/**
 * \brief Cette fonction simule le passage de IN en MR par la reception du 
 *        SYNCH d'un CH
 */
static void scenario3(sock_rrm_t *s_rrc,  sock_rrm_t *s_cmm)
{
    printf("\nSCENARIO 3: ...\n\n" ) ;

// ========================= Attachement d'un MR

    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,0.2, cnt_actdiff++, s_rrc,msg_rrc_phy_synch_to_CH_ind(0, 1, L2_id_mr ) ) ;
    pthread_mutex_unlock( &actdiff_exclu ) ;
                
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC,RRC_PHY_SYNCH_TO_CH_IND,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;

// ========================= Connexion etablit du MR au CH
    prg_rrc_cx_establish_ind( s_rrc, 1.0, &L2_id_ch, L3_info_ch,IPv4_ADDR, 10, 20 ) ;
}

/**
 * \brief Cette fonction simule le passage de IN en CH (TIMEOUT),
 *        puis l'attachement d'un MR ,
 *        puis l'ouverture d'un RB.
 *        puis la remontee de mesures du MR au CH.
 */
static void scenario4(sock_rrm_t *s_rrc,  sock_rrm_t *s_cmm)
{
    SENSING_MEAS_T Sensing_meas[3]={
        { 15, {{0xAA,0xCC,0x33,0x55,0x11,0x00,0x22,0x00}} },
        { 20, {{0xAA,0xCC,0x33,0x55,0x22,0x00,0x22,0x00}} },
        { 10, {{0xAA,0xCC,0x33,0x55,0x33,0x00,0x22,0x00}} }
    };
    printf("\nSCENARIO 4: ...\n\n" ) ;
// ========================= ISOLATED NODE to CLUSTERHEAD 
    prg_phy_synch_to_MR( s_rrc, 0.1 );

// ========================= Attachement d'un MR
    prg_rrc_MR_attach_ind( s_rrc, 2.0 , &L2_id_mr  ) ;

// ========================= Indicateur d'une connexion établie
    prg_rrc_cx_establish_ind( s_rrc, 5.0, &L2_id_mr, L3_info_mr, IPv4_ADDR, 0, 0 ) ;

// ========================= Remontée de mesure par le RRC

    // Meas 1
    prg_rrc_sensing_meas_ind( s_rrc, 5.00, &L2_id_mr, 1, Sensing_meas );
    
    // Meas 2
    prg_rrc_sensing_meas_ind( s_rrc, 5.25, &L2_id_mr, 3, Sensing_meas );
    
    // Meas 3
    prg_rrc_sensing_meas_ind( s_rrc, 5.50, &L2_id_mr, 2, Sensing_meas );
    
    // Meas 4
    prg_rrc_sensing_meas_ind( s_rrc, 5.75, &L2_id_mr, 0, Sensing_meas );
}

/**
 * \brief Cette fonction simule le passage de IN en CH (TIMEOUT),
 *        puis l'attachement de 3 MR ,
 *        puis l'ouverture d'un RB.
 *        puis la remontee de mesures du MR au CH.
 */
static void scenario5(sock_rrm_t *s_rrc,  sock_rrm_t *s_cmm)
{
    static SENSING_MEAS_T Sensing_meas[3]={
        { 15, {{0xAA,0xCC,0x33,0x55,0x00,0x11,0x00,0x00}} },
        { 20, {{0xAA,0xCC,0x33,0x55,0x00,0x00,0x44,0x00}} },
        { 10, {{0xAA,0xCC,0x33,0x55,0x00,0x00,0x33,0x00}} }
    };
    static SENSING_MEAS_T Sensing_meas2[3]={
        { 16, {{0xAA,0xCC,0x33,0x55,0x00,0x11,0x00,0x00}} },
        { 25, {{0xAA,0xCC,0x33,0x55,0x00,0x00,0x22,0x00}} },
        { 30, {{0xAA,0xCC,0x33,0x55,0x00,0x00,0x44,0x00}} }
    };
    static SENSING_MEAS_T Sensing_meas3[3]={
        { 14, {{0xAA,0xCC,0x33,0x55,0x00,0x11,0x00,0x00}} },
        { 17, {{0xAA,0xCC,0x33,0x55,0x00,0x00,0x22,0x00}} },
        { 29, {{0xAA,0xCC,0x33,0x55,0x00,0x00,0x33,0x00}} }
    };
    
    printf("\nSCENARIO 5: ...\n\n" ) ;
// ========================= ISOLATED NODE to CLUSTERHEAD :
    prg_phy_synch_to_MR( s_rrc, 0.1 );

// ========================= Attachement d'un MR
    prg_rrc_MR_attach_ind( s_rrc, 2.0 , &L2_id_mr  ) ;
    prg_rrc_MR_attach_ind( s_rrc, 2.0 , &L2_id_mr2  ) ;
    prg_rrc_MR_attach_ind( s_rrc, 2.0 , &L2_id_mr3  ) ;

// =========================  Indicateur d'une connexion établie
    prg_rrc_cx_establish_ind( s_rrc, 5.0, &L2_id_mr, L3_info_mr, IPv6_ADDR, 0, 0 ) ;
    prg_rrc_cx_establish_ind( s_rrc, 5.0, &L2_id_mr2, L3_info_mr, IPv6_ADDR, 0, 0 ) ;
    prg_rrc_cx_establish_ind( s_rrc, 5.0, &L2_id_mr3, L3_info_mr, IPv6_ADDR, 0, 0 ) ;

// ========================= Remontée de mesure par le RRC

    // Meas 1
    prg_rrc_sensing_meas_ind( s_rrc, 5.10, &L2_id_mr, 1, Sensing_meas );
    prg_rrc_sensing_meas_ind( s_rrc, 5.10, &L2_id_mr2, 1, Sensing_meas2 );
    prg_rrc_sensing_meas_ind( s_rrc, 5.10, &L2_id_mr3, 1, Sensing_meas3 );
    
    // Meas 2
    prg_rrc_sensing_meas_ind( s_rrc, 5.25, &L2_id_mr, 2, Sensing_meas );
    prg_rrc_sensing_meas_ind( s_rrc, 5.25, &L2_id_mr2, 2, Sensing_meas2 );
    prg_rrc_sensing_meas_ind( s_rrc, 5.25, &L2_id_mr3, 2, Sensing_meas3 );
    
    // Meas 3
    prg_rrc_sensing_meas_ind( s_rrc, 5.50, &L2_id_mr, 3, Sensing_meas );
    prg_rrc_sensing_meas_ind( s_rrc, 5.50, &L2_id_mr2, 3, Sensing_meas2 );
    prg_rrc_sensing_meas_ind( s_rrc, 5.50, &L2_id_mr3, 3, Sensing_meas3 );
    
}

/**
 * \brief Cette fonction simule le passage de IN en CH (TIMEOUT) ,
 *        ensuite  l'ouverture d'un RB, la modification et finalement la
 *        libération.
 */
static void scenario6(sock_rrm_t *s_rrc,  sock_rrm_t *s_cmm )
{
    
    static MAC_RLC_MEAS_T Meas1_CH= { .Rssi=25 , .Sinr={ 1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16}, .Harq_delay =45,
                              .Bler=1234,  .rlc_sdu_buffer_occ=13,.rlc_sdu_loss_indicator=25000};
    static MAC_RLC_MEAS_T Meas2_CH= { .Rssi=15 , .Sinr={ 1,2,3,4, 5,6,7,8, 9,10,11,12, 13,14,15,16}, .Harq_delay =35,
                              .Bler=4,  .rlc_sdu_buffer_occ=13,.rlc_sdu_loss_indicator=35000};
                              
    static MAC_RLC_MEAS_T Meas1_MR= { .Rssi=35 , .Sinr={ 11,12,13,14, 15,16,17,18, 19,20,21,22, 23,24,25,26}, .Harq_delay =55,
                              .Bler=2134,  .rlc_sdu_buffer_occ=23,.rlc_sdu_loss_indicator=15000};

    static MAC_RLC_MEAS_T Meas2_MR= { .Rssi=45 , .Sinr={ 11,12,13,14, 15,16,17,18, 19,20,21,22, 23,24,25,26}, .Harq_delay =25,
                              .Bler=3000,  .rlc_sdu_buffer_occ=11,.rlc_sdu_loss_indicator=300};
    printf("\nSCENARIO 6: ...\n\n" ) ;
// ========================= ISOLATED NODE to CLUSTERHEAD 
    prg_phy_synch_to_MR( s_rrc, 0.1 );

// ========================= Ouverture d'un RB
    prg_opening_RB( s_cmm, 2.0, &L2_id_ch,&L2_id_mr,QOS_DTCH_USER1 );
    

    prg_rrc_rb_meas_ind( s_rrc, 2.5, 4 , &L2_id_mr,PERIODIC,&Meas1_MR )  ;
    prg_rrc_rb_meas_ind( s_rrc, 2.5, 4 , &L2_id_ch,PERIODIC,&Meas1_CH )  ;

    prg_rrc_rb_meas_ind( s_rrc, 2.7, 4 , &L2_id_ch,PERIODIC,&Meas2_CH )  ;
    prg_rrc_rb_meas_ind( s_rrc, 2.7, 4 , &L2_id_mr,PERIODIC,&Meas2_MR )  ;

}

/**
 * \brief Cette fonction simule la phase de sensing du point de vue du FC,
 *        - l'attachement de 3 sensors,
 *        - le comancement du processus de sensing,
 *        - la remontee de mesures des sensors.
 */
static void scenario7(sock_rrm_t *s_rrc,  sock_rrm_t *s_cmm)
{
    static Sens_ch_t Sensing_meas[3]={
        { 100.1, 200, 1, 13.5, 0, NULL },
        { 200.1, 300, 2,  7.4, 1, NULL },
        { 300.1, 400, 3,  8.5, 1, NULL }
        
    };
    
    static Sens_ch_t Sensing_meas2[3]={
        { 100.1, 200, 1, 23.5, 0, NULL },
        { 200.1, 300, 2, 27.4, 0, NULL },
        { 300.1, 400, 3, 28.5, 0, NULL }
        
    };
    
    unsigned int occ_channels [2]= {1, 3};
    printf("\nSCENARIO 7: ...\n\n" ) ;
    
// ========================= ISOLATED NODE to CLUSTERHEAD :
    prg_phy_synch_to_MR( s_rrc, 0.1 );

// ========================= Attachement d'un MR
    prg_rrc_MR_attach_ind( s_rrc, 2.0 , &L2_id_mr  ) ;
    prg_rrc_MR_attach_ind( s_rrc, 2.0 , &L2_id_mr2  ) ;
    prg_rrc_MR_attach_ind( s_rrc, 2.0 , &L2_id_mr3  ) ;

// ========================= Starting sensing operation

    prg_cmm_init_sensing( s_cmm,  1, Start_fr,Stop_fr,Meas_band,Meas_tpf,Nb_channels,Overlap,Sampl_freq);
// ========================= Remontée de mesure par le RRC

    prg_rrc_update_sens( s_rrc, 5.10, &L2_id_mr,3, Sensing_meas );
    prg_rrc_update_sens( s_rrc, 5.20, &L2_id_mr2,3, Sensing_meas );
    prg_rrc_update_sens( s_rrc, 5.30, &L2_id_mr3,3, Sensing_meas );
    //prg_rrc_update_sens( s_rrc, 5.50, &L2_id_mr,2, Sensing_meas2 ); // answer to mon request
    
// ========================= Frequency query
    //prg_rrc_ask_for_freq( s_rrc, 5.80, &L2_id_bts, 0);
    
    //prg_rrc_update_SN_freq( s_rrc, 6, &L2_id_bts, 2, occ_channels);
// ========================= End sensing operation

    prg_cmm_stop_sensing( s_cmm, 5.60);
    //prg_rrc_end_scan_conf( s_rrc, 5.80, &L2_id_mr);
    //prg_rrc_end_scan_conf( s_rrc, 5.90, &L2_id_mr2);
    //prg_rrc_end_scan_conf( s_rrc, 6.0,  &L2_id_mr3);
  
}


/**
 * \brief Cette fonction simule la phase de sensing du point de vue du sensor,
 *        - l'attachement de 3 sensors,
 *        - le comancement du processus de sensing,
 *        - la remontee de mesures des sensors.
 */
static void scenario8(sock_rrm_t *s_rrc,  sock_rrm_t *s_cmm)
{
    static Sens_ch_t Sensing_meas[3]={
        { 100, 200, 1, 13.5, 0, NULL },
        { 200, 300, 2,  7.4, 0, NULL },
        { 300, 400, 3,  8.5, 0, NULL }
        
    };
    static Sens_ch_t Sensing_meas2[3]={
        { 100, 200, 1, 23.5, 0, NULL },
        { 200, 300, 2, 27.4, 0, NULL },
        { 300, 400, 3, 28.5, 0, NULL }
        
    };
    
    /*static SENSING_MEAS_T Sensing_meas2[3]={
        { 16, {{0xAA,0xCC,0x33,0x55,0x00,0x11,0x00,0x00}} },
        { 25, {{0xAA,0xCC,0x33,0x55,0x00,0x00,0x22,0x00}} },
        { 30, {{0xAA,0xCC,0x33,0x55,0x00,0x00,0x44,0x00}} }
    };
    static SENSING_MEAS_T Sensing_meas3[3]={
        { 14, {{0xAA,0xCC,0x33,0x55,0x00,0x11,0x00,0x00}} },
        { 17, {{0xAA,0xCC,0x33,0x55,0x00,0x00,0x22,0x00}} },
        { 29, {{0xAA,0xCC,0x33,0x55,0x00,0x00,0x33,0x00}} }
    };*/
    
    printf("\nSCENARIO 8: ...\n\n" ) ;

// ========================= Attachement d'un senseur

    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,0.2, cnt_actdiff++, s_rrc,msg_rrc_phy_synch_to_CH_ind(0, 1, L2_id_ch ) ) ;
    pthread_mutex_unlock( &actdiff_exclu ) ;
                
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC,RRC_PHY_SYNCH_TO_CH_IND,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;

// ========================= Connexion etablit du MR au CH
    prg_rrc_cx_establish_ind( s_rrc, 1.0, &L2_id_ch, L3_info_ch,IPv4_ADDR, 10, 20 ) ;

// ========================= Starting sensing operation
    unsigned int interv= 1;
    float date = 1.5;
    prg_rrc_init_scan_req( s_rrc, date, &L2_id_ch,Start_fr,Stop_fr,Meas_band,Meas_tpf,Nb_channels,Overlap,Sampl_freq);
    //prg_cmm_init_sensing( s_cmm, 5.0, Start_fr,Stop_fr,Meas_band,Meas_tpf,Nb_channels,Overlap,Sampl_freq );
// ========================= Remontée de mesure par le RRC
    /*for (int i=0; i<3;i++){
        date+=interv;
        prg_rrc_update_sens( s_rrc, date, &L2_id_mr,3, Sensing_meas );
    }*/
// ========================= Monitoring
    /*unsigned int ch_to_scan[3]={1,2};
    unsigned int NB_chan= 2;
    date+=interv;
    prg_rrc_init_mon_req( s_rrc, date, &L2_id_ch, ch_to_scan, NB_chan, interv );
    date+=interv;
    prg_rrc_update_sens( s_rrc, date, &L2_id_mr,NB_chan, Sensing_meas2 );*/
    
// ========================= End of sensing
    date+=interv;
    prg_rrc_end_scan_req( s_rrc, date, &L2_id_ch );
    
  
}


/**
 * \brief BTS actions in SENDORA first scenario,
 *        
 */
static void scenario9(sock_rrm_t *s_rrc,  sock_rrm_t *s_cmm)
{
    static Sens_ch_t Sensing_meas[3]={
        { 100.1, 200, 1, 13.5, 0, NULL },
        { 200.1, 300, 2,  7.4, 0, NULL },
        { 300.1, 400, 3,  8.5, 0, NULL }
        
    };
    
    /*static CHANNEL_T Free_frequencies[3]={
        { 100, 200, 1, 1 },
        { 200, 300, 2, 2 },
        { 300, 400, 3, 3 }
        
    };*/
    
    
    printf("\nSCENARIO 9: ...\n\n" ) ;
// ========================= ISOLATED NODE to CLUSTERHEAD (FC) :
    prg_phy_synch_to_MR( s_rrc, 0.1 );
    
//  ========================= Attachement d'un senseur
    prg_rrc_MR_attach_ind( s_rrc, 2.0 , &L2_id_mr  ) ;

// ========================= Attachement de la BTS
    prg_rrc_MR_attach_ind( s_rrc, 2.0 , &L2_id_bts  ) ;

// ========================= Starting frequency query
    /*if (SCEN_1)
        prg_cmm_ask_freq ( s_cmm, 2.5 );
    if (SCEN_2_CENTR)
        prg_cmm_need_to_tx( s_cmm, 2.5, 1);
    prg_rrc_open_freq ( s_rrc, 4.0, &L2_id_fc, 3, Free_frequencies );
    */
}

/**
 * \brief CH actions in SENDORA second scenario centralized
 */
static void scenario10(sock_rrm_t *s_rrc,  sock_rrm_t *s_cmm)
{
    static Sens_ch_t Sensing_meas[3]={
        { 100.1, 200, 1, 13.5, 0, NULL },
        { 200.1, 300, 2,  7.4, 1, NULL },
        { 300.1, 400, 3,  8.5, 1, NULL }
        
    };
    
    static Sens_ch_t Sensing_meas2[3]={
        { 100.1, 200, 1, 23.5, 0, NULL },
        { 200.1, 300, 2, 27.4, 0, NULL },
        { 300.1, 400, 3, 28.5, 0, NULL }
        
    };
    
    unsigned int occ_channels [2]= {1, 3};
    printf("\nSCENARIO 7: ...\n\n" ) ;
// ========================= ISOLATED NODE to CLUSTERHEAD :
    prg_phy_synch_to_MR( s_rrc, 0.1 );

// ========================= Attachement d'un MR
    prg_rrc_MR_attach_ind( s_rrc, 2.0 , &L2_id_mr  ) ;
    prg_rrc_MR_attach_ind( s_rrc, 2.0 , &L2_id_mr2  ) ;
    //prg_rrc_MR_attach_ind( s_rrc, 2.0 , &L2_id_mr3  ) ;

// ========================= Starting sensing operation
    //prg_rrc_clust_scan_req(s_rrc, 5.0, &L2_id_ch, 0.5, 1);
    
// ========================= Remontée de mesure par le RRC

    prg_rrc_update_sens( s_rrc, 7.10, &L2_id_mr,3, Sensing_meas );
    prg_rrc_update_sens( s_rrc, 7.20, &L2_id_mr2,3, Sensing_meas );
    //prg_rrc_update_sens( s_rrc, 5.30, &L2_id_mr3,3, Sensing_meas );
    
// ========================= Monitoring
    unsigned int ch_to_scan[3]={1,2};
    unsigned int NB_chan= 2;
    unsigned int interv = 0.5;
    unsigned int date = 8.0;
    //date+=interv;
    //prg_rrc_clust_mon_req( s_rrc, date, &L2_id_ch, ch_to_scan, NB_chan, interv );
    date+=(interv*4);
    prg_rrc_update_sens( s_rrc, date, &L2_id_mr,NB_chan, Sensing_meas2 );
    date+=interv;
    prg_rrc_update_sens( s_rrc, date, &L2_id_mr2,NB_chan, Sensing_meas2 );
    

// ========================= End sensing operation
    prg_rrc_end_scan_req( s_rrc, 8.0, &L2_id_ch );
    //prg_cmm_stop_sensing( s_cmm, 5.60);
    //prg_rrc_end_scan_conf( s_rrc, 9.80, &L2_id_mr);
    //prg_rrc_end_scan_conf( s_rrc, 9.90, &L2_id_mr2);
    //prg_rrc_end_scan_conf( s_rrc, 6.0,  &L2_id_mr3);
  
}


/**
 * \brief This function simulate the connection of two SUs from the first SU point of view
 *        
 */
static void scenario11(sock_rrm_t *s_rrc,  sock_rrm_t *s_cmm)
{
    static Sens_ch_t Sensing_meas[4]={
        { 100.1, 200, 1,  3.5, 1, NULL },
        { 200.1, 300, 2,  7.4, 1, NULL },
        { 300.1, 400, 3,  8.5, 0, NULL },
        { 400.1, 500, 4,  2.5, 1, NULL }
        
    };
    static Sens_ch_t Sensing_meas2[3]={
        { 100.1, 200, 1, 23.5, 0, NULL },
        { 200.1, 300, 2, 27.4, 0, NULL },
        { 300.1, 400, 3, 28.5, 0, NULL }
        
    };
    /*static CHANNEL_T Free_frequencies[3]={
        { 100.1, 200, 1, 0 },
        { 200.1, 300, 2, 0 },
        { 400.1, 500, 4, 0 }
        
    };*/
    
    printf("\nSCENARIO 11: ...\n\n" ) ;

// ========================= Starting sensing operation
    unsigned int interv= 1;
    float date = 1.5;
    unsigned int Session_id = 7;
    prg_rrc_init_scan_req( s_rrc, date, &L2_id_ch,Start_fr,Stop_fr,Meas_band,Meas_tpf,Nb_channels,Overlap,Sampl_freq);
    //prg_cmm_init_sensing( s_cmm, 5.0, Start_fr,Stop_fr,Meas_band,Meas_tpf,Nb_channels,Overlap,Sampl_freq );
// ========================= Remontée de mesure par le RRC
    for (int i=0; i<3;i++){
        date+=interv;
        prg_rrc_update_sens( s_rrc, date, &L2_id_mr,4, Sensing_meas );
    }
// ========================= Transaction
   /* date+=interv;
    prg_cmm_init_trans_req( s_cmm, date, &L2_id_mr2, Session_id, 1);
    date+=interv;
    prg_rrc_init_conn_conf ( s_rrc, date, &L2_id_mr2, Session_id );
    date+=interv;
    prg_rrc_freq_all_prop_conf ( s_rrc, date, &L2_id_mr2, Session_id, 3, Free_frequencies);
*/
// ========================= End of sensing
    date+=interv;
    prg_rrc_end_scan_req( s_rrc, date, &L2_id_ch );
    
  
}

/**
 * \brief This function simulate the connection of two SUs from the second SU point of view
 *        
 */
static void scenario12(sock_rrm_t *s_rrc,  sock_rrm_t *s_cmm)
{
    static Sens_ch_t Sensing_meas[4]={
        { 100.1, 200, 1,  3.5, 1, NULL },
        { 200.1, 300, 2,  7.4, 0, NULL },
        { 300.1, 400, 3,  8.5, 0, NULL },
        { 400.1, 500, 4,  2.5, 1, NULL }
        
    };
    
   /*static CHANNEL_T Free_frequencies[3]={
        { 100.1, 200, 1, 0 },
        { 200.1, 300, 2, 0 },
        { 400.1, 500, 4, 0 }
        
    };*/
    
    printf("\nSCENARIO 12: ...\n\n" ) ;

// ========================= Starting sensing operation
    unsigned int interv= 1;
    float date = 1.5;
    unsigned int Session_id = 7;
    prg_rrc_init_scan_req( s_rrc, date, &L2_id_ch,Start_fr,Stop_fr,Meas_band,Meas_tpf,Nb_channels,Overlap,Sampl_freq);
    //prg_cmm_init_sensing( s_cmm, 5.0, Start_fr,Stop_fr,Meas_band,Meas_tpf,Nb_channels,Overlap,Sampl_freq );
// ========================= Remontée de mesure par le RRC
    for (int i=0; i<3;i++){
        date+=interv;
        prg_rrc_update_sens( s_rrc, date, &L2_id_mr,4, Sensing_meas );
    }
// ========================= Transaction
    /*date+=interv;
    prg_rrc_init_conn_req( s_rrc, date, &L2_id_mr2, Session_id, 1);
    date+=interv;
    prg_rrc_freq_all_prop ( s_rrc, date, &L2_id_mr2, Session_id, 3, Free_frequencies);
    date+=interv;
    prg_rrc_rep_freq_ack ( s_rrc, date,  &L2_id_ch, &L2_id_mr2, &L2_id_mr,
            Session_id, Free_frequencies[2]   );
    */
// ========================= End of sensing
    date+=interv;
    prg_rrc_end_scan_req( s_rrc, date, &L2_id_ch );
    
  
}

/**
 * \brief This function simulate the connection of two SUs from the CH point of view
 *        
 */
static void scenario13(sock_rrm_t *s_rrc,  sock_rrm_t *s_cmm)
{
    static Sens_ch_t Sensing_meas[4]={
        { 100.1, 200, 1,  3.5, 0, NULL },
        { 200.1, 300, 2,  7.4, 1, NULL },
        { 300.1, 400, 3,  8.5, 0, NULL },
        { 400.1, 500, 4,  2.5, 1, NULL }
        
    };
    
    /*static CHANNEL_T Free_frequencies[3]={
        { 100.1, 200, 1, 0 },
        { 200.1, 300, 2, 0 },
        { 400.1, 500, 4, 0 }
        
    };*/
    
    printf("\nSCENARIO 13: ...\n\n" ) ;

// ========================= ISOLATED NODE to CLUSTERHEAD :
    prg_phy_synch_to_MR( s_rrc, 0.1 );

// ========================= Attachement d'un MR
    prg_rrc_MR_attach_ind( s_rrc, 2.0 , &L2_id_mr  ) ;
    //prg_rrc_MR_attach_ind( s_rrc, 2.0 , &L2_id_mr2  ) ;
    //prg_rrc_MR_attach_ind( s_rrc, 2.0 , &L2_id_mr3  ) ;
// ========================= Starting sensing operation
    unsigned int interv= 1;
    float date = 1.5;
    unsigned int Session_id = 7;
    prg_rrc_init_scan_req( s_rrc, date, &L2_id_ch,Start_fr,Stop_fr,Meas_band,Meas_tpf,Nb_channels,Overlap,Sampl_freq);
    //prg_cmm_init_sensing( s_cmm, 5.0, Start_fr,Stop_fr,Meas_band,Meas_tpf,Nb_channels,Overlap,Sampl_freq );
// ========================= Remontée de mesure par le RRC
    for (int i=0; i<3;i++){
        date+=interv;
        prg_rrc_update_sens( s_rrc, date, &L2_id_mr,4, Sensing_meas );
    }
// ========================= Transaction
   /* date+=interv;
    prg_rrc_rep_freq_all ( s_rrc, date, &L2_id_mr, &L2_id_mr2,
            Session_id, 3, Free_frequencies   );
    */
// ========================= End of sensing
    //date+=interv;
    //prg_rrc_end_scan_req( s_rrc, date, &L2_id_ch );
    
  
}

/**
 * \brief Sensor node simulation:
 *      - the node is connected to FC
 *      - the node receives the init scan request order
 *      - the node receive the end scan order
 */
static void scenario14(sock_rrm_t *s_rrc,  sock_rrm_t *s_cmm, sock_rrm_t *s_sns)
{
    static Sens_ch_t Sensing_meas[3]={
        { 100, 200, 1, 13.5, 0, NULL },
        { 200, 300, 2,  7.4, 0, NULL },
        { 300, 400, 3,  8.5, 0, NULL }
        
    };
    static Sens_ch_t Sensing_meas2[3]={
        { 100, 200, 1, 23.5, 0, NULL },
        { 200, 300, 2, 27.4, 0, NULL },
        { 300, 400, 3, 28.5, 0, NULL }
        
    };
    
    /*static SENSING_MEAS_T Sensing_meas2[3]={
        { 16, {{0xAA,0xCC,0x33,0x55,0x00,0x11,0x00,0x00}} },
        { 25, {{0xAA,0xCC,0x33,0x55,0x00,0x00,0x22,0x00}} },
        { 30, {{0xAA,0xCC,0x33,0x55,0x00,0x00,0x44,0x00}} }
    };
    static SENSING_MEAS_T Sensing_meas3[3]={
        { 14, {{0xAA,0xCC,0x33,0x55,0x00,0x11,0x00,0x00}} },
        { 17, {{0xAA,0xCC,0x33,0x55,0x00,0x00,0x22,0x00}} },
        { 29, {{0xAA,0xCC,0x33,0x55,0x00,0x00,0x33,0x00}} }
    };*/
    
    printf("\nSCENARIO 14: ...\n\n" ) ;

// ========================= Attachement d'un senseur

    rrc_transaction++;
    pthread_mutex_lock( &actdiff_exclu  ) ;
    add_actdiff(&list_actdiff,0.2, cnt_actdiff++, s_rrc,msg_rrc_phy_synch_to_CH_ind(0, 1, L2_id_ch ) ) ;
    pthread_mutex_unlock( &actdiff_exclu ) ;
                
    pthread_mutex_lock( &rrc_transact_exclu ) ;
    add_item_transact( &rrc_transact_list, rrc_transaction, INT_RRC,RRC_PHY_SYNCH_TO_CH_IND,0,NO_PARENT);
    pthread_mutex_unlock( &rrc_transact_exclu ) ;

// ========================= Connexion etablit du MR au CH
    prg_rrc_cx_establish_ind( s_rrc, 1.0, &L2_id_ch, L3_info_ch,IPv4_ADDR, 10, 20 ) ;

// ========================= Starting sensing operation
    unsigned int interv= 1;
    float date = 1.5;
    prg_rrc_init_scan_req( s_rrc, date, &L2_id_ch,Start_fr,Stop_fr,Meas_band,Meas_tpf,Nb_channels,Overlap,Sampl_freq);
    //prg_cmm_init_sensing( s_cmm, 5.0, Start_fr,Stop_fr,Meas_band,Meas_tpf,Nb_channels,Overlap,Sampl_freq );
// ========================= Remontée de mesure par le RRC
    /*for (int i=0; i<3;i++){
        date+=interv;
        prg_rrc_update_sens( s_rrc, date, &L2_id_mr,3, Sensing_meas );
    }*/
// ========================= Monitoring
    /*unsigned int ch_to_scan[3]={1,2};
    unsigned int NB_chan= 2;
    date+=interv;
    prg_rrc_init_mon_req( s_rrc, date, &L2_id_ch, ch_to_scan, NB_chan, interv );
    date+=interv;
    prg_rrc_update_sens( s_rrc, date, &L2_id_mr,NB_chan, Sensing_meas2 );*/
    
// ========================= End of sensing
    date+=interv;
     printf( "prima sns socket -> %d\n" , s_sns->s ) ;//dbg
    prg_rrc_end_scan_req( s_rrc, date, &L2_id_ch );
     printf( "dopo sns socket -> %d\n" , s_sns->s ) ;//dbg  
  
}



void scenario(int num , sock_rrm_t *s_rrc,  sock_rrm_t *s_cmm,  sock_rrm_t *s_sns )
{
    switch ( num )
    {
        case 0 : scenario0(s_rrc,  s_cmm ) ; break ;
        case 1 : scenario1(s_rrc,  s_cmm ) ; break ;
        case 2 : scenario2(s_rrc,  s_cmm ) ; break ;
        case 3 : scenario3(s_rrc,  s_cmm ) ; break ;
        case 4 : scenario4(s_rrc,  s_cmm ) ; break ;
        case 5 : scenario5(s_rrc,  s_cmm ) ; break ;
        case 6 : scenario6(s_rrc,  s_cmm ) ; break ;
        case 7 : scenario7(s_rrc,  s_cmm ) ; break ;
        case 8 : scenario8(s_rrc,  s_cmm ) ; break ;
        case 9 : scenario9(s_rrc,  s_cmm ) ; break ;
        case 10 : scenario10(s_rrc,  s_cmm ) ; break ;
        case 11 : scenario11(s_rrc,  s_cmm ) ; break ;
        case 12 : scenario12(s_rrc,  s_cmm ) ; break ;
        case 13 : scenario13(s_rrc,  s_cmm ) ; break ;
        //mod_lor_10_04_15 -> from here scenarios with sensing unit included 
        case 14 : scenario14(s_rrc,  s_cmm, s_sns ) ; break ;
        default:
            fprintf( stderr,"Erreur : '%d' => Numero de test inconnu\n" , num ) ;
    }
}
