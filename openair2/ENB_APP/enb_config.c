/*******************************************************************************

 Eurecom OpenAirInterface
 Copyright(c) 1999 - 2014 Eurecom

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

#include <string.h>
#include <libconfig.h>

#include "log.h"
#include "assertions.h"
#include "enb_config.h"
#if defined(OAI_EMU)
# include "OCG.h"
# include "OCG_extern.h"
#endif
#if defined(ENABLE_ITTI)
# include "intertask_interface.h"
# if defined(ENABLE_USE_MME)
#   include "s1ap_eNB.h"
#   include "sctp_eNB_task.h"
# endif
#endif

#include "LAYER2/MAC/extern.h"

// Hard to find a defined value for max enb...
#define MAX_ENB 16
Enb_properties_t *g_enb_properties[MAX_ENB];
int               g_num_enb_properties = 0;

int enb_config_init(char* lib_config_file_name_pP) {

  config_t          cfg;
  config_setting_t *setting;
  config_setting_t *setting_mme_addresses;
  config_setting_t *setting_mme_address;
  config_setting_t *setting_enb;
  int               num_enbs, num_mme_address, i, j, parse_error = 0, enb_properties_index;
  long int          enb_id;
  const char*       cell_type;
  long int          tac;
  const char*       enb_name;
  long int          mcc;
  long int          mnc;
  const char*       default_drx;
  const char*       frame_type;
  const char*       prefix_type;
  long int          downlink_frequency;
  long int          uplink_frequency_offset;
  char*             ipv4;
  char*             ipv6;
  char*             active;
  char*             preference;
  const char*       active_enb[MAX_ENB];


  memset((char*)g_enb_properties, 0 , MAX_ENB * sizeof(Enb_properties_t*));
  memset((char*)active_enb,       0 , MAX_ENB * sizeof(char*));

  config_init(&cfg);

  if(lib_config_file_name_pP != NULL)
  {
      /* Read the file. If there is an error, report it and exit. */
      if(! config_read_file(&cfg, lib_config_file_name_pP))
      {
          LOG_E(ENB_APP, "%s:%d - %s\n", lib_config_file_name_pP, config_error_line(&cfg), config_error_text(&cfg));
          config_destroy(&cfg);
          AssertFatal (0, "Failed to parse eNB configuration file %s!\n", lib_config_file_name_pP);
      }
  }
  else
  {
      LOG_E(ENB_APP, "No eNB configuration file provided!\n");
      config_destroy(&cfg);
      AssertFatal (0, "No eNB configuration file provided!\n");
  }

  // Get list of active eNBs, (only these will be configured)
  g_num_enb_properties = 0;
  setting = config_lookup(&cfg, ENB_CONFIG_STRING_ACTIVE_ENBS);
  if(setting != NULL)
  {
      num_enbs = config_setting_length(setting);
      for (i = 0; i < num_enbs; i++) {
          setting_enb   = config_setting_get_elem(setting, i);
          active_enb[i] = config_setting_get_string (setting_enb);
          AssertFatal (active_enb[i] != NULL,
                       "Failed to parse config file %s, %uth attribute %s \n",
                       lib_config_file_name_pP, i, ENB_CONFIG_STRING_ACTIVE_ENBS);
          active_enb[i] = strdup(active_enb[i]);
          g_num_enb_properties += 1;
      }
  }

  /* Output a list of all eNBs. */
  setting = config_lookup(&cfg, ENB_CONFIG_STRING_ENB_LIST);
  if(setting != NULL)
  {
      enb_properties_index = 0;
      parse_error      = 0;
      num_enbs = config_setting_length(setting);
      for (i = 0; i < num_enbs; i++) {
          setting_enb = config_setting_get_elem(setting, i);
          if(  !(
                        config_setting_lookup_int   (setting_enb, ENB_CONFIG_STRING_ENB_ID,              &enb_id)
                     && config_setting_lookup_string(setting_enb, ENB_CONFIG_STRING_CELL_TYPE,           &cell_type)
                     && config_setting_lookup_string(setting_enb, ENB_CONFIG_STRING_ENB_NAME,            &enb_name)
                     && config_setting_lookup_int   (setting_enb, ENB_CONFIG_STRING_TRACKING_AREA_CODE,  &tac)
                     && config_setting_lookup_int   (setting_enb, ENB_CONFIG_STRING_MOBILE_COUNTRY_CODE, &mcc)
                     && config_setting_lookup_int   (setting_enb, ENB_CONFIG_STRING_MOBILE_NETWORK_CODE, &mnc)
                     && config_setting_lookup_string(setting_enb, ENB_CONFIG_STRING_DEFAULT_PAGING_DRX,  &default_drx)
                )
            ) {
              AssertError (0, parse_error ++,
                      "Failed to parse eNB configuration file %s, %u th enb\n",
                      lib_config_file_name_pP, i);
          }
          // search if in active list
          for (j=0; j < g_num_enb_properties; j++) {
              if (strcmp(active_enb[j], enb_name) == 0) {
                  g_enb_properties[enb_properties_index] = calloc(1, sizeof(Enb_properties_t));

                  g_enb_properties[enb_properties_index]->eNB_id   = enb_id;
                  if (strcmp(cell_type, "CELL_MACRO_ENB") == 0) {
                      g_enb_properties[enb_properties_index]->cell_type = CELL_MACRO_ENB;
                  } else  if (strcmp(cell_type, "CELL_HOME_ENB") == 0) {
                      g_enb_properties[enb_properties_index]->cell_type = CELL_HOME_ENB;
                  } else {
                      AssertError (0, parse_error ++,
                              "Failed to parse eNB configuration file %s, enb %d unknown value \"%s\" for cell_type choice: CELL_MACRO_ENB or CELL_HOME_ENB !\n",
                              lib_config_file_name_pP, i, cell_type);
                  }
                  g_enb_properties[enb_properties_index]->eNB_name = strdup(enb_name);
                  g_enb_properties[enb_properties_index]->tac      = (uint16_t)tac;
                  g_enb_properties[enb_properties_index]->mcc      = (uint16_t)mcc;
                  g_enb_properties[enb_properties_index]->mnc      = (uint16_t)mnc;

                  if (strcmp(default_drx, "PAGING_DRX_32") == 0) {
                      g_enb_properties[enb_properties_index]->default_drx = PAGING_DRX_32;
                  } else  if (strcmp(default_drx, "PAGING_DRX_64") == 0) {
                      g_enb_properties[enb_properties_index]->default_drx = PAGING_DRX_64;
                  } else  if (strcmp(default_drx, "PAGING_DRX_128") == 0) {
                      g_enb_properties[enb_properties_index]->default_drx = PAGING_DRX_128;
                  } else  if (strcmp(default_drx, "PAGING_DRX_256") == 0) {
                      g_enb_properties[enb_properties_index]->default_drx = PAGING_DRX_256;
                  } else {
                      AssertError (0, parse_error ++,
                              "Failed to parse eNB configuration file %s, enb %d unknown value \"%s\" for default_drx choice: PAGING_DRX_32..PAGING_DRX_256 !\n",
                              lib_config_file_name_pP, i, default_drx);
                  }

                  // Parse optional physical parameters
                  if(config_setting_lookup_string(setting_enb, ENB_CONFIG_STRING_FRAME_TYPE, &frame_type)) {
                      if (strcmp(frame_type, "FDD") == 0) {
                          g_enb_properties[enb_properties_index]->frame_type = FDD;
                      } else  if (strcmp(frame_type, "TDD") == 0) {
                          g_enb_properties[enb_properties_index]->frame_type = TDD;
                      } else {
                          AssertError (0, parse_error ++,
                                  "Failed to parse eNB configuration file %s, enb %d unknown value \"%s\" for frame_type choice: FDD or TDD !\n",
                                  lib_config_file_name_pP, i, frame_type);
                      }
                  } else {
                      g_enb_properties[enb_properties_index]->frame_type = FDD; // Default frame type
                  }

                  if(config_setting_lookup_string(setting_enb, ENB_CONFIG_STRING_PREFIX_TYPE, &prefix_type)) {
                      if (strcmp(prefix_type, "NORMAL") == 0) {
                          g_enb_properties[enb_properties_index]->prefix_type = NORMAL;
                      } else  if (strcmp(prefix_type, "EXTENDED") == 0) {
                          g_enb_properties[enb_properties_index]->prefix_type = EXTENDED;
                      } else {
                          AssertError (0, parse_error ++,
                                  "Failed to parse eNB configuration file %s, enb %d unknown value \"%s\" for prefix_type choice: NORMAL or EXTENDED !\n",
                                  lib_config_file_name_pP, i, prefix_type);
                      }
                  } else {
                      g_enb_properties[enb_properties_index]->prefix_type = NORMAL; // Default prefix type
                  }

                  if(config_setting_lookup_int(setting_enb, ENB_CONFIG_STRING_DOWNLINK_FREQUENCY, &downlink_frequency)) {
                      g_enb_properties[enb_properties_index]->downlink_frequency = downlink_frequency;
                  } else {
                      g_enb_properties[enb_properties_index]->downlink_frequency = 2680000000; // Default downlink frequency
                  }

                  if(config_setting_lookup_int(setting_enb, ENB_CONFIG_STRING_UPLINK_FREQUENCY_OFFSET, &uplink_frequency_offset)) {
                      g_enb_properties[enb_properties_index]->uplink_frequency_offset = uplink_frequency_offset;
                  } else {
                      g_enb_properties[enb_properties_index]->uplink_frequency_offset = -120000000; // Default uplink frequency offset
                  }

                  setting_mme_addresses = config_setting_get_member (setting_enb, ENB_CONFIG_STRING_MME_IP_ADDRESS);
                  num_mme_address     = config_setting_length(setting_mme_addresses);
                  g_enb_properties[enb_properties_index]->nb_mme = 0;
                  for (j = 0; j < num_mme_address; j++) {
                      setting_mme_address = config_setting_get_elem(setting_mme_addresses, j);
                      if(  !(
                              config_setting_lookup_string(setting_mme_address, ENB_CONFIG_STRING_MME_IPV4_ADDRESS, (const char **)&ipv4)
                              && config_setting_lookup_string(setting_mme_address, ENB_CONFIG_STRING_MME_IPV6_ADDRESS, (const char **)&ipv6)
                              && config_setting_lookup_string(setting_mme_address, ENB_CONFIG_STRING_MME_IP_ADDRESS_ACTIVE, (const char **)&active)
                              && config_setting_lookup_string(setting_mme_address, ENB_CONFIG_STRING_MME_IP_ADDRESS_PREFERENCE, (const char **)&preference)
                            )
                        ) {
                          AssertError (0, parse_error ++,
                                  "Failed to parse eNB configuration file %s, %u th enb %u th mme address !\n",
                                  lib_config_file_name_pP, i, j);
                      }
                      g_enb_properties[enb_properties_index]->nb_mme += 1;

                      g_enb_properties[enb_properties_index]->mme_ip_address[j].ipv4_address = strdup(ipv4);
                      g_enb_properties[enb_properties_index]->mme_ip_address[j].ipv6_address = strdup(ipv6);
                      if (strcmp(active, "yes") == 0) {
                          g_enb_properties[enb_properties_index]->mme_ip_address[j].active = 1;
#if defined(ENABLE_USE_MME)
                          EPC_MODE_ENABLED = 1;
#endif
                      } // else { (calloc)

                      if (strcmp(preference, "ipv4") == 0) {
                          g_enb_properties[enb_properties_index]->mme_ip_address[j].ipv4 = 1;
                      } else if (strcmp(preference, "ipv6") == 0) {
                          g_enb_properties[enb_properties_index]->mme_ip_address[j].ipv6 = 1;
                      } else if (strcmp(preference, "no") == 0) {
                          g_enb_properties[enb_properties_index]->mme_ip_address[j].ipv4 = 1;
                          g_enb_properties[enb_properties_index]->mme_ip_address[j].ipv6 = 1;
                      }
                  }
                  enb_properties_index += 1;
                  break;
              }
          }
      }
  }
  AssertError (enb_properties_index == g_num_enb_properties, parse_error ++,
          "Failed to parse eNB configuration file %s, mismatch between %u active eNBs and %u corresponding defined eNBs !\n",
          lib_config_file_name_pP, g_num_enb_properties, enb_properties_index);

  AssertFatal (parse_error == 0,
               "Failed to parse eNB configuration file %s, found %d error%s !\n",
               lib_config_file_name_pP, parse_error, parse_error > 1 ? "s" : "");
  return 0;
}
