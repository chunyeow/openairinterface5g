/***************************************************************************
                          rrc_util_config.c  -
                          -------------------
    copyright            : (C) 2002, 2010 by Eurecom
    created by           : michelle.wetterwald@eurecom.fr
                           raymond.knopp@eurecom.fr
                           lionel.gauthier@eurecom.fr
 **************************************************************************
    Utilitiesfor config compression ...
    WARNING 08/07/2010 : NOT Compiled yet - rrc_xx_dummies.c is used instead
 ***************************************************************************/
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"

/********************
// RRC definitions
 ********************/
#include "rrc_ue_vars.h"
#include "rrc_nas_sap.h"
#include "rrc_msg_constant.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_int.h"


//-----------------------------------------------------------------------------
//   WARNING 08/07/2010 : NOT Compiled yet - rrc_xx_dummies.c is used instead
void rrc_compress_config (MT_CONFIG * config, char *dest_buffer, int *dest_length){
//-----------------------------------------------------------------------------
  int             i, j, k, el;
  char           *outbuf = dest_buffer;

#ifdef DEBUG_LOAD_CONFIG
  msg ("[RRC][CONFIG][COMPRESS] config size = %d -> %p\n", sizeof (MT_CONFIG), dest_buffer);
#endif
  *outbuf++ = config->nb_commands;

  memcpy (outbuf, &config->rrm_commands[0], config->nb_commands * sizeof (RRM_COMMAND_MT));

  outbuf += config->nb_commands * sizeof (RRM_COMMAND_MT);

  for (i = 0; i < JRRM_MAX_COMMANDS_PER_TRANSACTION; i++) {
    el = config->rrm_commands[i].rrm_element_index;
    switch (config->rrm_commands[i].rrm_action) {
        case ACTION_ADD:
        case ACTION_MODIFY:
#ifdef DEBUG_LOAD_CONFIG
          msg ("[RRC][CONFIG][COMPRESS] Command %d : action %d element %d index %d\n", i, config->rrm_commands[i].rrm_action, config->rrm_commands[i].rrm_element, el);
#endif

          switch (config->rrm_commands[i].rrm_element) {
                //-------------------------------------------
              case SIGNALLING_RADIO_BEARER:
              case RADIO_ACCESS_BEARER:
                //-------------------------------------------
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS]  RB (%d)\n", (int) outbuf - (int) dest_buffer);
#endif
                memcpy (outbuf, (void *) &config->bearer_dl[el], sizeof (RADIOBEARER));
                outbuf += sizeof (RADIOBEARER);

                memcpy (outbuf, (void *) &config->bearer_ul[el], sizeof (RADIOBEARER));
                outbuf += sizeof (RADIOBEARER);
                break;
                //-------------------------------------------
              case TRCH_UL:
                //-------------------------------------------
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS] TRCH_UL (%d)\n", (int) outbuf - (int) dest_buffer);
#endif
                memcpy ((void *) outbuf, (void *) &config->trch_ul[el].peer_id, sizeof (u8));
                outbuf += sizeof (u8);
                memcpy ((void *) outbuf, (void *) &config->trch_ul[el].type, sizeof (u8));
                outbuf += sizeof (u8);
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS]  TRCH_TYPE = %d (%d)\n", config->trch_ul[el].type, (int) outbuf - (int) dest_buffer);
#endif

                *outbuf++ = config->trch_ul[el].cctrch;
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS]  TRCH_CCTRCH = %d (%d)\n", config->trch_ul[el].cctrch, (int) outbuf - (int) dest_buffer);
#endif
                *outbuf++ = config->trch_ul[el].tti;
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS]  TRCH_TTI = %d (%d)\n", config->trch_ul[el].tti, (int) outbuf - (int) dest_buffer);
#endif
                *outbuf++ = config->trch_ul[el].code;
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS]  TRCH_CODE = %d (%d)\n", config->trch_ul[el].code, (int) outbuf - (int) dest_buffer);
#endif

                memcpy ((void *) outbuf, (void *) &config->trch_ul[el].num_tf, sizeof (u8));
                outbuf += sizeof (u8);
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS]  TRCH_NUM_TF = %d (%d)\n", config->trch_ul[el].num_tf, (int) outbuf - (int) dest_buffer);
#endif

                memcpy (outbuf, (void *) &config->trch_ul[el].tf, config->trch_ul[el].num_tf * sizeof (TF));
                outbuf += config->trch_ul[el].num_tf * sizeof (TF);
                break;
                //-------------------------------------------
              case TRCH_DL:
                //-------------------------------------------
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS] TRCH_DL (%d)\n", (int) outbuf - (int) dest_buffer);
#endif
                memcpy (outbuf, (void *) &config->trch_dl[el].peer_id, sizeof (u8));
                outbuf += sizeof (u8);
                memcpy (outbuf, (void *) &config->trch_dl[el].type, sizeof (u8));
                outbuf += sizeof (u8);
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS]  TRCH_TYPE = %d (%d)\n", config->trch_dl[el].type, (int) outbuf - (int) dest_buffer);
#endif

                *outbuf++ = config->trch_dl[el].cctrch;
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS]  TRCH_CCTRCH = %d (%d)\n", config->trch_dl[el].cctrch, (int) outbuf - (int) dest_buffer);
#endif
                *outbuf++ = config->trch_dl[el].tti;
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS]  TRCH_TTI = %d (%d)\n", config->trch_dl[el].tti, (int) outbuf - (int) dest_buffer);
#endif
                *outbuf++ = config->trch_dl[el].code;
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS]  TRCH_CODE = %d (%d)\n", config->trch_dl[el].code, (int) outbuf - (int) dest_buffer);
#endif

                memcpy (outbuf, (void *) &config->trch_dl[el].num_tf, sizeof (u8));
                outbuf += sizeof (u8);
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS]  TRCH_NUM_TF = %d (%d)\n", config->trch_dl[el].num_tf, (int) outbuf - (int) dest_buffer);
#endif

                memcpy (outbuf, (void *) &config->trch_dl[el].tf, config->trch_dl[el].num_tf * sizeof (TF));
                outbuf += config->trch_dl[el].num_tf * sizeof (TF);
                break;
                //-------------------------------------------
              case CCTRCH_UL:
                //-------------------------------------------
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS] CCTRCH_UL (%d,%d)\n", (int) outbuf - (int) dest_buffer, (int) &config->cctrch_ul[el] - (int) config);
#endif
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS] num tfc %d\n", config->cctrch_ul[el].tfcs.num_tfc);
#endif
                memcpy (outbuf, (void *) &config->cctrch_ul[el].tfcs.num_tfc, sizeof (u8));
                outbuf += sizeof (u8);
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS] num trch %d\n", config->cctrch_ul[el].num_trch);
#endif
                memcpy (outbuf, (void *) &config->cctrch_ul[el].num_trch, sizeof (u8));
                outbuf += sizeof (u8);

                for (j = 0; j < config->cctrch_ul[el].tfcs.num_tfc; j++)
                  for (k = 0; k < config->cctrch_ul[el].num_trch; k++) {
#ifdef DEBUG_LOAD_CONFIG
                    msg ("[RRC][CONFIG][COMPRESS] tfc %d %d\n", j, k);
#endif
                    *outbuf++ = config->cctrch_ul[el].tfcs.tfc[j][k];
                  }

                memcpy (outbuf, (void *) &config->cctrch_ul[el].trch_id, config->cctrch_ul[el].num_trch);
                outbuf += config->cctrch_ul[el].num_trch;

                *outbuf++ = config->cctrch_ul[el].type;

                *outbuf++ = config->cctrch_ul[el].pl;

                *outbuf++ = config->cctrch_ul[el].num_tfci_bits;

                *outbuf++ = config->cctrch_ul[el].tti;

                *outbuf++ = config->cctrch_ul[el].num_pch;

                memcpy (outbuf, (void *) &config->cctrch_ul[el].pch[0], sizeof (PCH) * config->cctrch_ul[el].num_pch);
                outbuf += sizeof (PCH) * config->cctrch_ul[el].num_pch;

                *outbuf++ = config->cctrch_ul[el].DSP_FLAG;

                memcpy (outbuf, (void *) &config->cctrch_ul[el].gain_adjust, sizeof (u16));
                outbuf += sizeof (u16);

                *outbuf++ = config->cctrch_ul[el].sec_interl;
                break;
                //-------------------------------------------
              case CCTRCH_DL:
                //-------------------------------------------
                msg ("[RRC][CONFIG][COMPRESS] CCTRCH_DL (%d,%d)\n", (int) outbuf - (int) dest_buffer, (int) &config->cctrch_dl[el] - (int) config);
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS] num tfc %d\n", config->cctrch_dl[el].tfcs.num_tfc);
#endif
                memcpy (outbuf, (void *) &config->cctrch_dl[el].tfcs.num_tfc, sizeof (u8));
                outbuf += sizeof (u8);
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][COMPRESS] num trch %d\n", config->cctrch_dl[el].num_trch);
#endif
                memcpy (outbuf, (void *) &config->cctrch_dl[el].num_trch, sizeof (u8));
                outbuf += sizeof (u8);

                for (j = 0; j < config->cctrch_dl[el].tfcs.num_tfc; j++)
                  for (k = 0; k < config->cctrch_dl[el].num_trch; k++) {
                    msg ("[RRC][CONFIG][COMPRESS] tfc %d %d\n", j, k);
                    *outbuf++ = config->cctrch_dl[el].tfcs.tfc[j][k];
                  }

                memcpy (outbuf, (void *) &config->cctrch_dl[el].trch_id, config->cctrch_dl[el].num_trch);
                outbuf += config->cctrch_dl[el].num_trch;

                *outbuf++ = config->cctrch_dl[el].type;

                *outbuf++ = config->cctrch_dl[el].pl;

                *outbuf++ = config->cctrch_dl[el].num_tfci_bits;

                *outbuf++ = config->cctrch_dl[el].tti;

                *outbuf++ = config->cctrch_dl[el].num_pch;

                memcpy (outbuf, (void *) &config->cctrch_dl[el].pch[0], sizeof (PCH) * config->cctrch_dl[el].num_pch);
                outbuf += sizeof (PCH) * config->cctrch_dl[el].num_pch;


                *outbuf++ = config->cctrch_dl[el].DSP_FLAG;

                memcpy (outbuf, (void *) &config->cctrch_dl[el].gain_adjust, sizeof (u16));
                outbuf += sizeof (u16);

                *outbuf++ = config->cctrch_dl[el].sec_interl;
                break;
          }
          break;
          //-------------------------------------------
        default:
          //-------------------------------------------
          break;
    }

  }
  *dest_length = (int) outbuf - (int) dest_buffer;
  msg ("[RRC][CONFIG][COMPRESS] : Compressed config length = %d end @ %p\n", *dest_length, outbuf);
}

//-----------------------------------------------------------------------------
//   WARNING 08/07/2010 : NOT Compiled yet - rrc_xx_dummies.c is used instead
void rrc_uncompress_config (MT_CONFIG * config, char *source_buffer){
//-----------------------------------------------------------------------------
  int             i, j, k, el;
  char           *inbuf = source_buffer;

  config->nb_commands = *inbuf++;

#ifdef DEBUG_LOAD_CONFIG
  msg ("[RRC][CONFIG][EXPAND] config %p\n", source_buffer);
  msg ("[RRC][CONFIG][EXPAND] RRM_COMMAND_MT = 1\n TRCH_DL = %d \n TRCH_UL = %d\n CCTRCH_DL = %d\n CCTRCH_UL = %d\n RADIOBEARER_DL = %d\n RADIOBEARER_UL = %d\n L3= %d\n\n",
       1 + JRRM_MAX_COMMANDS_PER_TRANSACTION * sizeof (RRM_COMMAND_MT),
       1 + JRRM_MAX_COMMANDS_PER_TRANSACTION * sizeof (RRM_COMMAND_MT) + JRRM_MAX_TRCH_MOBILE * sizeof (TRCH),
       1 + JRRM_MAX_COMMANDS_PER_TRANSACTION * sizeof (RRM_COMMAND_MT) + 2 * JRRM_MAX_TRCH_MOBILE * sizeof (TRCH),
       1 + JRRM_MAX_COMMANDS_PER_TRANSACTION * sizeof (RRM_COMMAND_MT) + 2 * JRRM_MAX_TRCH_MOBILE * sizeof (TRCH) + JRRM_MAX_CCTRCH_MOBILE * sizeof (CCTRCH),
       1 + JRRM_MAX_COMMANDS_PER_TRANSACTION * sizeof (RRM_COMMAND_MT) + 2 * JRRM_MAX_TRCH_MOBILE * sizeof (TRCH) + 2 * JRRM_MAX_CCTRCH_MOBILE * sizeof (CCTRCH),
       1 + JRRM_MAX_COMMANDS_PER_TRANSACTION * sizeof (RRM_COMMAND_MT) + 2 * JRRM_MAX_TRCH_MOBILE * sizeof (TRCH) + 2 * JRRM_MAX_CCTRCH_MOBILE * sizeof (CCTRCH) +
       JRRM_MAX_RB_MOBILE * sizeof (RADIOBEARER),
       1 + JRRM_MAX_COMMANDS_PER_TRANSACTION * sizeof (RRM_COMMAND_MT) + 2 * JRRM_MAX_TRCH_MOBILE * sizeof (TRCH) + 2 * JRRM_MAX_CCTRCH_MOBILE * sizeof (CCTRCH) +
       2 * JRRM_MAX_RB_MOBILE * sizeof (RADIOBEARER));
  msg ("[RRC][CONFIG][EXPAND] nb_commands = %d\n", config->nb_commands);
#endif
  memcpy ((void *) &config->rrm_commands[0], inbuf, config->nb_commands * sizeof (RRM_COMMAND_MT));

  inbuf += config->nb_commands * sizeof (RRM_COMMAND_MT);

  for (i = 0; i < JRRM_MAX_COMMANDS_PER_TRANSACTION; i++) {
    el = config->rrm_commands[i].rrm_element_index;
    switch (config->rrm_commands[i].rrm_action) {

        case ACTION_ADD:
        case ACTION_MODIFY:
#ifdef DEBUG_LOAD_CONFIG
          msg ("[RRC][CONFIG][EXPAND] Command %d : action %d element %d index %d\n", i, config->rrm_commands[i].rrm_action, config->rrm_commands[i].rrm_element, el);
#endif

          switch (config->rrm_commands[i].rrm_element) {
                //-------------------------------------------
              case SIGNALLING_RADIO_BEARER:
              case RADIO_ACCESS_BEARER:
                //-------------------------------------------
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][EXPAND]  RB %d (%d)\n", el, (int) inbuf - (int) source_buffer);
#endif
                memcpy ((void *) &config->bearer_dl[el], inbuf, sizeof (RADIOBEARER));
                inbuf += sizeof (RADIOBEARER);

                memcpy ((void *) &config->bearer_ul[el], inbuf, sizeof (RADIOBEARER));
                inbuf += sizeof (RADIOBEARER);
                break;
                //-------------------------------------------
              case TRCH_UL:
                //-------------------------------------------
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][EXPAND]  TRCH_UL %d (%d)\n", el, (int) inbuf - (int) source_buffer);
#endif
                memcpy ((void *) &config->trch_ul[el].peer_id, inbuf, sizeof (u8));
                inbuf += sizeof (u8);
                memcpy ((void *) &config->trch_ul[el].type, inbuf, sizeof (u8));
                inbuf += sizeof (u8);
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][EXPAND]  TRCH_TYPE = %d (%d)\n", config->trch_ul[el].type, (int) inbuf - (int) source_buffer);
#endif

                config->trch_ul[el].cctrch = *inbuf++;
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][EXPAND]  TRCH_CCTRCH = %d (%d)\n", config->trch_ul[el].cctrch, (int) inbuf - (int) source_buffer);
#endif
                config->trch_ul[el].tti = *inbuf++;
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][EXPAND]  TRCH_TTI = %d (%d)\n", config->trch_ul[el].tti, (int) inbuf - (int) source_buffer);
#endif
                config->trch_ul[el].code = *inbuf++;
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][EXPAND]  TRCH_CODE = %d (%d)\n", config->trch_ul[el].code, (int) inbuf - (int) source_buffer);
#endif

                memcpy ((void *) &config->trch_ul[el].num_tf, inbuf, sizeof (u8));
                inbuf += sizeof (u8);
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][EXPAND]  TRCH_NUM_TF = %d (%d)\n", config->trch_ul[el].num_tf, (int) inbuf - (int) source_buffer);
#endif

                memcpy ((void *) &config->trch_ul[el].tf, inbuf, config->trch_ul[el].num_tf * sizeof (TF));
                inbuf += config->trch_ul[el].num_tf * sizeof (TF);
                break;
                //-------------------------------------------
              case TRCH_DL:
                //-------------------------------------------
                msg ("[RRC][CONFIG][EXPAND]  TRCH_DL %d (%d)\n", el, (int) inbuf - (int) source_buffer);
                memcpy ((void *) &config->trch_dl[el].peer_id, inbuf, sizeof (u8));
                inbuf += sizeof (u8);
                memcpy ((void *) &config->trch_dl[el].type, inbuf, sizeof (u8));
                inbuf += sizeof (u8);
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][EXPAND]  TRCH_TYPE = %d (%d)\n", config->trch_dl[el].type, (int) inbuf - (int) source_buffer);
#endif

                config->trch_dl[el].cctrch = *inbuf++;
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][EXPAND]  TRCH_CCTRCH = %d (%d)\n", config->trch_dl[el].cctrch, (int) inbuf - (int) source_buffer);
#endif
                config->trch_dl[el].tti = *inbuf++;
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][EXPAND]  TRCH_TTI = %d (%d)\n", config->trch_dl[el].tti, (int) inbuf - (int) source_buffer);
#endif
                config->trch_dl[el].code = *inbuf++;
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][EXPAND]  TRCH_CODE = %d (%d)\n", config->trch_dl[el].code, (int) inbuf - (int) source_buffer);
#endif

                memcpy ((void *) &config->trch_dl[el].num_tf, inbuf, sizeof (u8));
                inbuf += sizeof (u8);
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][EXPAND]  TRCH_NUM_TF = %d (%d)\n", config->trch_dl[el].num_tf, (int) inbuf - (int) source_buffer);
#endif

                memcpy ((void *) &config->trch_dl[el].tf, inbuf, config->trch_dl[el].num_tf * sizeof (TF));
                inbuf += config->trch_dl[el].num_tf * sizeof (TF);
                break;
                //-------------------------------------------
              case CCTRCH_UL:
                //-------------------------------------------
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][EXPAND]  CCTRCH_UL %d (%d)\n", el, (int) inbuf - (int) source_buffer);
#endif
                memcpy ((void *) &config->cctrch_ul[el].tfcs.num_tfc, inbuf, sizeof (u8));
                inbuf += sizeof (u8);
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][EXPAND] NumTFC = %d\n", config->cctrch_ul[el].tfcs.num_tfc);
#endif
                memcpy ((void *) &config->cctrch_ul[el].num_trch, inbuf, sizeof (u8));
                inbuf += sizeof (u8);
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][EXPAND] NumTrCh = %d\n", config->cctrch_ul[el].num_trch);
#endif

                for (j = 0; j < config->cctrch_ul[el].tfcs.num_tfc; j++)
                  for (k = 0; k < config->cctrch_ul[el].num_trch; k++) {
#ifdef DEBUG_LOAD_CONFIG
                    msg ("[RRC][CONFIG][EXPAND] tfc %d %d\n", j, k);
#endif
                    config->cctrch_ul[el].tfcs.tfc[j][k] = *inbuf++;
                  }
                memcpy ((void *) &config->cctrch_ul[el].trch_id, inbuf, config->cctrch_ul[el].num_trch);
                inbuf += config->cctrch_ul[el].num_trch;

                config->cctrch_ul[el].type = *inbuf++;

                config->cctrch_ul[el].pl = *inbuf++;

                config->cctrch_ul[el].num_tfci_bits = *inbuf++;

                config->cctrch_ul[el].tti = *inbuf++;

                config->cctrch_ul[el].num_pch = *inbuf++;

                memcpy ((void *) &config->cctrch_ul[el].pch, inbuf, config->cctrch_ul[el].num_pch * sizeof (PCH));
                inbuf += config->cctrch_ul[el].num_pch * sizeof (PCH);

                config->cctrch_ul[el].DSP_FLAG = *inbuf++;

                memcpy ((void *) &config->cctrch_ul[el].gain_adjust, inbuf, sizeof (u16));
                inbuf += sizeof (u16);

                config->cctrch_ul[el].sec_interl = *inbuf++;
                break;
                //-------------------------------------------
              case CCTRCH_DL:
                //-------------------------------------------
#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][EXPAND]  CCTRCH_DL %d (%d)\n", el, (int) inbuf - (int) source_buffer);
#endif
                memcpy ((void *) &config->cctrch_dl[el].tfcs.num_tfc, inbuf, sizeof (u8));
                inbuf += sizeof (u8);

                memcpy ((void *) &config->cctrch_dl[el].num_trch, inbuf, sizeof (u8));
                inbuf += sizeof (u8);

#ifdef DEBUG_LOAD_CONFIG
                msg ("[RRC][CONFIG][EXPAND] NumTrCh = %d\n", config->cctrch_dl[el].num_trch);
#endif

                for (j = 0; j < config->cctrch_dl[el].tfcs.num_tfc; j++)
                  for (k = 0; k < config->cctrch_dl[el].num_trch; k++) {
#ifdef DEBUG_LOAD_CONFIG
                    msg ("[RRC][CONFIG][EXPAND] tfc %d %d\n", j, k);
#endif

                    config->cctrch_dl[el].tfcs.tfc[j][k] = *inbuf++;
                  }

                memcpy ((void *) &config->cctrch_dl[el].trch_id, inbuf, config->cctrch_dl[el].num_trch);
                inbuf += config->cctrch_dl[el].num_trch;

                config->cctrch_dl[el].type = *inbuf++;

                config->cctrch_dl[el].pl = *inbuf++;

                config->cctrch_dl[el].num_tfci_bits = *inbuf++;

                config->cctrch_dl[el].tti = *inbuf++;

                config->cctrch_dl[el].num_pch = *inbuf++;

                memcpy ((void *) &config->cctrch_dl[el].pch, inbuf, config->cctrch_dl[el].num_pch * sizeof (PCH));
                inbuf += config->cctrch_dl[el].num_pch * sizeof (PCH);

                config->cctrch_dl[el].DSP_FLAG = *inbuf++;

                memcpy ((void *) &config->cctrch_dl[el].gain_adjust, inbuf, sizeof (u16));
                inbuf += sizeof (u16);

                config->cctrch_dl[el].sec_interl = *inbuf++;
                break;
          }
          break;
          //-------------------------------------------
        default:
          //-------------------------------------------
          break;
    }
  }
}
