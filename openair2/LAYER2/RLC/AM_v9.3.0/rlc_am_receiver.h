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
/*! \file rlc_am_receiver.h
* \brief This file defines the prototypes of the functions dealing with the first stage of the receiving process.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \note
* \bug
* \warning
*/
/** @defgroup _rlc_am_internal_receiver_impl_ RLC AM Receiver Internal Reference Implementation
* @ingroup _rlc_am_internal_impl_
* @{
*/
#    ifndef __RLC_AM_RECEIVER_H__
#        define __RLC_AM_RECEIVER_H__
#        ifdef RLC_AM_RECEIVER_C
#            define private_rlc_am_receiver(x)    x
#            define protected_rlc_am_receiver(x)  x
#            define public_rlc_am_receiver(x)     x
#        else
#            ifdef RLC_AM_MODULE
#                define private_rlc_am_receiver(x)
#                define protected_rlc_am_receiver(x)  extern x
#                define public_rlc_am_receiver(x)     extern x
#            else
#                define private_rlc_am_receiver(x)
#                define protected_rlc_am_receiver(x)
#                define public_rlc_am_receiver(x)     extern x
#            endif
#        endif
/*! \fn signed int rlc_am_get_data_pdu_infos(u32_t frame, rlc_am_pdu_sn_10_t* headerP, s16_t sizeP, rlc_am_pdu_info_t* pdu_infoP)
* \brief    Extract PDU informations (header fields, data size, etc) from the serialized PDU.
* \param[in]  frame          Frame index.
* \param[in]  headerP        RLC AM header PDU pointer.
* \param[in]  sizeP          Size of RLC AM PDU.
* \param[in]  pdu_infoP      Structure containing extracted informations from PDU.
* \return     0 if no error was encountered during the parsing of the PDU, else -1;
*/
protected_rlc_am_receiver( signed int rlc_am_get_data_pdu_infos(u32_t frame, rlc_am_pdu_sn_10_t* headerP, s16_t sizeP, rlc_am_pdu_info_t* pdu_infoP));

/*! \fn void rlc_am_display_data_pdu_infos(rlc_am_entity_t *rlcP, u32_t frame, rlc_am_pdu_info_t* pdu_infoP)
* \brief      Display RLC AM PDU informations.
* \param[in]  rlcP           RLC AM protocol instance pointer.
* \param[in]  frame          Frame index.
* \param[in]  pdu_infoP      Structure containing extracted informations of a PDU.
*/
protected_rlc_am_receiver( void rlc_am_display_data_pdu_infos(rlc_am_entity_t *rlcP, u32_t frame, rlc_am_pdu_info_t* pdu_infoP);)

/*! \fn void rlc_am_rx_update_vr_ms(rlc_am_entity_t *rlcP,mem_block_t* tbP)
* \brief      Update RLC AM protocol variable VR(MS).
* \param[in]  rlcP           RLC AM protocol instance pointer.
* \param[in]  frame          Frame index.
* \param[in]  tbP            PDU embedded in a mem_block_t struct.
* \note It is assumed that the sequence number of the transport block is equal to VR(MS)
*/
protected_rlc_am_receiver( void rlc_am_rx_update_vr_ms(rlc_am_entity_t *rlcP,u32_t frame, mem_block_t* tbP);)

/*! \fn void rlc_am_rx_update_vr_r (rlc_am_entity_t *rlcP,u32_t frame,mem_block_t* tbP)
* \brief      Update RLC AM protocol variable VR(R).
* \param[in]  rlcP           RLC AM protocol instance pointer.
* \param[in]  frame          Frame index.
* \param[in]  tbP            PDU embedded in a mem_block_t struct.
* \note It is assumed that the sequence number of the transport block is equal to VR(R)
*/
protected_rlc_am_receiver( void rlc_am_rx_update_vr_r (rlc_am_entity_t *rlcP,u32_t frame, mem_block_t* tbP);)

/*! \fn void rlc_am_receive_routing (rlc_am_entity_t *rlcP, u32_t frame, u8_t eNB_flag, struct mac_data_ind data_indP)
* \brief      Convert transport blocks received from MAC layer into RLC AM PDUs, and dispatch to the right processing block these PDUS upon their type (CONTROL/DATA).
* \param[in]  rlcP           RLC AM protocol instance pointer.
* \param[in]  frame          Frame index.
* \param[in]  eNB_flag       Flag to indicate eNB (1) or UE (0).
* \param[in]  data_indP      Transport blocks received from MAC layer.
*/
protected_rlc_am_receiver( void rlc_am_receive_routing (rlc_am_entity_t *rlcP, u32_t frame, u8_t eNB_flag, struct mac_data_ind data_indP));

/*! \fn void rlc_am_receive_process_data_pdu (rlc_am_entity_t *rlcP, u32_t frame, u8_t eNB_flag, mem_block_t* tbP, u8_t* first_byteP, u16_t tb_size_in_bytesP)
* \brief      Process an incoming data PDU received from MAC layer.
* \param[in]  rlcP              RLC AM protocol instance pointer.
* \param[in]  frame             Frame index.
* \param[in]  eNB_flag          Flag to indicate eNB (1) or UE (0).
* \param[in]  tbP               PDU embedded in a mem_block_t struct.
* \param[in]  first_byteP       Pointer on first byte of the PDU.
* \param[in]  tb_size_in_bytesP Transport block size in bytes (same as PDU size in bytes).
*/
private_rlc_am_receiver( void rlc_am_receive_process_data_pdu (rlc_am_entity_t *rlcP, u32_t frame, u8_t eNB_flag, mem_block_t* tbP, u8_t* first_byteP, u16_t tb_size_in_bytesP));
/** @} */
#    endif
