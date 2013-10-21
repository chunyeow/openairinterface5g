/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2012 Eurecom

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

#include <stdint.h>

#include "queue.h"
#include "tree.h"

#include "sctp_eNB_defs.h"

#ifndef S1AP_ENB_DEFS_H_
#define S1AP_ENB_DEFS_H_

#define S1AP_ENB_NAME_LENGTH_MAX    (150)

typedef enum {
    /* Disconnected state: initial state for any association. */
    S1AP_ENB_STATE_DISCONNECTED = 0x0,
    /* State waiting for S1 Setup response message if eNB is MME accepted or
     * S1 Setup failure if MME rejects the eNB.
     */
    S1AP_ENB_STATE_WAITING     = 0x1,
    /* The eNB is successfully connected to MME, UE contexts can be created. */
    S1AP_ENB_STATE_CONNECTED   = 0x2,
    /* The MME has sent an overload start message. Once the MME disables the
     * OVERLOAD marker, the state of the association will be
     * S1AP_ENB_STATE_CONNECTED.
     */
    S1AP_ENB_OVERLOAD          = 0x3,
    /* Max number of states available */
    S1AP_ENB_STATE_MAX,
} s1ap_eNB_state_t;

/* If the Overload Action IE in the OVERLOAD START message is set to
 * - “reject all RRC connection establishments for non-emergency mobile
 *    originated data transfer “ (i.e. reject traffic corresponding to RRC cause
 *    “mo-data “ (TS 36.331 [16])), or
 * - “reject all RRC connection establishments for signalling “ (i.e. reject
 *    traffic corresponding to RRC cause “modata” and “mo-signalling”
 *    (TS 36.331 [16])),or
 * - “only permit RRC connection establishments for emergency sessions and
 *    mobile terminated services” (i.e. only permit traffic corresponding to RRC
 *    cause “emergency” and “mt-Access” (TS 36.331 [16])).
 *
 * NOTE: When the Overload Action IE is set to “only permit RRC connection
 * establishments for emergency sessions and mobile terminated services”,
 * emergency calls with RRC cause “highPriorityAcess” from high priority users
 * are rejected (TS 24.301 [24]).
 */
typedef enum {
    S1AP_OVERLOAD_REJECT_MO_DATA        = 0x0,
    S1AP_OVERLOAD_REJECT_ALL_SIGNALLING = 0x1,
    S1AP_OVERLOAD_ONLY_EMERGENCY_AND_MT = 0x2,
    S1AP_NO_OVERLOAD                    = 0x3,
    S1AP_OVERLOAD_MAX,
} s1ap_overload_state_t;

typedef enum {
    PAGING_DRX_32  = 0x0,
    PAGING_DRX_64  = 0x1,
    PAGING_DRX_128 = 0x2,
    PAGING_DRX_256 = 0x3,
} paging_drx_t;

typedef struct {
    /* Octet string data */
    uint8_t  *buffer;
    /* Length of the octet string */
    uint32_t  length;
} nas_pdu_t, ue_radio_cap_t;

typedef struct {
    uint16_t mcc;
    uint16_t mnc;
    uint8_t  mme_code;
    uint16_t mme_group_id;
} gummei_t;

typedef struct {
    uint8_t  mme_code;
    uint32_t m_tmsi;
} s_tmsi_t;

/* Provides the establishment cause for the RRC connection request as provided
 * by the upper layers. W.r.t. the cause value names: highPriorityAccess
 * concerns AC11..AC15, ‘mt’ stands for ‘Mobile Terminating’ and ‘mo’ for
 * 'Mobile Originating'. Defined in TS 36.331.
 */
typedef enum {
    RRC_CAUSE_EMERGENCY        = 0x0,
    RRC_CAUSE_HIGH_PRIO_ACCESS = 0x1,
    RRC_CAUSE_MT_ACCESS        = 0x2,
    RRC_CAUSE_MO_SIGNALLING    = 0x3,
    RRC_CAUSE_MO_DATA          = 0x4,
    RRC_CAUSE_MAX              = RRC_CAUSE_MO_DATA,
} rrc_establishment_cause_t;

typedef struct {
    uint8_t qci;

} e_rab_level_qos_parameter_t;

typedef struct {
    /* Length of the transport layer address buffer. S1AP layer received a
     * bit string<1..160> containing one of the following addresses: ipv4,
     * ipv6, or ipv4 and ipv6. The layer doesn't interpret the buffer but
     * silently forward it to S1-U.
     */
    uint8_t length;
    uint8_t buffer[20];
} transport_layer_addr_t;

typedef struct {
    /* Unique e_rab_id for the UE. */
    uint8_t                     e_rab_id;
    /* Quality of service for this e_rab */
    e_rab_level_qos_parameter_t qos;
    /* The NAS PDU should be forwarded by the RRC layer to the NAS layer */
    nas_pdu_t                   nas_pdu;
    /* The transport layer address for the IP packets */
    transport_layer_addr_t      sgw_addr;
    /* S-GW Tunnel endpoint identifier */
    uint32_t                    gtp_teid;
} e_rab_t;

typedef struct {
    /* Unique e_rab_id for the UE. */
    uint8_t e_rab_id;
    /* The transport layer address for the IP packets */
    transport_layer_addr_t eNB_addr;
    /* S-GW Tunnel endpoint identifier */
    uint32_t               gtp_teid;
} e_rab_setup_t;

typedef struct {
    /* Unique e_rab_id for the UE. */
    uint8_t e_rab_id;
    /* Cause of the failure */
//     cause_t cause;
} e_rab_failed_t;

typedef struct {
#define S_TMSI_PROVIDED 0x0
#define GUMMEI_PROVIDED 0x1
    unsigned     present:1;
    union {
        gummei_t gummei;
        s_tmsi_t s_tmsi;
    } identity;
} ue_identity_t;

typedef struct {
    /* The NAS First Req is the first message exchanged between RRC and S1AP
     * for an UE.
     * The rnti uniquely identifies an UE within a cell. Later the enb_ue_s1ap_id
     * will be the unique identifier used between RRC and S1AP.
     */
    uint16_t                  rnti;

    rrc_establishment_cause_t establishment_cause;
    nas_pdu_t                 nas_pdu;
    /* If this flag is set S1AP layer is expecting the GUMMEI. If = 0,
     * the temporary s-tmsi is used.
     */
    ue_identity_t ue_identity;
} s1ap_nas_first_req_t;

typedef struct {
    unsigned  eNB_ue_s1ap_id:24;
    nas_pdu_t nas_pdu;
} s1ap_nas_uplink_t;

typedef struct {
    nas_pdu_t nas_pdu;
//     cause_t   cause;
} s1ap_nas_non_delivery_t;

typedef struct {
    uint16_t  rnti;
    unsigned  eNB_ue_s1ap_id:24;

    /* Number of e_rab to be setup in the list */
    uint8_t   nb_of_e_rabs;
    /* list of e_rab to be setup by RRC layers */
    e_rab_t **e_rab_param;
} s1ap_e_rab_setup_req_t,
s1ap_initial_ctxt_setup_req_t;

typedef struct {
    unsigned  eNB_ue_s1ap_id:24;
    ue_radio_cap_t ue_radio_cap;
} s1ap_ue_cap_info_ind_t;

typedef struct {
    unsigned  eNB_ue_s1ap_id:24;

    /* Number of e_rab setup-ed in the list */
    uint8_t   nb_of_e_rabs;
    /* list of e_rab setup-ed by RRC layers */
    e_rab_setup_t *e_rabs;
    /* Number of e_rab failed to be setup in list */
    uint8_t   nb_of_e_rabs_failed;
    /* list of e_rabs that failed to be setup */
    e_rab_failed_t *e_rabs_failed;
} s1ap_initial_ctxt_setup_resp_t;

typedef enum {
    /** RRC -> S1AP API **/
#define S1AP_API_REQ_IS_INPUT(x) (x & 0x10)
    /* RRC should use this api request for the first NAS message */
    S1AP_API_NAS_FIRST_REQ              = 0x10,
    /* NAS messages with no context activation */
    S1AP_API_NAS_UPLINK                 = 0x11,
    /* When the eNB decides to not start the delivery of a NAS message that has
     * been received over a UE-associated logical S1-connection or the eNB is
     * unable to ensure that the message has been received by the UE, it shall
     * report the non-delivery of this NAS message.
     */
    S1AP_API_NAS_NON_DELIVERY_IND       = 0x12,
    /* This message is sent by the eNB to request the MME to switch DL GTP
     * tunnel termination point(s) from one end-point to another.
     */
    S1AP_API_PATH_SWITCH_REQ            = 0x13,
    /* This message is used to report the outcome of the request from the
     * S1AP_API_NAS_INITIAL_CONTEXT_SETUP_REQ API request.
     */
    S1AP_API_INITIAL_CONTEXT_SETUP_RESP = 0x14,
    /* This message is used to report the unsuccessfull outcome of the request
     * from the S1AP_API_NAS_INITIAL_CONTEXT_SETUP_REQ API request.
     */
    S1AP_API_INITIAL_CONTEXT_SETUP_FAIL = 0x15,
    /* This message is used to report the outcome of the request from the
     * S1AP_API_INITIAL_CONTEXT_SETUP_REQ API request.
     */
    S1AP_API_E_RAB_SETUP_RESP           = 0x16,
    /* This message is used to report the outcome of the request from the
     * S1AP_API_E_RAB_SETUP_REQ API request.
     */
    S1AP_API_E_RAB_MODIFY_RESP          = 0x17,
    /* This message is used to report the outcome of the request from the
     * S1AP_API_E_RAB_MODIFY_REQ API request.
     */
    S1AP_API_E_RAB_RELEASE_RESP         = 0x18,
    /* The purpose of the UE Capability Info Indication procedure is to enable
     * the eNB to provide to the MME UE capability-related information.
     */
    S1AP_API_UE_CAP_INFO_IND            = 0x19,

    /** S1AP -> RRC API **/
#define S1AP_API_REQ_IS_OUTPUT(x) (x & 0x20)
    /* S1AP layer received a valid downlink info transfer message */
    S1AP_API_NAS_DOWNLINK                   = 0x20,
    /* The purpose of the Initial Context Setup procedure is to establish the
     * necessary overall initial UE Context including ERAB context, the Security
     * Key, Handover Restriction List, UE Radio capability and UE Security
     * Capabilities etc.
     */
    S1AP_API_NAS_INITIAL_CONTEXT_SETUP_REQ  = 0x21,
    /* The purpose of the E-RAB Setup procedure is to assign resources on Uu and
     * S1 for one or several E-RABs and to setup corresponding Data Radio
     * Bearers for a given UE.
     */
    S1AP_API_E_RAB_SETUP_REQ                = 0x22,
    /* This message is sent by the MME and is used to request the eNB to modify
     * the Data Radio Bearers and the allocated resources on Uu and S1 for one
     * or several E-RABs.
     */
    S1AP_API_E_RAB_MODIFY_REQ               = 0x23,
    /* This message is sent by the MME and is used to request the eNB to release
     * allocated resources on Uu and S1 for one or several E-RABs.
     */
    S1AP_API_E_RAB_RELEASE_REQ              = 0x24,

    /** S1AP <-> RRC API **/
    /** Messages below are bi-directionnal and can be triggered by both RRC and
     * S1AP.
     **/
#define S1AP_API_REQ_IS_BIDIR(x) (x & 0x40)
    /* At reception of RESET message, the eNB (or MME) shall release all
     * allocated resources on S1 and Uu related to the UE association(s)
     * indicated explicitly or implicitly in the RESET message and remove the
     * indicated UE contexts including S1AP ID.
     */
    S1AP_API_RESET                          = 0x40,
    S1AP_API_RESET_ACK                      = 0x41,
} s1ap_rrc_api_req_type_t;

typedef struct {
    /* The API request type */
    s1ap_rrc_api_req_type_t api_req;
    union {
        /** RRC -> S1AP requests **/
        s1ap_nas_first_req_t            first_nas_req;
        s1ap_nas_uplink_t               nas_uplink;
        s1ap_initial_ctxt_setup_resp_t  initial_ctxt_resp;
        s1ap_nas_non_delivery_t         nas_non_delivery;
        s1ap_ue_cap_info_ind_t          ue_cap_info_ind;
        /** S1AP -> RRC requests **/
        s1ap_e_rab_setup_req_t        e_rab_setup_req;
        s1ap_initial_ctxt_setup_req_t initial_ctxt_setup_req;
    } msg;
} s1ap_rrc_api_req_t;

/* Callback notifier.
 * Called when a new event has to be notified between S1AP -> RRC
 */
typedef int (*rrc_event_notify_t)(s1ap_rrc_api_req_t *api_req);

#endif /* S1AP_ENB_DEFS_H_ */
