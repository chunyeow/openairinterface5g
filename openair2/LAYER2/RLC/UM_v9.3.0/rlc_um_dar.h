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
/*! \file rlc_um_dar.h
* \brief This file defines the prototypes of the functions dealing with the reassembly buffer.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \note
* \bug
* \warning
*/
/** @defgroup _rlc_um_receiver_impl_ RLC UM Receiver Implementation
* @ingroup _rlc_um_impl_
* @{
*/
#    ifndef __RLC_UM_DAR_H__
#        define __RLC_UM_DAR_H__
//-----------------------------------------------------------------------------
#        include "rlc_um_entity.h"
#        include "rlc_um_structs.h"
#        include "rlc_um_constants.h"
#        include "list.h"
//-----------------------------------------------------------------------------
#        ifdef RLC_UM_DAR_C
#            define private_rlc_um_dar(x)    x
#            define protected_rlc_um_dar(x)  x
#            define public_rlc_um_dar(x)     x
#        else
#            ifdef RLC_UM_MODULE
#                define private_rlc_um_dar(x)
#                define protected_rlc_um_dar(x)  extern x
#                define public_rlc_um_dar(x)     extern x
#            else
#                define private_rlc_um_dar(x)
#                define protected_rlc_um_dar(x)
#                define public_rlc_um_dar(x)     extern x
#            endif
#        endif
/*! \fn signed int rlc_um_get_pdu_infos(u32_t frame,rlc_um_pdu_sn_10_t* headerP, s16_t total_sizeP, rlc_um_pdu_info_t* pdu_infoP)
* \brief    Extract PDU informations (header fields, data size, etc) from the serialized PDU.
* \param[in]  frame             Frame index.
* \param[in]  headerP             RLC UM header PDU pointer.
* \param[in]  total_sizeP         Size of RLC UM PDU.
* \param[in]  pdu_infoP           Structure containing extracted informations from PDU.
* \return     0 if no error was encountered during the parsing of the PDU, else -1;
*/
protected_rlc_um_dar(  signed int rlc_um_get_pdu_infos(u32_t frame, rlc_um_pdu_sn_10_t* headerP, s16_t total_sizeP, rlc_um_pdu_info_t* pdu_infoP));

/*! \fn int rlc_um_read_length_indicators(unsigned char**dataP, rlc_um_e_li_t* e_liP, unsigned int* li_arrayP, unsigned int *num_liP, unsigned int *data_sizeP)
* \brief    Reset protocol variables and state variables to initial values.
* \param[in,out]  dataP          Pointer on data payload.
* \param[in]      e_liP          Pointer on first LI + e bit in PDU.
* \param[in,out]  li_arrayP      Array containing read LI.
* \param[in,out]  num_liP        Number of LI read.
* \param[in,out]  data_sizeP     Pointer on data size.
* \return     0 if no error was encountered during the parsing of the PDU, else -1;
*/
private_rlc_um_dar(  int rlc_um_read_length_indicators(unsigned char**dataP, rlc_um_e_li_t* e_liP, unsigned int* li_arrayP, unsigned int *num_liP, unsigned int *data_sizeP));

/*! \fn void rlc_um_try_reassembly      (rlc_um_entity_t *rlcP, u32_t frame, u8_t eNB_flag, signed int start_snP, signed int end_snP)
* \brief    Try reassembly PDUs from DAR buffer, starting at sequence number snP.
* \param[in]  rlcP        RLC UM protocol instance pointer.
* \param[in]  frame       Frame index.
* \param[in]  eNB_flag    Flag to indicate eNB (1) or UE (0).
* \param[in]  start_snP   First PDU to be reassemblied if possible.
* \param[in]  end_snP     Last excluded highest sequence number of PDU to be reassemblied.
*/
private_rlc_um_dar(  void rlc_um_try_reassembly      (rlc_um_entity_t *rlcP, u32_t frame, u8_t eNB_flag, signed int start_snP, signed int end_snP));

/*! \fn void rlc_um_check_timer_reordering(rlc_um_entity_t *rlcP,u32_t frame)
* \brief      Check if timer reordering has timed-out, if so it is stopped and has the status "timed-out".
* \param[in]  rlcP              RLC UM protocol instance pointer.
* \param[in]  frame             Frame index
*/
protected_rlc_um_dar(void rlc_um_check_timer_reordering(rlc_um_entity_t  *rlcP,u32_t frame));

/*! \fn void rlc_um_stop_and_reset_timer_reordering(rlc_um_entity_t *rlcP,u32_t frame)
* \brief      Stop and reset the timer reordering.
* \param[in]  rlcP              RLC UM protocol instance pointer.
* \param[in]  frame             Frame index.
*/
protected_rlc_um_dar(void rlc_um_stop_and_reset_timer_reordering(rlc_um_entity_t *rlcP,u32_t frame);)

/*! \fn void rlc_um_start_timer_reordering(rlc_um_entity_t *rlcP,u32_t frame)
* \brief      Re-arm (based on RLC UM config parameter) and start timer reordering.
* \param[in]  rlcP              RLC UM protocol instance pointer.
* \param[in]  frame             Frame index.
*/
protected_rlc_um_dar(void rlc_um_start_timer_reordering(rlc_um_entity_t *rlcP,u32_t frame);)

/*! \fn void rlc_um_init_timer_reordering(rlc_um_entity_t *rlcP, u32_t time_outP)
* \brief      Initialize the timer reordering with RLC UM time-out config parameter.
* \param[in]  rlcP              RLC UM protocol instance pointer.
* \param[in]  time_outP         Time-out in frame units.
*/
protected_rlc_um_dar(void rlc_um_init_timer_reordering(rlc_um_entity_t *rlcP, u32_t time_outP);)

/*! \fn void rlc_um_check_timer_dar_time_out(rlc_um_entity_t *rlcP,u32_t frame,u8_t eNB_flag)
* \brief    Check if t-Reordering expires and take the appropriate actions as described in 3GPP specifications.
* \param[in]  rlcP        RLC UM protocol instance pointer.
* \param[in]  frame       Frame index.
* \param[in]  eNB_flag    Flag to indicate eNB(1) or UE (1)
*/
protected_rlc_um_dar(  void rlc_um_check_timer_dar_time_out(rlc_um_entity_t *rlcP,u32_t frame,u8_t eNB_flag));

/*! \fn mem_block_t *rlc_um_remove_pdu_from_dar_buffer(rlc_um_entity_t *rlcP, u16_t snP)
* \brief    Remove the PDU with sequence number snP from the DAR buffer and return it.
* \param[in]  rlcP        RLC UM protocol instance pointer.
* \param[in]  snP         Sequence number.
* \return     The PDU stored in the DAR buffer having sequence number snP, else return NULL.
*/
private_rlc_um_dar(  mem_block_t *rlc_um_remove_pdu_from_dar_buffer(rlc_um_entity_t *rlcP, u16_t snP));

/*! \fn mem_block_t *rlc_um_remove_pdu_from_dar_buffer(rlc_um_entity_t *rlcP, u16_t snP)
* \brief    Get the PDU with sequence number snP from the DAR buffer.
* \param[in]  rlcP        RLC UM protocol instance pointer.
* \param[in]  snP         Sequence number.
* \return     The PDU stored in the DAR buffer having sequence number snP, else return NULL.
*/
protected_rlc_um_dar(  inline mem_block_t* rlc_um_get_pdu_from_dar_buffer(rlc_um_entity_t *rlcP, u16_t snP));

/*! \fn signed int rlc_um_in_window(rlc_um_entity_t *rlcP, u32_t frame, signed int lower_boundP, signed int snP, signed int higher_boundP)
* \brief    Compute if the sequence number of a PDU is in a window .
* \param[in]  rlcP           RLC UM protocol instance pointer.
* \param[in]  frame          Frame index.
* \param[in]  lower_boundP   Lower bound of a window.
* \param[in]  snP            Sequence number of a theorical PDU.
* \param[in]  higher_boundP  Higher bound of a window.
* \return     -2 if lower_boundP  > sn, -1 if higher_boundP < sn, 0 if lower_boundP  < sn < higher_boundP, 1 if lower_boundP  == sn, 2 if higher_boundP == sn, 3 if higher_boundP == sn == lower_boundP.
*/
protected_rlc_um_dar(inline signed int rlc_um_in_window(rlc_um_entity_t *rlcP, u32_t frame, signed int lower_boundP, signed int snP, signed int higher_boundP));

/*! \fn signed int rlc_um_in_reordering_window(rlc_um_entity_t *rlcP, u32_t frame, signed int snP)
* \brief    Compute if the sequence number of a PDU is in a window .
* \param[in]  rlcP           RLC UM protocol instance pointer.
* \param[in]  frame          Frame index.
* \param[in]  snP            Sequence number of a theorical PDU.
* \return     0 if snP is in reordering window, else -1.
*/
protected_rlc_um_dar(inline signed int rlc_um_in_reordering_window(rlc_um_entity_t *rlcP, u32_t frame, signed int snP));

/*! \fn void rlc_um_receive_process_dar (rlc_um_entity_t *rlcP, u32_t frame, u8_t eNB_flag, mem_block_t *pdu_memP,rlc_um_pdu_sn_10_t *pduP, u16_t tb_sizeP)
* \brief    Apply the DAR process for a PDU: put it in DAR buffer and try to reassembly or discard it.
* \param[in]  rlcP       RLC UM protocol instance pointer.
* \param[in]  frame      Frame index.
* \param[in]  eNB_flag   Flag to indicated eNB (1) or UE (0).
* \param[in]  pdu_memP   mem_block_t wrapper for a UM PDU .
* \param[in]  pduP       Pointer on the header of the UM PDU.
* \param[in]  tb_sizeP   Size of the UM PDU.
*/
protected_rlc_um_dar(void rlc_um_receive_process_dar (rlc_um_entity_t *rlcP, u32_t frame, u8_t eNB_flag, mem_block_t *pdu_memP,rlc_um_pdu_sn_10_t *pduP, u16_t tb_sizeP));
/** @} */
#    endif
