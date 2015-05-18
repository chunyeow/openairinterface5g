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
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <linux/sched.h>
#include <signal.h>
#include <execinfo.h>
#include <getopt.h>
#include <syscall.h>


#include "UTIL/LOG/log_extern.h"
#include "assertions.h"
#include "intertask_interface_init.h"
#include "intertask_interface.h"
#include "timer.h"
#include "sctp_eNB_task.h"
#include "s1ap_eNB.h"
#include "enb_config.h"

#include "oaisim_mme_test_s1c.h"

#   define ENB_REGISTER_RETRY_DELAY 5
//------------------------------------------------------------------------------
static void get_options (int argc, char **argv);
static uint32_t eNB_app_register(const uint32_t enb_id_start, const uint32_t enb_id_end, const Enb_properties_array_t *enb_properties);
static void    *eNB_app_task    (void *args_p);
//------------------------------------------------------------------------------
static char                    *conf_config_file_name = NULL;
static char                    *itti_dump_file        = NULL;
const Enb_properties_array_t   *enb_properties        = NULL;
int16_t                         glog_level            = LOG_INFO;
int16_t                         glog_verbosity        = LOG_MED;
//------------------------------------------------------------------------------
int debug          = 0;
int error_count    = 0;
int break_on_error = 0;

/* -1 means invalid */
static const signed char hex_digits[0x100] = {
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  0, 1, 2, 3, 4, 5, 6, 7, 8, 9,-1,-1,-1,-1,-1,-1,
  -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,10,11,12,13,14,15,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
  -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};

//------------------------------------------------------------------------------
void
fail (const char *format, ...)
//------------------------------------------------------------------------------
{
  char str[1024];
  va_list arg_ptr;

  va_start (arg_ptr, format);
  vsnprintf ( str, sizeof(str), format, arg_ptr);
  va_end (arg_ptr);
  fputs(str, stderr);
  error_count++;

  if (break_on_error)
    exit (1);
}

//------------------------------------------------------------------------------
void
success (const char *format, ...)
//------------------------------------------------------------------------------
{
  char str[1024];
  va_list arg_ptr;

  va_start (arg_ptr, format);
  vsnprintf ( str, sizeof(str), format, arg_ptr);
  va_end (arg_ptr);
  fputs(str, stderr);
}

//------------------------------------------------------------------------------
void
escapeprint (const char *str, size_t len)
//------------------------------------------------------------------------------
{
  size_t i;

  printf (" (length %d bytes):\n\t", (int) len);

  for (i = 0; i < len; i++) {
    if (((str[i] & 0xFF) >= 'A' && (str[i] & 0xFF) <= 'Z') ||
        ((str[i] & 0xFF) >= 'a' && (str[i] & 0xFF) <= 'z') ||
        ((str[i] & 0xFF) >= '0' && (str[i] & 0xFF) <= '9')
        || (str[i] & 0xFF) == ' ' || (str[i] & 0xFF) == '.')
      printf ("%c", (str[i] & 0xFF));
    else
      printf ("\\x%02X", (str[i] & 0xFF));

    if ((i + 1) % 16 == 0 && (i + 1) < len)
      printf ("'\n\t'");
  }

  printf ("\n");
}

//------------------------------------------------------------------------------
void
hexprint (const void *_str, size_t len)
//------------------------------------------------------------------------------
{
  size_t i;
  const char* str = _str;

  printf ("\t;; ");

  for (i = 0; i < len; i++) {
    printf ("%02x ", (str[i] & 0xFF));

    if ((i + 1) % 8 == 0)
      printf (" ");

    if ((i + 1) % 16 == 0 && i + 1 < len)
      printf ("\n\t;; ");
  }

  printf ("\n");
}

//------------------------------------------------------------------------------
void
binprint (const void *_str, size_t len)
//------------------------------------------------------------------------------
{
  size_t i;
  const char* str = _str;

  printf ("\t;; ");

  for (i = 0; i < len; i++) {
    printf ("%d%d%d%d%d%d%d%d ",
            (str[i] & 0xFF) & 0x80 ? 1 : 0,
            (str[i] & 0xFF) & 0x40 ? 1 : 0,
            (str[i] & 0xFF) & 0x20 ? 1 : 0,
            (str[i] & 0xFF) & 0x10 ? 1 : 0,
            (str[i] & 0xFF) & 0x08 ? 1 : 0,
            (str[i] & 0xFF) & 0x04 ? 1 : 0,
            (str[i] & 0xFF) & 0x02 ? 1 : 0, (str[i] & 0xFF) & 0x01 ? 1 : 0);

    if ((i + 1) % 3 == 0)
      printf (" ");

    if ((i + 1) % 6 == 0 && i + 1 < len)
      printf ("\n\t;; ");
  }

  printf ("\n");
}

//------------------------------------------------------------------------------
int
compare_buffer(const uint8_t *buffer, const uint32_t length_buffer,
               const uint8_t *pattern, const uint32_t length_pattern)
//------------------------------------------------------------------------------
{
  int i;

  if (length_buffer != length_pattern) {
    printf("Length mismatch, expecting %d bytes, got %d bytes\n", length_pattern,
           length_buffer);
    hexprint(buffer, length_buffer);
    return -1;
  }

  for (i = 0; i < length_buffer; i++) {
    if (pattern[i] != buffer[i]) {
      printf("Expecting:\n");
      hexprint(pattern, length_pattern);
      printf("Received:\n");
      hexprint(buffer, length_buffer);
      printf("Mismatch fount in byte %d\nExpecting 0x%02x, got 0x%02x\n",
             i, pattern[i], buffer[i]);
      return -1;
    }
  }

  return 0;
}

//------------------------------------------------------------------------------
unsigned
decode_hex_length(const char *h)
//------------------------------------------------------------------------------
{
  const unsigned char *hex = (const unsigned char *) h;
  unsigned count;
  unsigned i;

  for (count = i = 0; hex[i]; i++) {
    if (isspace(hex[i]))
      continue;

    if (hex_digits[hex[i]] < 0)
      abort();

    count++;
  }

  if (count % 2)
    abort();

  return count / 2;
}

//------------------------------------------------------------------------------
int
decode_hex(uint8_t *dst, const char *h)
//------------------------------------------------------------------------------
{
  const unsigned char *hex = (const unsigned char *) h;
  unsigned i = 0;

  for (;;) {
    int high, low;

    while (*hex && isspace(*hex))
      hex++;

    if (!*hex)
      return 1;

    high = hex_digits[*hex++];

    if (high < 0)
      return 0;

    while (*hex && isspace(*hex))
      hex++;

    if (!*hex)
      return 0;

    low = hex_digits[*hex++];

    if (low < 0)
      return 0;

    dst[i++] = (high << 4) | low;
  }
}

//------------------------------------------------------------------------------
uint8_t *
decode_hex_dup(const char *hex)
//------------------------------------------------------------------------------
{
  uint8_t *p;
  unsigned length = decode_hex_length(hex);

  p = malloc(length * sizeof(uint8_t));

  if (decode_hex(p, hex))
    return p;
  else {
    free(p);
    return NULL;
  }
}

//------------------------------------------------------------------------------
static void get_options (int argc, char **argv)
//------------------------------------------------------------------------------
{
  int c;


  while ((c = getopt_long (argc, argv, "K:g:G:O:",NULL,NULL)) != -1) {
    switch (c) {
    case 'K':
      itti_dump_file = strdup(optarg);
      break;

    case 'O':
      conf_config_file_name = optarg;
      break;

    case 'g':
      glog_level=atoi(optarg); // value between 1 - 9
      break;

    case 'G':
      glog_verbosity=atoi(optarg);// value from 0, 0x5, 0x15, 0x35, 0x75
      break;

    default:
      break;
    }
  }

  AssertFatal(conf_config_file_name != NULL,"Please provide a configuration file\n");


}


//------------------------------------------------------------------------------
static uint32_t eNB_app_register(const uint32_t enb_id_start, const uint32_t enb_id_end, const Enb_properties_array_t *enb_properties)
//------------------------------------------------------------------------------
{
  uint32_t         enb_id;
  uint32_t         mme_id;
  MessageDef      *msg_p;
  uint32_t         register_enb_pending = 0;
  char            *str                  = NULL;
  struct in_addr   addr;


  for (enb_id = enb_id_start; (enb_id < enb_id_end) ; enb_id++) {
    {
      s1ap_register_enb_req_t *s1ap_register_eNB;

      /* note:  there is an implicit relationship between the data structure and the message name */
      msg_p = itti_alloc_new_message (TASK_ENB_APP, S1AP_REGISTER_ENB_REQ);

      s1ap_register_eNB = &S1AP_REGISTER_ENB_REQ(msg_p);

      /* Some default/random parameters */
      s1ap_register_eNB->eNB_id           = enb_properties->properties[enb_id]->eNB_id;
      s1ap_register_eNB->cell_type        = enb_properties->properties[enb_id]->cell_type;
      s1ap_register_eNB->eNB_name         = enb_properties->properties[enb_id]->eNB_name;
      s1ap_register_eNB->tac              = enb_properties->properties[enb_id]->tac;
      s1ap_register_eNB->mcc              = enb_properties->properties[enb_id]->mcc;
      s1ap_register_eNB->mnc              = enb_properties->properties[enb_id]->mnc;
      s1ap_register_eNB->mnc_digit_length = enb_properties->properties[enb_id]->mnc_digit_length;
      s1ap_register_eNB->default_drx      = enb_properties->properties[enb_id]->pcch_defaultPagingCycle[0];

      s1ap_register_eNB->nb_mme =         enb_properties->properties[enb_id]->nb_mme;
      AssertFatal (s1ap_register_eNB->nb_mme <= S1AP_MAX_NB_MME_IP_ADDRESS, "Too many MME for eNB %d (%d/%d)!", enb_id, s1ap_register_eNB->nb_mme,
                   S1AP_MAX_NB_MME_IP_ADDRESS);

      for (mme_id = 0; mme_id < s1ap_register_eNB->nb_mme; mme_id++) {
        s1ap_register_eNB->mme_ip_address[mme_id].ipv4 = enb_properties->properties[enb_id]->mme_ip_address[mme_id].ipv4;
        s1ap_register_eNB->mme_ip_address[mme_id].ipv6 = enb_properties->properties[enb_id]->mme_ip_address[mme_id].ipv6;
        strncpy (s1ap_register_eNB->mme_ip_address[mme_id].ipv4_address,
                 enb_properties->properties[enb_id]->mme_ip_address[mme_id].ipv4_address,
                 sizeof(s1ap_register_eNB->mme_ip_address[0].ipv4_address));
        strncpy (s1ap_register_eNB->mme_ip_address[mme_id].ipv6_address,
                 enb_properties->properties[enb_id]->mme_ip_address[mme_id].ipv6_address,
                 sizeof(s1ap_register_eNB->mme_ip_address[0].ipv6_address));
      }

      s1ap_register_eNB->enb_ip_address.ipv6 = 0;
      s1ap_register_eNB->enb_ip_address.ipv4 = 1;
      addr.s_addr = enb_properties->properties[enb_id]->enb_ipv4_address_for_S1_MME;
      str = inet_ntoa(addr);
      strcpy(s1ap_register_eNB->enb_ip_address.ipv4_address, str);

      itti_send_msg_to_task (TASK_S1AP, ENB_MODULE_ID_TO_INSTANCE(enb_id), msg_p);

      register_enb_pending++;
    }
  }

  return register_enb_pending;
}

//------------------------------------------------------------------------------
static void *eNB_app_task(void *args_p)
//------------------------------------------------------------------------------
{
  const Enb_properties_array_t   *enb_properties_p  = NULL;
  uint32_t                        enb_nb = 1; /* Default number of eNB is 1 */
  uint32_t                        enb_id_start = 0;
  uint32_t                        enb_id_end = enb_id_start + enb_nb;
  uint32_t                        register_enb_pending;
  uint32_t                        registered_enb;
  long                            enb_register_retry_timer_id;
  MessageDef                     *msg_p           = NULL;
  const char                     *msg_name        = NULL;
  instance_t                      instance;
  int                             result;

  itti_mark_task_ready (TASK_ENB_APP);

  enb_properties_p = enb_config_get();

  AssertFatal (enb_nb <= enb_properties_p->number,
               "Number of eNB is greater than eNB defined in configuration file (%d/%d)!",
               enb_nb, enb_properties_p->number);


  /* Try to register each eNB */
  registered_enb = 0;
  register_enb_pending = eNB_app_register (enb_id_start, enb_id_end, enb_properties_p);

  do {
    // Wait for a message
    itti_receive_msg (TASK_ENB_APP, &msg_p);

    msg_name = ITTI_MSG_NAME (msg_p);
    instance = ITTI_MSG_INSTANCE (msg_p);

    switch (ITTI_MSG_ID(msg_p)) {
    case TERMINATE_MESSAGE:
      itti_exit_task ();
      break;

    case MESSAGE_TEST:
      LOG_I(ENB_APP, "Received %s\n", ITTI_MSG_NAME(msg_p));
      break;


    case S1AP_REGISTER_ENB_CNF:
      LOG_I(ENB_APP, "[eNB %d] Received %s: associated MME %d\n", instance, msg_name,
            S1AP_REGISTER_ENB_CNF(msg_p).nb_mme);

      DevAssert(register_enb_pending > 0);
      register_enb_pending--;

      /* Check if at least eNB is registered with one MME */
      if (S1AP_REGISTER_ENB_CNF(msg_p).nb_mme > 0) {
        registered_enb++;
      }

      /* Check if all register eNB requests have been processed */
      if (register_enb_pending == 0) {
        if (registered_enb == enb_nb) {
          /* If all eNB are registered, start L2L1 task */
          MessageDef *msg_init_p;

          msg_init_p = itti_alloc_new_message (TASK_ENB_APP, INITIALIZE_MESSAGE);
          itti_send_msg_to_task (TASK_L2L1, INSTANCE_DEFAULT, msg_init_p);


        } else {
          uint32_t not_associated = enb_nb - registered_enb;

          LOG_W(ENB_APP, " %d eNB %s not associated with a MME, retrying registration in %d seconds ...\n",
                not_associated, not_associated > 1 ? "are" : "is", ENB_REGISTER_RETRY_DELAY);

          /* Restart the eNB registration process in ENB_REGISTER_RETRY_DELAY seconds */
          if (timer_setup (ENB_REGISTER_RETRY_DELAY, 0, TASK_ENB_APP, INSTANCE_DEFAULT, TIMER_ONE_SHOT,
                           NULL, &enb_register_retry_timer_id) < 0) {
            LOG_E(ENB_APP, " Can not start eNB register retry timer, use \"sleep\" instead!\n");

            sleep(ENB_REGISTER_RETRY_DELAY);
            /* Restart the registration process */
            registered_enb = 0;
            register_enb_pending = eNB_app_register (enb_id_start, enb_id_end, enb_properties_p);
          }
        }
      }

      break;

    case S1AP_DEREGISTERED_ENB_IND:
      LOG_W(ENB_APP, "[eNB %d] Received %s: associated MME %d\n", instance, msg_name,
            S1AP_DEREGISTERED_ENB_IND(msg_p).nb_mme);

      /* TODO handle recovering of registration */
      break;

    case TIMER_HAS_EXPIRED:
      LOG_I(ENB_APP, " Received %s: timer_id %d\n", msg_name, TIMER_HAS_EXPIRED(msg_p).timer_id);

      if (TIMER_HAS_EXPIRED (msg_p).timer_id == enb_register_retry_timer_id) {
        /* Restart the registration process */
        registered_enb = 0;
        register_enb_pending = eNB_app_register (enb_id_start, enb_id_end, enb_properties_p);
      }

      break;

    default:
      LOG_E(ENB_APP, "Received unexpected message %s\n", msg_name);
      break;
    }

    result = itti_free (ITTI_MSG_ORIGIN_ID(msg_p), msg_p);
    AssertFatal (result == EXIT_SUCCESS, "Failed to free memory (%d)!\n", result);
  } while (1);

  return NULL;
}


//------------------------------------------------------------------------------
void mme_test_s1_notify_sctp_data_ind(uint32_t assoc_id, int32_t stream, const uint8_t * const data, const uint32_t data_length)
//------------------------------------------------------------------------------
{

}

//------------------------------------------------------------------------------
int main( int argc, char **argv )
//------------------------------------------------------------------------------
{
  // initialize the log (see log.h for details)
  logInit();

  get_options (argc, argv); //Command-line options
  set_glog(glog_level, glog_verbosity);
  log_set_instance_type (LOG_INSTANCE_ENB);

  /* Read eNB configuration file */
  enb_properties = enb_config_init(conf_config_file_name);

  itti_init(TASK_MAX, THREAD_MAX, MESSAGES_ID_MAX, tasks_info, messages_info, messages_definition_xml, itti_dump_file);

  itti_wait_ready(1);

  if (itti_create_task (TASK_SCTP, sctp_eNB_task, NULL) < 0) {
    LOG_E(SCTP, "Create task for SCTP failed\n");
    return -1;
  }

  if (itti_create_task (TASK_S1AP, s1ap_eNB_task, NULL) < 0) {
    LOG_E(S1AP, "Create task for S1AP failed\n");
    return -1;
  }

  if (itti_create_task (TASK_ENB_APP, eNB_app_task, NULL) < 0) {
    LOG_E(S1AP, "Create task for S1AP failed\n");
    return -1;
  }

  itti_wait_ready(0);



  sleep(30);

  logClean();
  return 0;
}
