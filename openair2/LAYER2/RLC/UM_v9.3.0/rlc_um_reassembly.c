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
#define RLC_UM_MODULE
#define RLC_UM_REASSEMBLY_C
#include "platform_types.h"
//-----------------------------------------------------------------------------
#ifdef USER_MODE
#include <string.h>
#endif
#include "rlc.h"
#include "rlc_um.h"
#include "rlc_primitives.h"
#include "list.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"

//#define DEBUG_RLC_UM_DISPLAY_ASCII_DATA 1

//-----------------------------------------------------------------------------
inline void
rlc_um_clear_rx_sdu (rlc_um_entity_t *rlcP)
{
//-----------------------------------------------------------------------------
  rlcP->output_sdu_size_to_write = 0;
}

//-----------------------------------------------------------------------------
void
rlc_um_reassembly (u8_t * srcP, s32_t lengthP, rlc_um_entity_t *rlcP,u32_t frame)
{
//-----------------------------------------------------------------------------
  int             sdu_max_size;
#ifdef DEBUG_RLC_UM_DISPLAY_ASCII_DATA
  int             index;
#endif

  LOG_D(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d][REASSEMBLY] reassembly()  %d bytes %d bytes already reassemblied\n", rlcP->module_id, rlcP->rb_id, frame, lengthP, rlcP->output_sdu_size_to_write);

  if (lengthP <= 0) {
      return;
  }

  if ((rlcP->is_data_plane)) {
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

    // check if no overflow in size
    if ((rlcP->output_sdu_size_to_write + lengthP) <= sdu_max_size) {
      memcpy (&rlcP->output_sdu_in_construction->data[rlcP->output_sdu_size_to_write], srcP, lengthP);
      rlcP->output_sdu_size_to_write += lengthP;
#ifdef DEBUG_RLC_UM_DISPLAY_ASCII_DATA
      rlcP->output_sdu_in_construction->data[rlcP->output_sdu_size_to_write] = 0;
      LOG_T(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d][REASSEMBLY] DATA :", rlcP->module_id, rlcP->rb_id, frame);
      rlc_util_print_hex_octets(RLC, (unsigned char*)rlcP->output_sdu_in_construction->data, rlcP->output_sdu_size_to_write);
#endif
    } else {
      LOG_E(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d][REASSEMBLY] [max_sdu size %d] ERROR  SDU SIZE OVERFLOW SDU GARBAGED\n", rlcP->module_id, rlcP->rb_id, frame, sdu_max_size);
      // erase  SDU
      rlcP->output_sdu_size_to_write = 0;
    }
  } else {
    LOG_E(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d][REASSEMBLY]ERROR  OUTPUT SDU IS NULL\n", rlcP->module_id, rlcP->rb_id, frame);
  }

}
//-----------------------------------------------------------------------------
void
rlc_um_send_sdu (rlc_um_entity_t *rlcP,u32_t frame, u8_t eNB_flag)
{
//-----------------------------------------------------------------------------

  if ((rlcP->output_sdu_in_construction)) {
    LOG_D(RLC, "\n\n\n[RLC_UM][MOD %d][RB %d][FRAME %05d][SEND_SDU] %d bytes sdu %p\n", rlcP->module_id, rlcP->rb_id, frame, rlcP->output_sdu_size_to_write, rlcP->output_sdu_in_construction);

    if (rlcP->output_sdu_size_to_write > 0) {
        u8_t UE_id, eNB_id;
        if (eNB_flag == 0) {
          UE_id = rlcP->module_id - NB_eNB_INST;
          /* FIXME: force send on eNB 0 */
          eNB_id = 0;
        } else {
          UE_id = rlcP->rb_id / NB_RB_MAX;
          eNB_id = rlcP->module_id;
        }
        rlcP->stat_rx_pdcp_sdu += 1;
        rlcP->stat_rx_pdcp_bytes += rlcP->output_sdu_size_to_write;
#ifdef TEST_RLC_UM
        #ifdef DEBUG_RLC_UM_DISPLAY_ASCII_DATA
        rlcP->output_sdu_in_construction->data[rlcP->output_sdu_size_to_write] = 0;
        LOG_T(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d][SEND_SDU] DATA :", rlcP->module_id, rlcP->rb_id, frame);
        rlc_util_print_hex_octets(RLC, rlcP->output_sdu_in_construction->data, rlcP->output_sdu_size_to_write);
        #endif
        rlc_um_v9_3_0_test_data_ind (rlcP->module_id, rlcP->rb_id, rlcP->output_sdu_size_to_write, rlcP->output_sdu_in_construction);
#else
        // msg("[RLC] DATA IND ON MOD_ID %d RB ID %d, size %d\n",rlcP->module_id, rlcP->rb_id, frame,rlcP->output_sdu_size_to_write);
        rlc_data_ind (rlcP->module_id, eNB_id, UE_id, frame, eNB_flag, rlcP->is_mxch, rlcP->rb_id, rlcP->output_sdu_size_to_write, rlcP->output_sdu_in_construction,rlcP->is_data_plane);
#endif
        rlcP->output_sdu_in_construction = NULL;
    } else {
      LOG_E(RLC, "[RLC_UM][MOD %d][RB %d][FRAME %05d][SEND_SDU] ERROR SIZE <= 0 ... DO NOTHING, SET SDU SIZE TO 0\n",rlcP->module_id, rlcP->rb_id, frame);
    }
    rlcP->output_sdu_size_to_write = 0;
  }
}
