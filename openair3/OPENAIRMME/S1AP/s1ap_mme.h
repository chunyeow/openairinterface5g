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

#if !defined(MME_CLIENT_TEST)
# include "intertask_interface.h"
#endif

#include "mme_config.h"

#ifndef S1AP_MME_H_
#define S1AP_MME_H_

#define MAX_NUMBER_OF_ENB       2
#define MAX_NUMBER_OF_UE_BY_ENB 64
#define MAX_NUMBER_OF_UE        (MAX_NUMBER_OF_ENB * MAX_NUMBER_OF_UE_BY_ENB)

// Forward declarations
struct eNB_description_s;

enum s1_eNB_state_s {
    S1AP_RESETING,      ///< After a reset request (eNB or MME initiated)
    S1AP_READY          ///< MME and eNB are S1 associated, UE contexts can be added
};

enum s1_ue_state_s {
    S1AP_UE_WAITING_CSR,    ///< Waiting for Initial Context Setup Response
    S1AP_UE_HANDOVER,       ///< Handover procedure triggered
    S1AP_UE_CONNECTED,      ///< UE context ready
};

/** Main structure representing UE association over s1ap
 *  Generated every time a new InitialUEMessage is received
 **/
typedef struct ue_description_s {
    struct ue_description_s  *next_ue;       ///< Next UE in the list
    struct ue_description_s  *previous_ue;   ///< Previous UE in the list

    struct eNB_description_s *eNB;           ///< Which eNB this UE is attached to

    enum s1_ue_state_s        s1_ue_state;       ///< S1AP UE state

    uint32_t                  eNB_ue_s1ap_id;    ///< Unique UE id over eNB (24 bits wide)
    uint32_t                  mme_ue_s1ap_id;    ///< Unique UE id over MME (32 bits wide)

    /** SCTP stream on which S1 message will be sent/received.
     *  During an UE S1 connection, a pair of streams is
     *  allocated and is used during all the connection.
     *  Stream 0 is reserved for non UE signalling.
     *  @name sctp stream identifier
     **/
    /*@{*/
    uint8_t                   sctp_stream_recv; ///< eNB -> MME stream
    uint8_t                   sctp_stream_send; ///< MME -> eNB stream
    /*@}*/

    uint32_t                  teid; ///< S11 TEID

//     uint8_t                   eRAB_id;
} ue_description_t;

/* Main structure representing eNB association over s1ap
 * Generated (or updated) every time a new S1SetupRequest is received.
 */
typedef struct eNB_description_s {
    struct eNB_description_s *next_eNB;         ///< Next eNB in the list of eNB
    struct eNB_description_s *previous_eNB;     ///< Previous eNB in the list of eNB

    enum s1_eNB_state_s       s1_state;         ///< State of the eNB S1AP association over MME

    /** eNB related parameters **/
    /*@{*/
    char                      eNB_name[150];    ///< Printable eNB Name
    uint32_t                  eNB_id;           ///< Unique eNB ID
    uint8_t                   default_paging_drx; ///< Default paging DRX interval for eNB
    /*@}*/

    /** UE list for this eNB **/
    /*@{*/
    uint8_t                   nb_ue_associated; ///< Number of NAS associated UE on this eNB
    ue_description_t         *ue_list_head;     ///< List head of NAS associated UE on this eNB
    ue_description_t         *ue_list_tail;     ///< List tail of NAS associated UE on this eNB
    /*@}*/

    /** SCTP stuff **/
    /*@{*/
    uint32_t                  sctp_assoc_id;    ///< SCTP association id on this machine
    uint8_t                   next_sctp_stream; ///< Next SCTP stream
    uint16_t                  instreams;        ///< Number of streams avalaible on eNB -> MME
    uint16_t                  outstreams;       ///< Number of streams avalaible on MME -> eNB
    /*@}*/
} eNB_description_t;

extern uint8_t     nb_eNB_associated;
extern MessageDef *receivedMessage;

/** \brief S1AP layer top init
 * @returns -1 in case of failure
 **/
int s1ap_mme_init(const mme_config_t *mme_config);

/** \brief Look for given eNB id in the list
 * \param eNB_id The unique eNB id to search in list
 * @returns NULL if no eNB matchs the eNB id, or reference to the eNB element in list if matches
 **/
eNB_description_t* s1ap_is_eNB_id_in_list(uint32_t eNB_id);

/** \brief Look for given eNB SCTP assoc id in the list
 * \param eNB_id The unique sctp assoc id to search in list
 * @returns NULL if no eNB matchs the sctp assoc id, or reference to the eNB element in list if matches
 **/
eNB_description_t* s1ap_is_eNB_assoc_id_in_list(uint32_t sctp_assoc_id);

/** \brief Look for given ue eNB id in the list
 * \param eNB_id The unique ue_eNB_id to search in list
 * @returns NULL if no UE matchs the ue_eNB_id, or reference to the ue element in list if matches
 **/
ue_description_t* s1ap_is_ue_eNB_id_in_list(eNB_description_t *eNB_ref, uint32_t eNB_ue_s1ap_id);

/** \brief Look for given ue mme id in the list
 * \param eNB_id The unique ue_mme_id to search in list
 * @returns NULL if no UE matchs the ue_mme_id, or reference to the ue element in list if matches
 **/
ue_description_t* s1ap_is_ue_mme_id_in_list(uint32_t ue_mme_id);
ue_description_t* s1ap_is_teid_in_list(uint32_t teid);

/** \brief Allocate and add to the list a new eNB descriptor
 * @returns Reference to the new eNB element in list
 **/
eNB_description_t* s1ap_new_eNB(void);

/** \brief Allocate and add to the right eNB list a new UE descriptor
 * \param sctp_assoc_id association ID over SCTP
 * @returns Reference to the new UE element in list
 **/
ue_description_t* s1ap_new_ue(uint32_t sctp_assoc_id);

/** \brief Dump the eNB list
 * Calls dump_eNB for each eNB in list
 **/
void s1ap_dump_eNB_list(void);

/** \brief Dump eNB related information.
 * Calls dump_ue for each UE in list
 * \param eNB_ref eNB structure reference to dump
 **/
void s1ap_dump_eNB(eNB_description_t *eNB_ref);

/** \brief Dump UE related information.
 * \param ue_ref ue structure reference to dump
 **/
void s1ap_dump_ue(ue_description_t *ue_ref);

/** \brief Remove target UE from the list
 * \param ue_ref UE structure reference to remove
 **/
void s1ap_remove_ue(ue_description_t *ue_ref);

/** \brief Remove target eNB from the list and remove any UE associated
 * \param eNB_ref eNB structure reference to remove
 **/
void s1ap_remove_eNB(eNB_description_t *eNB_ref);

#endif /* S1AP_MME_H_ */
