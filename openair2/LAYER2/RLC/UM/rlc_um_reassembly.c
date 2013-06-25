/*
                            rlc_um_reassembly.c

                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr



 ***************************************************************************/
#ifndef USER_MODE
#    define __NO_VERSION__

#    ifdef RTAI
#        include <rtai.h>
#    else
      /* RTLINUX */
#        include <rtl.h>
#    endif

#else
#    include <stdio.h>
#    include <stdlib.h>
#endif
//-----------------------------------------------------------------------------
#include "print.h"
#include "rlc_um_entity.h"
#include "rlc_um_structs.h"
#include "rlc_primitives.h"
#include "rlc_um_constants.h"
#include "lists_proto_extern.h"
#include "protocol_vars_extern.h"
#include "debug_l2.h"
#include "LAYER2/MAC/extern.h"
#define DEBUG_RLC_UM_DISPLAY_ASCII_DATA
#define DEBUG_RLC_UM_SEND_SDU
//-----------------------------------------------------------------------------
void
rlc_um_reassembly (u8_t * srcP, u16_t lengthP, struct rlc_um_entity *rlcP)
{
//-----------------------------------------------------------------------------
  int             sdu_max_size;

#ifdef DEBUG_RLC_UM_DISPLAY_ASCII_DATA
  int             index;
#endif

#ifdef DEBUG_RLC_UM_REASSEMBLY
  msg ("[RLC_UM][RB %d][REASSEMBLY] reassembly()  %d bytes\n", rlcP->rb_id, lengthP);
#endif

  if ((rlcP->data_plane)) {
    sdu_max_size = RLC_SDU_MAX_SIZE_DATA_PLANE;
  } else {
    sdu_max_size = RLC_SDU_MAX_SIZE_CONTROL_PLANE;
  }
  if (rlcP->output_sdu_in_construction == NULL) {
    rlcP->output_sdu_in_construction = get_free_mem_block (sdu_max_size + sizeof (struct rlc_indication));
    rlcP->output_sdu_size_to_write = 0;
  }
  if ((rlcP->output_sdu_in_construction)) {
#ifdef DEBUG_RLC_UM_DISPLAY_ASCII_DATA
    msg ("[RLC_UM][RB %d][REASSEMBLY] DATA :", rlcP->rb_id);
    for (index = 0; index < lengthP; index++) {
      msg ("%02X-", srcP[index]);
    }
    msg ("\n");
#endif

    memcpy (&rlcP->output_sdu_in_construction->data[rlcP->output_sdu_size_to_write + sizeof (struct rlc_indication)], srcP, lengthP);
    rlcP->output_sdu_size_to_write += lengthP;

  } else {
    msg ("[RLC_UM][RB %d][REASSEMBLY] ERROR  OUTPUT SDU IS NULL\n", rlcP->rb_id);
  }
}

//-----------------------------------------------------------------------------
void
rlc_um_send_sdu (struct rlc_um_entity *rlcP)
{
//-----------------------------------------------------------------------------

  if ((rlcP->output_sdu_in_construction)) {

#ifdef DEBUG_RLC_UM_SEND_SDU
    msg ("[RLC_UM][RB %d][SEND_SDU] %d bytes \n", rlcP->rb_id, rlcP->output_sdu_size_to_write);
#endif
    if (rlcP->output_sdu_size_to_write > 0) {
#ifdef DEBUG_RLC_STATS
      rlcP->rx_sdus += 1;
#endif

      ((struct rlc_indication *) (rlcP->output_sdu_in_construction->data))->type = RLC_UM_DATA_IND;
      ((struct rlc_indication *) (rlcP->output_sdu_in_construction->data))->primitive.um_ind.data_size = rlcP->output_sdu_size_to_write;
#ifdef BENCH_QOS_L2
      fprintf (bench_l2, "[SDU DELIVERY] FRAME %d SIZE %d RB %d RLC-UM %p\n", mac_xface->frame, rlcP->output_sdu_size_to_write, rlcP->rb_id, rlcP);
#endif

      rlcP->rlc_data_ind (rlcP->upper_layer, rlcP->output_sdu_in_construction, rlcP->rb_id);
    } else {
#ifdef DEBUG_RLC_UM_SEND_SDU
      msg ("[RLC_UM][RB %d][SEND_SDU] ERROR SIZE 0\n", rlcP->rb_id, rlcP->output_sdu_size_to_write);
#endif
      free_mem_block (rlcP->output_sdu_in_construction);
    }
    rlcP->output_sdu_in_construction = NULL;
    rlcP->output_sdu_size_to_write = 0;
  }
}
