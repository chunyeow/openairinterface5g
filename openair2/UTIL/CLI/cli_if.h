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
/*! \file cli_if.h
* \brief cli interface
* \author Navid Nikaein
* \date 2011
* \version 0.1
* \warning This component can be runned only in the user-space
* @ingroup util
*/
#ifndef __CLI_IF_H__
#    define __CLI_IF_H__


/*--- INCLUDES ---------------------------------------------------------------*/
#    include "cli.h"
/*----------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif

#    ifdef COMPONENT_CLI
#        ifdef COMPONENT_CLI_IF
#            define private_cli_if(x) x
#            define friend_cli_if(x) x
#            define public_cli_if(x) x
#        else
#            define private_cli_if(x)
#            define friend_cli_if(x) extern x
#            define public_cli_if(x) extern x
#        endif
#    else
#        define private_cli_if(x)
#        define friend_cli_if(x)
#        define public_cli_if(x) extern x
#    endif

/** @defgroup _cli_if Interfaces of CLI
 * @{*/ 


  public_cli_if( void cli_init (void); )
  public_cli_if( int cli_server_init(cli_handler_t handler); )
  public_cli_if(void cli_server_cleanup(void);)
  public_cli_if(void cli_server_recv(const void * data, socklen_t len);)
/* @}*/ 

#ifdef __cplusplus
}
#endif

#endif

