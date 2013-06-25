/*
                            rlc_um_reassembly.c

                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#define RLC_UM_C
#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc.h"
#include "rlc_um_entity.h"
#include "rlc_um_structs.h"
#include "rlc_primitives.h"
#include "rlc_um_constants.h"
#include "list.h"
#include "LAYER2/MAC/extern.h"

//#define DEBUG_RLC_UM_REASSEMBLY 1
//#define DEBUG_RLC_UM_DISPLAY_ASCII_DATA 1
//#define DEBUG_RLC_UM_SEND_SDU

void            rlc_um_clear_rx_sdu (struct rlc_um_entity *rlcP);
void            rlc_um_reassembly (u8_t * srcP, s32_t lengthP, struct rlc_um_entity *rlcP);
void            rlc_um_send_sdu_minus_1_byte (struct rlc_um_entity *rlcP);
void            rlc_um_send_sdu (struct rlc_um_entity *rlcP);
//-----------------------------------------------------------------------------
inline void
rlc_um_clear_rx_sdu (struct rlc_um_entity *rlcP)
{
//-----------------------------------------------------------------------------
  rlcP->output_sdu_size_to_write = 0;
}

//-----------------------------------------------------------------------------
void
rlc_um_reassembly (u8_t * srcP, s32_t lengthP, struct rlc_um_entity *rlcP)
{
//-----------------------------------------------------------------------------
  int             sdu_max_size;
#ifdef DEBUG_RLC_UM_DISPLAY_ASCII_DATA
  int             index;
#endif

#ifdef DEBUG_RLC_UM_REASSEMBLY
  msg ("[RLC_UM_LITE][MOD %d][RB %d][REASSEMBLY] reassembly()  %d bytes\n", rlcP->module_id, rlcP->rb_id, lengthP);
#endif

  if ((rlcP->data_plane)) {
    sdu_max_size = RLC_SDU_MAX_SIZE_DATA_PLANE;
  } else {
    sdu_max_size = RLC_SDU_MAX_SIZE_CONTROL_PLANE;
  }
  if (rlcP->output_sdu_in_construction == NULL) {
    //    msg("[RLC_UM_LITE] Getting mem_block ...\n");
    rlcP->output_sdu_in_construction = get_free_mem_block (sdu_max_size);
    rlcP->output_sdu_size_to_write = 0;
  }
  if ((rlcP->output_sdu_in_construction)) {

#ifdef DEBUG_RLC_UM_DISPLAY_ASCII_DATA
    msg ("[RLC_UM_LITE][RB %d][REASSEMBLY] DATA :", rlcP->rb_id);
    for (index = 0; index < lengthP; index++) {
      msg ("%02X.", srcP[index]);
    }
    msg ("\n");
#endif
    // check if no overflow in size
    if ((rlcP->output_sdu_size_to_write + lengthP) <= sdu_max_size) {
      memcpy (&rlcP->output_sdu_in_construction->data[rlcP->output_sdu_size_to_write], srcP, lengthP);
      rlcP->output_sdu_size_to_write += lengthP;
    } else {
      msg ("[RLC_UM_LITE][RB %d][REASSEMBLY] ERROR  SDU SIZE OVERFLOW SDU GARBAGED\n", rlcP->rb_id);
      // erase  SDU
      rlcP->output_sdu_size_to_write = 0;
    }
  } else {
    msg ("[RLC_UM_LITE][RB %d][REASSEMBLY] ERROR  OUTPUT SDU IS NULL\n", rlcP->rb_id);
  }
}

//-----------------------------------------------------------------------------
inline void
rlc_um_send_sdu_minus_1_byte (struct rlc_um_entity *rlcP)
{
//-----------------------------------------------------------------------------
  rlcP->output_sdu_size_to_write -= 1;
  rlc_um_send_sdu (rlcP);
}

//-----------------------------------------------------------------------------
void
rlc_um_send_sdu (struct rlc_um_entity *rlcP)
{
//-----------------------------------------------------------------------------
/*#ifndef USER_MODE
  unsigned long int rlc_um_time_us;
  int min, sec, usec;
#endif*/

  if ((rlcP->output_sdu_in_construction)) {
#ifdef DEBUG_RLC_UM_SEND_SDU
    msg ("[RLC_UM_LITE][MOD %d][RB %d][SEND_SDU] %d bytes frame %d\n", rlcP->module_id, rlcP->rb_id, rlcP->output_sdu_size_to_write, Mac_rlc_xface->frame);
/*#ifndef USER_MODE
  rlc_um_time_us = (unsigned long int)(rt_get_time_ns ()/(RTIME)1000);
  sec = (rlc_um_time_us/ 1000000);
  min = (sec / 60) % 60;
  sec = sec % 60;
  usec =  rlc_um_time_us % 1000000;
  msg ("[RLC_UM_LITE][RB  %d] at time %2d:%2d.%6d\n", rlcP->rb_id, min, sec , usec);
#endif*/
#endif
    if (rlcP->output_sdu_size_to_write > 0) {
#ifdef DEBUG_RLC_STATS
      rlcP->rx_sdus += 1;
#endif

#ifdef BENCH_QOS_L2
      fprintf (bench_l2, "[SDU DELIVERY] FRAME %d SIZE %d RB %d RLC-UM %p\n", Mac_rlc_xface->frame, rlcP->output_sdu_size_to_write, rlcP->rb_id, rlcP);
#endif
      // msg("[RLC] DATA IND ON MOD_ID %d RB ID %d, size %d\n",rlcP->module_id, rlcP->rb_id,rlcP->output_sdu_size_to_write);
      rlc_data_ind (rlcP->module_id, rlcP->rb_id, rlcP->output_sdu_size_to_write, rlcP->output_sdu_in_construction, rlcP->data_plane);
    } else {
#ifdef DEBUG_RLC_UM_SEND_SDU
      msg ("[RLC_UM_LITE][RB %d][SEND_SDU] ERROR SIZE <= 0\n", rlcP->rb_id);
#endif
      msg ("[RLC_UM_LITE][RB %d][SEND_SDU] ERROR SIZE <= 0\n", rlcP->rb_id);
      msg("[RLC_UM_LITE] Freeing mem_block ...\n");
      free_mem_block (rlcP->output_sdu_in_construction);
    }
    rlcP->output_sdu_in_construction = NULL;
    rlcP->output_sdu_size_to_write = 0;
  }
}
