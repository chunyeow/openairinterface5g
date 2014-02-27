/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2011 Eurecom

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
  time_stats_t encrption;
  time_stats_t decrption;
  time_stats_t pdcp_ip;
  time_stats_t ip_pdcp;

}pdcp_stats_t; // common to eNB and UE


typedef struct pdcp_t {
  boolean_t instanciated_instance;
  u16       header_compression_profile;

  /* SR: added this flag to distinguish UE/eNB instance as pdcp_run for virtual
   * mode can receive data on NETLINK for eNB while eNB_flag = 0 and for UE when eNB_flag = 1
   */
  boolean_t is_ue;

  /* Configured security algorithms */
  u8 cipheringAlgorithm;
  u8 integrityProtAlgorithm;

  /* User-Plane encryption key
   * Control-Plane RRC encryption key
   * Control-Plane RRC integrity key
   * These keys are configured by RRC layer
   */
  u8 *kUPenc;
  u8 *kRRCint;
  u8 *kRRCenc;

  u8 security_activated;

  rlc_mode_t rlc_mode;
  u8 status_report;
  u8 seq_num_size;

  logical_chan_id_t lcid;
  /*
   * Sequence number state variables
   *
   * TX and RX window
   */
  pdcp_sn_t next_pdcp_tx_sn;
  pdcp_sn_t next_pdcp_rx_sn;
  /*
   * TX and RX Hyper Frame Numbers
   */
  pdcp_hfn_t tx_hfn;
  pdcp_hfn_t rx_hfn;
  /*
   * SN of the last PDCP SDU delivered to upper layers
   */
  pdcp_sn_t  last_submitted_pdcp_rx_sn;

  /*
   * Following array is used as a bitmap holding missing sequence
   * numbers to generate a PDCP Control PDU for PDCP status
   * report (see 6.2.6)
   */
  u8 missing_pdu_bitmap[512];
  /*
   * This is intentionally signed since we need a 'NULL' value
   * which is not also a valid sequence number
   */
  short int first_missing_pdu;
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

/*! \fn boolean_t pdcp_data_req(module_id_t, u32_t, u8_t, rb_id_t, sdu_size_t, unsigned char*)
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
public_pdcp(boolean_t pdcp_data_req(module_id_t eNB_id, module_id_t UE_id, frame_t frame, eNB_flag_t eNB_flag, rb_id_t rb_id, mui_t muiP, u32 confirmP, \
    sdu_size_t sdu_buffer_size, unsigned char* sdu_buffer, u8 mode));

/*! \fn boolean_t pdcp_data_ind(module_id_t, module_id_t, frame_t, eNB_flag_t, MBMS_flag_t, rb_id_t, sdu_size_t, mem_block_t*, boolean_t)
* \brief This functions handles data transfer indications coming from RLC
* \param[in]  enb_mod_idP        Virtualized enb module identifier, Not used if eNB_flagP = 0.
* \param[in]  ue_mod_idP         Virtualized ue module identifier.
* \param[in] frame Frame number
* \param[in] Shows if relevant PDCP entity is part of an eNB or a UE
* \param[in] Tells if MBMS traffic
* \param[in] rab_id Radio Bearer ID
* \param[in] sdu_buffer_size Size of incoming SDU in bytes
* \param[in] sdu_buffer Buffer carrying SDU
* \param[in] is_data_plane flag to indicate whether the userplane data belong to the control plane or data plane
* \return TRUE on success, FALSE otherwise
* \note None
* @ingroup _pdcp
*/
public_pdcp(boolean_t pdcp_data_ind(module_id_t eNB_id, module_id_t UE_id, frame_t frame, eNB_flag_t eNB_flag, MBMS_flag_t MBMS_flagP, rb_id_t rb_id, sdu_size_t sdu_buffer_size,
                   mem_block_t* sdu_buffer, boolean_t is_data_plane));

/*! \fn void rrc_pdcp_config_req(module_id_t , module_id_t ,frame_t,eNB_flag_t,u32,rb_id_t,u8)
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
public_pdcp(void rrc_pdcp_config_req (module_id_t enb_idP,
                                      module_id_t ue_idP,
                                      frame_t     frameP,
                                      eNB_flag_t  eNB_flagP,
                                      u32         actionP,
                                      rb_id_t     rb_idP,
                                      u8          security_modeP);)

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
boolean_t rrc_pdcp_config_asn1_req (module_id_t          eNB_idP,
                               module_id_t          ue_idP,
                               frame_t              frameP,
                               eNB_flag_t           eNB_flagP,
                               SRB_ToAddModList_t  *srb2add_list,
                               DRB_ToAddModList_t  *drb2add_list,
                               DRB_ToReleaseList_t *drb2release_list,
                               u8                   security_modeP,
                               u8                  *kRRCenc,
                               u8                  *kRRCint,
                               u8                  *kUPenc
#ifdef Rel10
                              ,PMCH_InfoList_r9_t  *pmch_InfoList_r9
#endif
                               ));

/*! \fn boolean_t pdcp_config_req_asn1 (module_id_t module_id, frame_t frame, eNB_flag_t eNB_flag, u32  action, rb_id_t rb_id, u8 rb_sn, u8 rb_report, u16 header_compression_profile, u8 security_mode)
* \brief  Function for RRC to configure a Radio Bearer.
* \param[in]  pdcp_pP            Pointer on PDCP structure.
* \param[in]  enb_mod_idP        Virtualized enb module identifier, Not used if eNB_flagP = 0.
* \param[in]  ue_mod_idP         Virtualized ue module identifier.
* \param[in]  frame              Frame index.
* \param[in]  eNB_flag           Flag to indicate eNB (1) or UE (0)
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
public_pdcp(boolean_t pdcp_config_req_asn1 (pdcp_t      *pdcp_pP,
                                       module_id_t enb_idP,
                                       module_id_t ue_idP,
                                       frame_t     frameP,
                                       eNB_flag_t  eNB_flagP,
                                       rlc_mode_t  rlc_mode,
                                       u32         action,
                                       u16         lc_id,
                                       u16         mch_id,
                                       rb_id_t     rb_id,
                                       u8          rb_sn,
                                       u8          rb_report,
                                       u16         header_compression_profile,
                                       u8          security_mode,
                                       u8         *kRRCenc,
                                       u8         *kRRCint,
                                       u8         *kUPenc));
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
public_pdcp(void pdcp_run            (frame_t frameP, eNB_flag_t eNB_flagP, module_id_t ue_mod_idP, module_id_t enb_mod_idP);)
public_pdcp(int pdcp_module_init     (void);)
public_pdcp(void pdcp_module_cleanup (void);)
public_pdcp(void pdcp_layer_init     (void);)
public_pdcp(void pdcp_layer_cleanup  (void);)
public_pdcp(int pdcp_netlink_init    (void);)

#define PDCP2NAS_FIFO 21
#define NAS2PDCP_FIFO 22

protected_pdcp_fifo(int pdcp_fifo_flush_sdus                      (frame_t frameP, eNB_flag_t eNB_flagP, module_id_t enb_idP, module_id_t ue_mod_idP);)
protected_pdcp_fifo(int pdcp_fifo_read_input_sdus_remaining_bytes (frame_t frameP, eNB_flag_t eNB_flagP);)
protected_pdcp_fifo(int pdcp_fifo_read_input_sdus                 (frame_t frameP, eNB_flag_t eNB_flagP, module_id_t ue_mod_idP, module_id_t enb_mod_idP);)
protected_pdcp_fifo(void pdcp_fifo_read_input_sdus_from_otg       (frame_t frameP, eNB_flag_t eNB_flagP, module_id_t ue_mod_idP, module_id_t enb_mod_idP);)

//-----------------------------------------------------------------------------

/*
 * Following two types are utilized between NAS driver and PDCP
 */


typedef struct pdcp_data_req_header_s {
  rb_id_t             rb_id;
  sdu_size_t          data_size;
  signed int          inst;
  traffic_type_t      traffic_type;
} pdcp_data_req_header_t;

typedef struct pdcp_data_ind_header_s {
  rb_id_t             rb_id;
  sdu_size_t          data_size;
  signed int          inst;
  traffic_type_t      dummy_traffic_type;
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

protected_pdcp(signed int             pdcp_2_nas_irq;)
public_pdcp(pdcp_stats_t           UE_pdcp_stats[NUMBER_OF_UE_MAX];)
public_pdcp(pdcp_stats_t           eNB_pdcp_stats[NUMBER_OF_eNB_MAX];)
protected_pdcp(pdcp_t                 pdcp_array_ue[NUMBER_OF_UE_MAX][NB_RB_MAX];)
protected_pdcp(pdcp_t                 pdcp_array_eNB[NUMBER_OF_eNB_MAX][NUMBER_OF_UE_MAX][NB_RB_MAX];)
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
