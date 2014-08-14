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
/*! \file rlc_um.h
* \brief This file, and only this file must be included by code that interact with RLC UM layer.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \note
* \bug
* \warning
*/
/** @defgroup _rlc_um_impl_ RLC UM Layer Reference Implementation
* @ingroup _rlc_impl_
* @{
*/
#    ifndef __RLC_UM_H__
#        define __RLC_UM_H__
#        ifdef RLC_UM_C
#            define private_rlc_um(x)
#            define protected_rlc_um(x)
#            define public_rlc_um(x)
#        else
#            ifdef RLC_UM_MODULE
#                define private_rlc_um(x)
#                define protected_rlc_um(x)  extern x
#                define public_rlc_um(x)     extern x
#            else
#                define private_rlc_um(x)
#                define protected_rlc_um(x)
#                define public_rlc_um(x)     extern x
#            endif
#        endif
#        include "platform_types.h"
#        include "rlc_def.h"
#        include "rlc_def_lte.h"
#        include "rlc_um_constants.h"
#        include "rlc_um_structs.h"
#        include "rlc_um_entity.h"
#        include "mem_block.h"
#        include "rlc_um_control_primitives.h"
#        include "rlc_um_dar.h"
#        include "rlc_um_fsm.h"
#        include "rlc_um_reassembly.h"
#        include "rlc_um_receiver.h"
#        include "rlc_um_segment.h"
#        include "rlc_um_test.h"
#ifdef USER_MODE
//#        include "rlc_um_very_simple_test.h"
#endif


/*! \fn void     rlc_um_stat_req     (rlc_um_entity_t *rlcP,
                        unsigned int* stat_tx_pdcp_sdu,
                        unsigned int* stat_tx_pdcp_bytes,
                        unsigned int* stat_tx_pdcp_sdu_discarded,
                        unsigned int* stat_tx_pdcp_bytes_discarded,
                        unsigned int* stat_tx_data_pdu,
                        unsigned int* stat_tx_data_bytes,
                        unsigned int* stat_rx_pdcp_sdu,
                        unsigned int* stat_rx_pdcp_bytes,
                        unsigned int* stat_rx_data_pdus_duplicate,
                        unsigned int* stat_rx_data_bytes_duplicate,
                        unsigned int* stat_rx_data_pdu,
                        unsigned int* stat_rx_data_bytes,
                        unsigned int* stat_rx_data_pdu_dropped,
                        unsigned int* stat_rx_data_bytes_dropped,
                        unsigned int* stat_rx_data_pdu_out_of_window,
                        unsigned int* stat_rx_data_bytes_out_of_window,
                        unsigned int* stat_timer_reordering_timed_out)
* \brief    Request TX and RX statistics of a RLC AM protocol instance.
* \param[in]  rlcP                                 RLC AM protocol instance pointer.
* \param[out] stat_tx_pdcp_sdu                     Number of SDUs coming from upper layers.
* \param[out] stat_tx_pdcp_bytes                   Number of bytes coming from upper layers.
* \param[out] stat_tx_pdcp_sdu_discarded           Number of discarded SDUs coming from upper layers.
* \param[out] stat_tx_pdcp_bytes_discarded         Number of discarded bytes coming from upper layers.
* \param[out] stat_tx_data_pdu                     Number of transmitted data PDUs to lower layers.
* \param[out] stat_tx_data_bytes                   Number of transmitted data bytes to lower layers.
* \param[out] stat_rx_pdcp_sdu                     Number of SDUs delivered to upper layers.
* \param[out] stat_rx_pdcp_bytes                   Number of bytes delivered to upper layers.
* \param[out] stat_rx_data_pdus_duplicate          Number of duplicate PDUs received.
* \param[out] stat_rx_data_bytes_duplicate         Number of duplicate bytes received.
* \param[out] stat_rx_data_pdu                     Number of received PDUs from lower layers.
* \param[out] stat_rx_data_bytes                   Number of received bytes from lower layers.
* \param[out] stat_rx_data_pdu_dropped             Number of received PDUs from lower layers, then dropped.
* \param[out] stat_rx_data_bytes_dropped           Number of received bytes from lower layers, then dropped.
* \param[out] stat_rx_data_pdu_out_of_window       Number of data PDUs received out of the receive window.
* \param[out] stat_rx_data_bytes_out_of_window     Number of data bytes received out of the receive window.
* \param[out] stat_timer_reordering_timed_out      Number of times the timer "reordering" has timed-out.
*/
public_rlc_um(void     rlc_um_stat_req     (rlc_um_entity_t *rlcP,
                        unsigned int* stat_tx_pdcp_sdu,
                        unsigned int* stat_tx_pdcp_bytes,
                        unsigned int* stat_tx_pdcp_sdu_discarded,
                        unsigned int* stat_tx_pdcp_bytes_discarded,
                        unsigned int* stat_tx_data_pdu,
                        unsigned int* stat_tx_data_bytes,
                        unsigned int* stat_rx_pdcp_sdu,
                        unsigned int* stat_rx_pdcp_bytes,
                        unsigned int* stat_rx_data_pdus_duplicate,
                        unsigned int* stat_rx_data_bytes_duplicate,
                        unsigned int* stat_rx_data_pdu,
                        unsigned int* stat_rx_data_bytes,
                        unsigned int* stat_rx_data_pdu_dropped,
                        unsigned int* stat_rx_data_bytes_dropped,
                        unsigned int* stat_rx_data_pdu_out_of_window,
                        unsigned int* stat_rx_data_bytes_out_of_window,
                        unsigned int* stat_timer_reordering_timed_out);)

/*! \fn void     rlc_um_get_pdus (void *rlcP)
* \brief    Request the segmentation of SDUs based on status previously sent by MAC.
* \param[in]  rlcP                      RLC UM protocol instance pointer.
*/
private_rlc_um(   void     rlc_um_get_pdus (void *rlcP);)

/*! \fn void rlc_um_rx (void *rlcP, uint32_t frame, uint8_t eNB_flag, struct mac_data_ind data_indication)
* \brief    Process the received PDUs from lower layer.
* \param[in]  rlcP                      RLC UM protocol instance pointer.
* \param[in]  frame                     Frame Index.
* \param[in]  eNB_flag                  Flag to indicate eNB (1) or UE (0).
* \param[in]  data_indication           PDUs from MAC.
*/
protected_rlc_um( void     rlc_um_rx (void *rlcP, uint32_t frame, uint8_t eNB_flag, struct mac_data_ind data_indication);)

/*! \fn struct mac_status_resp rlc_um_mac_status_indication (void *rlcP, uint32_t frame, uint8_t eNB_flag, uint16_t tbs_sizeP, struct mac_status_ind tx_statusP)
* \brief    Request the maximum number of bytes that can be served by RLC instance to MAC and fix the amount of bytes requested by MAC for next RLC transmission.
* \param[in]  rlcP                      RLC UM protocol instance pointer.
* \param[in]  frame                     Frame index.
* \param[in]  eNB_flag                  Flag to indicate eNB (1) or UE (0).
* \param[in]  tbs_sizeP                 Number of bytes requested by MAC for next transmission.
* \param[in]  tx_statusP                Transmission status given by MAC on previous MAC transmission of the PDU.
* \return     The maximum number of bytes that can be served by RLC instance to MAC.
*/
public_rlc_um(    struct mac_status_resp rlc_um_mac_status_indication (void *rlcP, uint32_t frame,  uint8_t eNB_flag, uint16_t tbs_sizeP, struct mac_status_ind tx_statusP);)

/*! \fn struct mac_data_req rlc_um_mac_data_request (void *rlcP, uint32_t frame)
* \brief    Gives PDUs to lower layer MAC.
* \param[in]  rlcP                      RLC UM protocol instance pointer.
* \param[in]  frame                     Frame Index.
* \return     A PDU of the previously requested number of bytes, and the updated maximum number of bytes that can be served by RLC instance to MAC for next RLC transmission.
*/
public_rlc_um(    struct mac_data_req rlc_um_mac_data_request (void *rlcP, uint32_t frame);)


/*! \fn void     rlc_um_mac_data_indication (void *rlcP, uint32_t frame, uint8_t eNB_flag,struct mac_data_ind data_indP)
* \brief    Receive PDUs from lower layer MAC.
* \param[in]  rlcP             RLC UM protocol instance pointer.
* \param[in]  frame            Frame index.
* \param[in]  eNB_flag         Flag to indicate eNB (1) or UE (0).
* \param[in]  data_indP        PDUs from MAC.
*/
public_rlc_um(   void     rlc_um_mac_data_indication (void *rlcP, uint32_t frame, uint8_t eNB_flag, struct mac_data_ind data_indP);)


/*! \fn void     rlc_um_data_req (void *rlcP, uint32_t frame, mem_block_t *sduP)
* \brief    Interface with higher layers, buffer higher layer SDUS for transmission.
* \param[in]  rlcP             RLC UM protocol instance pointer.
* \param[in]  frame            Frame Index
* \param[in]  sduP             SDU. (A struct rlc_um_data_req is mapped on sduP->data.)
*/
public_rlc_um(    void     rlc_um_data_req (void *rlcP, uint32_t frame, mem_block_t *sduP);)
/** @} */
#    endif
