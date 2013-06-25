/*________________________openair_rrc_L2_interface.c________________________

 Authors : Hicham Anouar
 Company : EURECOM
 Emails  : anouar@eurecom.fr
________________________________________________________________*/

#ifdef USER_MODE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#else //USER_MODE

#include <asm/io.h>
#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/segment.h>
#include <asm/page.h>
#include <asm/delay.h>

#include <linux/init.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/mm.h>
#include <linux/mman.h>

#include <linux/slab.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/fs.h>

#include <linux/errno.h>


#ifdef KERNEL2_6

#include <linux/slab.h>
#endif

#ifdef KERNEL2_4
#include <linux/malloc.h>
#include <linux/wrapper.h>
#endif
#endif //USER_MODE

#include "openair_types.h"
#include "openair_defs.h"

#ifdef CELLULAR
#include "rrc_L2_proto.h"
#else
#include "RRC/LITE/defs.h"
#endif //CELLULAR
#include "COMMON/mac_rrc_primitives.h"
#include "openair_rrc_L2_interface.h"
 
/********************************************************************************************************************/
s8 mac_rrc_data_req(u8 Mod_id, u32 frame, u16 Srb_id, u8 Nb_tb,char *Buffer,u8 eNB_flag,u8 eNB_index){
/********************************************************************************************************************/
#ifdef CELLULAR
  return(rrc_L2_data_req_rx(Mod_id,Srb_id,Nb_tb,Buffer,eNB_index));
#else 
  return(mac_rrc_lite_data_req(Mod_id,frame,Srb_id,Nb_tb,Buffer,eNB_flag,eNB_index));
#endif //CELLULAR
}   
   
/********************************************************************************************************************/
s8 mac_rrc_data_ind(u8 Mod_id, u32 frame, u16 Srb_id, char *Sdu,u16 Sdu_len,u8 eNB_flag, u8 eNB_index ){ 
/********************************************************************************************************************/
#ifdef CELLULAR
  return(rrc_L2_mac_data_ind_rx(Mod_id, Srb_id, Sdu, Sdu_len, eNB_index));
#else 
  return(mac_rrc_lite_data_ind(Mod_id,frame,Srb_id,Sdu,Sdu_len,eNB_flag,eNB_index));
#endif //CELLULAR
}

/********************************************************************************************************************/
void rlcrrc_data_ind( u8 Mod_id, u32 frame, u8 eNB_flag, unsigned int Srb_id, unsigned int Sdu_size,u8 *Buffer){
/********************************************************************************************************************/
#ifdef CELLULAR
  rrc_L2_rlc_data_ind_rx(Mod_id,Srb_id, Sdu_size, Buffer);
#else  // now this is called from PDCP
  //rlcrrc_lite_data_ind(Mod_id,frame,eNB_flag,Srb_id,Sdu_size,Buffer);
  rrc_lite_data_ind(Mod_id,frame,eNB_flag,Srb_id,Sdu_size,Buffer);
#endif //CELLULAR
}

/********************************************************************************************************************/
void pdcp_rrc_data_ind( u8 Mod_id, u32 frame, u8 eNB_flag, unsigned int Srb_id, unsigned int Sdu_size,u8 *Buffer){
/********************************************************************************************************************/
#ifdef CELLULAR
  rrc_L2_rlc_data_ind_rx(Mod_id,Srb_id, Sdu_size, Buffer);
#else  // now this is called from PDCP
  //rlcrrc_lite_data_ind(Mod_id,frame,eNB_flag,Srb_id,Sdu_size,Buffer);
  rrc_lite_data_ind(Mod_id,frame,eNB_flag,Srb_id,Sdu_size,Buffer);
#endif //CELLULAR
}

/********************************************************************************************************************/
void mac_rrc_meas_ind(u8 Mod_id,MAC_MEAS_REQ_ENTRY *Meas_entry){
/********************************************************************************************************************/
#ifdef CELLULAR
  rrc_L2_mac_meas_ind_rx ();
#else
  //  mac_rrc_meas_ind(Mod_id,Meas_entry);
#endif //CELLULAR
}

/********************************************************************************************************************/
void mac_sync_ind(u8 Mod_id,u8 Status){
/********************************************************************************************************************/
#ifdef CELLULAR
  rrc_L2_sync_ind_rx(Mod_id);
#else 
  mac_lite_sync_ind(Mod_id,Status);
#endif //CELLULAR
}

/********************************************************************************************************************/
void mac_in_sync_ind(u8 Mod_id,u32 frame, u16 eNB_index){
/********************************************************************************************************************/
#ifdef CELLULAR
  rrc_L2_sync_ind_rx(Mod_id);
#else
  rrc_lite_in_sync_ind(Mod_id,frame,eNB_index);
#endif
}

/********************************************************************************************************************/
void mac_out_of_sync_ind(u8 Mod_id,u32 frame, u16 eNB_index){
/********************************************************************************************************************/
#ifdef CELLULAR
  rrc_L2_out_sync_ind_rx();
#else 
  rrc_lite_out_of_sync_ind(Mod_id,frame,eNB_index);
#endif //CELLULAR
}

/********************************************************************************************************************/
int mac_get_rrc_status(u8 Mod_id,u8 eNB_flag,u8 index) {
/********************************************************************************************************************/
#ifdef CELLULAR
  return (rrc_L2_get_rrc_status(Mod_id,eNB_flag,index));
#else 
  return mac_get_rrc_lite_status(Mod_id, eNB_flag, index);
#endif //CELLULAR
}

/********************************************************************************************************************/
char openair_rrc_ue_init(u8 Mod_id, unsigned char eNB_index){
/********************************************************************************************************************/
#ifdef CELLULAR
  return (rrc_L2_ue_init(Mod_id,eNB_index));
#else 
  openair_rrc_lite_ue_init(Mod_id, eNB_index);
#endif //CELLULAR
}

/********************************************************************************************************************/
char openair_rrc_eNB_init(u8 Mod_id){
/********************************************************************************************************************/
#ifdef CELLULAR
 return( rrc_L2_eNB_init(Mod_id));
#else 
  openair_rrc_lite_eNB_init(Mod_id);
#endif //CELLULAR
}
