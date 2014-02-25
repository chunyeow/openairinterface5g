/*******************************************************************************

  Eurecom OpenAirInterface 2
  Copyright(c) 1999 - 2010 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/

/*! \file l2_interface.c
* \brief layer 2 interface 
* \author Raymond Knopp and Navid Nikaein 
* \date 2011
* \version 1.0 
* \company Eurecom
* \email: navid.nikaein@eurecom.fr 
*/ 

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
s8 mac_rrc_data_req(module_id_t module_idP, frame_t frameP, rb_id_t srb_idP, u8 nb_tbP,u8 *buffer_pP, eNB_flag_t eNB_flagP, u8 eNB_indexP, u8 mbsfn_sync_areaP){
/********************************************************************************************************************/
#ifdef CELLULAR
  return(rrc_L2_data_req_rx(module_idP,srb_idP,nb_tbP,buffer_pP,eNB_indexP));
#else 
  return(mac_rrc_lite_data_req(module_idP,frameP,srb_idP,nb_tbP,buffer_pP,eNB_flagP,eNB_indexP,mbsfn_sync_areaP));
#endif //CELLULAR
}   

/********************************************************************************************************************/
s8 mac_rrc_data_ind(module_id_t module_idP, frame_t frameP, rb_id_t srb_idP, u8 *sduP, sdu_size_t sdu_lenP, eNB_flag_t eNB_flagP, u8 eNB_indexP,u8 mbsfn_sync_area){
/********************************************************************************************************************/
#ifdef CELLULAR
  return(rrc_L2_mac_data_ind_rx(module_idP, Srb_id, Sdu, Sdu_len, eNB_indexP));
#else 
  return(mac_rrc_lite_data_ind(module_idP,frameP,srb_idP,sduP,sdu_lenP,eNB_flagP,eNB_indexP,mbsfn_sync_area));
#endif //CELLULAR
}

/********************************************************************************************************************/
void rlcrrc_data_ind(module_id_t enb_instP, module_id_t ue_instP, frame_t frameP, eNB_flag_t eNB_flagP, rb_id_t srb_idP, sdu_size_t sdu_lenP,u8 *buffer_pP){
/********************************************************************************************************************/
#ifdef CELLULAR
  rrc_L2_rlc_data_ind_rx(module_idP,Srb_id, sdu_lenP, buffer_pP);
#else  // now this is called from PDCP
  //rlcrrc_lite_data_ind(module_idP,frameP,eNB_flag,Srb_id,Sdu_size,Buffer);
  rrc_lite_data_ind(enb_instP, ue_instP,frameP,eNB_flagP,srb_idP,sdu_lenP,buffer_pP);
#endif //CELLULAR
}

/********************************************************************************************************************/
u8 pdcp_rrc_data_req(module_id_t enb_instP, module_id_t ue_instP, frame_t frameP, eNB_flag_t eNB_flagP, rb_id_t rb_idP, mui_t muiP, boolean_t confirmP,
      sdu_size_t sdu_buffer_sizeP, u8* sdu_buffer_pP, u8 modeP) {
  /********************************************************************************************************************/
#ifdef CELLULAR
  return pdcp_data_req( module_id, frameP, eNB_flag, rb_id, muiP, confirmP, sdu_buffer_sizeP, sdu_buffer_pP, modeP);
#else
  return rrc_lite_data_req (enb_instP, ue_instP, frameP, eNB_flagP, rb_idP, muiP, confirmP, sdu_buffer_sizeP, sdu_buffer_pP, modeP);
#endif
}

/********************************************************************************************************************/
void pdcp_rrc_data_ind(module_id_t enb_instP, module_id_t ue_instP, frame_t frameP, eNB_flag_t eNB_flagP, rb_id_t srb_idP, sdu_size_t sdu_sizeP,u8 *buffer_pP){
/********************************************************************************************************************/
#ifdef CELLULAR
  rrc_L2_rlc_data_ind_rx(module_idP,srb_idP, sdu_sizeP, buffer_pP);
#else  // now this is called from PDCP
  //rlcrrc_lite_data_ind(module_idP,frameP,eNB_flag,Srb_id,Sdu_size,Buffer);
  rrc_lite_data_ind(enb_instP, ue_instP,frameP,eNB_flagP,srb_idP,sdu_sizeP,buffer_pP);
#endif //CELLULAR
}

/********************************************************************************************************************/
void mac_rrc_meas_ind(module_id_t module_idP,MAC_MEAS_REQ_ENTRY *Meas_entry_pP){
/********************************************************************************************************************/
#ifdef CELLULAR
  rrc_L2_mac_meas_ind_rx ();
#else
  //  mac_rrc_meas_ind(module_idP,Meas_entry_pP);
#endif //CELLULAR
}

/********************************************************************************************************************/
void mac_sync_ind(module_id_t module_idP,u8 Status){
/********************************************************************************************************************/
#ifdef CELLULAR
  rrc_L2_sync_ind_rx(module_idP);
#else 
  mac_lite_sync_ind(module_idP,Status);
#endif //CELLULAR
}

/********************************************************************************************************************/
void mac_in_sync_ind(module_id_t module_idP,frame_t frameP, u16 eNB_indexP){
/********************************************************************************************************************/
#ifdef CELLULAR
  rrc_L2_sync_ind_rx(module_idP);
#else
  rrc_lite_in_sync_ind(module_idP,frameP,eNB_indexP);
#endif
}

/********************************************************************************************************************/
void mac_out_of_sync_ind(module_id_t module_idP,frame_t frameP, u16 eNB_indexP){
/********************************************************************************************************************/
#ifdef CELLULAR
  rrc_L2_out_sync_ind_rx();
#else 
  rrc_lite_out_of_sync_ind(module_idP,frameP,eNB_indexP);
#endif //CELLULAR
}

/********************************************************************************************************************/
int mac_get_rrc_status(module_id_t module_idP,eNB_flag_t eNB_flagP,u8 indexP) {
/********************************************************************************************************************/
#ifdef CELLULAR
  return (rrc_L2_get_rrc_status(module_idP,eNB_flagP,indexP));
#else 
  return mac_get_rrc_lite_status(module_idP, eNB_flagP, indexP);
#endif //CELLULAR
}

/********************************************************************************************************************/
char openair_rrc_ue_init(module_id_t module_idP, unsigned char eNB_indexP){
/********************************************************************************************************************/
#ifdef CELLULAR
  return (rrc_L2_ue_init(module_idP,eNB_indexP));
#else 
  return openair_rrc_lite_ue_init(module_idP, eNB_indexP);
#endif //CELLULAR
}

/********************************************************************************************************************/
char openair_rrc_eNB_init(module_id_t module_idP){
/********************************************************************************************************************/
#ifdef CELLULAR
 return( rrc_L2_eNB_init(module_idP));
#else 
  return openair_rrc_lite_eNB_init(module_idP);
#endif //CELLULAR
}
