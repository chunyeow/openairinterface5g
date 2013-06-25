/***************************************************************************
                          rrc_utilities.c  -
                          -------------------
    copyright            : (C) 2002, 2010 by Eurecom
    created by           : michelle.wetterwald@eurecom.fr
 **************************************************************************
    Utilities: print buffer, measures ...
 ***************************************************************************/
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"

/********************
// RRC definitions
 ********************/
#ifdef NODE_MT
#include "rrc_ue_vars.h"
#endif
#ifdef NODE_RG
#include "rrc_rg_vars.h"
#endif
#include "rrc_nas_sap.h"
#include "rrc_msg_constant.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_int.h"
//-----------------------------------------------------------------------------
#ifdef USER_MODE
#include <sys/stat.h>
#include <errno.h>
//#include <fcntl.h>
#endif

#ifdef USER_MODE
//-----------------------------------------------------------------------------
void rrc_create_fifo (char *rrc_fifo){
//-----------------------------------------------------------------------------
  mode_t mode_RWXall = S_IRWXU | S_IRWXG | S_IRWXO;
  if (mkfifo (rrc_fifo, mode_RWXall) && (errno != EEXIST)) {
    msg ("Impossible to create %s\n", rrc_fifo);
  } else {
    msg ("Creation %s\n", rrc_fifo);
  }
}
#endif

//-----------------------------------------------------------------------------
// Print the content of a buffer in hexadecimal
void rrc_print_buffer (char *buffer, int length){
//-----------------------------------------------------------------------------
#define DEBUG_RRC_DETAILS
#ifdef DEBUG_RRC_DETAILS
  int i;
  int used_length;
  if (length > 1000) {
    used_length = 1000;
    msg ("[RRC-PRINT] buffer length %d too large, restricted to 1000 first bytes.\n", length);
  } else {
    used_length = length;
  }
  msg ("\n[RRC-PRINT] Buffer content: ");
  for (i = 0; i < used_length; i++)
    msg ("-%hx-", (unsigned char) buffer[i]);
  msg (",\t total length %d\n\n", length);
#endif
}

/**
 * @brief Print out the name of the message 
 */
//-----------------------------------------------------------------------------
void rrc_mbms_mcch_message_name_print(int mcchMsgType){
//-----------------------------------------------------------------------------
  switch (mcchMsgType) {
    case MCCH_mbmsAccessInformation:
      msg("AccessInformation"); break;
    case MCCH_mbmsCommonPTMRBInformation:
      msg("CommonPTMRBInformation"); break;
    case MCCH_mbmsCurrentCellPTMRBInformation:
      msg("CurrentCellPTMRBInformation"); break;
    case MCCH_mbmsGeneralInformation:
      msg("GeneralInformation"); break;
    case MCCH_mbmsModifiedServicesInformation:
      msg("ModifiedServicesInformation"); break;
    case MCCH_mbmsNeighbouringCellPTMRBInformation:
      msg("NeighbouringCellPTMRBInformation"); break;
    case MCCH_mbmsUnmodifiedServicesInformation:
      msg("UnmodifiedServicesInformation"); break;
  }
}

/**
 * @brief find the serivice id in the service list 
 * @param services the service list 
 * @param numService the number of services in the list
 * @param serviceID service id need to find
 * @return the index in the list
 */
//-----------------------------------------------------------------------------
int rrc_mbms_service_id_find(u8 numService, void * services, int serviceID){
 //-----------------------------------------------------------------------------
  int index;

  for (index = 0; index < numService; index ++){
    if (((o3*) services)[index] == (o3) serviceID)
      break;
  }
  return index <  numService ? index: -1;
}


#ifdef NODE_MT
//-----------------------------------------------------------------------------
void rrc_ue_print_meas_control (struct rrc_ue_meas_cmd *p){
//-----------------------------------------------------------------------------
  int i = 0;
  char *command[3]    = { "Setup", "Modify", "Release" };
  char *transf[2]     = { "RLC AM", "RLC UM" };
  char *trigger[2]    = { "Periodical", "Event trigger" };
  char *bool[2]       = { "NO", "YES" };
  char *validity[3]   = { "cell_DCH", "all_But_Cell_DCH", "all_States" };
  char *uequantity[2] = { "UE Transmitted Power", "UTRA carrier RSSI" };

  msg ("\n[RRC] ******************    Measurement Control Message   ******************\n");
  msg ("[RRC] System Measurement Time : %d\n", protocol_ms->rrc.current_SFN);
  msg ("[RRC] Measurement Identity: %d\t Command: %s\t Transfer Mode: %s\t Reporting Mode: %s\n", p->identity, command[p->command - 1], transf[p->xfer_mode], trigger[p->trigger]);

  if (p->command == MC_release)
    return;

  switch (p->type) {
      // Intra Frequency Measurement
    case MT_intraFrequencyMeasurement:
      msg ("\n[RRC] Measurement type: Intra Frequency Measurement\n");
      msg ("[RRC] Measurement quantity-> Filter coefficient : %i, \tState Validity : %s\n", p->if_coeff, validity[p->if_validity]);
      msg ("[RRC] Reporting quantities : \tCell identity : %s\tTimeslot ISCP : %s\tP-CCPCH RSCP : %s\tPathloss : %s\n",
            bool[p->if_cellIdentity_ri], bool[p->if_timeslotISCP_ri], bool[p->if_BCH_RSCP_ri], bool[p->if_pathloss_ri]);
      break;
      // Traffic Volume Measurement
    case MT_trafficVolumeMeasurement:
      msg ("\n[RRC] Measurement type: Traffic Volume Measurement\n");
      msg ("[RRC] State Validity : %s, \tPayload : %s, \tPayload Average : %s, \tPayload Variance : %s\n",
            validity[p->tv_validity], bool[p->tv_payload_ri], bool[p->tv_average_ri], bool[p->tv_variance_ri]);
      break;
      // Quality Measurement
    case MT_qualityMeasurement:
      msg ("\n[RRC] Measurement type: Quality Measurement\n");
      msg ("[RRC] Measurement quantity-> TRCH BLER : %s, SIR for TFCS : ", bool[p->q_dl_trch_bler_ri]);
      for (i = 0; i < MAXMEASTFCS; i++)
        msg ("%d\t", p->q_sir_TFCSid[i]);
      msg ("\n");
      break;

      // UE Internal Measurement
    case MT_ue_InternalMeasurement:
      msg ("\n[RRC] Measurement type: UE Internal Measurement\n");
      msg ("[RRC] Measurement quantity-> Filter coefficient : %i, \tMeasured Quantity : %s\n", p->int_coeff, uequantity[p->int_quantity]);
      msg ("[RRC] Reporting quantities : \tUE Transmitted Power : %s\tApplied Timing Advance : %s\n", bool[p->int_rep_ue_TransmittedPower], bool[p->int_rep_appliedTA]);
      break;
  }
  //
  switch (p->criteria_type) {
    case RC_ThresholdReportingCriteria:
      msg ("\n[RRC] Report criteria: Threshold reporting -- FFS\n");
      break;
    case RC_PeriodicalReportingCriteria:
      msg ("\n[RRC] Report criteria: Periodical Reporting\tAmount of reporting : %d  \tReporting interval : %i\n", p->rep_amount, p->rep_interval);
      break;
    case RC_NoReporting:
      msg ("\n[RRC] Report criteria: No reporting\n");
  }
  //
  msg ("[RRC] *******END********    Measurement Control Message   ****\n\n");
}
#endif

#ifdef NODE_RG
//-----------------------------------------------------------------------------
void rrc_rg_print_meas_report (struct rrc_rg_mt_meas_rep *p){
//-----------------------------------------------------------------------------
#ifdef DEBUG_RRC_MEASURE_REPORT
  int i, j;
  char *payload[20] =
    { "pl0", "pl4", "pl8", "pl16", "pl32", "pl64", "pl128", "pl256", "pl512", "pl1024", "pl2k", "pl4k", "pl8k", "pl16k", "pl32k", "pl64k", "pl128k", "pl256k", "pl512k", "pl1024k" };
  char *average[20] =
    { "pla0", "pla4", "pla8", "pla16", "pla32", "pla64", "pla128", "pla256", "pla512", "pla1024", "pla2k", 
      "pla4k", "pla8k", "pla16k", "pla32k", "pla64k", "pla128k", "pla256k", "pla512k","pla1024k" };
  char *variance[14] = { "plv0", "plv4", "plv8", "plv16", "plv32", "plv64", "plv128", "plv256", "plv512", "plv1024", "plv2k", "plv4k", "plv8k", "plv16k" };


  msg ("\n[RRC] **************    Measurement Report Message   *****************\n");
  msg ("[RRC] Measurement Time: %d\n", protocol_bs->rrc.current_SFN);
  msg ("[RRC] Measurement Identity: %d\t Measured Results type: ", p->identity);
  switch (p->meas_results_type) {
      case MR_intraFreqMeasuredResultsList:
        msg ("Intra Frequency Measurement\t Number of cells: %d\n", p->if_num_cells);
        for (i = 0; i < p->if_num_cells; i++) {
          msg ("[RRC] Cell number: %d\tCell Identity: %d\tCell Parameters Id: %d\tP-CCPCH RSCP: %d\tPathloss: %d\n", i, p->if_cell_id[i], p->if_cell_parms_id[i], p->if_BCH_RSCP[i], p->if_pathloss[i]);
          msg ("\tTimeslot ISCP for each slot:\t");
          for (j = 0; j < JRRM_SLOTS_PER_FRAME; j++)
            msg ("%d: %d, ", j, p->if_slot_iscp[i][j]);
          msg ("\n");
        }
        break;
        // u16 tv_num_rbs;
        // u16 tv_rbid[MAXMEASRBS];
        // u16 tv_rb_payload[MAXMEASRBS];
        // u16 tv_rb_average[MAXMEASRBS];
        // u16 tv_rb_variance[MAXMEASRBS];
      case MR_trafficVolumeMeasuredResultsList:
        msg ("Traffic Volume Measurement -\t Number of RBs: %d\n", p->tv_num_rbs);
        for (i = 0; i < p->tv_num_rbs; i++)
          msg ("[RRC] RB_Id : %d\t, RLC_BufferPayload : %s\t, Average : %s\t, Variance : %s\n",
               p->tv_rbid[i], payload[p->tv_rb_payload[i]], average[p->tv_rb_average[i]], variance[p->tv_rb_variance[i]]);
        break;
        // u16 q_num_TrCH;
        // u16 q_dl_TrCH_id[MAXMEASTRCH];
        // u16 q_dl_TrCH_BLER[MAXMEASTRCH];
        // u16 q_num_tfcs;
        // u16 q_tfcs_id[MAXMEASTFCS];
        // u16 q_sir[MAXMEASTFCS][JRRM_SLOTS_PER_FRAME];
      case MR_qualityMeasuredResults:
        msg ("Quality Measurement -\t Number of Transport Channels : %d, Number of TFCS : %d\n", p->q_num_TrCH, p->q_num_tfcs);
        msg ("[RRC] BLER for each Transport Channel:\t");
        for (i = 0; i < p->q_num_TrCH; i++)
          msg ("%d: %d, ", p->q_dl_TrCH_id[i], p->q_dl_TrCH_BLER[i]);
        msg ("\n");
        msg ("[RRC] SIR for each TFCS for each slot:\n");
        for (i = 0; i < p->q_num_tfcs; i++) {
          msg ("%d: ", p->q_tfcs_id[i]);
          for (j = 0; j < JRRM_SLOTS_PER_FRAME; j++)
            msg ("%d, ", p->q_sir[i][j]);
          msg ("\n");
        }
        break;
      case MR_ue_InternalMeasuredResults:
        msg ("UE internal Measurement\t Timing Advance: %d\n", p->int_timing_advance);
        msg ("[RRC] Transmitted Power for each slot:\t");
        for (j = 0; j < JRRM_SLOTS_PER_FRAME; j++)
          msg ("%d: %d, ", j, p->int_xmit_power[j]);
        msg ("\n");
        break;
  }
  msg ("[RRC] *****END*********    Measurement Report Message   *******\n\n");
#endif
}

//-----------------------------------------------------------------------------
void rrc_rg_print_meas_bs_control (struct rrc_rg_bs_meas_cmd *p){
//-----------------------------------------------------------------------------
    // u16 identity;
    // u16 command;
    // u16 trigger;
    // u16 type;
    // u16 criteria_type;
    // u16 rep_amount;
    // u16 rep_interval;
#ifdef DEBUG_RRC_MEASURE_REPORT
  char *command[3] = { "Setup", "Modify", "Release" };
  char *trigger[2] = { "Periodical", "Event trigger" };
  char *type[3] = { "BS internal", "Traffic Volume", "Quality" };

  msg ("\n[RRC] ******************   BS Measurement Control Message   ******************\n");
  msg ("[RRC] System Measurement Time : %d\n", protocol_bs->rrc.current_SFN);
  msg ("[RRC] Measurement Identity: %d\t Command: %s\t ", p->identity, command[p->command - 1]);

  if (p->type != IX_ibsM)
    msg ("Mobile Id: %d\t", p->UE_Id);

  if (p->command == MC_release) {
    msg ("\n");
    return;
  }

  msg ("Reporting Mode: %s\n", trigger[p->trigger]);
  msg ("\n[RRC] Measurement type: %s Measurement\n", type[p->type]);
  //
  switch (p->criteria_type) {
    case RC_ThresholdReportingCriteria:
      msg ("[RRC] Report criteria: Threshold reporting -- FFS\n");
      break;
    case RC_PeriodicalReportingCriteria:
      msg ("[RRC] Report criteria: Periodical Reporting\tAmount of reporting : %d  \tReporting interval : %i\n", p->rep_amount, p->rep_interval);
      break;
    case RC_NoReporting:
      msg ("[RRC] Report criteria: No reporting\n");
  }
  //
  msg ("[RRC] *******END********    BS Measurement Control Message   ****\n\n");
#endif
}

//-----------------------------------------------------------------------------
void rrc_rg_print_bs_meas_report (struct rrc_rg_bs_meas_rep *p){
//-----------------------------------------------------------------------------
#ifdef DEBUG_RRC_MEASURE_REPORT
  int i, j;
  char *payload[20] =
    { "pl0", "pl4", "pl8", "pl16", "pl32", "pl64", "pl128", "pl256", "pl512", "pl1024", "pl2k", "pl4k", "pl8k", "pl16k", "pl32k", "pl64k", "pl128k", "pl256k", "pl512k", "pl1024k" };
  char *average[20] =
    { "pla0", "pla4", "pla8", "pla16", "pla32", "pla64", "pla128", "pla256", "pla512", "pla1024", "pla2k", 
      "pla4k", "pla8k", "pla16k", "pla32k", "pla64k", "pla128k", "pla256k", "pla512k", "pla1024k" };
  char *variance[14] = { "plv0", "plv4", "plv8", "plv16", "plv32", "plv64", "plv128", "plv256", "plv512", "plv1024", "plv2k", "plv4k", "plv8k", "plv16k" };


  msg ("\n[RRC] **************    Measurement Report Message   *****************\n");
  msg ("[RRC] Measurement Identity: %d\t Measured Results type: ", p->identity);
  switch (p->meas_results_type) {
      case IX_tvbM:
//    u16 tv_num_rbs;
//    u16 tv_rbid[MAXMEASRBS];
//    u16 tv_rb_payload[MAXMEASRBS];
//    u16 tv_rb_average[MAXMEASRBS];
//    u16 tv_rb_variance[MAXMEASRBS];
        msg ("Traffic Volume Measurement -\t Number of RBs: %d\n", p->tv_num_rbs);
        for (i = 0; i < p->tv_num_rbs; i++)
          msg ("[RRC] RB_Id : %d\t, RLC_BufferPayload : %s\t, Average : %s\t, Variance : %s\n",
               p->tv_rbid[i], payload[p->tv_rb_payload[i]], average[p->tv_rb_average[i]], variance[p->tv_rb_variance[i]]);
        break;
      case IX_qbM:
//              u16 q_num_TrCH;
//    u16 q_dl_TrCH_id[MAXMEASTRCH];
//    u16 q_dl_TrCH_BLER[MAXMEASTRCH];
//    u16 q_num_tfcs;
//    u16 q_tfcs_id[MAXMEASTFCS];
//              u16 q_sir[MAXMEASTFCS][JRRM_SLOTS_PER_FRAME];
        msg ("Quality Measurement -\t Number of Transport Channels : %d, Number of TFCS : %d\n", p->q_num_TrCH, p->q_num_tfcs);
        msg ("[RRC] BLER for each Transport Channel:\t");
        for (i = 0; i < p->q_num_TrCH; i++)
          msg ("%d: %d, ", p->q_dl_TrCH_id[i], p->q_dl_TrCH_BLER[i]);
        msg ("\n");
        msg ("[RRC] SIR for each TFCS for each slot:\n");
        for (i = 0; i < p->q_num_tfcs; i++) {
          msg ("%d: ", p->q_tfcs_id[i]);
          for (j = 0; j < JRRM_SLOTS_PER_FRAME; j++)
            msg ("%d, ", p->q_sir[i][j]);
          msg ("\n");
        }
        break;
      case IX_ibsM:
//    u16 int_xmit_power[numANTENNAS][JRRM_SLOTS_PER_FRAME];
//    u16 int_rscp[JRRM_SLOTS_PER_FRAME][MAXCH];
//    u16 int_rssi_rf[numANTENNAS][JRRM_SLOTS_PER_FRAME];
//    u16 int_spare[NUMSPARE];
        msg ("BS internal Measurement\n");
        for (i = 0; i < numANTENNAS; i++) {
          msg ("[RRC] Transmitted Power for each slot on antenna %d :  ", i);
          for (j = 0; j < JRRM_SLOTS_PER_FRAME; j++)
            msg ("%d: %d, ", j, p->int_xmit_power[i][j]);
          msg ("\n");
        }
        for (i = 0; i < JRRM_SLOTS_PER_FRAME; i++) {
          msg ("[RRC] RSCP for each channel on slot %d :  ", i);
          for (j = 0; j < MAXCH; j++)
            msg ("%d: %d, ", j, p->int_rscp[i][j]);
          msg ("\n");
        }
        for (i = 0; i < numANTENNAS; i++) {
          msg ("[RRC] RSSI RF for each slot on antenna %d :  ", i);
          for (j = 0; j < JRRM_SLOTS_PER_FRAME; j++)
            msg ("%d: %d, ", j, p->int_rssi_rf[i][j]);
          msg ("\n");
        }
        for (i = 0; i < NUMSPARE; i++) {
          msg ("[RRC] Spare value : %d: %d\n", i, p->int_spare[i]);
        }

        break;
  }
  msg ("[RRC] *****END*********    Measurement Report Message   *******\n\n");
#endif
}

#endif
