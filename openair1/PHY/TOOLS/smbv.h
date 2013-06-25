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
  Forums       : http://forums.eurecom.fr/openairinterface
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/

/*
  This library offers some functions to remotely program the R&S SMBV100A.
*/


#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <netinet/in.h>
#include <netdb.h>
#ifndef CONFIG_SMBV
#include "../impl_defs_lte.h"
#include "../impl_defs_top.h"
#include "../defs.h"
#include "../LTE_TRANSPORT/defs.h"
#else
#define msg printf
#endif

#define DEFAULT_SMBV_IP "192.168.12.201"
#define DEFAULT_SMBV_FNAME "smbv_config_file.smbv"
#define MYPORT "5025" 
#define BUFFER_LENGTH 256
#define MAX_INST_LENGTH 256
#define MAX_CONFIG_FRAMES 4

unsigned short slen; // sequence length in frames, max is 4
/*
#ifdef EXMIMO
const unsigned short config_frames[MAX_CONFIG_FRAMES];
#else
extern const unsigned short config_frames[MAX_CONFIG_FRAMES];
#endif
*/

// function sends the config file "fname" to SMBV through socket sockfd
int smbv_configure_from_file(int sockfd, const char* fname);

// Initiates the connection to SMBV and sends all commands in config file "fname"
int smbv_send_config (const char* fname, char* smbv_ip);

#ifndef CONFIG_SMBV
// writes basic init commands to config file
int smbv_init_config(const char* fname, u16 sequence_length);

// writes config from frame_parms to config file
int smbv_write_config_from_frame_parms(const char* fname, LTE_DL_FRAME_PARMS *frame_parms);

// This function creates a datalist for an allocation containing the data in buffer
// len is in bytes
int smbv_configure_datalist_for_alloc(const char* fname, u8 alloc, u8 subframe, u8 *buffer, u32 len);

// checks if frame is part of the config_frames[]
int smbv_is_config_frame(u32 frame);

// This function creates a datalist for user containing the data in buffer
// len is in bytes
int smbv_configure_datalist_for_user(const char* fname, u8 user, u8 *buffer, u32 len);

// configures one of maximal 4 users
int smbv_configure_user(const char* fname, u8 user, u8 transmission_mode, u16 rnti);

// Configures the PDCCH 
int smbv_configure_pdcch(const char* fname,u8 subframe,u8 num_pdcch_symbols,u8 num_dci);

// Configures the common DCIs SI, RA and PA
// type is either "SI", "RA" or "PA"
// item is the row in the DCI table
int smbv_configure_common_dci(const char* fname, u8 subframe, const char* type, DCI_ALLOC_t *dci_alloc, u8 item);

// Configures UE-spec DCI for user 1,2,3,4
// item is the row in the DCI table
int smbv_configure_ue_spec_dci(const char* fname, u8 subframe, u8 user, DCI_ALLOC_t *dci_alloc, u8 item);
#endif
