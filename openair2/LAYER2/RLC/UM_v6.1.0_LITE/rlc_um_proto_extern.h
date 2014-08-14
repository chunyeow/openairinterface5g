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

/******************************************************************************
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
*******************************************************************************/
#    ifndef __RLC_UM_PROTO_EXTERN_H__
#        define __RLC_UM_PROTO_EXTERN_H__

#        include "rlc_um_entity.h"
#        include "mem_block.h"

extern void     rlc_um_stat_req     (struct rlc_um_entity *rlcP, 
							  unsigned int* tx_pdcp_sdu,
							  unsigned int* tx_pdcp_sdu_discarded,
							  unsigned int* tx_data_pdu,
							  unsigned int* rx_sdu,
							  unsigned int* rx_error_pdu,  
							  unsigned int* rx_data_pdu,
							  unsigned int* rx_data_pdu_out_of_window);
extern void     rlc_um_get_pdus (void *argP);
extern void     rlc_um_rx (void *argP, struct mac_data_ind data_indP);
extern struct mac_status_resp rlc_um_mac_status_indication (void *rlcP, uint16_t no_tbP, uint16_t tb_sizeP, struct mac_status_ind tx_statusP);
extern struct mac_data_req rlc_um_mac_data_request (void *rlcP);
extern void     rlc_um_mac_data_indication (void *rlcP, struct mac_data_ind data_indP);
extern void     rlc_um_data_req (void *rlcP, mem_block_t *sduP);
#    endif
