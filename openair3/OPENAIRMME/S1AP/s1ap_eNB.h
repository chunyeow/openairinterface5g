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

#ifndef S1AP_ENB_H_
#define S1AP_ENB_H_

/** @defgroup _s1ap_impl_ S1AP Layer Reference Implementation
 * @ingroup _ref_implementation_
 * @{
 */

typedef struct s1ap_eNB_config_s {
    char    *ip_address;
    uint8_t  present;
    uint8_t  eNB_instance_id;
} s1ap_eNB_config_t;

struct s1ap_e_rab_level_qos_parameter {
    uint8_t qci;
    //TODO: be completed
};

enum s1ap_ip_addr_e {
    S1AP_ADDR_IPV4 = 0x1,       ///< IP address is only IPv4
    S1AP_ADDR_IPV6 = 0x2,       ///< IP address is only IPv6
    S1AP_ADDR_IPV4_IPV6 = 0x3,  ///< IP address present IPv4 and IPv6
};

struct s1ap_ip_addr_s {
    enum s1ap_ip_addr_e ip_addr_present;
    uint8_t ipv4_addr[4];   ///< IPv4 address = 32bits
    uint8_t ipv6_addr[8];   ///< IPv6 address = 128bits
};

struct s1ap_ue_e_rab_description_s {
    struct s1ap_ue_e_rab_description_s *next_e_rab;

    uint8_t e_rab_id;                                    ///< E-RAB Id
    uint8_t gtp_teid[4];                                 ///< GTP-Tunnel Endpoint Identifier
    struct s1ap_e_rab_level_qos_parameter qos_parameter; ///< E-RAB QOS level parameter
    struct s1ap_ip_addr_s ip_addr;
};

struct s1ap_eNB_UE_description_s {
    struct s1ap_eNB_UE_description_s *next_ue;
    struct s1ap_eNB_UE_description_s *previous_ue;

    struct s1ap_eNB_description_s *eNB; ///< Quick reference to eNB this UE is attached to

    uint32_t eNB_UE_s1ap_id;
    uint32_t mme_UE_s1ap_id;
    uint32_t stream_recv;
    uint32_t stream_send;

    /** \brief Maximum aggregate bitrates
     * @{ */
    uint64_t maximum_bit_rate_downlink; ///< Downlink bitrate
    uint64_t maximum_bit_rate_uplink;   ///< Uplink bitrate
    /* @} */

    /** \brief UE security capabilities
     * @{ */
    uint16_t encryptions_algorithms; ///< = 0, only EEA0 supported. = 1/2 128-EEA 1/2 supported
    uint16_t integrity_algorithms;   ///< = 1, 128-EIA1 supported, = 2 128-EIA2 supported
    /* @} */

    uint8_t security_key[32];   ///KeNB security key

    /** \brief E-RAB parameters
     * @{ */
    struct s1ap_ue_e_rab_description_s *e_rab_list_head;    ///< List of E-RAB setup-ed for UE
    uint8_t nb_of_e_rabs;                                   ///< Number of E-RAB
    /* @} */
};

struct s1ap_eNB_description_s {
    struct s1ap_eNB_description_s *next_eNB;
    struct s1ap_eNB_description_s *previous_eNB;

    struct s1ap_eNB_UE_description_s *ue_list_head;
    struct s1ap_eNB_UE_description_s *ue_list_tail;
    uint32_t nb_ue;

    uint8_t  eNB_id;
    uint8_t  state;
    uint32_t assocId;
    uint32_t instreams;
    uint32_t outstreams;
    uint32_t nextstream; ///< First usable stream for UE associated signalling
} s1ap_eNB_description_t;

enum s1ap_eNB_state_s{
    S1AP_ENB_STATE_DECONNECTED = 0x0,
    S1AP_ENB_STATE_WAITING     = 0x1,
    S1AP_ENB_STATE_CONNECTED   = 0x2,
    S1AP_ENB_STATE_MAX         = S1AP_ENB_STATE_CONNECTED,
} s1ap_eNB_state_t;

int s1ap_eNB_init(const char *mme_ip_address, const uint8_t eNB_id);

struct s1ap_eNB_description_s* s1ap_eNB_add_new(void);
struct s1ap_eNB_description_s* s1ap_get_eNB_assoc_id(uint32_t assocId);
struct s1ap_eNB_description_s* s1ap_get_eNB_eNB_id(uint8_t eNB_id);

struct s1ap_eNB_UE_description_s *s1ap_UE_add_new(struct s1ap_eNB_description_s *eNB_ref);
struct s1ap_eNB_UE_description_s *s1ap_get_ue_id_pair(uint8_t eNB_id, uint8_t ue_id);
struct s1ap_eNB_UE_description_s *s1ap_get_ue_assoc_id_eNB_ue_s1ap_id(
    uint32_t assoc_id,
    uint32_t eNB_ue_s1ap_id);

#endif /* S1AP_ENB_H_ */
