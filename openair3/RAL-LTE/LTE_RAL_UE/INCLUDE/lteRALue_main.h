/***************************************************************************
                         lteRALue_main.h  -  description
 ***************************************************************************
  Eurecom OpenAirInterface 3
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
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 450 route des Chappes, 06410 Biot Sophia Antipolis, France
*******************************************************************************/
/*! \file lteRALue_main.h
 * \brief This file defines the prototypes of the functions for lteRALue_main.c
 * \author WETTERWALD Michelle, GAUTHIER Lionel, MAUREL Frederic
 * \date 2013
 * \company EURECOM
 * \email: michelle.wetterwald@eurecom.fr, lionel.gauthier@eurecom.fr, frederic.maurel@eurecom.fr
 */
/*******************************************************************************/
#ifndef __MRALLTE_MAIN_H__
#    define __MRALLTE_MAIN_H__
//-----------------------------------------------------------------------------
#        ifdef MRALLTE_MAIN_C
#            define private_mRALlte_main(x)    x
#            define protected_mRALlte_main(x)  x
#            define public_mRALlte_main(x)     x
#        else
#            ifdef MRAL_MODULE
#                define private_mRALlte_main(x)
#                define protected_mRALlte_main(x)  extern x
#                define public_mRALlte_main(x)     extern x
#            else
#                define private_mRALlte_main(x)
#                define protected_mRALlte_main(x)
#                define public_mRALlte_main(x)     extern x
#            endif
#        endif
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/time.h>
#include <ctype.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <libgen.h>
//-----------------------------------------------------------------------------
#endif






