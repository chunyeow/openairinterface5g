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
  Forums       : http://forums.eurecom.fr/openairinterface
  Address      : EURECOM, Campus SophiaTech, 450 Route des Chappes
                 06410 Biot FRANCE

*******************************************************************************/

#ifndef S11_IE_FORMATTER_H_
#define S11_IE_FORMATTER_H_

/* Imsi Information Element
 * 3GPP TS.29.274 #8.3
 * NOTE: Imsi is TBCD coded
 * octet 5   | Number digit 2 | Number digit 1   |
 * octet n+4 | Number digit m | Number digit m-1 |
 */
NwRcT s11_imsi_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

int s11_imsi_ie_set(NwGtpv2cMsgHandleT *msg, const Imsi_t *imsi);

NwRcT s11_msisdn_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

/* PDN Type Information Element
 * 3GPP TS 29.274 #8.34
 * PDN type:
 *      * 1 = IPv4
 *      * 2 = IPv6
 *      * 3 = IPv4v6
 */
NwRcT s11_pdn_type_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

int s11_pdn_type_ie_set(NwGtpv2cMsgHandleT *msg, const pdn_type_t *pdn_type);

/* RAT type Information Element
 * WARNING: the RAT type used in MME and S/P-GW is not the same as the one
 * for S11 interface defined in 3GPP TS 29.274 #8.17.
 */
NwRcT s11_rat_type_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

int s11_rat_type_ie_set(NwGtpv2cMsgHandleT *msg, const rat_type_t *rat_type);

/* EPS Bearer Id Information Element
 * 3GPP TS 29.274 #8.8
 */
NwRcT s11_ebi_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

int s11_ebi_ie_set(NwGtpv2cMsgHandleT *msg, const unsigned ebi);

/* Bearer Contexts to Create Information Element as part of Create Session Request
 * 3GPP TS 29.274 Table 7.2.1-2.
 */
NwRcT s11_bearer_context_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

int s11_bearer_context_to_create_ie_set(
    NwGtpv2cMsgHandleT *msg, const bearer_to_create_t *bearer_to_create);

NwRcT s11_bearer_context_to_modifiy_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

/* EPS Bearer Id Information Element
 * 3GPP TS 29.274 #8.8
 * ebi is 4 bits long
 */
int s11_ebi_ie_set(NwGtpv2cMsgHandleT *msg, const unsigned ebi);

/* Cause Information Element */
NwRcT s11_cause_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

int s11_cause_ie_set(NwGtpv2cMsgHandleT *msg,
                     const gtp_cause_t  *cause);

/* Bearer Context Created grouped Information Element */
NwRcT s11_bearer_context_created_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

int s11_bearer_context_created_ie_set(
    NwGtpv2cMsgHandleT *msg, const bearer_context_created_t *bearer);

/* Serving Network Information Element
 * 3GPP TS 29.274 #8.18
 */
NwRcT s11_serving_network_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

int s11_serving_network_ie_set(
    NwGtpv2cMsgHandleT     *msg,
    const ServingNetwork_t *serving_network);

NwRcT s11_fteid_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

/* PDN Address Allocation Information Element */
NwRcT s11_paa_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

int s11_paa_ie_set(NwGtpv2cMsgHandleT *msg, const PAA_t *paa);

/* Access Point Name Information Element
 * 3GPP TS 29.274 #8.6
 * NOTE: The APN field is not encoded as a dotted string as commonly used in
 * documentation.
 * The encoding of the APN field follows 3GPP TS 23.003 subclause 9.1
 */
NwRcT s11_apn_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

int s11_apn_ie_set(NwGtpv2cMsgHandleT *msg, const char *apn);

NwRcT s11_ambr_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

NwRcT s11_mei_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

NwRcT s11_uli_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

/* APN restrtiction Information Element */
int s11_apn_restriction_ie_set(
    NwGtpv2cMsgHandleT *msg, const uint8_t apn_restriction);

/* Bearer level Qos Information Element
 * 3GPP TS 29.274 #8.15
 */
int s11_bearer_qos_ie_set(NwGtpv2cMsgHandleT *msg, const BearerQOS_t *bearer_qos);

/* IP address Information Element
 * 3GPP TS 29.274 #8.9
 */
NwRcT s11_ip_address_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

int s11_ip_address_ie_set(NwGtpv2cMsgHandleT     *msg,
                          const gtp_ip_address_t *ip_address);

/* Delay Value Information Element
 * 3GPP TS 29.274 #8.27
 */
NwRcT s11_delay_value_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

int s11_delay_value_ie_set(NwGtpv2cMsgHandleT *msg,
                           const DelayValue_t *delay_value);

/* UE Time Zone Information Element
 * 3GPP TS 29.274 #8.44
 */
NwRcT s11_ue_time_zone_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

int s11_ue_time_zone_ie_set(NwGtpv2cMsgHandleT *msg,
                            const UETimeZone_t *ue_time_zone);

/* Target Identification Information Element
 * 3GPP TS 29.274 #8.51
 */
NwRcT s11_target_identification_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

/* Bearer Flags Information Element
 * 3GPP TS 29.274 #8.32
 */
NwRcT s11_bearer_flags_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

int s11_bearer_flags_ie_set(NwGtpv2cMsgHandleT   *msg,
                            const bearer_flags_t *bearer_flags);

NwRcT s11_indication_flags_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

/* FQ-CSID Information Element
 * 3GPP TS 29.274 #8.62
 */

NwRcT s11_fqcsid_ie_get(
    NwU8T ieType, NwU8T ieLength, NwU8T ieInstance, NwU8T *ieValue, void *arg);

#endif /* S11_IE_FORMATTER_H_ */
