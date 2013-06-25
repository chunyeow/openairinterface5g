/*******************************************************************************

  Eurecom OpenAirInterface 2
  Copyright(c) 1999 - 2010 Eurecom

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
/***************************************************************************
                          graal_sap.h  -  description
                             -------------------
    copyright            : (C) 2002 by Eurecom
    email                : michelle.wetterwald@eurecom.fr
                           yan.moret@eurecom.fr
 ***************************************************************************

 ***************************************************************************/

#ifndef _NAS_SAP_H
#define _NAS_SAP_H


// RT-FIFO identifiers ** must be identical to Access Stratum as_sap.h and rrc_sap.h

#define RRC_DEVICE_GC          RRC_SAPI_GCSAP
#define RRC_DEVICE_NT          RRC_SAPI_NTSAP
#define RRC_DEVICE_DC_INPUT0   RRC_SAPI_DCSAP_IN
#define RRC_DEVICE_DC_OUTPUT0  RRC_SAPI_DCSAP_OUT


//FIFO indexes in control blocks

#define NAS_DC_INPUT_SAPI	  0
#define NAS_DC_OUTPUT_SAPI	1
#define NAS_SAPI_CX_MAX	    2

#define NAS_GC_SAPI 		    0
#define NAS_NT_SAPI 		    1

#define NAS_RAB_INPUT_SAPI	    2
#define NAS_RAB_OUTPUT_SAPI	    3


#define NAS_SAPI_MAX		   4




#endif



