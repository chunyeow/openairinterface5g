
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

/*! \file MIH_C_Link_Constants.h
 * \brief This file defines constants, for the MIH Link SAP.
 * \author BRIZZOLARA Davide, GAUTHIER Lionel, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */

#ifndef __MIH_C_LINK_CONSTANTS_H__
#define __MIH_C_LINK_CONSTANTS_H__

#define MIH_C_BIT_0_VALUE                   0x01
#define MIH_C_BIT_1_VALUE                   0x02
#define MIH_C_BIT_2_VALUE                   0x04
#define MIH_C_BIT_3_VALUE                   0x08
#define MIH_C_BIT_4_VALUE                   0x10
#define MIH_C_BIT_5_VALUE                   0x20
#define MIH_C_BIT_6_VALUE                   0x40
#define MIH_C_BIT_7_VALUE                   0x80
#define MIH_C_BIT_8_VALUE                   0x100
#define MIH_C_BIT_9_VALUE                   0x200
#define MIH_C_BIT_10_VALUE                  0x400
#define MIH_C_BIT_11_VALUE                  0x800
#define MIH_C_BIT_12_VALUE                  0x1000
#define MIH_C_BIT_13_VALUE                  0x2000
#define MIH_C_BIT_14_VALUE                  0x4000
#define MIH_C_BIT_15_VALUE                  0x8000
#define MIH_C_BIT_16_VALUE                  0x10000
#define MIH_C_BIT_17_VALUE                  0x20000
#define MIH_C_BIT_18_VALUE                  0x40000
#define MIH_C_BIT_19_VALUE                  0x80000
#define MIH_C_BIT_20_VALUE                  0x100000
#define MIH_C_BIT_21_VALUE                  0x200000
#define MIH_C_BIT_22_VALUE                  0x400000
#define MIH_C_BIT_23_VALUE                  0x800000
#define MIH_C_BIT_24_VALUE                  0x1000000
#define MIH_C_BIT_25_VALUE                  0x2000000
#define MIH_C_BIT_26_VALUE                  0x4000000
#define MIH_C_BIT_27_VALUE                  0x8000000
#define MIH_C_BIT_28_VALUE                  0x10000000
#define MIH_C_BIT_29_VALUE                  0x20000000
#define MIH_C_BIT_30_VALUE                  0x40000000
#define MIH_C_BIT_31_VALUE                  0x80000000

#define MIH_C_3GPP_ADDR_LENGTH             16
#define MIH_C_3GPP2_ADDR_LENGTH            8
#define MIH_C_OTHER_L2_ADDR_LENGTH         64
#define MIH_C_LINK_SCAN_RSP_LENGTH         8
#define MIH_C_THRESHOLD_LIST_LENGTH        8
#define MIH_C_LINK_ADDR_LIST_LENGTH        8
#define MIH_C_QOS_LIST_LENGTH              255
#define MIH_C_LINK_STATUS_REQ_LIST_LENGTH  32
#define MIH_C_LINK_CFG_PARAM_LIST_LENGTH 16


#endif

#ifndef __MIH_C_IANA_CONSTANTS_H__
#define __MIH_C_IANA_CONSTANTS_H__
#define IANA_ADDR_FAMILY_RESERVED          0
#define IANA_ADDR_FAMILY_IP                1
#define IANA_ADDR_FAMILY_IP6               2
#define IANA_ADDR_FAMILY_NSAP              3
#define IANA_ADDR_FAMILY_HDLC              4
#define IANA_ADDR_FAMILY_BBN_1822          5
#define IANA_ADDR_FAMILY_802               6
#define IANA_ADDR_FAMILY_E_163             7
#define IANA_ADDR_FAMILY_E_164             8
#define IANA_ADDR_FAMILY_F_69              9
#define IANA_ADDR_FAMILY_X_121             10
#define IANA_ADDR_FAMILY_IPX               11
#define IANA_ADDR_FAMILY_Appletalk         12
#define IANA_ADDR_FAMILY_Decnet_IV         13
#define IANA_ADDR_FAMILY_Banyan_Vines      14
#define IANA_ADDR_FAMILY_E_164_with_NSAP   15
#define IANA_ADDR_FAMILY_DNS               16
// More defined values, see http://www.iana.org/assignments/address-family-numbers/address-family-numbers.xml
#endif
