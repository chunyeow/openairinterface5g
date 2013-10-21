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

/*! \file sgw_lite_messages_types.h
 * \brief S11 definitions for interaction between MME and S11
 * 3GPP TS 29.274.
 * Messages are the same as for GTPv2-C but here we abstract the UDP layer
 * \author Sebastien ROUX <sebastien.roux@eurecom.fr>
 * \date 2013
 * \version 0.1
 */

#include "../SGW-LITE/sgw_lite_ie_defs.h"

#ifndef SGW_LITE_MESSAGES_TYPES_H_
#define SGW_LITE_MESSAGES_TYPES_H_

/** @struct SgwCreateSessionRequest
 *  @brief Create Session Request
 *
 * The Create Session Request will be sent on S11 interface as
 * part of these procedures:
 * - E-UTRAN Initial Attach
 * - UE requested PDN connectivity
 * - Tracking Area Update procedure with Serving GW change
 * - S1/X2-based handover with SGW change
 */
typedef struct {
    Teid_t             teid;                ///< S11- S-GW Tunnel Endpoint Identifier
    Imsi_t             imsi;
    Msisdn_t           msisdn;
    Mei_t              mei;
    Uli_t              uli;
    ServingNetwork_t   serving_network;
    rat_type_t         rat_type;
    FTeid_t            sender_fteid_for_cp; ///< Sender F-TEID for control plane (MME)
    FTeid_t            pgw_address_for_cp;  ///< PGW S5/S8 address for control plane or PMIP
    char               apn[APN_MAX_LENGTH + 1]; ///< Access Point Name
    SelectionMode_t    selection_mode;      ///< Selection Mode
    pdn_type_t         pdn_type;            ///< PDN Type
    PAA_t              paa;                 ///< PDN Address Allocation
    /* Shall include APN Restriction but not used in our case */
    ambr_t             ambr;                ///< Aggregate Maximum Bit Rate
    /* TODO: add Protocol Configuration options */
    FQ_CSID_t          mme_fq_csid;
    UETimeZone_t       ue_time_zone;
    UCI_t              uci;                 ///< User CSG Information
    bearer_to_create_t bearer_to_create;    ///< Bearer Contexts to be created

    indication_flags_t indication_flags;

    /* S11 stack specific parameter. Not used in standalone epc mode */
    void              *trxn;                ///< Transaction identifier
    uint32_t           peer_ip;             ///< MME ipv4 address for S-GW or S-GW ipv4 address for MME
    uint16_t           peer_port;           ///< MME port for S-GW or S-GW port for MME
} SgwCreateSessionRequest;

/** @struct SgwCreateSessionResponse
 *  @brief Create Session Response
 *
 * The Create Session Response will be sent on S11 interface as
 * part of these procedures:
 * - E-UTRAN Initial Attach
 * - UE requested PDN connectivity
 * - Tracking Area Update procedure with SGW change
 * - S1/X2-based handover with SGW change
 */
typedef struct {
    Teid_t                   teid;                ///< Tunnel Endpoint Identifier

    SGWCause_t               cause;
    FTeid_t                  s11_sgw_teid;        ///< Sender F-TEID for control plane
    FTeid_t                  s5_s8_pgw_teid;      ///< Sender F-TEID for control plane
    PAA_t                    paa;                 ///< PDN Address Allocation
    APNRestriction_t         apn_restriction;
    bearer_context_created_t bearer_context_created;
    FQ_CSID_t                pgw_fq_csid;
    FQ_CSID_t                sgw_fq_csid;

    /* S11 stack specific parameter. Not used in standalone epc mode */
    void                    *trxn;               ///< Transaction identifier
    uint32_t                 peer_ip;            ///< MME ipv4 address
} SgwCreateSessionResponse;

/** @struct SgwModifyBearerRequest
 *  @brief Modify Bearer Request
 *
 * The Modify Bearer Request will be sent on S11 interface as
 * part of these procedures:
 * - E-UTRAN Tracking Area Update without SGW Change
 * - UE triggered Service Request
 * - S1-based Handover
 * - E-UTRAN Initial Attach
 * - UE requested PDN connectivity
 * - X2-based handover without SGWrelocation
 */
typedef struct {
    Teid_t                     teid;                       ///< Tunnel Endpoint Identifier
//    FTeid_t                    s11_sgw_teid;               ///< S11- S-GW Tunnel Endpoint Identifier
    /* Delay Value in integer multiples of 50 millisecs, or zero */
    DelayValue_t               delay_dl_packet_notif_req;
    bearer_context_to_modify_t bearer_context_to_modify;
    FQ_CSID_t                  mme_fq_csid;
    indication_flags_t         indication_flags;
    rat_type_t                 rat_type;

    /* S11 stack specific parameter. Not used in standalone epc mode */
    void                      *trxn;                        ///< Transaction identifier
} SgwModifyBearerRequest;

/** @struct SgwModifyBearerResponse
 *  @brief Modify Bearer Response
 *
 * The Modify Bearer Response will be sent on S11 interface as
 * part of these procedures:
 * - E-UTRAN Tracking Area Update without SGW Change
 * - UE triggered Service Request
 * - S1-based Handover
 * - E-UTRAN Initial Attach
 * - UE requested PDN connectivity
 * - X2-based handover without SGWrelocation
 */
typedef struct {
    Teid_t           teid;               ///< Tunnel Endpoint Identifier
    SGWCause_t       cause;
#define MODIFY_BEARER_RESPONSE_MOD  0x0
#define MODIFY_BEARER_RESPONSE_REM  0x1
    unsigned         present:1;          ///< Choice present in union choice
    union {
        bearer_context_modified_t bearer_modified;      ///< Bearer to be modified
        bearer_for_removal_t      bearer_for_removal;   ///< Bearer to be removed
    } choice;

    /* S11 stack specific parameter. Not used in standalone epc mode */
    void                         *trxn;                      ///< Transaction identifier
} SgwModifyBearerResponse;

typedef struct {
    Teid_t      teid;                   ///< Tunnel Endpoint Identifier
    EBI_t       lbi;                    ///< Linked EPS Bearer ID
    FTeid_t     sender_fteid_for_cp;    ///< Sender F-TEID for control plane

    /* Operation Indication: This flag shall be set over S4/S11 interface
     * if the SGW needs to forward the Delete Session Request message to 
     * the PGW. This flag shall not be set if the ISR associated GTP 
     * entity sends this message to the SGW in the Detach procedure.
     * This flag shall also not be set to 1 in the SRNS Relocation Cancel
     * Using S4 (6.9.2.2.4a in 3GPP TS 23.060 [4]), Inter RAT handover 
     * Cancel procedure with SGW change TAU with Serving GW change, 
     * Gn/Gb based RAU (see 5.5.2.5, 5.3.3.1, D.3.5 in 3GPP TS 23.401 [3],
     * respectively), S1 Based handover Cancel procedure with SGW change.
     */
    indication_flags_t indication_flags;

    /* GTPv2-C specific parameters */
    void       *trxn;
    uint32_t    peer_ip;
} SgwDeleteSessionRequest;

/** @struct SgwDeleteSessionResponse
 *  @brief Delete Session Response
 *
 * The Delete Session Response will be sent on S11 interface as
 * part of these procedures:
 * - EUTRAN Initial Attach
 * - UE, HSS or MME Initiated Detach
 * - UE or MME Requested PDN Disconnection
 * - Tracking Area Update with SGW Change
 * - S1 Based Handover with SGW Change
 * - X2 Based Handover with SGW Relocation
 * - S1 Based handover cancel with SGW change
 */
typedef struct {
    Teid_t      teid;                   ///< Remote Tunnel Endpoint Identifier
    SGWCause_t  cause;

    /* GTPv2-C specific parameters */
    void       *trxn;
    uint32_t    peer_ip;
} SgwDeleteSessionResponse;

#endif
