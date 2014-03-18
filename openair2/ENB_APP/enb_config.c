/*******************************************************************************
Eurecom OpenAirInterface 2
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
Forums       : http://forums.eurecom.fsr/openairinterface
Address      : EURECOM,
               Campus SophiaTech,
               450 Route des Chappes,
               CS 50193
               06904 Biot Sophia Antipolis cedex,
               FRANCE
*******************************************************************************/
/*
                                enb_config.c
                             -------------------
  AUTHOR  : Lionel GAUTHIER, Laurent Winckel
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
*/

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

#define ENB_CONFIG_STRING_ACTIVE_ENBS                   "Active_eNBs"

#define ENB_CONFIG_STRING_ENB_LIST                      "eNBs"
#define ENB_CONFIG_STRING_ENB_ID                        "eNB_ID"
#define ENB_CONFIG_STRING_CELL_TYPE                     "cell_type"
#define ENB_CONFIG_STRING_ENB_NAME                      "eNB_name"

#define ENB_CONFIG_STRING_TRACKING_AREA_CODE            "tracking_area_code"
#define ENB_CONFIG_STRING_MOBILE_COUNTRY_CODE           "mobile_country_code"
#define ENB_CONFIG_STRING_MOBILE_NETWORK_CODE           "mobile_network_code"

#define ENB_CONFIG_STRING_DEFAULT_PAGING_DRX            "default_paging_drx"

#define ENB_CONFIG_STRING_FRAME_TYPE                    "frame_type"
#define ENB_CONFIG_STRING_TDD_CONFIG                    "tdd_config"
#define ENB_CONFIG_STRING_TDD_CONFIG_S                  "tdd_config_s"
#define ENB_CONFIG_STRING_PREFIX_TYPE                   "prefix_type"
#define ENB_CONFIG_STRING_EUTRA_BAND                    "eutra_band"
#define ENB_CONFIG_STRING_DOWNLINK_FREQUENCY            "downlink_frequency"
#define ENB_CONFIG_STRING_UPLINK_FREQUENCY_OFFSET       "uplink_frequency_offset"

#define ENB_CONFIG_STRING_MME_IP_ADDRESS                "mme_ip_address"
#define ENB_CONFIG_STRING_MME_IPV4_ADDRESS              "ipv4"
#define ENB_CONFIG_STRING_MME_IPV6_ADDRESS              "ipv6"
#define ENB_CONFIG_STRING_MME_IP_ADDRESS_ACTIVE         "active"
#define ENB_CONFIG_STRING_MME_IP_ADDRESS_PREFERENCE     "preference"

#define ENB_CONFIG_STRING_NETWORK_INTERFACES_CONFIG     "NETWORK_INTERFACES"
#define ENB_CONFIG_STRING_ENB_INTERFACE_NAME_FOR_S1_MME "ENB_INTERFACE_NAME_FOR_S1_MME"
#define ENB_CONFIG_STRING_ENB_IPV4_ADDRESS_FOR_S1_MME   "ENB_IPV4_ADDRESS_FOR_S1_MME"
#define ENB_CONFIG_STRING_ENB_INTERFACE_NAME_FOR_S1U    "ENB_INTERFACE_NAME_FOR_S1U"
#define ENB_CONFIG_STRING_ENB_IPV4_ADDR_FOR_S1U         "ENB_IPV4_ADDRESS_FOR_S1U"

#define KHz (1000UL)
#define MHz (1000 * KHz)

typedef struct eutra_band_s
{
    int16_t             band;
    uint32_t            ul_min;
    uint32_t            ul_max;
    uint32_t            dl_min;
    uint32_t            dl_max;
    lte_frame_type_t    frame_type;
} eutra_band_t;

static const eutra_band_t eutra_bands[] =
{
    { 1, 1920    * MHz, 1980    * MHz, 2110    * MHz, 2170    * MHz, FDD},
    { 2, 1850    * MHz, 1910    * MHz, 1930    * MHz, 1990    * MHz, FDD},
    { 3, 1710    * MHz, 1785    * MHz, 1805    * MHz, 1880    * MHz, FDD},
    { 4, 1710    * MHz, 1755    * MHz, 2110    * MHz, 2155    * MHz, FDD},
    { 5,  824    * MHz,  849    * MHz,  869    * MHz,  894    * MHz, FDD},
    { 6,  830    * MHz,  840    * MHz,  875    * MHz,  885    * MHz, FDD},
    { 7, 2500    * MHz, 2570    * MHz, 2620    * MHz, 2690    * MHz, FDD},
    { 8,  880    * MHz,  915    * MHz,  925    * MHz,  960    * MHz, FDD},
    { 9, 1749900 * KHz, 1784900 * KHz, 1844900 * KHz, 1879900 * KHz, FDD},
    {10, 1710    * MHz, 1770    * MHz, 2110    * MHz, 2170    * MHz, FDD},
    {11, 1427900 * KHz, 1452900 * KHz, 1475900 * KHz, 1500900 * KHz, FDD},
    {12,  698    * MHz,  716    * MHz,  728    * MHz,  746    * MHz, FDD},
    {13,  777    * MHz,  787    * MHz,  746    * MHz,  756    * MHz, FDD},
    {14,  788    * MHz,  798    * MHz,  758    * MHz,  768    * MHz, FDD},

    {17,  704    * MHz,  716    * MHz,  734    * MHz,  746    * MHz, FDD},

    {33, 1900    * MHz, 1920    * MHz, 1900    * MHz, 1920    * MHz, TDD},
    {34, 2010    * MHz, 2025    * MHz, 2010    * MHz, 2025    * MHz, TDD},
    {35, 1850    * MHz, 1910    * MHz, 1850    * MHz, 1910    * MHz, TDD},
    {36, 1930    * MHz, 1990    * MHz, 1930    * MHz, 1990    * MHz, TDD},
    {37, 1910    * MHz, 1930    * MHz, 1910    * MHz, 1930    * MHz, TDD},
    {38, 2570    * MHz, 2620    * MHz, 2570    * MHz, 2630    * MHz, TDD},
    {39, 1880    * MHz, 1920    * MHz, 1880    * MHz, 1920    * MHz, TDD},
    {40, 2300    * MHz, 2400    * MHz, 2300    * MHz, 2400    * MHz, TDD},
};

static Enb_properties_array_t enb_properties;

static void enb_config_display(void) {
    int i;

    printf( "\n----------------------------------------------------------------------\n");
    printf( " ENB CONFIG FILE CONTENT LOADED (TBC):\n");
    printf( "----------------------------------------------------------------------\n");
    for (i = 0; i < enb_properties.number; i++) {
        printf( "ENB CONFIG no %u:\n\n", i);
        printf( "\teNB name: \t%s:\n",enb_properties.properties[i]->eNB_name);
        printf( "\teNB ID:   \t%u:\n",enb_properties.properties[i]->eNB_id);
        printf( "\tCell type:\t%s:\n",enb_properties.properties[i]->cell_type == CELL_MACRO_ENB ? "CELL_MACRO_ENB":"CELL_HOME_ENB");
        printf( "\tTAC:      \t%u:\n",enb_properties.properties[i]->tac);
        printf( "\tMCC:      \t%u:\n",enb_properties.properties[i]->mcc);
        printf( "\tMNC:      \t%u:\n",enb_properties.properties[i]->mnc);
        printf( "\n--------------------------------------------------------\n");
    }
}


static int enb_check_band_frequencies(char* lib_config_file_name_pP,
                                      int enb_properties_index,
                                      int16_t band,
                                      uint32_t downlink_frequency,
                                      int32_t uplink_frequency_offset,
                                      lte_frame_type_t frame_type)
{
    int errors = 0;

    if (band > 0)
    {
        int band_index;

        for (band_index = 0; band_index < sizeof (eutra_bands) / sizeof (eutra_bands[0]); band_index++)
        {
            if (band == eutra_bands[band_index].band)
            {
                uint32_t uplink_frequency = downlink_frequency + uplink_frequency_offset;

                AssertError (eutra_bands[band_index].dl_min < downlink_frequency, errors ++,
                             "Failed to parse eNB configuration file %s, enb %d downlink frequency %u too low (%u) for band %d!",
                             lib_config_file_name_pP, enb_properties_index, downlink_frequency, eutra_bands[band_index].dl_min, band);
                AssertError (downlink_frequency < eutra_bands[band_index].dl_max, errors ++,
                             "Failed to parse eNB configuration file %s, enb %d downlink frequency %u too high (%u) for band %d!",
                             lib_config_file_name_pP, enb_properties_index, downlink_frequency, eutra_bands[band_index].dl_max, band);

                AssertError (eutra_bands[band_index].ul_min < uplink_frequency, errors ++,
                             "Failed to parse eNB configuration file %s, enb %d uplink frequency %u too low (%u) for band %d!",
                             lib_config_file_name_pP, enb_properties_index, uplink_frequency, eutra_bands[band_index].ul_min, band);
                AssertError (uplink_frequency < eutra_bands[band_index].ul_max, errors ++,
                             "Failed to parse eNB configuration file %s, enb %d uplink frequency %u too high (%u) for band %d!",
                             lib_config_file_name_pP, enb_properties_index, uplink_frequency, eutra_bands[band_index].ul_max, band);

                AssertError (eutra_bands[band_index].frame_type == frame_type, errors ++,
                             "Failed to parse eNB configuration file %s, enb %d invalid frame type (%d/%d) for band %d!",
                             lib_config_file_name_pP, enb_properties_index, eutra_bands[band_index].frame_type, frame_type, band);
            }
        }
    }
    return errors;
}

const Enb_properties_array_t *enb_config_init(char* lib_config_file_name_pP) {
  config_t          cfg;
  config_setting_t *setting                       = NULL;
  config_setting_t *subsetting                    = NULL;
  config_setting_t *setting_mme_addresses         = NULL;
  config_setting_t *setting_mme_address           = NULL;
  config_setting_t *setting_enb                   = NULL;
  int               num_enb_properties            = 0;
  int               enb_properties_index          = 0;
  int               num_enbs;
  int               num_mme_address;
  int               i;
  int               j;
  int               parse_errors                  = 0;
  long int          enb_id                        = 0;
  const char*       cell_type                     = NULL;
  long int          tac                           = 0;
  const char*       enb_name                      = NULL;
  long int          mcc                           = 0;
  long int          mnc                           = 0;
  const char*       default_drx                   = NULL;
  const char*       frame_type;
  long int          tdd_config;
  long int          tdd_config_s;
  const char*       prefix_type;
  long int          eutra_band;
  double            downlink_frequency;
  double            uplink_frequency_offset;
  char*             ipv4                          = NULL;
  char*             ipv6                          = NULL;
  char*             active                        = NULL;
  char*             preference                    = NULL;
  const char*       active_enb[MAX_ENB];
  char*             enb_interface_name_for_S1U    = NULL;
  char*             enb_ipv4_address_for_S1U      = NULL;
  char*             enb_interface_name_for_S1_MME = NULL;
  char*             enb_ipv4_address_for_S1_MME   = NULL;
  char             *address                       = NULL;
  char             *cidr                          = NULL;

  memset((char*) (enb_properties.properties), 0 , MAX_ENB * sizeof(Enb_properties_t *));
  memset((char*)active_enb,     0 , MAX_ENB * sizeof(char*));

  config_init(&cfg);

  if(lib_config_file_name_pP != NULL)
  {
      /* Read the file. If there is an error, report it and exit. */
      if(! config_read_file(&cfg, lib_config_file_name_pP))
      {
          config_destroy(&cfg);
          AssertFatal (0, "Failed to parse eNB configuration file %s!\n", lib_config_file_name_pP);
      }
  }
  else
  {
      config_destroy(&cfg);
      AssertFatal (0, "No eNB configuration file provided!\n");
  }

  // Get list of active eNBs, (only these will be configured)
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
          num_enb_properties += 1;
      }
  }

  /* Output a list of all eNBs. */
  setting = config_lookup(&cfg, ENB_CONFIG_STRING_ENB_LIST);
  if(setting != NULL)
  {
      enb_properties_index = 0;
      parse_errors      = 0;
      num_enbs = config_setting_length(setting);
      for (i = 0; i < num_enbs; i++) {
          setting_enb = config_setting_get_elem(setting, i);

          if(! config_setting_lookup_int(setting_enb, ENB_CONFIG_STRING_ENB_ID, &enb_id)) {
              /* Calculate a default eNB ID */
# if defined(ENABLE_USE_MME)
              uint32_t hash;

              hash = s1ap_generate_eNB_id ();
              enb_id = i + (hash & 0xFFFF8);
# else
              enb_id = i;
# endif
          }

          if(  !(       config_setting_lookup_string(setting_enb, ENB_CONFIG_STRING_CELL_TYPE,           &cell_type)
                     && config_setting_lookup_string(setting_enb, ENB_CONFIG_STRING_ENB_NAME,            &enb_name)
                     && config_setting_lookup_int   (setting_enb, ENB_CONFIG_STRING_TRACKING_AREA_CODE,  &tac)
                     && config_setting_lookup_int   (setting_enb, ENB_CONFIG_STRING_MOBILE_COUNTRY_CODE, &mcc)
                     && config_setting_lookup_int   (setting_enb, ENB_CONFIG_STRING_MOBILE_NETWORK_CODE, &mnc)
                     && config_setting_lookup_string(setting_enb, ENB_CONFIG_STRING_DEFAULT_PAGING_DRX,  &default_drx)
                )
            ) {
              AssertError (0, parse_errors ++,
                      "Failed to parse eNB configuration file %s, %u th enb\n",
                      lib_config_file_name_pP, i);
          }
          // search if in active list
          for (j=0; j < num_enb_properties; j++) {
              if (strcmp(active_enb[j], enb_name) == 0) {
                  enb_properties.properties[enb_properties_index] = calloc(1, sizeof(Enb_properties_t));

                  enb_properties.properties[enb_properties_index]->eNB_id   = enb_id;
                  if (strcmp(cell_type, "CELL_MACRO_ENB") == 0) {
                      enb_properties.properties[enb_properties_index]->cell_type = CELL_MACRO_ENB;
                  } else  if (strcmp(cell_type, "CELL_HOME_ENB") == 0) {
                      enb_properties.properties[enb_properties_index]->cell_type = CELL_HOME_ENB;
                  } else {
                      AssertError (0, parse_errors ++,
                              "Failed to parse eNB configuration file %s, enb %d unknown value \"%s\" for cell_type choice: CELL_MACRO_ENB or CELL_HOME_ENB !\n",
                              lib_config_file_name_pP, i, cell_type);
                  }
                  enb_properties.properties[enb_properties_index]->eNB_name = strdup(enb_name);
                  enb_properties.properties[enb_properties_index]->tac      = (uint16_t)tac;
                  enb_properties.properties[enb_properties_index]->mcc      = (uint16_t)mcc;
                  enb_properties.properties[enb_properties_index]->mnc      = (uint16_t)mnc;

                  if (strcmp(default_drx, "PAGING_DRX_32") == 0) {
                      enb_properties.properties[enb_properties_index]->default_drx = PAGING_DRX_32;
                  } else  if (strcmp(default_drx, "PAGING_DRX_64") == 0) {
                      enb_properties.properties[enb_properties_index]->default_drx = PAGING_DRX_64;
                  } else  if (strcmp(default_drx, "PAGING_DRX_128") == 0) {
                      enb_properties.properties[enb_properties_index]->default_drx = PAGING_DRX_128;
                  } else  if (strcmp(default_drx, "PAGING_DRX_256") == 0) {
                      enb_properties.properties[enb_properties_index]->default_drx = PAGING_DRX_256;
                  } else {
                      AssertError (0, parse_errors ++,
                              "Failed to parse eNB configuration file %s, enb %d unknown value \"%s\" for default_drx choice: PAGING_DRX_32..PAGING_DRX_256 !\n",
                              lib_config_file_name_pP, i, default_drx);
                  }

                  // Parse optional physical parameters
                  if(config_setting_lookup_string(setting_enb, ENB_CONFIG_STRING_FRAME_TYPE, &frame_type)) {
                      if (strcmp(frame_type, "FDD") == 0) {
                          enb_properties.properties[enb_properties_index]->frame_type = FDD;
                      } else  if (strcmp(frame_type, "TDD") == 0) {
                          enb_properties.properties[enb_properties_index]->frame_type = TDD;
                      } else {
                          AssertError (0, parse_errors ++,
                                  "Failed to parse eNB configuration file %s, enb %d unknown value \"%s\" for frame_type choice: FDD or TDD !\n",
                                  lib_config_file_name_pP, i, frame_type);
                      }
                  } else {
                      enb_properties.properties[enb_properties_index]->frame_type = FDD; // Default frame type
                  }

                  if(config_setting_lookup_int(setting_enb, ENB_CONFIG_STRING_TDD_CONFIG, &tdd_config)) {
                      enb_properties.properties[enb_properties_index]->tdd_config = tdd_config;
                      AssertError (tdd_config <= TDD_Config__subframeAssignment_sa6, parse_errors ++,
                              "Failed to parse eNB configuration file %s, enb %d illegal tdd_config %ld (should be 0-%d)!",
                              lib_config_file_name_pP, i, tdd_config, TDD_Config__subframeAssignment_sa6);
                  } else {
                      enb_properties.properties[enb_properties_index]->tdd_config = 3; // Default TDD sub-frame configuration
                  }

                  if(config_setting_lookup_int(setting_enb, ENB_CONFIG_STRING_TDD_CONFIG_S, &tdd_config_s)) {
                      enb_properties.properties[enb_properties_index]->tdd_config_s = tdd_config_s;
                      AssertError (tdd_config_s <= TDD_Config__specialSubframePatterns_ssp8, parse_errors ++,
                              "Failed to parse eNB configuration file %s, enb %d illegal tdd_config_s %ld (should be 0-%d)!",
                              lib_config_file_name_pP, i, tdd_config_s, TDD_Config__specialSubframePatterns_ssp8);
                  } else {
                      enb_properties.properties[enb_properties_index]->tdd_config_s = 0; // Default TDD S-sub-frame configuration
                  }

                  if(config_setting_lookup_string(setting_enb, ENB_CONFIG_STRING_PREFIX_TYPE, &prefix_type)) {
                      if (strcmp(prefix_type, "NORMAL") == 0) {
                          enb_properties.properties[enb_properties_index]->prefix_type = NORMAL;
                      } else  if (strcmp(prefix_type, "EXTENDED") == 0) {
                          enb_properties.properties[enb_properties_index]->prefix_type = EXTENDED;
                      } else {
                          AssertError (0, parse_errors ++,
                                  "Failed to parse eNB configuration file %s, enb %d unknown value \"%s\" for prefix_type choice: NORMAL or EXTENDED !\n",
                                  lib_config_file_name_pP, i, prefix_type);
                      }
                  } else {
                      enb_properties.properties[enb_properties_index]->prefix_type = NORMAL; // Default prefix type
                  }

                  if(config_setting_lookup_int(setting_enb, ENB_CONFIG_STRING_EUTRA_BAND, &eutra_band)) {
                      enb_properties.properties[enb_properties_index]->eutra_band = eutra_band;
                  } else {
                      enb_properties.properties[enb_properties_index]->eutra_band = 7; // Default band
                  }

                  if(config_setting_lookup_float(setting_enb, ENB_CONFIG_STRING_DOWNLINK_FREQUENCY, &downlink_frequency)) {
                      enb_properties.properties[enb_properties_index]->downlink_frequency = downlink_frequency;
                  } else {
                      enb_properties.properties[enb_properties_index]->downlink_frequency = 2680000000UL; // Default downlink frequency
                  }

                  if(config_setting_lookup_float(setting_enb, ENB_CONFIG_STRING_UPLINK_FREQUENCY_OFFSET, &uplink_frequency_offset)) {
                      enb_properties.properties[enb_properties_index]->uplink_frequency_offset = uplink_frequency_offset;
                  } else {
                      // Default uplink frequency offset
                      if (enb_properties.properties[enb_properties_index]->frame_type == FDD) {
                          enb_properties.properties[enb_properties_index]->uplink_frequency_offset = -120000000;
                      } else {
                          enb_properties.properties[enb_properties_index]->uplink_frequency_offset = 0;
                      }
                  }

                  parse_errors += enb_check_band_frequencies(lib_config_file_name_pP,
                                             enb_properties_index,
                                             enb_properties.properties[enb_properties_index]->eutra_band,
                                             enb_properties.properties[enb_properties_index]->downlink_frequency,
                                             enb_properties.properties[enb_properties_index]->uplink_frequency_offset,
                                             enb_properties.properties[enb_properties_index]->frame_type);

                  setting_mme_addresses = config_setting_get_member (setting_enb, ENB_CONFIG_STRING_MME_IP_ADDRESS);
                  num_mme_address     = config_setting_length(setting_mme_addresses);
                  enb_properties.properties[enb_properties_index]->nb_mme = 0;
                  for (j = 0; j < num_mme_address; j++) {
                      setting_mme_address = config_setting_get_elem(setting_mme_addresses, j);
                      if(  !(
                              config_setting_lookup_string(setting_mme_address, ENB_CONFIG_STRING_MME_IPV4_ADDRESS, (const char **)&ipv4)
                              && config_setting_lookup_string(setting_mme_address, ENB_CONFIG_STRING_MME_IPV6_ADDRESS, (const char **)&ipv6)
                              && config_setting_lookup_string(setting_mme_address, ENB_CONFIG_STRING_MME_IP_ADDRESS_ACTIVE, (const char **)&active)
                              && config_setting_lookup_string(setting_mme_address, ENB_CONFIG_STRING_MME_IP_ADDRESS_PREFERENCE, (const char **)&preference)
                            )
                        ) {
                          AssertError (0, parse_errors ++,
                                  "Failed to parse eNB configuration file %s, %u th enb %u th mme address !\n",
                                  lib_config_file_name_pP, i, j);
                      }
                      enb_properties.properties[enb_properties_index]->nb_mme += 1;

                      enb_properties.properties[enb_properties_index]->mme_ip_address[j].ipv4_address = strdup(ipv4);
                      enb_properties.properties[enb_properties_index]->mme_ip_address[j].ipv6_address = strdup(ipv6);
                      if (strcmp(active, "yes") == 0) {
                          enb_properties.properties[enb_properties_index]->mme_ip_address[j].active = 1;
#if defined(ENABLE_USE_MME)
                          EPC_MODE_ENABLED = 1;
#endif
                      } // else { (calloc)

                      if (strcmp(preference, "ipv4") == 0) {
                          enb_properties.properties[enb_properties_index]->mme_ip_address[j].ipv4 = 1;
                      } else if (strcmp(preference, "ipv6") == 0) {
                          enb_properties.properties[enb_properties_index]->mme_ip_address[j].ipv6 = 1;
                      } else if (strcmp(preference, "no") == 0) {
                          enb_properties.properties[enb_properties_index]->mme_ip_address[j].ipv4 = 1;
                          enb_properties.properties[enb_properties_index]->mme_ip_address[j].ipv6 = 1;
                      }
                  }
                  subsetting = config_setting_get_member (setting_enb, ENB_CONFIG_STRING_NETWORK_INTERFACES_CONFIG);
                  if(subsetting != NULL) {
                      if(  (
                              config_setting_lookup_string( subsetting, ENB_CONFIG_STRING_ENB_INTERFACE_NAME_FOR_S1_MME,
                                      (const char **)&enb_interface_name_for_S1_MME)
                           && config_setting_lookup_string( subsetting, ENB_CONFIG_STRING_ENB_IPV4_ADDRESS_FOR_S1_MME,
                                   (const char **)&enb_ipv4_address_for_S1_MME)
                           && config_setting_lookup_string( subsetting, ENB_CONFIG_STRING_ENB_INTERFACE_NAME_FOR_S1U,
                                   (const char **)&enb_interface_name_for_S1U)
                           && config_setting_lookup_string( subsetting, ENB_CONFIG_STRING_ENB_IPV4_ADDR_FOR_S1U,
                                   (const char **)&enb_ipv4_address_for_S1U)
                         )
                     ){
                          enb_properties.properties[enb_properties_index]->enb_interface_name_for_S1U = strdup(enb_interface_name_for_S1U);
                          cidr = enb_ipv4_address_for_S1U;
                          address = strtok(cidr, "/");
                          if (address) {
                              IPV4_STR_ADDR_TO_INT_NWBO ( address, enb_properties.properties[enb_properties_index]->enb_ipv4_address_for_S1U, "BAD IP ADDRESS FORMAT FOR eNB S1_U !\n" );
                          }

                          enb_properties.properties[enb_properties_index]->enb_interface_name_for_S1_MME = strdup(enb_interface_name_for_S1_MME);
                          cidr = enb_ipv4_address_for_S1_MME;
                          address = strtok(cidr, "/");
                          if (address) {
                              IPV4_STR_ADDR_TO_INT_NWBO ( address, enb_properties.properties[enb_properties_index]->enb_ipv4_address_for_S1_MME, "BAD IP ADDRESS FORMAT FOR eNB S1_MME !\n" );
                          }
                      }
                  }
                  enb_properties_index += 1;
                  break;
              }
          }
      }
  }
  enb_properties.number = num_enb_properties;

  AssertError (enb_properties_index == num_enb_properties, parse_errors ++,
          "Failed to parse eNB configuration file %s, mismatch between %u active eNBs and %u corresponding defined eNBs !\n",
          lib_config_file_name_pP, num_enb_properties, enb_properties_index);

  AssertFatal (parse_errors == 0,
               "Failed to parse eNB configuration file %s, found %d error%s !\n",
               lib_config_file_name_pP, parse_errors, parse_errors > 1 ? "s" : "");
  enb_config_display();
  return &enb_properties;

}

const Enb_properties_array_t *enb_config_get(void) {
    return &enb_properties;
}

