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
/*! \file pdcp_security.c
 * \brief PDCP Security Methods 
 * \author ROUX Sebastie and Navid Nikaein
 * \email openair_tech@eurecom.fr, navid.nikaein@eurecom.fr
 * \date 2014
 */
#include <stdint.h>

#include "assertions.h"

#include "UTIL/LOG/log.h"
#include "UTIL/OSA/osa_defs.h"

#include "UTIL/LOG/vcd_signal_dumper.h"

#include "LAYER2/MAC/extern.h"

#include "pdcp.h"
#include "pdcp_primitives.h"

#if defined(ENABLE_SECURITY)

static
uint32_t pdcp_get_next_count_tx(pdcp_t *pdcp_entity, srb_flag_t srb_flagP, uint16_t pdcp_sn);
static
uint32_t pdcp_get_next_count_rx(pdcp_t *pdcp_entity, srb_flag_t srb_flagP, uint16_t pdcp_sn);

static
uint32_t pdcp_get_next_count_tx(pdcp_t *pdcp_entity, srb_flag_t srb_flagP, uint16_t pdcp_sn)
{
  uint32_t count;
  /* For TX COUNT = TX_HFN << length of SN | pdcp SN */
  if (srb_flagP) {
    /* 5 bits length SN */
    count = ((pdcp_entity->tx_hfn << 5)  | (pdcp_sn & 0x001F));
  } else {
    if (pdcp_entity->seq_num_size == PDCP_Config__rlc_UM__pdcp_SN_Size_len7bits) {
      count = ((pdcp_entity->tx_hfn << 7) | (pdcp_sn & 0x07F));
    } else { /*Default is the 12 bits length SN */
      count = ((pdcp_entity->tx_hfn << 12) | (pdcp_sn & 0x0FFF));
    }
  }
  LOG_D(PDCP, "[OSA] TX COUNT = 0x%08x\n", count);
  
  return count;
}

static
uint32_t pdcp_get_next_count_rx(pdcp_t *pdcp_entity, srb_flag_t srb_flagP, uint16_t pdcp_sn) 
{
  uint32_t count;
  /* For RX COUNT = RX_HFN << length of SN | pdcp SN of received PDU */
  if (srb_flagP) {
    /* 5 bits length SN */
    count = (((pdcp_entity->rx_hfn + pdcp_entity->rx_hfn_offset) << 5)  | (pdcp_sn & 0x001F));
  } else {
    if (pdcp_entity->seq_num_size == PDCP_Config__rlc_UM__pdcp_SN_Size_len7bits) {
      /* 7 bits length SN */
      count = (((pdcp_entity->rx_hfn + pdcp_entity->rx_hfn_offset) << 7) | (pdcp_sn & 0x007F));
    } else { // default 
      /* 12 bits length SN */
      count = (((pdcp_entity->rx_hfn + pdcp_entity->rx_hfn_offset) << 12) | (pdcp_sn & 0x0FFF));
    }
  
  }
  // reset the hfn offset
  pdcp_entity->rx_hfn_offset =0;
  LOG_D(PDCP, "[OSA] RX COUNT = 0x%08x\n", count);
  
  return count;
}

int pdcp_apply_security(pdcp_t        *pdcp_entity, 
			srb_flag_t     srb_flagP,
			rb_id_t        rb_id,
			uint8_t        pdcp_header_len, 
			uint16_t       current_sn, 
			uint8_t       *pdcp_pdu_buffer,
                        uint16_t      sdu_buffer_size)
{
    uint8_t *buffer_encrypted = NULL;
    stream_cipher_t encrypt_params;

    DevAssert(pdcp_entity != NULL);
    DevAssert(pdcp_pdu_buffer != NULL);
    DevCheck(rb_id < NB_RB_MAX && rb_id >= 0, rb_id, NB_RB_MAX, 0);

    vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_PDCP_APPLY_SECURITY, VCD_FUNCTION_IN);

    encrypt_params.direction  = (pdcp_entity->is_ue == 1) ? SECU_DIRECTION_UPLINK : SECU_DIRECTION_DOWNLINK; 
    encrypt_params.bearer     = rb_id - 1;
    encrypt_params.count      = pdcp_get_next_count_tx(pdcp_entity, srb_flagP, current_sn);
    encrypt_params.key_length = 16;

    if (srb_flagP) {
        /* SRBs */
        uint8_t *mac_i;

        LOG_D(PDCP, "[OSA][RB %d] %s Applying control-plane security %d \n",
              rb_id, (pdcp_entity->is_ue != 0) ? "UE -> eNB" : "eNB -> UE", pdcp_entity->integrityProtAlgorithm);

        encrypt_params.message    = pdcp_pdu_buffer;
        encrypt_params.blength    = (pdcp_header_len + sdu_buffer_size) << 3;
        encrypt_params.key        = pdcp_entity->kRRCint + 16; // + 128;

        mac_i = &pdcp_pdu_buffer[pdcp_header_len + sdu_buffer_size];

        /* Both header and data parts are integrity protected for
         * control-plane PDUs */
        stream_compute_integrity(pdcp_entity->integrityProtAlgorithm, 
				 &encrypt_params,
                                 mac_i);

        encrypt_params.key = pdcp_entity->kRRCenc;  // + 128  // bit key 
    } else {
        LOG_D(PDCP, "[OSA][RB %d] %s Applying user-plane security\n",
              rb_id, (pdcp_entity->is_ue != 0) ? "UE -> eNB" : "eNB -> UE");

        encrypt_params.key = pdcp_entity->kUPenc;//  + 128;
    }

    encrypt_params.message    = &pdcp_pdu_buffer[pdcp_header_len];
    encrypt_params.blength    = sdu_buffer_size << 3;

    buffer_encrypted = &pdcp_pdu_buffer[pdcp_header_len];

    /* Apply ciphering if any requested */
    stream_encrypt(pdcp_entity->cipheringAlgorithm, 
		   &encrypt_params,
                   &buffer_encrypted);

    vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_PDCP_APPLY_SECURITY, VCD_FUNCTION_OUT);

    return 0;
}

int pdcp_validate_security(pdcp_t         *pdcp_entity, 
			   srb_flag_t     srb_flagP,
			   rb_id_t        rb_id,
                           uint8_t        pdcp_header_len, 
			   uint16_t       current_sn, 
			   uint8_t       *pdcp_pdu_buffer,
                           uint16_t       sdu_buffer_size)
{
    uint8_t *buffer_decrypted = NULL;
    stream_cipher_t decrypt_params;

    DevAssert(pdcp_entity != NULL);

    DevAssert(pdcp_pdu_buffer != NULL);
    DevCheck(rb_id < NB_RB_MAX && rb_id >= 0, rb_id, NB_RB_MAX, 0);

    vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_PDCP_VALIDATE_SECURITY, VCD_FUNCTION_IN);

    buffer_decrypted = (uint8_t*)&pdcp_pdu_buffer[pdcp_header_len];

    decrypt_params.direction  = (pdcp_entity->is_ue == 1) ? SECU_DIRECTION_DOWNLINK : SECU_DIRECTION_UPLINK ;
    decrypt_params.bearer     = rb_id - 1;
    decrypt_params.count      = pdcp_get_next_count_rx(pdcp_entity, srb_flagP, current_sn);
    decrypt_params.message    = &pdcp_pdu_buffer[pdcp_header_len];
    decrypt_params.blength    = (sdu_buffer_size - pdcp_header_len) << 3;
    decrypt_params.key_length = 16;

    if (srb_flagP) {
        LOG_D(PDCP, "[OSA][RB %d] %s Validating control-plane security\n",
              rb_id, (pdcp_entity->is_ue != 0) ? "eNB -> UE" : "UE -> eNB");
        decrypt_params.key = pdcp_entity->kRRCenc;// + 128;
    } else {
        LOG_D(PDCP, "[OSA][RB %d] %s Validating user-plane security\n",
              rb_id, (pdcp_entity->is_ue != 0) ? "eNB -> UE" : "UE -> eNB");
        decrypt_params.key = pdcp_entity->kUPenc;// + 128;
    }

    /* Uncipher the block */
    stream_decrypt(pdcp_entity->cipheringAlgorithm, 
		   &decrypt_params, 
		   &buffer_decrypted);

    if (srb_flagP) {
      /* Now check the integrity of the complete PDU */
      decrypt_params.message    = pdcp_pdu_buffer;
      decrypt_params.blength    = sdu_buffer_size << 3;
      decrypt_params.key        = pdcp_entity->kRRCint + 16;// 128;
      
      if (stream_check_integrity(pdcp_entity->integrityProtAlgorithm,
				 &decrypt_params, 
				 &pdcp_pdu_buffer[sdu_buffer_size]) != 0)
	{
	  LOG_E(PDCP, "[OSA] failed to validate MAC-I of incoming PDU\n");
	  vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_PDCP_VALIDATE_SECURITY, VCD_FUNCTION_OUT);
	  return -1;
	}
    }
    
    vcd_signal_dumper_dump_function_by_name(VCD_SIGNAL_DUMPER_FUNCTIONS_PDCP_VALIDATE_SECURITY, VCD_FUNCTION_OUT);
    
    return 0;
}

#endif /* ENABLE_SECURITY */
