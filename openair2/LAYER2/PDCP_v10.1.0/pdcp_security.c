/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2013 Eurecom

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
/*! \file pdcp_security.c
 * \brief PDCP Security Methods
 * \author ROUX Sebastien
 * \date 2013
 */
#include <stdint.h>

#include "assertions.h"

#include "UTIL/LOG/log.h"
#include "UTIL/OSA/osa_defs.h"

#include "LAYER2/MAC/extern.h"

#include "pdcp.h"
#include "pdcp_primitives.h"

#if defined(ENABLE_SECURITY)

static
u32 pdcp_get_next_count_tx(pdcp_t *pdcp_entity, u8 pdcp_header_len, u16 pdcp_sn);
static
u32 pdcp_get_next_count_rx(pdcp_t *pdcp_entity, u8 pdcp_header_len, u16 pdcp_sn);

static
u32 pdcp_get_next_count_tx(pdcp_t *pdcp_entity, u8 pdcp_header_len, u16 pdcp_sn)
{
    u32 count;
    /* For TX COUNT = TX_HFN << length of SN | pdcp SN */
    if (pdcp_header_len == PDCP_CONTROL_PLANE_DATA_PDU_SN_SIZE) {
        /* 5 bits length SN */
        count = (pdcp_entity->tx_hfn << 5)  | (pdcp_sn & 0x001F);
    } else {
        /* 12 bits length SN */
        count = (pdcp_entity->tx_hfn << 12) | (pdcp_sn & 0x0FFF);
    }
//     LOG_D(PDCP, "[OSA] TX COUNT = 0x%08x\n", count);

    return count;
}

static
u32 pdcp_get_next_count_rx(pdcp_t *pdcp_entity, u8 pdcp_header_len, u16 pdcp_sn)
{
    u32 count;
    /* For RX COUNT = RX_HFN << length of SN | pdcp SN of received PDU */
    if (pdcp_header_len == PDCP_CONTROL_PLANE_DATA_PDU_SN_SIZE) {
        /* 5 bits length SN */
        count = (pdcp_entity->rx_hfn << 5)  | (pdcp_sn & 0x001F);
    } else {
        /* 12 bits length SN */
        count = (pdcp_entity->rx_hfn << 12) | (pdcp_sn & 0x0FFF);
    }
//     LOG_D(PDCP, "[OSA] RX COUNT = 0x%08x\n", count);

    return count;
}

int pdcp_apply_security(pdcp_t *pdcp_entity, rb_id_t rb_id,
                        u8 pdcp_header_len, u16 current_sn, u8 *pdcp_pdu_buffer,
                        u16 sdu_buffer_size)
{
    u8 *buffer_encrypted = NULL;
    stream_cipher_t encrypt_params;

    DevAssert(pdcp_entity != NULL);
    DevAssert(pdcp_pdu_buffer != NULL);
    DevCheck(rb_id < NB_RB_MAX && rb_id >= 0, rb_id, NB_RB_MAX, 0);

    encrypt_params.direction  = (pdcp_entity->is_ue == 1) ? SECU_DIRECTION_UPLINK : SECU_DIRECTION_DOWNLINK;
    encrypt_params.bearer     = rb_id;
    encrypt_params.count      = pdcp_get_next_count_tx(pdcp_entity, pdcp_header_len, current_sn);
    encrypt_params.key_length = 16;

    if (rb_id < DTCH) {
        /* SRBs */
        u8 *mac_i;

        LOG_D(PDCP, "[OSA][RB %d] %s Applying control-plane security\n",
              rb_id, (pdcp_entity->is_ue != 0) ? "UE -> eNB" : "eNB -> UE");

        encrypt_params.message    = pdcp_pdu_buffer;
        encrypt_params.blength    = (pdcp_header_len + sdu_buffer_size) << 3;
        encrypt_params.key        = pdcp_entity->kRRCint;

        mac_i = &pdcp_pdu_buffer[pdcp_header_len + sdu_buffer_size];

        /* Both header and data parts are integrity protected for
         * control-plane PDUs */
        stream_compute_integrity(pdcp_entity->integrityProtAlgorithm, &encrypt_params,
                                 mac_i);

        encrypt_params.key = pdcp_entity->kRRCenc;
    } else {
        LOG_D(PDCP, "[OSA][RB %d] %s Applying user-plane security\n",
              rb_id, (pdcp_entity->is_ue != 0) ? "UE -> eNB" : "eNB -> UE");

        encrypt_params.key = pdcp_entity->kUPenc;
    }

    encrypt_params.message    = &pdcp_pdu_buffer[pdcp_header_len];
    encrypt_params.blength    = sdu_buffer_size << 3;

    buffer_encrypted = &pdcp_pdu_buffer[pdcp_header_len];

    /* Apply ciphering if any requested */
    stream_encrypt(pdcp_entity->cipheringAlgorithm, &encrypt_params,
                   &buffer_encrypted);

    return 0;
}

int pdcp_validate_security(pdcp_t *pdcp_entity, rb_id_t rb_id,
                           u8 pdcp_header_len, u16 current_sn, u8 *pdcp_pdu_buffer,
                           u16 sdu_buffer_size)
{
    u8 *buffer_decrypted = NULL;
    stream_cipher_t decrypt_params;

    DevAssert(pdcp_entity != NULL);

    DevAssert(pdcp_pdu_buffer != NULL);
    DevCheck(rb_id < NB_RB_MAX && rb_id >= 0, rb_id, NB_RB_MAX, 0);

    buffer_decrypted = (u8*)&pdcp_pdu_buffer[pdcp_header_len];

    decrypt_params.direction  = (pdcp_entity->is_ue == 1) ? SECU_DIRECTION_DOWNLINK : SECU_DIRECTION_UPLINK ;
    decrypt_params.bearer     = rb_id;
    decrypt_params.count      = pdcp_get_next_count_rx(pdcp_entity, pdcp_header_len, current_sn);
    decrypt_params.message    = &pdcp_pdu_buffer[pdcp_header_len];
    decrypt_params.blength    = (sdu_buffer_size - pdcp_header_len) << 3;
    decrypt_params.key_length = 16;

    if (rb_id < DTCH) {
        LOG_D(PDCP, "[OSA][RB %d] %s Validating control-plane security\n",
              rb_id, (pdcp_entity->is_ue != 0) ? "eNB -> UE" : "UE -> eNB");
        decrypt_params.key = pdcp_entity->kRRCenc;
    } else {
        LOG_D(PDCP, "[OSA][RB %d] %s Validating user-plane security\n",
              rb_id, (pdcp_entity->is_ue != 0) ? "eNB -> UE" : "UE -> eNB");
        decrypt_params.key = pdcp_entity->kUPenc;
    }

    /* Uncipher the block */
    stream_decrypt(pdcp_entity->cipheringAlgorithm, &decrypt_params, &buffer_decrypted);

    if (rb_id < DTCH) {
        /* Now check the integrity of the complete PDU */
        decrypt_params.message    = pdcp_pdu_buffer;
        decrypt_params.blength    = sdu_buffer_size << 3;
        decrypt_params.key        = pdcp_entity->kRRCint;

        if (stream_check_integrity(pdcp_entity->integrityProtAlgorithm,
            &decrypt_params, &pdcp_pdu_buffer[sdu_buffer_size]) != 0) {
            LOG_E(PDCP, "[OSA] failed to validate MAC-I of incoming PDU\n");
            return -1;
        }
    }
    return 0;
}

#endif /* ENABLE_SECURITY */
