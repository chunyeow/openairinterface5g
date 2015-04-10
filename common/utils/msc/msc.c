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
/*! \file msc.c
 * \brief Message chart generator logging utility (generated files to processed by a scrip to produce a mscgen input file for generating a sequence diagram document)
 * \author  Lionel GAUTHIER
 * \date 2015
 * \email: lionel.gauthier@eurecom.fr
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include "msc.h"
//-------------------------------
#define MSC_MAX_PROTO_NAME_LENGTH 16
//-------------------------------

FILE*    msc_fd[MAX_MSC_PROTOS]; // may need to split in more files to avoid overwrites (want to avoid mutexes)
char     msc_proto2str[MAX_MSC_PROTOS][MSC_MAX_PROTO_NAME_LENGTH];
uint64_t msc_event_counter = 0;

//------------------------------------------------------------------------------
int msc_init(msc_env_t envP)
//------------------------------------------------------------------------------
{
  int i;
  int rv;

  for (i = MIN_MSC_PROTOS; i < MAX_MSC_PROTOS; i++) {
	  msc_fd[i] = NULL;
      switch (i) {
        case MSC_NAS_UE:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "NAS_UE");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          if (envP == MSC_E_UTRAN) {
        	  msc_fd[i] = fopen("/tmp/openair.msc.nas_ue.log","w");
          }
          msc_log_declare_proto(i);
          break;
        case MSC_RRC_UE:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "RRC_UE");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          if (envP == MSC_E_UTRAN) {
        	  msc_fd[i] = fopen("/tmp/openair.msc.rrc_ue.log","w");
              msc_log_declare_proto(i);
          }
          break;
        case MSC_PDCP_UE:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "PDCP_UE");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          if (envP == MSC_E_UTRAN) {
        	  msc_fd[i] = fopen("/tmp/openair.msc.pdcp_ue.log","w");
        	  msc_log_declare_proto(i);
          }
          break;
        case MSC_RLC_UE:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "RLC_UE");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          if (envP == MSC_E_UTRAN) {
              msc_fd[i] = fopen("/tmp/openair.msc.rlc_ue.log","w");
              msc_log_declare_proto(i);
          }
          break;
        case MSC_MAC_UE:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "MAC_UE");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          if (envP == MSC_E_UTRAN) {
        	  msc_fd[i] = fopen("/tmp/openair.msc.mac_ue.log","w");
        	  msc_log_declare_proto(i);
          }
          break;
        case MSC_PHY_UE:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "PHY_UE");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          if (envP == MSC_E_UTRAN) {
        	  msc_fd[i] = fopen("/tmp/openair.msc.phy_ue.log","w");
        	  msc_log_declare_proto(i);
          }
          break;
        case MSC_PHY_ENB:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "PHY_ENB");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          if (envP == MSC_E_UTRAN) {
        	  msc_fd[i] = fopen("/tmp/openair.msc.phy_enb.log","w");
        	  msc_log_declare_proto(i);
          }
          break;
        case MSC_MAC_ENB:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "MAC_ENB");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          if (envP == MSC_E_UTRAN) {
        	  msc_fd[i] = fopen("/tmp/openair.msc.mac_enb.log","w");
        	  msc_log_declare_proto(i);
          }
          break;
        case MSC_RLC_ENB:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "RLC_ENB");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          msc_fd[i] = fopen("/tmp/openair.msc.rlc_enb.log","w");
          msc_log_declare_proto(i);
          break;
        case MSC_PDCP_ENB:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "PDCP_ENB");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          if (envP == MSC_E_UTRAN) {
        	  msc_fd[i] = fopen("/tmp/openair.msc.pdcp_enb.log","w");
        	  msc_log_declare_proto(i);
          }
          break;
        case MSC_RRC_ENB:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "RRC_ENB");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          if (envP == MSC_E_UTRAN) {
        	  msc_fd[i] = fopen("/tmp/openair.msc.rrc_enb.log","w");
        	  msc_log_declare_proto(i);
          }
          break;
        case MSC_S1AP_ENB:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "S1AP_ENB");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          msc_fd[i] = fopen("/tmp/openair.msc.s1ap_enb.log","w");
          msc_log_declare_proto(i);
          break;
        case MSC_GTPU_ENB:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "GTPU_ENB");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          msc_fd[i] = fopen("/tmp/openair.msc.gtpu_enb.log","w");
          msc_log_declare_proto(i);
          break;
        case MSC_GTPU_SGW:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "GTPU_SGW");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          msc_fd[i] = fopen("/tmp/openair.msc.gtpu_sgw.log","w");
          msc_log_declare_proto(i);
          break;
        case MSC_S1AP_MME:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "S1AP_MME");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          msc_fd[i] = fopen("/tmp/openair.msc.s1ap_mme.log","w");
          msc_log_declare_proto(i);
          break;
        case MSC_MMEAPP_MME:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "MME_APP");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          if (envP == MSC_EPC) {
        	  msc_fd[i] = fopen("/tmp/openair.msc.mme_app.log","w");
              msc_log_declare_proto(i);
          }
          break;
        case MSC_NAS_MME:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "NAS_MME");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          msc_fd[i] = fopen("/tmp/openair.msc.nas_mme.log","w");
          msc_log_declare_proto(i);
          break;
        case MSC_NAS_EMM_MME:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "NAS_EMM");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          if (envP == MSC_EPC) {
        	  msc_fd[i] = fopen("/tmp/openair.msc.nas_emm_mme.log","w");
              msc_log_declare_proto(i);
          }
          break;
        case MSC_NAS_ESM_MME:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "NAS_ESM");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          if (envP == MSC_EPC) {
        	  msc_fd[i] = fopen("/tmp/openair.msc.nas_esm_mme.log","w");
              msc_log_declare_proto(i);
          }
          break;
        case MSC_S6A_MME:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "S6A");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          if (envP == MSC_EPC) {
        	  msc_fd[i] = fopen("/tmp/openair.msc.s6a_mme.log","w");
              msc_log_declare_proto(i);
          }
          break;
        case MSC_HSS:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "NAS_ESM");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          if (envP == MSC_EPC) {
        	  msc_fd[i] = fopen("/tmp/openair.msc.hss.log","w");
        	  msc_log_declare_proto(i);
          }
          break;
        default:
          rv = snprintf(&msc_proto2str[i][0], MSC_MAX_PROTO_NAME_LENGTH, "UNKNOWN");
          if (rv >= MSC_MAX_PROTO_NAME_LENGTH) {msc_proto2str[i][MSC_MAX_PROTO_NAME_LENGTH-1] = 0;}
          msc_fd[i] = NULL;
      }
  }
  return 0;
}


//------------------------------------------------------------------------------
void msc_end(void)
//------------------------------------------------------------------------------
{
  int rv;
  int i;
  for (i = MIN_MSC_PROTOS; i < MAX_MSC_PROTOS; i++) {
	  if (msc_fd[i] != NULL) {
		  rv = fflush(msc_fd[i]);
		  if (rv != 0) {
			  fprintf(stderr, "Error while flushing stream of MSC log file: %s", strerror(errno));
		  }
		  rv = fclose(msc_fd[i]);
		  if (rv != 0) {
			  fprintf(stderr, "Error while closing MSC log file: %s", strerror(errno));
		  }
	  }
  }
}

//------------------------------------------------------------------------------
void msc_log_declare_proto(
    const msc_proto_t  protoP
  )
//------------------------------------------------------------------------------
{
  int rv;
  uint64_t local_msc_event_counter = msc_event_counter;
  msc_event_counter++;
  if ((protoP >= MIN_MSC_PROTOS) && (protoP < MAX_MSC_PROTOS)) {
      rv = fprintf(msc_fd[protoP], "%"PRIu64" [PROTO] %d %s\n", local_msc_event_counter, protoP, &msc_proto2str[protoP][0]);
      if (rv < 0) {
          fprintf(stderr, "Error while declaring new protocol in MSC log file: %s", strerror(errno));
      }
	  rv = fflush(msc_fd[protoP]);
	  if (rv != 0) {
		  fprintf(stderr, "Error while flushing stream of MSC log file: %s", strerror(errno));
	  }
  }
}
//------------------------------------------------------------------------------
void msc_log_event(
    const msc_proto_t  protoP,
    char *format, ...)
//------------------------------------------------------------------------------
{
  va_list    args;
  int        rv;
  uint64_t local_msc_event_counter = msc_event_counter;

  msc_event_counter++;

  if ((protoP < MIN_MSC_PROTOS) || (protoP >= MAX_MSC_PROTOS)) {
      return;
  }
  if (msc_fd != NULL) {
      rv = fprintf(msc_fd[protoP], "%"PRIu64" [EVENT] %d ", local_msc_event_counter, protoP);
      if (rv < 0) {
         fprintf(stderr, "Error while logging MSC event : %s", &msc_proto2str[protoP][0]);
      }
      va_start(args, format);
      rv = vfprintf(msc_fd[protoP], format, args);
      va_end(args);
      if (rv < 0) {
         fprintf(stderr, "Error while logging MSC event : %s", &msc_proto2str[protoP][0]);
      }
      rv = fprintf(msc_fd[protoP], "\n");
      if (rv < 0) {
         fprintf(stderr, "Error while logging MSC event : %s", &msc_proto2str[protoP][0]);
      }
#if 1
	  rv = fflush(msc_fd[protoP]);
	  if (rv != 0) {
		  fprintf(stderr, "Error while flushing stream of MSC log file: %s", strerror(errno));
	  }
#endif
  }
}
//------------------------------------------------------------------------------
void msc_log_rx_message(
    const msc_proto_t  receiverP,
    const msc_proto_t  senderP,
    const char*        bytesP,
    const unsigned int num_bytes,
    char *format, ...)
//------------------------------------------------------------------------------
{
  va_list    args;
  int        rv;
  uint64_t   mac = 0;
  uint64_t local_msc_event_counter = msc_event_counter;

  msc_event_counter++;

  if ((receiverP < MIN_MSC_PROTOS) || (receiverP >= MAX_MSC_PROTOS) ||
      (senderP < MIN_MSC_PROTOS)   || (senderP >= MAX_MSC_PROTOS)) {
      return;
  }
  if (msc_fd[receiverP] != NULL) {
      rv = fprintf(msc_fd[receiverP], "%"PRIu64" [MESSAGE] %d <- %d %"PRIu64" ",
                   local_msc_event_counter, receiverP, senderP, mac);
      if (rv < 0) {
         fprintf(stderr, "Error while logging MSC RX message : %s", &msc_proto2str[receiverP][0]);
      }
      va_start(args, format);
      rv = vfprintf(msc_fd[receiverP], format, args);
      va_end(args);
      if (rv < 0) {
         fprintf(stderr, "Error while logging MSC RX message : %s", &msc_proto2str[receiverP][0]);
      }
      rv = fprintf(msc_fd[receiverP], "\n");
      if (rv < 0) {
         fprintf(stderr, "Error while logging MSC RX message : %s", &msc_proto2str[receiverP][0]);
      }
      if ((msc_event_counter & 0x000000000000000F) == 0x000000000000000F) {
          fflush(msc_fd[receiverP]);
      }
  }
}
//------------------------------------------------------------------------------
void msc_log_rx_discarded_message(
    const msc_proto_t  receiverP,
    const msc_proto_t  senderP,
    const char*        bytesP,
    const unsigned int num_bytes,
    char *format, ...)
//------------------------------------------------------------------------------
{
  va_list    args;
  int        rv;
  uint64_t   mac = 0;
  uint64_t local_msc_event_counter = msc_event_counter;

  msc_event_counter++;

  if ((receiverP < MIN_MSC_PROTOS) || (receiverP >= MAX_MSC_PROTOS) ||
      (senderP < MIN_MSC_PROTOS)   || (senderP >= MAX_MSC_PROTOS)) {
      return;
  }
  if (msc_fd[receiverP] != NULL) {
      rv = fprintf(msc_fd[receiverP], "%"PRIu64" [MESSAGE] %d x- %d %"PRIu64" ",
                   local_msc_event_counter, receiverP, senderP, mac);
      if (rv < 0) {
         fprintf(stderr, "Error while logging MSC RX message : %s", &msc_proto2str[receiverP][0]);
      }
      va_start(args, format);
      rv = vfprintf(msc_fd[receiverP], format, args);
      va_end(args);
      if (rv < 0) {
         fprintf(stderr, "Error while logging MSC RX message : %s", &msc_proto2str[receiverP][0]);
      }
      rv = fprintf(msc_fd[receiverP], "\n");
      if (rv < 0) {
         fprintf(stderr, "Error while logging MSC RX message : %s", &msc_proto2str[receiverP][0]);
      }
      if ((msc_event_counter & 0x000000000000000F) == 0x000000000000000F) {
          fflush(msc_fd[receiverP]);
      }
  }
}
//------------------------------------------------------------------------------
void msc_log_tx_message(
    const msc_proto_t  senderP,
    const msc_proto_t  receiverP,
    const char*        bytesP,
    const unsigned int num_bytes,
    char *format, ...)
//------------------------------------------------------------------------------
{
  va_list    args;
  int        rv;
  uint64_t   mac = 0;
  uint64_t   local_msc_event_counter = msc_event_counter;

  msc_event_counter++;

  if ((receiverP < MIN_MSC_PROTOS) || (receiverP >= MAX_MSC_PROTOS) ||
      (senderP < MIN_MSC_PROTOS)   || (senderP >= MAX_MSC_PROTOS)) {
      return;
  }
  if (msc_fd[senderP] != NULL) {
      rv = fprintf(msc_fd[senderP], "%"PRIu64" [MESSAGE] %d -> %d %"PRIu64" ",
                   local_msc_event_counter, senderP, receiverP, mac);
      if (rv < 0) {
         fprintf(stderr, "Error while logging MSC TX message : %s", &msc_proto2str[senderP][0]);
      }
      va_start(args, format);
      rv = vfprintf(msc_fd[senderP], format, args);
      va_end(args);
      if (rv < 0) {
         fprintf(stderr, "Error while logging MSC TX message : %s", &msc_proto2str[senderP][0]);
      }
      rv = fprintf(msc_fd[senderP], "\n");
      if (rv < 0) {
         fprintf(stderr, "Error while logging MSC TX message : %s", &msc_proto2str[senderP][0]);
      }
      if ((msc_event_counter & 0x000000000000000F) == 0x000000000000000F) {
          fflush(msc_fd[senderP]);
      }
  }
}


