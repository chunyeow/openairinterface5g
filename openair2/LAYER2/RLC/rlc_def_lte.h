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
/*! \file rlc_def_lte.h
* \brief This file contains LTE constants for RLC internal use.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
* \note
* \bug
* \warning
*/
#ifndef __RLC_DEF_LTE_H__
#    define __RLC_DEF_LTE_H__
//----------------------------------------------------------

#    define RLC_FI_1ST_BYTE_DATA_IS_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_LAST_BYTE_SDU         0x0
#    define RLC_FI_1ST_BYTE_DATA_IS_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_NOT_LAST_BYTE_SDU     0x1
#    define RLC_FI_1ST_BYTE_DATA_IS_NOT_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_LAST_BYTE_SDU     0x2
#    define RLC_FI_1ST_BYTE_DATA_IS_NOT_1ST_BYTE_SDU_LAST_BYTE_DATA_IS_NOT_LAST_BYTE_SDU 0x3

#    define RLC_E_FIXED_PART_DATA_FIELD_FOLLOW                                          0x0
#    define RLC_E_FIXED_PART_E_AND_LI_FIELD_FOLLOW                                      0x1

#    define RLC_E_EXTENSION_PART_DATA_FIELD_FOLLOW                                      0x0
#    define RLC_E_EXTENSION_PART_E_AND_LI_FIELD_FOLLOW                                  0x1

#endif
