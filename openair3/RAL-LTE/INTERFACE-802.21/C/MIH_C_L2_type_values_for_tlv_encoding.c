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

#define MIH_C_INTERFACE
#define MIH_C_L2_TYPE_VALUES_FOR_TLV_ENCODING_C
#include "MIH_C_L2_type_values_for_tlv_encoding.h"

//-----------------------------------------------------------------------------
unsigned int MIH_C_TLV2String(MIH_C_TLV_TYPE_T *typeP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    switch(*typeP) {
        case MIH_C_TLV_SOURCE_MIHF_ID:                      buffer_index += sprintf(&bufP[buffer_index], "TLV_SOURCE_MIHF_ID");break;
        case MIH_C_TLV_DESTINATION_MIHF_ID:                 buffer_index += sprintf(&bufP[buffer_index], "TLV_DESTINATION_MIHF_ID");break;
        case MIH_C_TLV_STATUS:                              buffer_index += sprintf(&bufP[buffer_index], "TLV_STATUS");break;
        case MIH_C_TLV_LINK_TYPE:                           buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_TYPE");break;
        case MIH_C_TLV_MIH_EVENT_LIST:                      buffer_index += sprintf(&bufP[buffer_index], "TLV_MIH_EVENT_LIST");break;
        case MIH_C_TLV_MIH_COMMAND_LIST:                    buffer_index += sprintf(&bufP[buffer_index], "TLV_MIH_COMMAND_LIST");break;
        case MIH_C_TLV_MIIS_QUERY_TYPE_LIST:                buffer_index += sprintf(&bufP[buffer_index], "TLV_MIIS_QUERY_TYPE_LIST");break;
        case MIH_C_TLV_TRANSPORT_OPTION_LIST:               buffer_index += sprintf(&bufP[buffer_index], "TLV_TRANSPORT_OPTION_LIST");break;
        case MIH_C_TLV_LINK_ADDRESS_LIST:                   buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_ADDRESS_LIST");break;
        case MIH_C_TLV_MBB_HANDOVER_SUPPORT:                buffer_index += sprintf(&bufP[buffer_index], "TLV_MBB_HANDOVER_SUPPORT");break;
        case MIH_C_TLV_REGISTER_REQUEST_CODE:               buffer_index += sprintf(&bufP[buffer_index], "TLV_REGISTER_REQUEST_CODE");break;
        case MIH_C_TLV_VALID_TIME_INTERVAL:                 buffer_index += sprintf(&bufP[buffer_index], "TLV_VALID_TIME_INTERVAL");break;
        case MIH_C_TLV_LINK_IDENTIFIER:                     buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_IDENTIFIER");break;
        case MIH_C_TLV_NEW_LINK_IDENTIFIER:                 buffer_index += sprintf(&bufP[buffer_index], "TLV_NEW_LINK_IDENTIFIER");break;
        case MIH_C_TLV_OLD_ACCESS_ROUTER:                   buffer_index += sprintf(&bufP[buffer_index], "TLV_OLD_ACCESS_ROUTER");break;
        case MIH_C_TLV_NEW_ACCESS_ROUTER:                   buffer_index += sprintf(&bufP[buffer_index], "TLV_NEW_ACCESS_ROUTER");break;
        case MIH_C_TLV_IP_RENEWAL_FLAG:                     buffer_index += sprintf(&bufP[buffer_index], "TLV_IP_RENEWAL_FLAG");break;
        case MIH_C_TLV_MOBILITY_MANAGEMENT_SUPPORT:         buffer_index += sprintf(&bufP[buffer_index], "TLV_MOBILITY_MANAGEMENT_SUPPORT");break;
        case MIH_C_TLV_IP_ADDRESS_CONFIGURATION_METHODS:    buffer_index += sprintf(&bufP[buffer_index], "TLV_IP_ADDRESS_CONFIGURATION_METHODS");break;
        case MIH_C_TLV_LINK_DOWN_REASON_CODE:               buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_DOWN_REASON_CODE");break;
        case MIH_C_TLV_LINK_TIME_INTERVAL:                  buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_TIME_INTERVAL");break;
        case MIH_C_TLV_LINK_GOING_DOWN_REASON_CODE:         buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_GOING_DOWN_REASON_CODE");break;
        case MIH_C_TLV_LINK_PARAMETER_REPORT_LIST:          buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_PARAMETER_REPORT_LIST");break;
        case MIH_C_TLV_DEVICE_STATES_REQUEST:               buffer_index += sprintf(&bufP[buffer_index], "TLV_DEVICE_STATES_REQUEST");break;
        case MIH_C_TLV_LINK_IDENTIFIER_LIST:                buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_IDENTIFIER_LIST");break;
        case MIH_C_TLV_DEVICE_STATES_RESPONSE_LIST:         buffer_index += sprintf(&bufP[buffer_index], "TLV_DEVICE_STATES_RESPONSE_LIST");break;
        case MIH_C_TLV_GET_STATUS_REQUEST_SET:              buffer_index += sprintf(&bufP[buffer_index], "TLV_GET_STATUS_REQUEST_SET");break;
        case MIH_C_TLV_GET_STATUS_RESPONSE_LIST:            buffer_index += sprintf(&bufP[buffer_index], "TLV_GET_STATUS_RESPONSE_LIST");break;
        case MIH_C_TLV_CONFIGURE_REQUEST_LIST:              buffer_index += sprintf(&bufP[buffer_index], "TLV_CONFIGURE_REQUEST_LIST");break;
        case MIH_C_TLV_CONFIGURE_RESPONSE_LIST:             buffer_index += sprintf(&bufP[buffer_index], "TLV_CONFIGURE_RESPONSE_LIST");break;
        case MIH_C_TLV_LIST_OF_LINK_POA_LIST:               buffer_index += sprintf(&bufP[buffer_index], "TLV_LIST_OF_LINK_POA_LIST");break;
        case MIH_C_TLV_PREFERRED_LINK_LIST:                 buffer_index += sprintf(&bufP[buffer_index], "TLV_PREFERRED_LINK_LIST");break;
        case MIH_C_TLV_HANDOVER_RESOURCE_QUERY_LIST:        buffer_index += sprintf(&bufP[buffer_index], "TLV_HANDOVER_RESOURCE_QUERY_LIST");break;
        case MIH_C_TLV_HANDOVER_STATUS:                     buffer_index += sprintf(&bufP[buffer_index], "TLV_HANDOVER_STATUS");break;
        case MIH_C_TLV_ACCESS_ROUTER_ADDRESS:               buffer_index += sprintf(&bufP[buffer_index], "TLV_ACCESS_ROUTER_ADDRESS");break;
        case MIH_C_TLV_DHCP_SERVER_ADDRESS:                 buffer_index += sprintf(&bufP[buffer_index], "TLV_DHCP_SERVER_ADDRESS");break;
        case MIH_C_TLV_FA_ADDRESS:                          buffer_index += sprintf(&bufP[buffer_index], "TLV_FA_ADDRESS");break;
        case MIH_C_TLV_LINK_ACTIONS_LIST:                   buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_ACTIONS_LIST");break;
        case MIH_C_TLV_LINK_ACTIONS_RESULT_LIST:            buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_ACTIONS_RESULT_LIST");break;
        case MIH_C_TLV_HANDOVER_RESULT:                     buffer_index += sprintf(&bufP[buffer_index], "TLV_HANDOVER_RESULT");break;
        case MIH_C_TLV_RESOURCE_STATUS:                     buffer_index += sprintf(&bufP[buffer_index], "TLV_RESOURCE_STATUS");break;
        case MIH_C_TLV_RESOURCE_RETENTION_STATUS:           buffer_index += sprintf(&bufP[buffer_index], "TLV_RESOURCE_RETENTION_STATUS");break;
        case MIH_C_TLV_INFO_QUERY_BINARY_DATA_LIST:         buffer_index += sprintf(&bufP[buffer_index], "TLV_INFO_QUERY_BINARY_DATA_LIST");break;
        case MIH_C_TLV_INFO_QUERY_RDF_DATA_LIST:            buffer_index += sprintf(&bufP[buffer_index], "TLV_INFO_QUERY_RDF_DATA_LIST");break;
        case MIH_C_TLV_INFO_QUERY_RDF_SCHEMA_URL:           buffer_index += sprintf(&bufP[buffer_index], "TLV_INFO_QUERY_RDF_SCHEMA_URL");break;
        case MIH_C_TLV_INFO_QUERY_RDF_SCHEMA_LIST:          buffer_index += sprintf(&bufP[buffer_index], "TLV_INFO_QUERY_RDF_SCHEMA_LIST");break;
        case MIH_C_TLV_MAX_RESPONSE_SIZE:                   buffer_index += sprintf(&bufP[buffer_index], "TLV_MAX_RESPONSE_SIZE");break;
        case MIH_C_TLV_INFO_RESPONSE_BINARY_DATA_LIST:      buffer_index += sprintf(&bufP[buffer_index], "TLV_INFO_RESPONSE_BINARY_DATA_LIST");break;
        case MIH_C_TLV_INFO_RESPONSE_RDF_DATA_LIST:         buffer_index += sprintf(&bufP[buffer_index], "TLV_INFO_RESPONSE_RDF_DATA_LIST");break;
        case MIH_C_TLV_INFO_RESPONSE_RDF_SCHEMA_URL_LIST:   buffer_index += sprintf(&bufP[buffer_index], "TLV_INFO_RESPONSE_RDF_SCHEMA_URL_LIST");break;
        case MIH_C_TLV_INFO_RESPONSE_RDF_SCHEMA_LIST:       buffer_index += sprintf(&bufP[buffer_index], "TLV_INFO_RESPONSE_RDF_SCHEMA_LIST");break;
        case MIH_C_TLV_MOBILE_NODE_MIHF_ID:                 buffer_index += sprintf(&bufP[buffer_index], "TLV_MOBILE_NODE_MIHF_ID");break;
        case MIH_C_TLV_QUERY_RESOURCE_REPORT_FLAG:          buffer_index += sprintf(&bufP[buffer_index], "TLV_QUERY_RESOURCE_REPORT_FLAG");break;
        case MIH_C_TLV_EVENT_CONFIGURATION_INFO_LIST:       buffer_index += sprintf(&bufP[buffer_index], "TLV_EVENT_CONFIGURATION_INFO_LIST");break;
        case MIH_C_TLV_TARGET_NETWORK_INFO:                 buffer_index += sprintf(&bufP[buffer_index], "TLV_TARGET_NETWORK_INFO");break;
        case MIH_C_TLV_LIST_OF_TARGET_NETWORK_INFO:         buffer_index += sprintf(&bufP[buffer_index], "TLV_LIST_OF_TARGET_NETWORK_INFO");break;
        case MIH_C_TLV_ASSIGNED_RESOURCE_SET:               buffer_index += sprintf(&bufP[buffer_index], "TLV_ASSIGNED_RESOURCE_SET");break;
        case MIH_C_TLV_LINK_DETECTED_INFO_LIST:             buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_DETECTED_INFO_LIST");break;
        case MIH_C_TLV_MN_LINK_ID:                          buffer_index += sprintf(&bufP[buffer_index], "TLV_MN_LINK_ID");break;
        case MIH_C_TLV_POA:                                 buffer_index += sprintf(&bufP[buffer_index], "TLV_POA");break;
        case MIH_C_TLV_UNAUTHENTICATED_INFORMATION_REQUEST: buffer_index += sprintf(&bufP[buffer_index], "TLV_UNAUTHENTICATED_INFORMATION_REQUEST");break;
        case MIH_C_TLV_NETWORK_TYPE:                        buffer_index += sprintf(&bufP[buffer_index], "TLV_NETWORK_TYPE");break;
        case MIH_C_TLV_REQUESTED_RESOURCE_SET:              buffer_index += sprintf(&bufP[buffer_index], "TLV_REQUESTED_RESOURCE_SET");break;
#ifdef MIH_C_MEDIEVAL_EXTENSIONS
        case MIH_C_TLV_LINK_EVENT_LIST:                     buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_EVENT_LIST");break;
        case MIH_C_TLV_LINK_CMD_LIST:                       buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_CMD_LIST");break;
        case MIH_C_TLV_LINK_PARAM_TYPE_LIST:                buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_PARAM_TYPE_LIST");break;
        case MIH_C_TLV_LINK_PARAMETERS_STATUS_LIST:         buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_PARAMETERS_STATUS_LIST");break;
        case MIH_C_TLV_LINK_STATES_REQ:                     buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_STATES_REQ");break;
        case MIH_C_TLV_LINK_STATES_RSP_LIST:                buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_STATES_RSP_LIST");break;
        case MIH_C_TLV_LINK_DESC_REQ:                       buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_DESC_REQ");break;
        case MIH_C_TLV_LINK_DESC_RSP_LIST:                  buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_DESC_RSP_LIST");break;
        case MIH_C_TLV_LINK_ACTION:                         buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_ACTION");break;
        case MIH_C_TLV_LINK_AC_RESULT:                      buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_AC_RESULT");break;
        case MIH_C_TLV_LINK_SCAN_RSP_LIST:                  buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_SCAN_RSP_LIST");break;
        case MIH_C_TLV_LINK_DET_INFO:                       buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_DET_INFO");break;
        case MIH_C_TLV_LINK_INTERFACE_TYPE_ADDR:            buffer_index += sprintf(&bufP[buffer_index], "TLV_LINK_INTERFACE_TYPE_ADDR");break;
        case MIH_C_TLV_MOS_DSCV:                            buffer_index += sprintf(&bufP[buffer_index], "TLV_MOS_DSCV");break;
#endif
        default:                                            buffer_index += sprintf(&bufP[buffer_index], "UNKNOWN TLV");
    }
    return buffer_index;
}
