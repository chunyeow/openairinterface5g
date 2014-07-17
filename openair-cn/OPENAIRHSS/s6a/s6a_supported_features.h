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

  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.

 *******************************************************************************/

#include "hss_config.h"

#ifndef S6A_SUPPORTED_FEATURES_H_
#define S6A_SUPPORTED_FEATURES_H_

/* Operator Determined Barring of all Packet Oriented Services. */
#define ODB_ALL_APN(x)       (x & 0x1)

/* Operator Determined Barring of Packet Oriented Services from access points
 * that are within the HPLMN whilst the subscriber is roaming in a VPLMN.
 */
#define ODB_HPLMN_APN(x)    ((x & 0x2) >> 1)

/* Operator Determined Barring of Packet Oriented Services from access points
 * that are within the roamed to VPLMN
 */
#define ODB_VPLMN_APN(x)    ((x & 0x4) >> 2)

/* Operator Determined Barring of all outgoing calls */
#define ODB_ALL_OG(x)       ((x & 0x8) >> 3)

/* Operator Determined Barring of all outgoing international calls */
#define ODB_ALL_INT_OG      ((x & 0x10) >> 4)

/* Operator Determined Barring of all outgoing international calls except those
 * directed to the home PLMN country
 */
#define ODB_ALL_INT_NOT_TO_HPLMN(x) ((x & 0x20) >> 5)

/* Operator Determined Barring of all outgoing inter-zonal calls */
#define ODB_ALL_INT_ZONE_OG(x)      ((x & 0x40) >> 6)

/* Operator Determined Barring of all outgoing inter-zonal calls except those
 * directed to the home PLMN country
 */
#define ODB_ALL_INT_ZONE_OG_NOT_TO_HPLMN(x) ((x & 0x80) >> 7)

/* Operator Determined Barring of all outgoing international calls except those
 * directed to the home PLMN country and Barring of all outgoing inter-zonal calls
 */
#define ODB_ALL_INT_ZONE_OG_AND_INT_OG_NOT_TO_HPLMN(x)  ((x & 0x100) >> 8)

/* Regional Subscription */
#define REG_SUB(x)  ((x & 0x200) >> 9)

/* Trace Function */
#define TRACE(x)    ((x & 0x400) >> 10)

/* All LCS Privacy Exception Classes */
#define LCS_ALL_PRIV_EXCEP(x)   ((x & 0x800) >> 11)

/* Allow location by any LCS client */
#define LCS_UNIVERSAL(x)        ((x & 0x1000) >> 12)

/* Allow location by any value added LCS client to which a call/session is
 * established from the target UE
 */
#define LCS_CALL_SESS_RELATED(x)    ((x & 0x2000) >> 13)

/* Allow location by designated external value added LCS clients */
#define LCS_CALL_SESS_UNRELATED(x)  ((x & 0x4000) >> 14)

/* Allow location by designated PLMN operator LCS clients */
#define LCS_PLMN_OPERATOR(x)    ((x & 0x8000) >> 15)

/* Allow location by LCS clients of a designated LCS service type */
#define LCS_SERVICE_TYPE(x)     ((x & 0x10000) >> 16)

/* All Mobile Originating Location Request Classes */
#define LCS_ALL_MOLR_SS(x)      ((x & 0x20000) >> 17)

/* Allow an MS to request its own location */
#define LCS_BASIC_SELF_LOCATION(x) ((x & 0x40000) >> 18)

/* Allow an MS to perform self location without interaction with the PLMN */
#define LCS_AUTO_SELF_LOCATION(x)   ((x & 0x80000) >> 19)

/* Allow an MS to request transfer of its location to another LCS client */
#define LCS_TRANSFER_TO_THIRD_PARTY(x)  ((x & 0x100000) >> 20)

/* Short Message MO-PP */
#define SM_MO_PP(x) ((x & 0x200000) >> 21)

/* Barring of Outgoing Calls */
#define BARRING_OUTGOING_CALLS(x)   ((x & 0x400000) >> 22)

/* Barring of all outgoing calls */
#define BAOC(x) ((x & 0x800000) >> 23)

/* Barring of outgoing international calls */
#define BOIC(x) ((x & 0x1000000) >> 24)

/* Barring of outgoing international calls except those directed to the home PLMN
 * Country
 */
#define BOIC_EXCEPT_HC(x)   ((x & 0x2000000) >> 25)

/* UE Reachability Notifcation */
#define UE_REACH_NOTIF(x)   ((x & 0x4000000) >> 26)

/* Terminating Access Domain Selection Data Retrieval */
#define T_ADS_DATA_RETR(x)  ((x & 0x8000000) >> 27)

/* State/Location Information Retrieval */
#define STATE_LOCATION_INFO_RETR(x) ((x & 0x10000000) >> 28)

/* Partial Purge from a Combined MME/SGSN */
#define PARTIAL_PURGE(x)    ((x & 0x20000000) >> 29)

#define SUPP_FEAT_PAD_VALID(x)  ((x & 0xfc000000) == 0)

#endif /* S6A_SUPPORTED_FEATURES_H_ */
