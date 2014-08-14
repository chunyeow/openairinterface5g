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

  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

/***************************************************************************
                          rlc_tm_reassembly.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#define RLC_TM_C
//#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc.h"
#include "rlc_tm_structs.h"
#include "rlc_primitives.h"
#include "rlc_tm_constants.h"
#include "list.h"
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void
rlc_tm_send_sdu_no_segment (struct rlc_tm_entity *rlcP, uint8_t error_indicationP, uint8_t * srcP, uint16_t length_in_bitsP)
{
//-----------------------------------------------------------------------------
  int             length_in_bytes;
#ifdef DEBUG_RLC_TM_DISPLAY_ASCII_DATA
  int             index;
#endif
#ifdef DEBUG_RLC_TM_REASSEMBLY
  msg ("[RLC_TM %p][SEND_SDU] %d bits\n", rlcP, length_in_bitsP);
#endif
  length_in_bytes = (length_in_bitsP + 7) >> 3;
  if (rlcP->output_sdu_in_construction == NULL) {
    rlcP->output_sdu_in_construction = get_free_mem_block (length_in_bytes);
  }
  if ((rlcP->output_sdu_in_construction)) {
#ifdef DEBUG_RLC_TM_DISPLAY_ASCII_DATA
    msg ("[RLC_TM %p][SEND_SDU] DATA :", rlcP);
    for (index = 0; index < length_in_bytes; index++) {
      msg ("%c", srcP[index]);
    }
    msg ("\n");
#endif

    memcpy (&rlcP->output_sdu_in_construction->data[rlcP->output_sdu_size_to_write], srcP, length_in_bytes);

#warning loss of error indication parameter
    rlc_data_ind (rlcP->module_id, rlcP->rb_id, length_in_bytes, rlcP->output_sdu_in_construction, rlcP->data_plane);
    rlcP->output_sdu_in_construction = NULL;
  } else {
    msg ("[RLC_TM %p][SEND_SDU] ERROR  OUTPUT SDU IS NULL\n", rlcP);
  }
}

//-----------------------------------------------------------------------------
void
rlc_tm_send_sdu_segment (struct rlc_tm_entity *rlcP, uint8_t error_indicationP)
{
//-----------------------------------------------------------------------------
  if ((rlcP->output_sdu_in_construction) && (rlcP->output_sdu_size_to_write)) {
#ifdef DEBUG_RLC_TM_SEND_SDU
    msg ("[RLC_TM %p] SEND_SDU   %d bytes\n", rlcP, rlcP->output_sdu_size_to_write);
#endif
#warning loss of error indication parameter
    rlc_data_ind (rlcP->module_id, rlcP->rb_id, rlcP->output_sdu_size_to_write, rlcP->output_sdu_in_construction, rlcP->data_plane);
    rlcP->output_sdu_in_construction = NULL;
    rlcP->output_sdu_size_to_write = 0;
  }
}
