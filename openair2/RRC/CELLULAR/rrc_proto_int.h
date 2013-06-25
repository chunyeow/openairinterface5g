/***************************************************************************
                          rrc_proto_int.h  -  description
                             -------------------
    begin                : Wed Aug 1 2001
    copyright            : (C) 2001, 2010 by Eurecom
    email                : Lionel.Gauthier@eurecom.fr
 ***************************************************************************/
#ifndef __RRC_PROTO_INT_H__
#define __RRC_PROTO_INT_H__

/**************************************************************/
#ifdef NODE_RG
// #include "protocol_pool_bs.h"
// extern void protocol_pool_bs_init (struct protocol_pool_bs *protP);
// extern void *rrc_init_bs (void *arg);
extern void rrc_rg_rxtx (void);
extern void rrc_rg_nas_xmit_enqueue (mem_block_t * p);
extern int  rrc_rg_read_DCin_FIFO (int UE_Id, u8 *buffer, int count);
extern int  rrc_rg_read_GC_FIFO (u8 *buffer, int count);
extern int  rrc_rg_read_NT_FIFO (u8 *buffer, int count);
//extern void rrc_rg_srb0_decode (mem_block_t * sduP, int length);
extern void rrc_rg_srb0_decode (char * sduP, int length);
extern void rrc_rg_srb1_decode (int UE_Id, char * sduP, int length);
extern void rrc_rg_srb2_decode (int UE_Id, char * sduP, int length);
extern void rrc_rg_srb3_decode (int UE_Id, char * sduP, int length);
extern void RRC_RG_O_NAS_DATA_IND (int UE_Id);
void RRC_RG_O_O_SEND_DCCH_AM (int msgId);
extern void RRC_RG_SEND_AM_SRB3 (int msgId);
int rrc_rg_send_to_srb_rlc (int UE_id, int rb_id, char * data_buffer, int data_length);
extern void rrc_rg_L1_check (void);
// extern int  rrc_rg_compute_configuration (int UE_Id, u8 action);
extern void rrc_process_sib18 (void);
// void rrc_rg_print_meas_bs_control (struct rrc_rg_bs_meas_cmd *p);
void rrc_rg_print_meas_report (struct rrc_rg_mt_meas_rep *p);
void rrc_rg_print_bs_meas_report (struct rrc_rg_bs_meas_rep *p);
void rrc_rg_send_bs_meas_report (int index);
void rrc_rg_sync_measures (int time);
void rrc_rg_fwd_meas_report (int UE_Id);
void rrc_rg_temp_checkConnection(int UE_Id);

// rrc_rg_nas_intf.c
void rrc_rg_write_FIFO (mem_block_t *p);
void rrc_rg_sap_init (void);
#endif
/**************************************************************/
#ifdef NODE_MT
// #include "protocol_pool_ms.h"
// #include "rrm_config_structs.h"
// extern void protocol_pool_ms_init (struct protocol_pool_ms *protP);
// extern void *rrc_init_ms (void *arg);
// void rrc_ue_write_FIFO (mem_block_t * p);
extern void rrc_ue_rxtx (void);
extern void rrc_ue_nas_xmit_enqueue (mem_block_t * p);
extern void rrc_ue_broadcast_encode_nas_sib1 (void);
extern void rrc_ue_broadcast_encode_nas_sib18 (void);
extern void rrc_ue_read_neighboring_cells_info (void);
//extern int  rrc_ue_read_DCin_FIFO (void);
extern void rrc_ue_read_DCin_FIFO (struct nas_ue_dc_element *p, int count);
extern void rrc_ue_t300_timeout (void);
//extern void rrc_ue_srb0_decode (u8 * dataP, u16 length);
//       void rrc_ue_srb0_decode (mem_block_t * sduP, int length);
extern void rrc_ue_srb0_decode (char * sduP, int length);
extern void rrc_ue_srb1_decode (char * sduP, int length);
extern void rrc_ue_srb2_decode (char * sduP, int length);
extern void rrc_ue_srb3_decode (char * sduP, int length);
extern void RRC_UE_O_NAS_NOTIFICATION_IND (void);
extern void RRC_UE_O_NAS_DATA_IND (void);
extern void RRC_UE_O_NAS_MEASUREMENT_IND (void);
extern void RRC_UE_SEND_AM_SRB3 (int msgId);

// L2 Frontend
int rrc_ue_send_to_srb_rlc (int rb_id, char * data_buffer, int data_length);

//rrc_ue_control.c
void rrc_ue_L1_check (void);
void rrc_ue_print_meas_control (struct rrc_ue_meas_cmd *p);
void rrc_ue_sync_measures (int time, int *message_id);
void rrc_ue_trigger_measure (void);

// rrc_ue_nas_intf.c
void rrc_ue_write_FIFO (mem_block_t *p);
#ifdef RRC_NETLINK
#else
void rrc_ue_sap_init (void);
#endif

#endif
/**************************************************************/

// all nodes
void rrc_create_fifo (char *rrc_fifo);

void rrc_print_buffer (char *buffer, int length);
void rrc_compress_buffer (char *src_buffer, int src_length, char *dest_buffer, int *dest_length);
void rrc_uncompress_buffer (char *src_buffer, int src_length, char *dest_buffer, int *dest_length);
void rrc_compress_config (MT_CONFIG * config, char *dest_buffer, int *dest_length);
void rrc_uncompress_config (MT_CONFIG * config, char *source_buffer);

#endif
