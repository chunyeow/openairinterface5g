/*******************************************************************************
Eurecom OpenAirInterface 2
Copyright(c) 1999 - 2014 Eurecom

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
Address      : EURECOM,
               Campus SophiaTech,
               450 Route des Chappes,
               CS 50193
               06904 Biot Sophia Antipolis cedex,
               FRANCE
*******************************************************************************/

/******************************************************************************
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
*******************************************************************************/
#    ifndef __RLC_UM_CONSTANTS_H__
#        define __RLC_UM_CONSTANTS_H__


// SN Field
#        define RLC_UM_SN_7BITS_MASK                        0xFF
// li field (values shifted 1 bit left)
#        define RLC_UM_SEGMENT_NB_MAX_LI_PER_PDU            32
#        define RLC_UM_LI_FIRST_PDU_BYTE_IS_FIRST_SDU_BYTE  0xFFF8
#        define RLC_LI7_PDU_PADDING                         0xFFFE

//----------------------------------------------------------
// Events defined for state model of the acknowledged mode entity
#        define RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_NULL_STATE_EVENT                 0x00
#        define RLC_UM_RECEIVE_CRLC_CONFIG_REQ_ENTER_DATA_TRANSFER_READY_STATE_EVENT  0x01
#        define RLC_UM_RECEIVE_CRLC_SUSPEND_REQ_EVENT                                 0x10
#        define RLC_UM_TRANSMIT_CRLC_SUSPEND_CNF_EVENT                                0x11
#        define RLC_UM_RECEIVE_CRLC_RESUME_REQ_EVENT                                  0x12
#    endif
