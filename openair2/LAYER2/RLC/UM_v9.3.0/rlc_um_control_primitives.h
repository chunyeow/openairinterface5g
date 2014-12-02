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
/*! \file rlc_um_control_primitives.h
* \brief This file defines the prototypes of the functions dealing with the control primitives and initialization.
* \author GAUTHIER Lionel
* \date 2010-2011
* \version
* \note
* \bug
* \warning
*/
/** @defgroup _rlc_um_init_impl_ RLC UM Init Reference Implementation
* @ingroup _rlc_um_impl_
* @{
*/
#    ifndef __RLC_UM_CONTROL_PRIMITIVES_H__
#        define __RLC_UM_CONTROL_PRIMITIVES_H__
//-----------------------------------------------------------------------------
#        include "rlc_um_entity.h"
#        include "mem_block.h"
#        include "rlc_um_structs.h"
#        include "rlc_um_constants.h"
#        include "platform_types.h"
#        include "DL-UM-RLC.h"
#        include "UL-UM-RLC.h"
//-----------------------------------------------------------------------------
#        ifdef RLC_UM_CONTROL_PRIMITIVES_C
#            define private_rlc_um_control_primitives(x)    x
#            define protected_rlc_um_control_primitives(x)  x
#            define public_rlc_um_control_primitives(x)     x
#        else
#            ifdef RLC_UM_MODULE
#                define private_rlc_um_control_primitives(x)
#                define protected_rlc_um_control_primitives(x)  extern x
#                define public_rlc_um_control_primitives(x)     extern x
#            else
#                define private_rlc_um_control_primitives(x)
#                define protected_rlc_um_control_primitives(x)
#                define public_rlc_um_control_primitives(x)     extern x
#            endif
#        endif

typedef volatile struct {
    uint32_t             is_uplink_downlink;
    uint32_t             timer_reordering;
    uint32_t             sn_field_length; // 5 or 10
    uint32_t             is_mXch; // boolean, true if configured for MTCH or MCCH
} rlc_um_info_t;


/*! \fn void config_req_rlc_um (
 * const module_id_t enb_module_idP,
 * const module_id_t ue_module_idP,
 * const frame_t frameP,
 * const eNB_flag_t eNB_flagP,
 * const srb_flag_t srb_flagP,
 * const rlc_um_info_t * const config_umP,
 * const rb_id_t rb_idP)
* \brief    Allocate memory for RLC UM instance, reset protocol variables, and set protocol parameters. After this configuration the RLC UM protocol instance will be in RLC_DATA_TRANSFER_READY_STATE state.
* \param[in]  frameP                     Frame index.
* \param[in]  eNB_flag                  Flag to indicate eNB (1) or UE (0)
* \param[in]  srb_flagP                 Flag to indicate signalling radio bearer (1) or data radio bearer (0).
* \param[in]  enb_module_idP            eNB Virtualized module identifier.
* \param[in]  ue_module_idP             UE Virtualized module identifier.
* \param[in]  config_umP                Configuration parameters for RLC UM instance.
* \param[in]  rb_idP                    Radio bearer identifier.
*/
public_rlc_um_control_primitives(   void config_req_rlc_um (
    const module_id_t enb_module_idP,
    const module_id_t ue_module_idP,
    const frame_t frameP,
    const eNB_flag_t eNB_flagP,
    const srb_flag_t srb_flagP,
    const rlc_um_info_t * const config_umP,
    const rb_id_t rb_idP);)

/*! \fn void config_req_rlc_um_asn1 (
 *     const module_id_t enb_module_idP,
 *     const module_id_t ue_module_idP,
 *     const frame_t frameP,
 *     const eNB_flag_t eNB_flagP,
 *     const srb_flag_t srb_flagP,
 *     const MBMS_flag_t mbms_flagP,
 *     const mbms_session_id_t  mbms_session_idP,
 *     const mbms_service_id_t  mbms_service_idP,
 *     const UL_UM_RLC_t* const ul_rlcP,
 *     const DL_UM_RLC_t* const dl_rlcP,
 *     const rb_id_t rb_idP)
* \brief    Allocate memory for RLC UM instance, reset protocol variables, and set protocol parameters. After this configuration the RLC UM protocol instance will be in RLC_DATA_TRANSFER_READY_STATE state.
* \param[in]  frameP                    Frame index.
* \param[in]  eNB_flagP                 Flag to indicate eNB (1) or UE (0)
* \param[in]  srb_flagP                 Flag to indicate signalling radio bearer (1) or data radio bearer (0).
* \param[in]  mbms_flagP                Flag to indicate if this RLC is configured for MBMS.
* \param[in]  enb_module_idP            eNB Virtualized module identifier.
* \param[in]  ue_module_idP             UE Virtualized module identifier.
* \param[in]  mbms_session_idP          MBMS session id.
* \param[in]  mbms_service_idP          MBMS service id.
* \param[in]  ul_rlcP                   Configuration parameters for RLC UM UL instance.
* \param[in]  dl_rlcP                   Configuration parameters for RLC UM DL instance.
* \param[in]  rb_idP                    Radio bearer identifier.
*/
public_rlc_um_control_primitives(   void config_req_rlc_um_asn1 (
    const module_id_t        enb_module_idP,
    const module_id_t        ue_module_idP,
    const frame_t            frameP,
    const eNB_flag_t         eNB_flagP,
    const srb_flag_t         srb_flagP,
    const MBMS_flag_t        mbms_flagP,
    const mbms_session_id_t  mbms_session_idP,
    const mbms_service_id_t  mbms_service_idP,
    const UL_UM_RLC_t       * const ul_rlcP,
    const DL_UM_RLC_t       * const dl_rlcP,
    const rb_id_t            rb_idP);)

/*! \fn void rlc_um_init (rlc_um_entity_t *rlcP)
* \brief    Initialize a RLC UM protocol instance, initialize all variables, lists, allocate buffers for making this instance ready to be configured with protocol configuration parameters. After this initialization the RLC UM protocol instance will be in RLC_NULL_STATE state.
* \param[in]  rlcP                      RLC UM protocol instance pointer.
*/
protected_rlc_um_control_primitives(void rlc_um_init (rlc_um_entity_t * const rlcP);)

/*! \fn void rlc_um_reset_state_variables (rlc_um_entity_t *rlcP)
* \brief    Reset protocol variables and state variables to initial values.
* \param[in]  rlcP                      RLC UM protocol instance pointer.
*/
protected_rlc_um_control_primitives(void rlc_um_reset_state_variables (rlc_um_entity_t * const rlcP);)

/*! \fn void rlc_um_cleanup(rlc_um_entity_t *rlcP)
* \brief    Free all allocated memory (lists and buffers) previously allocated by this RLC UM instance.
* \param[in]  rlcP                      RLC UM protocol instance pointer.
*/
public_rlc_um_control_primitives(   void rlc_um_cleanup(rlc_um_entity_t * const rlcP);)

/*! \fn void rlc_um_configure(
 *     rlc_um_entity_t * const rlcP,
 *     const frame_t frameP,
 *     const uint32_t timer_reorderingP,
 *     const uint32_t rx_sn_field_lengthP,
 *     const uint32_t tx_sn_field_lengthP,
 *     const uint32_t is_mXchP)
* \brief    Configure RLC UM protocol parameters.
* \param[in]  rlcP                      RLC UM protocol instance pointer.
* \param[in]  frameP                     Frame index.
* \param[in]  timer_reorderingP         t-Reordering timer initialization value, units in frameP.
* \param[in]  rx_sn_field_lengthP       Length of the sequence number, 5 or 10 bits in reception.
* \param[in]  tx_sn_field_lengthP       Length of the sequence number, 5 or 10 bits in transmission.
* \param[in]  is_mXchP                  Is the radio bearer for MCCH, MTCH.
*/
protected_rlc_um_control_primitives(void rlc_um_configure(
    rlc_um_entity_t *const  rlcP,
    const frame_t frameP,
    const uint32_t timer_reorderingP,
    const uint32_t rx_sn_field_lengthP,
    const uint32_t tx_sn_field_lengthP,
    const uint32_t is_mXchP);)

/*! \fn void rlc_um_set_debug_infos(
 *     rlc_um_entity_t * const rlcP,
 *     const module_id_t enb_module_idP,
 *     const module_id_t ue_module_idP,
 *     const frame_t     frameP,
 *     const eNB_flag_t  eNB_flagP,
 *     const srb_flag_t  srb_flagP,
 *     const rb_id_t     rb_idP)
* \brief    Set debug informations for a RLC UM protocol instance, these informations are only for trace purpose.
* \param[in]  rlcP                      RLC UM protocol instance pointer.
* \param[in]  frameP                     Frame index.
* \param[in]  eNB_flag                  Flag to indicate eNB (1) or UE (0)
* \param[in]  enb_module_idP            eNB Virtualized module identifier.
* \param[in]  ue_module_idP             UE Virtualized module identifier.
* \param[in]  rb_idP                    Radio bearer identifier.
* \param[in]  srb_flagP                 Flag to indicate signalling radio bearer (1) or data radio bearer (0).
*/
protected_rlc_um_control_primitives(void rlc_um_set_debug_infos(
    rlc_um_entity_t * const rlcP,
    const module_id_t enb_module_idP,
    const module_id_t ue_module_idP,
    const frame_t frameP,
    const eNB_flag_t eNB_flagP,
    const srb_flag_t srb_flagP,
    const rb_id_t rb_idP);)
/** @} */
#    endif
