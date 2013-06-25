/***************************************************************************
                          rrc_L2_proto.h  -  description
                             -------------------
    begin                : Sept 9, 2008
    copyright            : (C) 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  Prototypes related to L2 interface functions
 ***************************************************************************/
#ifndef __RRC_L2_PROTO_H__
#define __RRC_L2_PROTO_H__

// rrc_L2_interfaces.c
s8 rrc_L2_data_req_rx (unsigned char Mod_id, unsigned short Srb_id, unsigned char Nb_tb,char *Buffer,u8 CH_index);
//void rrc_L2_mac_data_ind_rx (void);
// s8 mac_rrc_data_ind(u8 Mod_id, u32 frame, u16 Srb_id, s8 *Sdu,u16 Sdu_len,u8 eNB_flag, u8 eNB_index ); 
s8 rrc_L2_mac_data_ind_rx (u8 Mod_id, u16 Srb_id, char *Sdu, u16 Sdu_len, u8 eNB_index);


void rrc_L2_rlc_data_ind_rx (unsigned char Mod_id, unsigned int Srb_id, unsigned int Sdu_size, unsigned char *Buffer);
void rrc_L2_mac_meas_ind_rx (void);
void rrc_L2_def_meas_ind_rx (unsigned char Mod_id, unsigned char Idx2);
void rrc_L2_sync_ind_rx (u8 Mod_id);
void rrc_L2_out_sync_ind_rx (void);

void rrc_init_mac_config(void);

int rrc_L2_get_rrc_status(u8 Mod_id,u8 eNB_flag,u8 index);
char rrc_L2_ue_init(u8 Mod_id, unsigned char eNB_index);
char rrc_L2_eNB_init(u8 Mod_id);

//void openair_rrc_lite_top_init(void);




#ifdef NODE_MT
//rrc_ue_init.c
void rrc_ue_init (u8 Mod_id);
char rrc_ue_rglite_init(u8 Mod_id, unsigned char eNB_index);

//rrc_ue_main.c
//void rrc_ue_main_scheduler (u8 Mod_id);
int rrc_ue_main_scheduler(u8 Mod_id,u32 frame, u8 eNB_flag,u8 index);
#endif


#ifdef NODE_RG
//rrc_rg_init.c
void rrc_rg_init (u8 Mod_id);
char rrc_rg_uelite_init(u8 Mod_id, unsigned char eNB_index);

//rrc_rg_main.c
//void rrc_rg_main_scheduler (u8 Mod_id);
int rrc_rg_main_scheduler(u8 Mod_id,u32 frame, u8 eNB_flag,u8 index);
#endif

#endif

