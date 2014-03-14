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

#ifndef SGW_LITE_IE_DEFS_H_
#define SGW_LITE_IE_DEFS_H_

typedef uint8_t  EBI_t;
typedef char*    APN_t;
typedef uint8_t  APNRestriction_t;
typedef uint8_t  DelayValue_t;
typedef uint32_t Teid_t;
typedef uint32_t SequenceNumber_t;

#define IMSI_DIGITS_MAX 15

typedef struct {
    uint8_t digit[IMSI_DIGITS_MAX];
} Imsi_t;

typedef struct {
    uint8_t *digit;
    uint8_t  number_of_digit;
} Msisdn_t;

#define MEI_IMEI    0x0
#define MEI_IMEISV  0x1

typedef struct {
    uint8_t present;
    union {
        unsigned imei:15;
        unsigned imeisv:16;
    } choice;
} Mei_t;

typedef struct {
    uint8_t  mcc[3];
    uint8_t  mnc[3];
    uint16_t lac;
    uint16_t ci;
} Cgi_t;

typedef struct {
    uint8_t  mcc[3];
    uint8_t  mnc[3];
    uint16_t lac;
    uint16_t sac;
} Sai_t;

typedef struct {
    uint8_t  mcc[3];
    uint8_t  mnc[3];
    uint16_t lac;
    uint16_t rac;
} Rai_t;

typedef struct {
    uint8_t  mcc[3];
    uint8_t  mnc[3];
    uint16_t tac;
} Tai_t;

typedef struct {
    uint8_t  mcc[3];
    uint8_t  mnc[3];
    uint32_t eci;
} Ecgi_t;

typedef struct {
    uint8_t  mcc[3];
    uint8_t  mnc[3];
    uint16_t lac;
} Lai_t;

#define ULI_CGI  0x01
#define ULI_SAI  0x02
#define ULI_RAI  0x04
#define ULI_TAI  0x08
#define ULI_ECGI 0x10
#define ULI_LAI  0x20

typedef struct {
    uint8_t present;
    union {
        Cgi_t  cgi;
        Sai_t  sai;
        Rai_t  rai;
        Tai_t  tai;
        Ecgi_t ecgi;
        Lai_t  lai;
    } choice;
} Uli_t;

typedef struct {
    uint8_t mcc[3];
    uint8_t mnc[3];
} ServingNetwork_t;

typedef enum RatType_e {
    RAT_TYPE_UTRAN = 1,
    RAT_TYPE_GERAN,
    RAT_TYPE_WLAN,
    RAT_TYPE_GAN,
    RAT_TYPE_HSPA_EVOLUTION,
    RAT_TYPE_EUTRAN,
} RatType_t;

/* WARNING: not complete... */
typedef enum InterfaceType_e {
    S1_U_ENODEB_GTP_U = 0,
    S1_U_SGW_GTP_U,
    S12_RNC_GTP_U,
    S12_SGW_GTP_U,
    S5_S8_SGW_GTP_U,
    S5_S8_PGW_GTP_U,
    S11_MME_GTP_C = 11,
} InterfaceType_t;

typedef struct {
    unsigned        ipv4:1;
    unsigned        ipv6:1;
    InterfaceType_t interface_type;
    uint32_t        teid; ///< TEID or GRE Key
    uint32_t        ipv4_address;
    uint8_t         ipv6_address[16];
} FTeid_t;

typedef enum SelectionMode_e {
    MS_O_N_P_APN_S_V    = 0,    ///< MS or network provided APN, subscribed verified
    MS_P_APN_S_N_V      = 1,    ///< MS provided APN, subscription not verified
    N_P_APN_S_N_V       = 2,    ///< Network provided APN, subscription not verified
} SelectionMode_t;

typedef enum PDNType_e {
    IPv4   = 1,
    IPv6   = 2,
    Ipv4v6 = 3
} PDNType_t;

typedef struct {
    PDNType_t pdn_type;
    uint32_t  ipv4_address;
    uint8_t   ipv6_address[16];
} PAA_t;

typedef struct {
    uint32_t uplink_ambr;
    uint32_t downlink_ambr;
} AMBR_t;

typedef enum NodeIdType_e {
    GLOBAL_UNICAST_IPv4 = 0,
    GLOBAL_UNICAST_IPv6 = 1,
    TYPE_EXOTIC         = 2, ///< (MCC * 1000 + MNC) << 12 + Integer value assigned to MME by operator
} NodeIdType_t;

typedef struct {
    NodeIdType_t node_id_type;
    uint16_t     csid;          ///< Connection Set Identifier
    union {
        uint32_t unicast_ipv4;
        uint8_t  unicast_ipv6[16];
        struct {
            uint8_t  mcc;
            uint8_t  mnc;
            uint16_t operator_specific_id;
        } exotic;
    } node_id;
} FQ_CSID_t;

typedef struct {
    uint8_t  time_zone;
    unsigned daylight_saving_time:2;
} UETimeZone_t;

typedef enum AccessMode_e {
    CLOSED_MODE = 0,
    HYBRID_MODE = 1,
} AccessMode_t;

typedef struct {
    uint8_t  mcc[3];
    uint8_t  mnc[3];
    uint32_t csg_id;
    AccessMode_t access_mode;
    unsigned lcsg:1;
    unsigned cmi:1;
} UCI_t;

#define PRE_EMPTION_CAPABILITY_ENABLED  (0x0)
#define PRE_EMPTION_CAPABILITY_DISABLED (0x1)
#define PRE_EMPTION_VULNERABILITY_ENABLED  (0x0)
#define PRE_EMPTION_VULNERABILITY_DISABLED (0x1)

typedef struct {
    /* PCI (Pre-emption Capability)
     * The following values are defined:
     * - PRE-EMPTION_CAPABILITY_ENABLED (0)
     *    This value indicates that the service data flow or bearer is allowed
     *    to get resources that were already assigned to another service data
     *    flow or bearer with a lower priority level.
     * - PRE-EMPTION_CAPABILITY_DISABLED (1)
     *    This value indicates that the service data flow or bearer is not
     *    allowed to get resources that were already assigned to another service
     *    data flow or bearer with a lower priority level.
     * Default value: PRE-EMPTION_CAPABILITY_DISABLED
     */
    unsigned pci:1;
    /* PL (Priority Level): defined in 3GPP TS.29.212 #5.3.45
     * Values 1 to 15 are defined, with value 1 as the highest level of priority.
     * Values 1 to 8 should only be assigned for services that are authorized to
     * receive prioritized treatment within an operator domain. Values 9 to 15
     * may be assigned to resources that are authorized by the home network and
     * thus applicable when a UE is roaming.
     */
    unsigned pl:4;
    /* PVI (Pre-emption Vulnerability): defined in 3GPP TS.29.212 #5.3.47
     * Defines whether a service data flow can lose the resources assigned to it
     * in order to admit a service data flow with higher priority level.
     * The following values are defined:
     * - PRE-EMPTION_VULNERABILITY_ENABLED (0)
     *   This value indicates that the resources assigned to the service data
     *   flow or bearer can be pre-empted and allocated to a service data flow
     *   or bearer with a higher priority level.
     * - PRE-EMPTION_VULNERABILITY_DISABLED (1)
     *   This value indicates that the resources assigned to the service data
     *   flow or bearer shall not be pre-empted and allocated to a service data
     *   flow or bearer with a higher priority level.
     * Default value: EMPTION_VULNERABILITY_ENABLED
     */
    unsigned pvi:1;
    uint8_t  qci;
    uint32_t mbr_uplink;    ///< Maximum bit rate for uplink
    uint32_t mbr_downlink;  ///< Maximum bit rate for downlink
    uint32_t gbr_uplink;    ///< Guaranteed bit rate for uplink
    uint32_t gbr_downlink;  ///< Guaranteed bit rate for downlink
} BearerQOS_t;

/* Cause as defined in 3GPP TS 29.274 #8.4 */
typedef enum SGWCause_e {
    /* Request / Initial message */
    LOCAL_DETACH                = 2,
    COMPLETE_DETACH             = 3,
    RAT_CHANGE_3GPP_TO_NON_3GPP = 4,  ///< RAT changed from 3GPP to Non-3GPP
    IMSI_DETACH_ONLY            = 7,
    /* Acceptance in a Response/Triggered message */
    REQUEST_ACCEPTED            = 16,
    REQUEST_ACCEPTED_PARTIALLY  = 17,
    NEW_PDN_TYPE_NW_PREF        = 18, ///< New PDN type due to network preference
    NEW_PDN_TYPE_SAB_ONLY       = 19, ///< New PDN type due to single address bearer only
    /* Rejection in a Response triggered message. */
    CONTEXT_NOT_FOUND           = 64,
    SYSTEM_FAILURE              = 72,
    GRE_KEY_NOT_FOUND           = 80,
    INVALID_PEER                = 109,
    M_PDN_APN_NOT_ALLOWED       = 116, ///< Multiple PDN connections for a given APN not allowed.
} SGWCause_t;

#endif  /* SGW_LITE_IE_DEFS_H_ */
