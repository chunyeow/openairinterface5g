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

/** @defgroup _intertask_interface_impl_ Intertask Interface Mechanisms
 * Implementation
 * @ingroup _ref_implementation_
 * @{
 */

/********************************************************************************
 *
 * !!! This header should only be included by the file that initialize
 *     the intertask interface module for the process !!!
 *
 * Other files should include "intertask_interface.h"
 *
 *******************************************************************************/

#ifndef INTERTASK_INTERFACE_INIT_H_
#define INTERTASK_INTERFACE_INIT_H_

#include "intertask_interface.h"

#ifndef CHECK_PROTOTYPE_ONLY

const char * const messages_definition_xml = {
#include "../messages_xml.h"
};

/* Map thread id to printable name. */
const char * const threads_name[] = {
    "unused",
#define TASK_DEF(tHREADiD, pRIO)            #tHREADiD,
#define SUB_TASK_DEF(tHREADiD, sUBtASKiD)
#include <tasks_def.h>
#undef SUB_TASK_DEF
#undef TASK_DEF
};

/* Map message id to message information */
const message_info_t messages_info[] = {
#define MESSAGE_DEF(iD, pRIO, sTRUCT, fIELDnAME) { iD, pRIO, sizeof(sTRUCT), #iD },
#include <messages_def.h>
#undef MESSAGE_DEF
};

#endif

/** \brief Init function for the intertask interface. Init queues, Mutexes and Cond vars.
 * \param thread_max Maximum number of threads
 * \param messages_id_max Maximum message id
 * \param threads_name Pointer on the threads name information as created by this include file
 * \param messages_info Pointer on messages information as created by this include file
 **/
int intertask_interface_init(thread_id_t thread_max, MessagesIds messages_id_max,
                             const char * const *threads_name, const message_info_t *messages_info,
                             const char * const messages_definition_xml);

#endif /* INTERTASK_INTERFACE_INIT_H_ */
/* @} */
