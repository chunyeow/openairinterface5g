/*******************************************************************************
 *
 * Eurecom OpenAirInterface 3
 * Copyright(c) 2012 Eurecom
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * The full GNU General Public License is included in this distribution in
 * the file called "COPYING".
 *
 * Contact Information
 * Openair Admin: openair_admin@eurecom.fr
 * Openair Tech : openair_tech@eurecom.fr
 * Forums       : http://forums.eurecom.fsr/openairinterface
 * Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France
 *
 *******************************************************************************/
/*! \file mRALlte_subscribe.h
 * \brief
 * \author BRIZZOLA Davide, GAUTHIER Lionel, MAUREL Frederic, WETTERWALD Michelle
 * \date 2012
 * \version
 * \note
 * \bug
 * \warning
 */

#ifndef __MRALLTE_SUBSCRIBE_H__
#    define __MRALLTE_SUBSCRIBE_H__
//-----------------------------------------------------------------------------
#        ifdef MRALLTE_SUBSCRIBE_C
#            define private_mRALlte_subscribe(x)    x
#            define protected_mRALlte_subscribe(x)  x
#            define public_mRALlte_subscribe(x)     x
#        else
#            ifdef MRAL_MODULE
#                define private_mRALlte_subscribe(x)
#                define protected_mRALlte_subscribe(x)  extern x
#                define public_mRALlte_subscribe(x)     extern x
#            else
#                define private_mRALlte_subscribe(x)
#                define protected_mRALlte_subscribe(x)
#                define public_mRALlte_subscribe(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <ctype.h>
//-----------------------------------------------------------------------------
#include "MIH_C.h"
//-----------------------------------------------------------------------------
#include "mRALlte_constants.h"
#include "mRALlte_mih_msg.h"
//-----------------------------------------------------------------------------
protected_mRALlte_subscribe(void mRALlte_subscribe_request  (MIH_C_Message_Link_Event_Subscribe_request_t*   messageP);)
protected_mRALlte_subscribe(void mRALlte_unsubscribe_request(MIH_C_Message_Link_Event_Unsubscribe_request_t* messageP);)

#endif
