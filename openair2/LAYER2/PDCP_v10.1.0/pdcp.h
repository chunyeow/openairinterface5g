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

/*! \file pdcp.c
* \brief pdcp interface with RLC, RRC
* \author  Lionel GAUTHIER and Navid Nikaein
* \date 2009-2012
* \version 1.0
*/

#ifndef __PDCP_H__
#    define __PDCP_H__
//-----------------------------------------------------------------------------
#    ifdef PDCP_C
#        define private_pdcp(x) x
#        define protected_pdcp(x) x
#        define public_pdcp(x) x
#    else
#        define private_pdcp(x)
#        define public_pdcp(x) extern x
#        ifdef PDCP_FIFO_C
#            define protected_pdcp(x) extern x
#        else
#            define protected_pdcp(x)
#        endif
#    endif

#    ifdef PDCP_FIFO_C
#        define private_pdcp_fifo(x) x
#        define protected_pdcp_fifo(x) x
#        define public_pdcp_fifo(x) x
#    else
#        define private_pdcp_fifo(x)
#        define public_pdcp_fifo(x) extern x
#        ifdef PDCP_C
#            define protected_pdcp_fifo(x) extern x
#        else
#            define protected_pdcp_fifo(x)
#        endif
#    endif
//-----------------------------------------------------------------------------
#ifndef NON_ACCESS_STRATUM
  #include "UTIL/MEM/mem_block.h"
  #include "UTIL/LISTS/list.h"
  #include "COMMON/mac_rrc_primitives.h"
#endif //NON_ACCESS_STRATUM
//-----------------------------------------------------------------------------
#include "COMMON/platform_constants.h"
#include "COMMON/platform_types.h"
#include "DRB-ToAddMod.h"
#include "DRB-ToAddModList.h"
#include "SRB-ToAddMod.h"
#include "SRB-ToAddModList.h"
#ifdef Rel10
#include "MBMS-SessionInfoList-r9.h"
#include "PMCH-InfoList-r9.h"
#endif


extern pthread_t       pdcp_thread;
extern pthread_attr_t  pdcp_thread_attr;
extern pthread_mutex_t pdcp_mutex;
extern pthread_cond_t  pdcp_cond;
extern int             pdcp_instance_cnt;

int init_pdcp_thread(void);
void cleanup_pdcp_thread(void);


public_pdcp(unsigned int Pdcp_stats_tx[NB_MODULES_MAX][NB_CNX_CH][NB_RAB_MAX]);
public_pdcp(unsigned int Pdcp_stats_tx_bytes[NB_MODULES_MAX][NB_CNX_CH][NB_RAB_MAX]);
public_pdcp(unsigned int Pdcp_stats_tx_bytes_last[NB_MODULES_MAX][NB_CNX_CH][NB_RAB_MAX]);
public_pdcp(unsigned int Pdcp_stats_tx_rate[NB_MODULES_MAX][NB_CNX_CH][NB_RAB_MAX]);
public_pdcp(unsigned int Pdcp_stats_rx[NB_MODULES_MAX][NB_CNX_CH][NB_RAB_MAX]);
public_pdcp(unsigned int Pdcp_stats_rx_bytes[NB_MODULES_MAX][NB_CNX_CH][NB_RAB_MAX]);
public_pdcp(unsigned int Pdcp_stats_rx_bytes_last[NB_MODULES_MAX][NB_CNX_CH][NB_RAB_MAX]);
public_pdcp(unsigned int Pdcp_stats_rx_rate[NB_MODULES_MAX][NB_CNX_CH][NB_RAB_MAX]);

typedef struct pdcp_stats_t {
  time_stats_t pdcp_run;
  time_stats_t data_req;
  time_stats_t data_ind;
  time_stats_t apply_security; // 
  time_stats_t validate_security;
  time_stats_t pdcp_ip;
  time_stats_t ip_pdcp; // separte thread
}pdcp_stats_t; // common to eNB and UE


typedef struct pdcp_t {
  boolean_t instanciated_instance;
  uint16_t       header_compression_profile;

  /* SR: added this flag to distinguish UE/eNB instance as pdcp_run for virtual
   * mode can receive data on NETLINK for eNB while eNB_flag = 0 and for UE when eNB_flag = 1
   */
  boolean_t is_ue;
  boolean_t is_srb;

  /* Configured security algorithms */
  uint8_t cipheringAlgorithm;
  uint8_t integrityProtAlgorithm;

  /* User-Plane encryption key
   * Control-Plane RRC encryption key
   * Control-Plane RRC integrity key
   * These keys are configured by RRC layer
   */
  uint8_t *kUPenc;
  uint8_t *kRRCint;
  uint8_t *kRRCenc;

  uint8_t security_activated;

  rlc_mode_t rlc_mode;
  uint8_t status_report;
  uint8_t seq_num_size;

  logical_chan_id_t lcid;
  rb_id_t           rb_id;
  /*
   * Sequence number state variables
   *
   * TX and RX window
   */
  pdcp_sn_t next_pdcp_tx_sn;
  pdcp_sn_t next_pdcp_rx_sn;
  pdcp_sn_t next_pdcp_rx_sn_before_integrity;
  /*
   * TX and RX Hyper Frame Numbers
   */
  pdcp_hfn_t tx_hfn;
  pdcp_hfn_t rx_hfn;
  pdcp_hfn_offset_t rx_hfn_offset; // related to sn mismatch
  
  /*
   * SN of the last PDCP SDU delivered to upper layers
   */
  pdcp_sn_t  last_submitted_pdcp_rx_sn;

  /*
   * Following array is used as a bitmap holding missing sequence
   * numbers to generate a PDCP Control PDU for PDCP status
   * report (see 6.2.6)
   */
  uint8_t missing_pdu_bitmap[512];
  /*
   * This is intentionally signed since we need a 'NULL' value
   * which is not also a valid sequence number
   */
  short int first_missing_pdu;
  /*
   * decipher using a different rx_hfn
   */
  
  
} pdcp_t;

#if defined(Rel10)
typedef struct pdcp_mbms_t {
  boolean_t instanciated_instance;
  rb_id_t   rb_id;
} pdcp_mbms_t;
#endif
/*
 * Following symbolic constant alters the behaviour of PDCP
 * and makes it linked to PDCP test code under targets/TEST/PDCP/
 *
 * For the version at SVN repository this should be UNDEFINED!
 * XXX And later this should be configured through the Makefile
 * under targets/TEST/PDCP/
 */

/*! \fn boolean_t pdcp_data_req(module_id_t , module_id_t , frame_t , eNB_flag_t , srb_flag_t , rb_id_t , mui_t , confirm_t ,sdu_size_t , unsigned char* , pdcp_transmission_mode_t )
* \brief This functions handles data transfer requests coming either from RRC or from IP
* \param[in]  enb_mod_idP        Virtualized enb module identifier, Not used if eNB_flagP = 0.
* \param[in]  ue_mod_idP         Virtualized ue module identifier.
* \param[in] frame Frame number
* \param[in] Shows if relevant PDCP entity is part of an eNB or a UE
* \param[in] rab_id Radio Bearer ID
* \param[in] muiP
* \param[in] confirmP
* \param[in] sdu_buffer_size Size of incoming SDU in bytes
* \param[in] sdu_buffer Buffer carrying SDU
* \param[in] mode flag to indicate whether the userplane data belong to the control plane or data plane or transparent
* \return TRUE on success, FALSE otherwise
* \note None
* @ingroup _pdcp
*/
public_pdcp(boolean_t pdcp_data_req(
        const module_id_t eNB_id,
        const module_id_t UE_id,
        const frame_t frame,
        const eNB_flag_t eNB_flag,
        const srb_flag_t srb_flagP,
        const rb_id_t rb_id,
        const mui_t muiP,
        const confirm_t confirmP, \
        const sdu_size_t sdu_buffer_size,
        unsigned char* const sdu_buffer,
        const pdcp_transmission_mode_t mode));

/*! \fn boolean_t pdcp_data_ind(module_id_t, module_id_t, frame_t, eNB_flag_t, srb_flag_t, MBMS_flag_t, rb_id_t, sdu_size_t, mem_block_t*, boolean_t)
* \brief This functions handles data transfer indications coming from RLC
* \param[in]  enb_mod_idP        Virtualized enb module identifier, Not used if eNB_flagP = 0.
* \param[in]  ue_mod_idP         Virtualized ue module identifier.
* \param[in] frame Frame number
* \param[in] Shows if relevant PDCP entity is part of an eNB or a UE
* \param[in] Shows if rb is SRB
* \param[in] Tells if MBMS traffic
* \param[in] rab_id Radio Bearer ID
* \param[in] sdu_buffer_size Size of incoming SDU in bytes
* \param[in] sdu_buffer Buffer carrying SDU
* \param[in] is_data_plane flag to indicate whether the userplane data belong to the control plane or data plane
* \return TRUE on success, FALSE otherwise
* \note None
* @ingroup _pdcp
*/
public_pdcp(boolean_t pdcp_data_ind(
        const module_id_t eNB_id,
        const module_id_t UE_id,
        const frame_t frame,
        const eNB_flag_t eNB_flag,
        const srb_flag_t srb_flagP,
        const MBMS_flag_t MBMS_flagP,
        const rb_id_t rb_id,
        const sdu_size_t sdu_buffer_size,
        mem_block_t* const sdu_buffer));

/*! \fn void rrc_pdcp_config_req(module_id_t , module_id_t ,frame_t,eNB_flag_t,uint32_t,rb_id_t,uint8_t)
* \brief This functions initializes relevant PDCP entity
* \param[in]  enb_mod_idP        Virtualized enb module identifier, Not used if eNB_flagP = 0.
* \param[in]  ue_mod_idP         Virtualized ue module identifier.
* \param[in] frameP frame counter (TTI)
* \param[in] eNB_flagP flag indicating the node type
* \param[in] actionP flag for action: add, remove , modify
* \param[in] rb_idP Radio Bearer ID of relevant PDCP entity
* \param[in] security_modeP Radio Bearer ID of relevant PDCP entity
* \return none
* \note None
* @ingroup _pdcp
*/
public_pdcp(void rrc_pdcp_config_req (
        const module_id_t enb_idP,
        const module_id_t ue_idP,
        const frame_t     frameP,
        const eNB_flag_t  eNB_flagP,
        const srb_flag_t  srb_flagP,
        const uint32_t    actionP,
        const rb_id_t     rb_idP,
        const uint8_t     security_modeP);)

/*! \fn bool rrc_pdcp_config_asn1_req (module_id_t module_id, frame_t frame, eNB_flag_t eNB_flag, SRB_ToAddModList_t* srb2add_list, DRB_ToAddModList_t* drb2add_list, DRB_ToReleaseList_t*  drb2release_list)
* \brief  Function for RRC to configure a Radio Bearer.
* \param[in]  enb_mod_idP        Virtualized enb module identifier.
* \param[in]  ue_mod_idP         Virtualized ue module identifier.
* \param[in]  frame              Frame index.
* \param[in]  eNB_flag           Flag to indicate eNB (1) or UE (0)
* \param[in]  index             index of UE or eNB depending on the eNB_flag
* \param[in]  srb2add_list      SRB configuration list to be created.
* \param[in]  drb2add_list      DRB configuration list to be created.
* \param[in]  drb2release_list  DRB configuration list to be released.
* \param[in]  security_mode     Security algorithm to apply for integrity/ciphering
* \param[in]  kRRCenc           RRC encryption key
* \param[in]  kRRCint           RRC integrity key
* \param[in]  kUPenc            User-Plane encryption key
* \return     A status about the processing, OK or error code.
*/
public_pdcp(
boolean_t rrc_pdcp_config_asn1_req (
        const module_id_t          eNB_idP,
        const module_id_t          ue_idP,
        const frame_t              frameP,
        const eNB_flag_t           eNB_flagP,
        SRB_ToAddModList_t  *const srb2add_list,
        DRB_ToAddModList_t  *const drb2add_list,
        DRB_ToReleaseList_t *const drb2release_list,
        const uint8_t                   security_modeP,
        uint8_t                  *const kRRCenc,
        uint8_t                  *const kRRCint,
        uint8_t                  *const kUPenc
#ifdef Rel10
        ,PMCH_InfoList_r9_t  *pmch_InfoList_r9
#endif
                               ));

/*! \fn boolean_t pdcp_config_req_asn1 (module_id_t module_id, frame_t frame, eNB_flag_t eNB_flag, srb_flag_t srb_flagP, uint32_t  action, rb_id_t rb_id, uint8_t rb_sn, uint8_t rb_report, uint16_t header_compression_profile, uint8_t security_mode)
* \brief  Function for RRC to configure a Radio Bearer.
* \param[in]  pdcp_pP            Pointer on PDCP structure.
* \param[in]  enb_mod_idP        Virtualized enb module identifier, Not used if eNB_flagP = 0.
* \param[in]  ue_mod_idP         Virtualized ue module identifier.
* \param[in]  frame              Frame index.
* \param[in]  eNB_flag           Flag to indicate eNB (1) or UE (0)
* \param[in]  srb_flagP          Flag to indicate SRB (1) or DRB (0)
* \param[in]  action             add, remove, modify a RB
* \param[in]  rb_id              radio bearer id
* \param[in]  rb_sn              sequence number for this radio bearer
* \param[in]  drb_report         set a pdcp report for this drb
* \param[in]  header_compression set the rohc profile
* \param[in]  security_mode      set the integrity and ciphering algs
* \param[in]  kRRCenc            RRC encryption key
* \param[in]  kRRCint            RRC integrity key
* \param[in]  kUPenc             User-Plane encryption key
* \return     A status about the processing, OK or error code.
*/
public_pdcp(boolean_t pdcp_config_req_asn1 (
        pdcp_t         *const pdcp_pP,
        const module_id_t      enb_idP,
        const module_id_t      ue_idP,
        const frame_t          frameP,
        const eNB_flag_t       eNB_flagP,
        const srb_flag_t       srb_flagP,
        const rlc_mode_t       rlc_mode,
        const uint32_t         action,
        const uint16_t         lc_id,
        const uint16_t         mch_id,
        const rb_id_t          rb_id,
        const uint8_t          rb_sn,
        const uint8_t          rb_report,
        const uint16_t         header_compression_profile,
        const uint8_t          security_mode,
        uint8_t         *const kRRCenc,
        uint8_t         *const kRRCint,
        uint8_t         *const kUPenc));
/*! \fn void rrc_pdcp_config_release(module_id_t, rb_id_t)
* \brief This functions is unused
* \param[in] module_id Module ID of relevant PDCP entity
* \param[in] rab_id Radio Bearer ID of relevant PDCP entity
* \return none
* \note None
* @ingroup _pdcp
*/
//public_pdcp(void rrc_pdcp_config_release (module_id_t, rb_id_t);)

/*! \fn void pdcp_run(frame_t, eNB_flag_t, module_id_t,module_id_t)
* \brief Runs PDCP entity to let it handle incoming/outgoing SDUs
* \param[in] frame Frame number
* \param[in] eNB_flag Indicates if this PDCP entity belongs to an eNB or to a UE
* \param[in]  enb_mod_idP        Virtualized enb module identifier, Not used if eNB_flagP = 0.
* \param[in]  ue_mod_idP         Virtualized ue module identifier.
* \return none
* \note None
* @ingroup _pdcp
*/
public_pdcp(void pdcp_run            (
        const frame_t frameP,
        const eNB_flag_t eNB_flagP,
        const module_id_t ue_mod_idP,
        const module_id_t enb_mod_idP);)
public_pdcp(int pdcp_module_init     (void);)
public_pdcp(void pdcp_module_cleanup (void);)
public_pdcp(void pdcp_layer_init     (void);)
public_pdcp(void pdcp_layer_cleanup  (void);)
public_pdcp(int pdcp_netlink_init    (void);)

#define PDCP2NAS_FIFO 21
#define NAS2PDCP_FIFO 22

protected_pdcp_fifo(int pdcp_fifo_flush_sdus                      (
        const frame_t frameP,
        const eNB_flag_t eNB_flagP,
        const module_id_t enb_idP,
        const module_id_t ue_mod_idP);)
protected_pdcp_fifo(int pdcp_fifo_read_input_sdus_remaining_bytes (
        const frame_t frameP,
        const eNB_flag_t eNB_flagP);)
protected_pdcp_fifo(int pdcp_fifo_read_input_sdus                 (
        const frame_t frameP,
        const eNB_flag_t eNB_flagP,
        const module_id_t ue_mod_idP,
        const module_id_t enb_mod_idP);)
protected_pdcp_fifo(void pdcp_fifo_read_input_sdus_from_otg       (
        const frame_t frameP,
        const eNB_flag_t eNB_flagP,
        const module_id_t ue_mod_idP,
        const module_id_t enb_mod_idP);)

//-----------------------------------------------------------------------------

/*
 * Following two types are utilized between NAS driver and PDCP
 */


typedef struct pdcp_data_req_header_s {
  rb_id_t             rb_id;
  sdu_size_t          data_size;
  signed int          inst;
  ip_traffic_type_t   traffic_type;
} pdcp_data_req_header_t;

typedef struct pdcp_data_ind_header_s {
  rb_id_t             rb_id;
  sdu_size_t          data_size;
  signed int          inst;
  ip_traffic_type_t   dummy_traffic_type;
} pdcp_data_ind_header_t;

struct pdcp_netlink_element_s {
    pdcp_data_req_header_t pdcp_read_header;

    /* Data part of the message */
    uint8_t *data;
};

#if 0
/*
 * Missing PDU information struct, a copy of this will be enqueued
 * into pdcp.missing_pdus for every missing PDU
 */
typedef struct pdcp_missing_pdu_info_t {
  pdcp_sn_t sequence_number;
} pdcp_missing_pdu_info_t;
#endif

/*
 * PDCP limit values
 */
#define PDCP_MAX_SDU_SIZE 8188 // octets, see 4.3.1 Services provided to upper layers
#define PDCP_MAX_SN_5BIT  31   // 2^5-1
#define PDCP_MAX_SN_7BIT  127  // 2^7-1
#define PDCP_MAX_SN_12BIT 4095 // 2^12-1

/*
 * Reordering_Window: half of the PDCP SN space
 */ 
#define REORDERING_WINDOW_SN_5BIT 16
#define REORDERING_WINDOW_SN_7BIT 64
#define REORDERING_WINDOW_SN_12BIT 2048

/* 
 * SN size 
 */
#define PDCP_SN_5BIT  5
#define PDCP_SN_7BIT  7
#define PDCP_SN_12BIT 12
 

protected_pdcp(signed int             pdcp_2_nas_irq;)
public_pdcp(pdcp_stats_t              UE_pdcp_stats[NUMBER_OF_UE_MAX];)
public_pdcp(pdcp_stats_t              eNB_pdcp_stats[NUMBER_OF_eNB_MAX];)
protected_pdcp(pdcp_t                 pdcp_array_srb_ue[NUMBER_OF_UE_MAX][2];)
protected_pdcp(pdcp_t                 pdcp_array_drb_ue[NUMBER_OF_UE_MAX][maxDRB];)
public_pdcp(pdcp_t                 pdcp_array_srb_eNB[NUMBER_OF_eNB_MAX][NUMBER_OF_UE_MAX][2];)
protected_pdcp(pdcp_t                 pdcp_array_drb_eNB[NUMBER_OF_eNB_MAX][NUMBER_OF_UE_MAX][maxDRB];)
#if defined(Rel10)
public_pdcp(pdcp_mbms_t               pdcp_mbms_array_ue[NUMBER_OF_UE_MAX][maxServiceCount][maxSessionPerPMCH];)   // some constants from openair2/RRC/LITE/MESSAGES/asn1_constants.h
public_pdcp(pdcp_mbms_t               pdcp_mbms_array_eNB[NUMBER_OF_eNB_MAX][maxServiceCount][maxSessionPerPMCH];) // some constants from openair2/RRC/LITE/MESSAGES/asn1_constants.h
#endif
protected_pdcp(sdu_size_t             pdcp_output_sdu_bytes_to_write;)
protected_pdcp(sdu_size_t             pdcp_output_header_bytes_to_write;)
protected_pdcp(list_t                 pdcp_sdu_list;)
protected_pdcp(int                    pdcp_sent_a_sdu;)
protected_pdcp(pdcp_data_req_header_t pdcp_input_header;)
protected_pdcp(unsigned char          pdcp_input_sdu_buffer[MAX_IP_PACKET_SIZE];)
protected_pdcp(sdu_size_t             pdcp_input_index_header;)
protected_pdcp(sdu_size_t             pdcp_input_sdu_size_read;)
protected_pdcp(sdu_size_t             pdcp_input_sdu_remaining_size_to_read;)

#endif
