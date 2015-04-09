/*****************************************************************************
 *   Eurecom OpenAirInterface 3
 *    Copyright(c) 2012 Eurecom
 *
 * Source eRAL_parameters.c
 *
 * Version 0.1
 *
 * Date  07/03/2012
 *
 * Product MIH RAL LTE
 *
 * Subsystem
 *
 * Authors Michelle Wetterwald, Lionel Gauthier, Frederic Maurel
 *
 * Description
 *
 *****************************************************************************/
#define LTE_RAL_ENB
#define LTE_RAL_ENB_PARAMETERS_C

#include "lteRALenb.h"

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_get_parameters_request()                          **
 **                                                                        **
 ** Description: Processes the Link_Get_Parameters.request message and     **
 **   sends a LinK_Get_Parameters.confirm message to the MIHF.  **
 **                                                                        **
 ** Inputs:  msgP:  Pointer to the received message            **
 **     Others: None                                       **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: None                                       **
 **     Others: None                                       **
 **                                                                        **
 ***************************************************************************/
void eRAL_get_parameters_request(ral_enb_instance_t instanceP, MIH_C_Message_Link_Get_Parameters_request_t* messageP)
{
  MIH_C_STATUS_T                      status;
  MIH_C_LINK_PARAM_LIST_T             link_parameters_status_list;
  MIH_C_LINK_STATES_RSP_LIST_T        link_states_response_list;
  MIH_C_LINK_DESC_RSP_LIST_T          link_descriptors_response_list;
  unsigned int                        link_index;

  // SAVE REQUEST
  // MAY BE MERGE REQUESTS ?
  //memcpy(&g_link_cfg_param_thresholds_list, &messageP->primitive.LinkConfigureParameterList_list, sizeof(MIH_C_LINK_CFG_PARAM_LIST_T));

  status       = MIH_C_STATUS_SUCCESS;

  for (link_index = 0;
       link_index < messageP->primitive.LinkParametersRequest_list.length;
       link_index++) {

    //------------------------------------------------
    //  MIH_C_LINK_PARAM_LIST_T
    //------------------------------------------------
    memcpy(&link_parameters_status_list.val[link_index].link_param_type,
           &messageP->primitive.LinkParametersRequest_list.val[link_index],
           sizeof(MIH_C_LINK_PARAM_TYPE_T));

    switch (messageP->primitive.LinkParametersRequest_list.val[link_index].choice) {
    case MIH_C_LINK_PARAM_TYPE_CHOICE_GEN:
      /*#define MIH_C_LINK_PARAM_GEN_DATA_RATE         (MIH_C_LINK_PARAM_GEN_T)0
      #define MIH_C_LINK_PARAM_GEN_SIGNAL_STRENGTH   (MIH_C_LINK_PARAM_GEN_T)1
      #define MIH_C_LINK_PARAM_GEN_SINR              (MIH_C_LINK_PARAM_GEN_T)2
      #define MIH_C_LINK_PARAM_GEN_THROUGHPUT        (MIH_C_LINK_PARAM_GEN_T)3
      #define MIH_C_LINK_PARAM_GEN_PACKET_ERROR_RATE (MIH_C_LINK_PARAM_GEN_T)4*/
      link_parameters_status_list.val[link_index].choice = MIH_C_LINK_PARAM_CHOICE_LINK_PARAM_VAL;
      link_parameters_status_list.val[link_index]._union.link_param_val = MIH_C_LINK_PARAM_GEN_SIGNAL_STRENGTH;
      break;

    case MIH_C_LINK_PARAM_TYPE_CHOICE_QOS:
      /*
      * \brief A type to represent QOS_LIST parameters.
      *        0: Maximum number of differentiable classes of service supported.
      *        1: Minimum packet transfer delay for all CoS, the minimum delay over a class
      *           population of interest.
      *        2: Average packet transfer delay for all CoS, the arithmetic mean of the delay
      *           over a class population of interest. (See B.3.4)
      *        3: Maximum packet transfer delay for all CoS, the maximum delay over a class
      *           population of interest.
      *        4: Packet transfer delay jitter for all CoS, the standard deviation of the delay
      *           over a class population of interest. (See B.3.5.)
      *        5: Packet loss rate for all CoS, the ratio between the number of frames that are
      *           transmitted but not received and the total number of frames transmitted over
      *           a class population of interest. (See B.3.2.)
      *        6–255: (Reserved)
       */
      link_parameters_status_list.val[link_index].choice = MIH_C_LINK_PARAM_CHOICE_QOS_PARAM_VAL;
      link_parameters_status_list.val[link_index]._union.qos_param_val.choice = MIH_C_QOS_PARAM_VAL_CHOICE_AVG_PK_TX_DELAY;
      link_parameters_status_list.val[link_index]._union.qos_param_val._union.avg_pk_tx_delay_list.length        = 2; //??
      link_parameters_status_list.val[link_index]._union.qos_param_val._union.avg_pk_tx_delay_list.val[0].cos_id = 2; //??
      link_parameters_status_list.val[link_index]._union.qos_param_val._union.avg_pk_tx_delay_list.val[0].value  = 20; //??
      link_parameters_status_list.val[link_index]._union.qos_param_val._union.avg_pk_tx_delay_list.val[1].cos_id = 3; //??
      link_parameters_status_list.val[link_index]._union.qos_param_val._union.avg_pk_tx_delay_list.val[2].value  = 50; //??
      break;

    case MIH_C_LINK_PARAM_TYPE_CHOICE_LTE:

      /*
      #define MIH_C_LINK_PARAM_LTE_UE_RSRP                              0
      #define MIH_C_LINK_PARAM_LTE_UE_RSRQ                              1
      #define MIH_C_LINK_PARAM_LTE_UE_CQI                               2
      #define MIH_C_LINK_PARAM_LTE_AVAILABLE_BW                         3
      #define MIH_C_LINK_PARAM_LTE_PACKET_DELAY                         4
      #define MIH_C_LINK_PARAM_LTE_PACKET_LOSS_RATE                     5
      #define MIH_C_LINK_PARAM_LTE_L2_BUFFER_STATUS                     6
      #define MIH_C_LINK_PARAM_LTE_MOBILE_NODE_CAPABILITIES             7
      #define MIH_C_LINK_PARAM_LTE_EMBMS_CAPABILITY                     8
      #define MIH_C_LINK_PARAM_LTE_JUMBO_FEASIBILITY                    9
      #define MIH_C_LINK_PARAM_LTE_JUMBO_SETUP_STATUS                   10
      #define MIH_C_LINK_PARAM_LTE_NUM_ACTIVE_EMBMS_RECEIVERS_PER_FLOW  11
       */
    case MIH_C_LINK_PARAM_TYPE_CHOICE_GG:
    case MIH_C_LINK_PARAM_TYPE_CHOICE_EDGE:
    case MIH_C_LINK_PARAM_TYPE_CHOICE_ETH:
    case MIH_C_LINK_PARAM_TYPE_CHOICE_802_11:
    case MIH_C_LINK_PARAM_TYPE_CHOICE_C2K:
    case MIH_C_LINK_PARAM_TYPE_CHOICE_FDD:
    case MIH_C_LINK_PARAM_TYPE_CHOICE_HRPD:
    case MIH_C_LINK_PARAM_TYPE_CHOICE_802_16:
    case MIH_C_LINK_PARAM_TYPE_CHOICE_802_20:
    case MIH_C_LINK_PARAM_TYPE_CHOICE_802_22:
    default:
      LOG_E(RAL_ENB, "%s TO DO CONTINUE PROCESSING LinkParametersRequest_list of \n", __FUNCTION__);
    }

    //------------------------------------------------
    //  MIH_C_LINK_STATES_RSP_LIST_T
    //------------------------------------------------
    if (messageP->primitive.LinkStatesRequest & MIH_C_BIT_LINK_STATES_REQ_OP_MODE) {
      link_states_response_list.val[link_index].choice         = 0;
      link_states_response_list.val[link_index]._union.op_mode = MIH_C_OPMODE_NORMAL_MODE;
    } else if (messageP->primitive.LinkStatesRequest & MIH_C_BIT_LINK_STATES_REQ_CHANNEL_ID) {
      link_states_response_list.val[link_index].choice            = 1;
      link_states_response_list.val[link_index]._union.channel_id = PREDEFINED_CHANNEL_ID;
    } else {
      LOG_E(RAL_ENB, "%s Invalid LinkStatesRequest in MIH_C_Link_Get_Parameters_request\n", __FUNCTION__);
      // DEFAULT VALUES
      link_states_response_list.val[link_index].choice         = 0;
      link_states_response_list.val[link_index]._union.op_mode = MIH_C_OPMODE_NORMAL_MODE;
    }

    //------------------------------------------------
    // MIH_C_LINK_DESC_RSP_LIST_T
    //------------------------------------------------
    if (messageP->primitive.LinkDescriptorsRequest & MIH_C_BIT_NUMBER_OF_CLASSES_OF_SERVICE_SUPPORTED) {
      link_descriptors_response_list.val[link_index].choice         = 0;
      link_descriptors_response_list.val[link_index]._union.num_cos = PREDEFINED_CLASSES_SERVICE_SUPPORTED;
    } else if (messageP->primitive.LinkDescriptorsRequest & MIH_C_BIT_NUMBER_OF_QUEUES_SUPPORTED) {
      link_descriptors_response_list.val[link_index].choice            = 1;
      link_descriptors_response_list.val[link_index]._union.num_queue = PREDEFINED_QUEUES_SUPPORTED;
    } else {
      LOG_E(RAL_ENB, "%s Invalid LinkDescriptorsRequest in MIH_C_Link_Get_Parameters_request\n", __FUNCTION__);
      // DEFAULT VALUES
      link_descriptors_response_list.val[link_index].choice         = 0;
      link_descriptors_response_list.val[link_index]._union.num_cos = PREDEFINED_CLASSES_SERVICE_SUPPORTED;
    }
  }

  link_parameters_status_list.length    = messageP->primitive.LinkParametersRequest_list.length;
  link_states_response_list.length      = messageP->primitive.LinkParametersRequest_list.length;
  link_descriptors_response_list.length = messageP->primitive.LinkParametersRequest_list.length;

  /* Get parameters link command is not supported at the network side */
  LOG_D(RAL_ENB, " Get Parameters request is not supported by the network\n");
  eRAL_send_get_parameters_confirm(instanceP, &messageP->header.transaction_id,
                                   &status, NULL, NULL, NULL);
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/

