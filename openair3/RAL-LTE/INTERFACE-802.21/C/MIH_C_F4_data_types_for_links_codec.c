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
#define MIH_C_F4_DATA_TYPES_FOR_LINKS_CODEC_C
#include "MIH_C_F4_data_types_for_links_codec.h"


#define MIH_C_OPMODE_NORMAL_MODE                 0
#define MIH_C_OPMODE_POWER_SAVING_MODE           1
#define MIH_C_OPMODE_POWERED_DOWN                2
//-----------------------------------------------------------------------------
unsigned int MIH_C_OPMODE2String2(MIH_C_OPMODE_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;

    switch (*dataP) {
        case MIH_C_OPMODE_NORMAL_MODE:
            buffer_index+= sprintf(&bufP[buffer_index], "OPMODE_NORMAL_MODE");
            break;
        case MIH_C_OPMODE_POWER_SAVING_MODE:
            buffer_index+= sprintf(&bufP[buffer_index], "POWER_SAVING_MODE");
            break;
        case MIH_C_OPMODE_POWERED_DOWN:
            buffer_index+= sprintf(&bufP[buffer_index], "POWERED_DOWN");
            break;
        default:
            buffer_index+= sprintf(&bufP[buffer_index], "UNKNOWN");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_DEV_STATE_RSP_encode(Bit_Buffer_t* bbP, MIH_C_DEV_STATE_RSP_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_encode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            MIH_C_DEVICE_INFO_encode(bbP, &dataP->_union.device_info);
            break;
        case 1:
            MIH_C_BATT_LEVEL_encode(bbP, &dataP->_union.batt_level);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE OF MIH_C_DEV_STATE_RSP_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_DEV_STATE_RSP_decode(Bit_Buffer_t* bbP, MIH_C_DEV_STATE_RSP_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_decode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            MIH_C_DEVICE_INFO_decode(bbP, &dataP->_union.device_info);
            break;
        case 1:
            MIH_C_BATT_LEVEL_decode(bbP, &dataP->_union.batt_level);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE OF MIH_C_DEV_STATE_RSP_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_EVENT_LIST2String2(MIH_C_LINK_EVENT_LIST_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;

    if (*dataP & MIH_C_BIT_LINK_DETECTED)            buffer_index+= sprintf(&bufP[buffer_index], "LK_DETECTED,");
    if (*dataP & MIH_C_BIT_LINK_UP)                  buffer_index+= sprintf(&bufP[buffer_index], "LK_UP,");
    if (*dataP & MIH_C_BIT_LINK_DOWN)                buffer_index+= sprintf(&bufP[buffer_index], "LK_DOWN,");
    if (*dataP & MIH_C_BIT_LINK_PARAMETERS_REPORT)   buffer_index+= sprintf(&bufP[buffer_index], "LK_PARAMETERS_REPORT,");
    if (*dataP & MIH_C_BIT_LINK_GOING_DOWN)          buffer_index+= sprintf(&bufP[buffer_index], "LK_GOING_DOWN,");
    if (*dataP & MIH_C_BIT_LINK_HANDOVER_IMMINENT)   buffer_index+= sprintf(&bufP[buffer_index], "LK_HANDOVER_IMMINENT,");
    if (*dataP & MIH_C_BIT_LINK_HANDOVER_COMPLETE)   buffer_index+= sprintf(&bufP[buffer_index], "LK_HANDOVER_COMPLETE,");
    if (*dataP & MIH_C_BIT_LINK_PDU_TRANSMIT_STATUS) buffer_index+= sprintf(&bufP[buffer_index], "LK_PDU_TRANSMIT_STATUS");
    if (*dataP == 0)                                 buffer_index+= sprintf(&bufP[buffer_index], "NULL");

    return buffer_index;
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_CMD_LIST2String2(MIH_C_LINK_CMD_LIST_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;

    if (*dataP & MIH_C_BIT_LINK_EVENT_SUBSCRIBE)      buffer_index+= sprintf(&bufP[buffer_index], "LK_EVT_SUBSCRIBE,");
    if (*dataP & MIH_C_BIT_LINK_EVENT_UNSUBSCRIBE)    buffer_index+= sprintf(&bufP[buffer_index], "LK_EVT_UNSUBSCRIBE,");
    if (*dataP & MIH_C_BIT_LINK_GET_PARAMETERS)       buffer_index+= sprintf(&bufP[buffer_index], "LK_GET_PARAMETERS,");
    if (*dataP & MIH_C_BIT_LINK_CONFIGURE_THRESHOLDS) buffer_index+= sprintf(&bufP[buffer_index], "LK_CONFIGURE_THRESHOLDS,");
    if (*dataP & MIH_C_BIT_LINK_ACTION)               buffer_index+= sprintf(&bufP[buffer_index], "LK_ACTION");
    if (*dataP == 0)                                  buffer_index+= sprintf(&bufP[buffer_index], "NULL");

    return buffer_index;
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_TYPE2String2(MIH_C_LINK_TYPE_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    switch (*dataP) {
        case MIH_C_WIRELESS_GSM:            buffer_index += sprintf(&bufP[buffer_index], "WIRELESS_GSM"); break;
        case MIH_C_WIRELESS_GPRS:           buffer_index += sprintf(&bufP[buffer_index], "WIRELESS_GPRS"); break;
        case MIH_C_WIRELESS_EDGE:           buffer_index += sprintf(&bufP[buffer_index], "WIRELESS_EDGE"); break;
        case MIH_C_ETHERNET:                buffer_index += sprintf(&bufP[buffer_index], "ETHERNET"); break;
        case MIH_C_WIRELESS_OTHER:          buffer_index += sprintf(&bufP[buffer_index], "WIRELESS_OTHER"); break;
        case MIH_C_WIRELESS_IEEE802_11:     buffer_index += sprintf(&bufP[buffer_index], "WIRELESS_IEEE802_11"); break;
        case MIH_C_WIRELESS_CDMA_2000:      buffer_index += sprintf(&bufP[buffer_index], "WIRELESS_CDMA_2000"); break;
        case MIH_C_WIRELESS_UMTS:           buffer_index += sprintf(&bufP[buffer_index], "WIRELESS_UMTS"); break;
        case MIH_C_WIRELESS_CDMA_2000_HRPD: buffer_index += sprintf(&bufP[buffer_index], "WIRELESS_CDMA_2000_HRPD"); break;
        case MIH_C_WIRELESS_IEEE802_16:     buffer_index += sprintf(&bufP[buffer_index], "WIRELESS_IEEE802_16"); break;
        case MIH_C_WIRELESS_IEEE802_20:     buffer_index += sprintf(&bufP[buffer_index], "WIRELESS_IEEE802_20"); break;
        case MIH_C_WIRELESS_IEEE802_22:     buffer_index += sprintf(&bufP[buffer_index], "WIRELESS_IEEE802_22"); break;
        default:                            buffer_index += sprintf(&bufP[buffer_index], "WIRELESS_IEEE802_22");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_ID2String(MIH_C_LINK_ID_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;

    buffer_index += sprintf(&bufP[buffer_index], "LINK_TYPE = ");
    buffer_index += MIH_C_LINK_TYPE2String2(&dataP->link_type, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], ", LINK_ADDR = ");
    buffer_index += MIH_C_LINK_ADDR2String(&dataP->link_addr, &bufP[buffer_index]);
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_ID_encode(Bit_Buffer_t* bbP, MIH_C_LINK_ID_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_TYPE_encode(bbP, &dataP->link_type);
    MIH_C_LINK_ADDR_encode(bbP, &dataP->link_addr);
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_ID_decode(Bit_Buffer_t* bbP, MIH_C_LINK_ID_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_TYPE_decode(bbP, &dataP->link_type);
    MIH_C_LINK_ADDR_decode(bbP, &dataP->link_addr);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_AC_ATTR2String2(MIH_C_LINK_AC_ATTR_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;

    if (*dataP & MIH_C_BIT_LINK_AC_ATTR_LINK_SCAN)       buffer_index+= sprintf(&bufP[buffer_index], "AC_ATTR_LINK_SCAN");
    if (*dataP & MIH_C_BIT_LINK_AC_ATTR_LINK_RES_RETAIN) buffer_index+= sprintf(&bufP[buffer_index], ",AC_ATTR_LINK_RES_RETAIN");
    if (*dataP & MIH_C_BIT_LINK_AC_ATTR_DATA_FWD_REQ)    buffer_index+= sprintf(&bufP[buffer_index], ",AC_ATTR_DATA_FWD_REQ");
    if (*dataP == 0)                                     buffer_index+= sprintf(&bufP[buffer_index], "NULL");
    return buffer_index;
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_ACTION2String(MIH_C_LINK_ACTION_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;

    buffer_index += sprintf(&bufP[buffer_index], "LINK_AC_TYPE = ");
    buffer_index += MIH_C_LINK_AC_TYPE2String2(&dataP->link_ac_type, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], ", LINK_AC_ATTR = ");
    buffer_index += MIH_C_LINK_AC_ATTR2String2(&dataP->link_ac_attr, &bufP[buffer_index]);
#ifdef MIH_C_MEDIEVAL_EXTENSIONS
    buffer_index += sprintf(&bufP[buffer_index], ", LINK_AC_PARAM = ");
    buffer_index += MIH_C_LINK_AC_PARAM2String(&dataP->link_ac_param, &bufP[buffer_index]);
#endif
    return buffer_index;
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_AC_TYPE2String2(MIH_C_LINK_AC_TYPE_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    switch (*dataP) {
        case MIH_C_LINK_AC_TYPE_NONE:                      buffer_index += sprintf(&bufP[buffer_index], "NONE"); break;
        case MIH_C_LINK_AC_TYPE_LINK_DISCONNECT:           buffer_index += sprintf(&bufP[buffer_index], "LINK_DISCONNECT"); break;
        case MIH_C_LINK_AC_TYPE_LINK_LOW_POWER:            buffer_index += sprintf(&bufP[buffer_index], "LINK_LOW_POWER"); break;
        case MIH_C_LINK_AC_TYPE_LINK_POWER_DOWN:           buffer_index += sprintf(&bufP[buffer_index], "POWER_DOWN"); break;
        case MIH_C_LINK_AC_TYPE_LINK_POWER_UP:             buffer_index += sprintf(&bufP[buffer_index], "POWER_UP"); break;
#ifdef MIH_C_MEDIEVAL_EXTENSIONS
        case MIH_C_LINK_AC_TYPE_LINK_FLOW_ATTR:            buffer_index += sprintf(&bufP[buffer_index], "FLOW_ATTR"); break;
        case MIH_C_LINK_AC_TYPE_LINK_ACTIVATE_RESOURCES:   buffer_index += sprintf(&bufP[buffer_index], "ACTIVATE_RESOURCES"); break;
        case MIH_C_LINK_AC_TYPE_LINK_DEACTIVATE_RESOURCES: buffer_index += sprintf(&bufP[buffer_index], "DEACTIVATE_RESOURCES"); break;
#endif
        default:                            buffer_index += sprintf(&bufP[buffer_index], "UNKNOWN");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_ACTION_encode(Bit_Buffer_t* bbP, MIH_C_LINK_ACTION_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_AC_TYPE_encode(bbP, &dataP->link_ac_type);
    MIH_C_LINK_AC_ATTR_encode(bbP, &dataP->link_ac_attr);
#ifdef MIH_C_MEDIEVAL_EXTENSIONS
    MIH_C_LINK_AC_PARAM_encode(bbP, &dataP->link_ac_param);
#endif
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_ACTION_decode(Bit_Buffer_t* bbP, MIH_C_LINK_ACTION_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_AC_TYPE_decode(bbP, &dataP->link_ac_type);
    MIH_C_LINK_AC_ATTR_decode(bbP, &dataP->link_ac_attr);
#ifdef MIH_C_MEDIEVAL_EXTENSIONS
    MIH_C_LINK_AC_PARAM_decode(bbP, &dataP->link_ac_param);
#endif
}

//-----------------------------------------------------------------------------
// MW Function to bypass ODTONE problem
inline void MIH_C_LINK_ACTION_short_decode(Bit_Buffer_t* bbP, MIH_C_LINK_ACTION_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_AC_TYPE_decode(bbP, &dataP->link_ac_type);
    MIH_C_LINK_AC_ATTR_decode(bbP, &dataP->link_ac_attr);
//#ifdef MIH_C_MEDIEVAL_EXTENSIONS
    //MIH_C_LINK_AC_PARAM_decode(bbP, &dataP->link_ac_param);
//#endif
}

//-----------------------------------------------------------------------------
inline void MIH_C_LINK_ACTION_REQ_encode(Bit_Buffer_t* bbP, MIH_C_LINK_ACTION_REQ_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_ID_encode(bbP, &dataP->link_id);
    MIH_C_CHOICE_encode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            break;
        case 1:
            MIH_C_LINK_ADDR_encode(bbP, &dataP->_union.link_addr);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE OF MIH_C_LINK_ACTION_REQ_T %d\n", dataP->choice);
    }
    MIH_C_LINK_ACTION_encode(bbP, &dataP->link_action);
    MIH_C_LINK_AC_EX_TIME_encode(bbP, &dataP->link_action_ex_time);
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_ACTION_REQ_decode(Bit_Buffer_t* bbP, MIH_C_LINK_ACTION_REQ_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_ID_decode(bbP, &dataP->link_id);
    MIH_C_CHOICE_decode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            break;
        case 1:
            MIH_C_LINK_ADDR_decode(bbP, &dataP->_union.link_addr);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE OF MIH_C_LINK_ACTION_REQ_T %d\n", dataP->choice);
    }
    MIH_C_LINK_ACTION_decode(bbP, &dataP->link_action);
    MIH_C_LINK_AC_EX_TIME_decode(bbP, &dataP->link_action_ex_time);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_SIG_STRENGTH2String(MIH_C_SIG_STRENGTH_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    switch (dataP->choice) {
        case 0:
            buffer_index += sprintf(&bufP[buffer_index], "%d dBm",dataP->_union.dbm );
            break;
        case 1:
            buffer_index += sprintf(&bufP[buffer_index], "%d/100",dataP->_union.percentage );
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "UNINITIALIZED STRENGTH");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_SIG_STRENGTH_encode(Bit_Buffer_t* bbP, MIH_C_SIG_STRENGTH_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_encode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            MIH_C_INTEGER1_encode(bbP, &dataP->_union.dbm);
            break;
        case 1:
            MIH_C_PERCENTAGE_encode(bbP, &dataP->_union.percentage);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE OF MIH_C_SIG_STRENGTH_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_SIG_STRENGTH_decode(Bit_Buffer_t* bbP, MIH_C_SIG_STRENGTH_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_decode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            MIH_C_INTEGER1_decode(bbP, &dataP->_union.dbm);
            break;
        case 1:
            MIH_C_PERCENTAGE_decode(bbP, &dataP->_union.percentage);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE OF MIH_C_SIG_STRENGTH_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_SCAN_RSP2String(MIH_C_LINK_SCAN_RSP_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;

    buffer_index += sprintf(&bufP[buffer_index], "LINK_ADDR=");
    buffer_index += MIH_C_LINK_ADDR2String(&dataP->link_addr, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], ", NETWORK_ID=");
    buffer_index += MIH_C_NETWORK_ID2String(&dataP->network_id, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], ", SIG_STRENGTH=");
    buffer_index += MIH_C_SIG_STRENGTH2String(&dataP->sig_strength, &bufP[buffer_index]);
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_SCAN_RSP_encode(Bit_Buffer_t* bbP, MIH_C_LINK_SCAN_RSP_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_ADDR_encode(bbP, &dataP->link_addr);
    MIH_C_NETWORK_ID_encode(bbP, &dataP->network_id);
    MIH_C_SIG_STRENGTH_encode(bbP, &dataP->sig_strength);
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_SCAN_RSP_decode(Bit_Buffer_t* bbP, MIH_C_LINK_SCAN_RSP_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_ADDR_decode(bbP, &dataP->link_addr);
    MIH_C_NETWORK_ID_decode(bbP, &dataP->network_id);
    MIH_C_SIG_STRENGTH_decode(bbP, &dataP->sig_strength);
}

//-----------------------------------------------------------------------------
inline void MIH_C_LINK_ACTION_RSP_encode(Bit_Buffer_t* bbP, MIH_C_LINK_ACTION_RSP_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_ID_encode(bbP, &dataP->link_id);
    MIH_C_LINK_AC_RESULT_encode(bbP, &dataP->link_ac_result);
    MIH_C_CHOICE_encode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            break;
        case 1:
            MIH_C_LINK_SCAN_RSP_LIST_encode(bbP, &dataP->_union.link_scan_rsp_list);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE OF MIH_C_LINK_ACTION_RSP_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_ACTION_RSP_decode(Bit_Buffer_t* bbP, MIH_C_LINK_ACTION_RSP_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_ID_decode(bbP, &dataP->link_id);
    MIH_C_LINK_AC_RESULT_decode(bbP, &dataP->link_ac_result);
    MIH_C_CHOICE_decode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            break;
        case 1:
            MIH_C_LINK_SCAN_RSP_LIST_decode(bbP, &dataP->_union.link_scan_rsp_list);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE OF MIH_C_LINK_ACTION_RSP_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_THRESHOLD2String(MIH_C_THRESHOLD_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "THRESHOLD_VAL=");
    buffer_index += MIH_C_THRESHOLD_VAL2String(&dataP->threshold_val, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], ", THRESHOLD_XDIR=");
    buffer_index += MIH_C_THRESHOLD_XDIR2String2(&dataP->threshold_xdir, &bufP[buffer_index]);
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_THRESHOLD_encode(Bit_Buffer_t* bbP, MIH_C_THRESHOLD_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_THRESHOLD_VAL_encode(bbP, &dataP->threshold_val);
    MIH_C_THRESHOLD_XDIR_encode(bbP, &dataP->threshold_xdir);
}
//-----------------------------------------------------------------------------
inline void MIH_C_THRESHOLD_decode(Bit_Buffer_t* bbP, MIH_C_THRESHOLD_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_THRESHOLD_VAL_decode(bbP, &dataP->threshold_val);
    MIH_C_THRESHOLD_XDIR_decode(bbP, &dataP->threshold_xdir);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_PARAM_TYPE2String( MIH_C_LINK_PARAM_TYPE_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    switch (dataP->choice) {
        case 0:
            buffer_index += sprintf(&bufP[buffer_index], "GENERIC PARAMETER: ");
            buffer_index += MIH_C_LINK_PARAM_GEN2String2(&dataP->_union.link_param_gen, &bufP[buffer_index]);
            break;
        case 1:  buffer_index += sprintf(&bufP[buffer_index], "QOS PARAMETER: "); break;
        case 2:  buffer_index += sprintf(&bufP[buffer_index], "GG PARAMETER: "); break;
        case 3:  buffer_index += sprintf(&bufP[buffer_index], "EDGE PARAMETER: "); break;
        case 4:  buffer_index += sprintf(&bufP[buffer_index], "ETH PARAMETER: "); break;
        case 5:  buffer_index += sprintf(&bufP[buffer_index], "802_11 PARAMETER: "); break;
        case 6:  buffer_index += sprintf(&bufP[buffer_index], "C2K PARAMETER: "); break;
        case 7:  buffer_index += sprintf(&bufP[buffer_index], "FDD PARAMETER: "); break;
        case 8:  buffer_index += sprintf(&bufP[buffer_index], "HRPD PARAMETER: "); break;
        case 9:  buffer_index += sprintf(&bufP[buffer_index], "802_16 PARAMETER: "); break;
        case 10: buffer_index += sprintf(&bufP[buffer_index], "802_20 PARAMETER: "); break;
        case 11: buffer_index += sprintf(&bufP[buffer_index], "802_22 PARAMETER: "); break;
        case 12: buffer_index += sprintf(&bufP[buffer_index], "LTE PARAMETER: "); break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "UNINITIALIZED LINK TYPE");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_PARAM_TYPE_encode(Bit_Buffer_t* bbP, MIH_C_LINK_PARAM_TYPE_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_encode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:  MIH_C_LINK_PARAM_GEN_encode(bbP, &dataP->_union.link_param_gen); break;
        case 1:  MIH_C_LINK_PARAM_QOS_encode(bbP, &dataP->_union.link_param_qos); break;
        case 2:  MIH_C_LINK_PARAM_GG_encode(bbP, &dataP->_union.link_param_gg); break;
        case 3:  MIH_C_LINK_PARAM_EDGE_encode(bbP, &dataP->_union.link_param_edge); break;
        case 4:  MIH_C_LINK_PARAM_ETH_encode(bbP, &dataP->_union.link_param_eth); break;
        case 5:  MIH_C_LINK_PARAM_802_11_encode(bbP, &dataP->_union.link_param_802_11); break;
        case 6:  MIH_C_LINK_PARAM_C2K_encode(bbP, &dataP->_union.link_param_c2k); break;
        case 7:  MIH_C_LINK_PARAM_FDD_encode(bbP, &dataP->_union.link_param_fdd); break;
        case 8:  MIH_C_LINK_PARAM_HRPD_encode(bbP, &dataP->_union.link_param_hrpd); break;
        case 9:  MIH_C_LINK_PARAM_802_16_encode(bbP, &dataP->_union.link_param_802_16); break;
        case 10: MIH_C_LINK_PARAM_802_20_encode(bbP, &dataP->_union.link_param_802_20); break;
        case 11: MIH_C_LINK_PARAM_802_22_encode(bbP, &dataP->_union.link_param_802_22); break;
        case 12: MIH_C_LINK_PARAM_LTE_encode(bbP, &dataP->_union.link_param_lte); break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE OF MIH_C_LINK_PARAM_TYPE_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_PARAM_TYPE_decode(Bit_Buffer_t* bbP, MIH_C_LINK_PARAM_TYPE_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_decode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            MIH_C_LINK_PARAM_GEN_decode(bbP, &dataP->_union.link_param_gen);
            break;
        case 1:  MIH_C_LINK_PARAM_QOS_decode(bbP, &dataP->_union.link_param_qos); break;
        case 2:  MIH_C_LINK_PARAM_GG_decode(bbP, &dataP->_union.link_param_gg); break;
        case 3:  MIH_C_LINK_PARAM_EDGE_decode(bbP, &dataP->_union.link_param_edge); break;
        case 4:  MIH_C_LINK_PARAM_ETH_decode(bbP, &dataP->_union.link_param_eth); break;
        case 5:  MIH_C_LINK_PARAM_802_11_decode(bbP, &dataP->_union.link_param_802_11); break;
        case 6:  MIH_C_LINK_PARAM_C2K_decode(bbP, &dataP->_union.link_param_c2k); break;
        case 7:  MIH_C_LINK_PARAM_FDD_decode(bbP, &dataP->_union.link_param_fdd); break;
        case 8:  MIH_C_LINK_PARAM_HRPD_decode(bbP, &dataP->_union.link_param_hrpd); break;
        case 9:  MIH_C_LINK_PARAM_802_16_decode(bbP, &dataP->_union.link_param_802_16); break;
        case 10: MIH_C_LINK_PARAM_802_20_decode(bbP, &dataP->_union.link_param_802_20); break;
        case 11: MIH_C_LINK_PARAM_802_22_decode(bbP, &dataP->_union.link_param_802_22); break;
        case 12: MIH_C_LINK_PARAM_LTE_decode(bbP, &dataP->_union.link_param_802_22); break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE OF MIH_C_LINK_PARAM_TYPE_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_CFG_PARAM2String(MIH_C_LINK_CFG_PARAM_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "LINK_PARAM_TYPE=");
    buffer_index += MIH_C_LINK_PARAM_TYPE2String(&dataP->link_param_type, &bufP[buffer_index]);
    switch (dataP->choice) {
        case 0:
            buffer_index += sprintf(&bufP[buffer_index], ", CHOICE NULL");
            break;
        case 1:
            buffer_index += sprintf(&bufP[buffer_index], ", TIMER_INTERVAL=");
            buffer_index += MIH_C_TIMER_INTERVAL2String(&dataP->_union.timer_interval, &bufP[buffer_index]);
            break;
#ifdef RAL_SAME_AS_MEDIEVAL_PROJECT_BUT_I_THINK_THIS_IS_AN_ERROR
        case 2:
            buffer_index += sprintf(&bufP[buffer_index], ", LINK_SCAN_RSP_LIST=");
            buffer_index += MIH_C_LINK_SCAN_RSP_LIST2String(&dataP->_union.link_scan_rsp_list, &bufP[buffer_index]);
            break;
#endif
        default:
            buffer_index += sprintf(&bufP[buffer_index], ", CHOICE UNINITIALIZED");
    }
    buffer_index += sprintf(&bufP[buffer_index], ", TH_ACTION=");
    buffer_index += MIH_C_TH_ACTION2String2(&dataP->th_action, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], ", THRESHOLD_LIST={");
    buffer_index += MIH_C_THRESHOLD_LIST2String(&dataP->threshold_list, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "}");
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_CFG_PARAM_encode(Bit_Buffer_t* bbP, MIH_C_LINK_CFG_PARAM_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_PARAM_TYPE_encode(bbP, &dataP->link_param_type);
    MIH_C_CHOICE_encode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            break;
        case 1:
            MIH_C_TIMER_INTERVAL_encode(bbP, &dataP->_union.timer_interval);
            break;
#ifdef RAL_SAME_AS_MEDIEVAL_PROJECT_BUT_I_THINK_THIS_IS_AN_ERROR
        case 2:
            MIH_C_LINK_SCAN_RSP_LIST_encode(bbP, &dataP->_union.link_scan_rsp_list);
            break;
#endif
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE OF MIH_C_LINK_CFG_PARAM_T %d\n", dataP->choice);
    }
    MIH_C_TH_ACTION_encode(bbP, &dataP->th_action);
    MIH_C_THRESHOLD_LIST_encode(bbP, &dataP->threshold_list);
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_CFG_PARAM_decode(Bit_Buffer_t* bbP, MIH_C_LINK_CFG_PARAM_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_PARAM_TYPE_decode(bbP, &dataP->link_param_type);
    MIH_C_CHOICE_decode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            break;
        case 1:
            MIH_C_TIMER_INTERVAL_decode(bbP, &dataP->_union.timer_interval);
            break;
#ifdef RAL_SAME_AS_MEDIEVAL_PROJECT_BUT_I_THINK_THIS_IS_AN_ERROR
        case 2:
            MIH_C_LINK_SCAN_RSP_LIST_decode(bbP, &dataP->_union.link_scan_rsp_list);
            break;
#endif
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE OF MIH_C_LINK_CFG_PARAM_T %d\n", dataP->choice);
    }
    MIH_C_TH_ACTION_decode(bbP, &dataP->th_action);
    MIH_C_THRESHOLD_LIST_decode(bbP, &dataP->threshold_list);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_CFG_STATUS2String(MIH_C_LINK_CFG_STATUS_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "LINK_PARAM_TYPE=");
    buffer_index += MIH_C_LINK_PARAM_TYPE2String(&dataP->link_param_type, &bufP[buffer_index]);

    buffer_index += sprintf(&bufP[buffer_index], ", THRESHOLD=");
    buffer_index += MIH_C_THRESHOLD2String(&dataP->threshold, &bufP[buffer_index]);

    buffer_index += sprintf(&bufP[buffer_index], ", CONFIG_STATUS=");
    buffer_index += MIH_C_CONFIG_STATUS2String(&dataP->config_status, &bufP[buffer_index]);
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_CFG_STATUS_encode(Bit_Buffer_t* bbP, MIH_C_LINK_CFG_STATUS_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_PARAM_TYPE_encode(bbP, &dataP->link_param_type);
    MIH_C_THRESHOLD_encode(bbP, &dataP->threshold);
    MIH_C_CONFIG_STATUS_encode(bbP, &dataP->config_status);
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_CFG_STATUS_decode(Bit_Buffer_t* bbP, MIH_C_LINK_CFG_STATUS_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_PARAM_TYPE_decode(bbP, &dataP->link_param_type);
    MIH_C_THRESHOLD_decode(bbP, &dataP->threshold);
    MIH_C_CONFIG_STATUS_decode(bbP, &dataP->config_status);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_DESC_RSP2String(MIH_C_LINK_DESC_RSP_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    switch (dataP->choice) {
        case 0:
            buffer_index += sprintf(&bufP[buffer_index], "NUM_COS=");
            buffer_index += MIH_C_NUM_COS2String(&dataP->_union.num_cos, &bufP[buffer_index]);
            break;
        case 1:
            buffer_index += sprintf(&bufP[buffer_index], "NUM_QUEUE=");
            buffer_index += MIH_C_NUM_QUEUE2String(&dataP->_union.num_queue, &bufP[buffer_index]);
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "LINK_DESC_RSP UNITIALIZED");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_DESC_RSP_encode(Bit_Buffer_t* bbP, MIH_C_LINK_DESC_RSP_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_encode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            MIH_C_NUM_COS_encode(bbP, &dataP->_union.num_cos);
            break;
        case 1:
            MIH_C_NUM_QUEUE_encode(bbP, &dataP->_union.num_queue);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE OF MIH_C_LINK_DESC_RSP_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_DESC_RSP_decode(Bit_Buffer_t* bbP, MIH_C_LINK_DESC_RSP_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_decode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            MIH_C_NUM_COS_decode(bbP, &dataP->_union.num_cos);
            break;
        case 1:
            MIH_C_NUM_QUEUE_decode(bbP, &dataP->_union.num_queue);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE OF MIH_C_LINK_DESC_RSP_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_PARAM2String(MIH_C_LINK_PARAM_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "LINK_PARAM_TYPE=");
    buffer_index += MIH_C_LINK_PARAM_TYPE2String(&dataP->link_param_type, &bufP[buffer_index]);
    switch (dataP->choice) {
        case 0:
            buffer_index += sprintf(&bufP[buffer_index], ", LINK_PARAM_VAL=");
            buffer_index += MIH_C_LINK_PARAM_VAL2String(&dataP->_union.link_param_val, &bufP[buffer_index]);
            break;
        case 1:
            buffer_index += sprintf(&bufP[buffer_index], ", QOS_PARAM_VAL=");
            buffer_index += MIH_C_QOS_PARAM_VAL2String(&dataP->_union.qos_param_val, &bufP[buffer_index]);
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "LINK_PARAM UNITIALIZED");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_PARAM_encode(Bit_Buffer_t* bbP, MIH_C_LINK_PARAM_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_PARAM_TYPE_encode(bbP, &dataP->link_param_type);
    MIH_C_CHOICE_encode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:  MIH_C_LINK_PARAM_VAL_encode(bbP, &dataP->_union.link_param_val); break;
        case 1:  MIH_C_QOS_PARAM_VAL_encode(bbP, &dataP->_union.qos_param_val); break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE OF MIH_C_LINK_PARAM_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_PARAM_decode(Bit_Buffer_t* bbP, MIH_C_LINK_PARAM_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_PARAM_TYPE_decode(bbP, &dataP->link_param_type);
    MIH_C_CHOICE_decode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:  MIH_C_LINK_PARAM_VAL_decode(bbP, &dataP->_union.link_param_val); break;
        case 1:  MIH_C_QOS_PARAM_VAL_decode(bbP, &dataP->_union.qos_param_val); break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE OF MIH_C_LINK_PARAM_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_PARAM_RPT2String(MIH_C_LINK_PARAM_RPT_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "LINK_PARAM=");
    buffer_index += MIH_C_LINK_PARAM2String(&dataP->link_param, &bufP[buffer_index]);
    switch (dataP->choice) {
        case 0:
            //buffer_index += sprintf(&bufP[buffer_index], ", CHOICE NULL");
            break;
        case 1:
            buffer_index += sprintf(&bufP[buffer_index], ", THRESHOLD=");
            buffer_index += MIH_C_THRESHOLD2String(&dataP->_union.threshold, &bufP[buffer_index]);
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], ", LINK_PARAM_RPT UNITIALIZED");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_PARAM_RPT_encode(Bit_Buffer_t* bbP, MIH_C_LINK_PARAM_RPT_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_PARAM_encode(bbP, &dataP->link_param);
    MIH_C_CHOICE_encode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:  break;
        case 1:  MIH_C_THRESHOLD_encode(bbP, &dataP->_union.threshold); break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE OF MIH_C_LINK_PARAM_RPT_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_PARAM_RPT_decode(Bit_Buffer_t* bbP, MIH_C_LINK_PARAM_RPT_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_PARAM_decode(bbP, &dataP->link_param);
    MIH_C_CHOICE_decode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:  break;
        case 1:  MIH_C_THRESHOLD_decode(bbP, &dataP->_union.threshold); break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE OF MIH_C_LINK_PARAM_RPT_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_POA_LIST_encode(Bit_Buffer_t* bbP, MIH_C_LINK_POA_LIST_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_ID_encode(bbP, &dataP->link_id);
    MIH_C_LINK_ADDR_LIST_encode(bbP, &dataP->link_addr_list);
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_POA_LIST_decode(Bit_Buffer_t* bbP, MIH_C_LINK_POA_LIST_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_ID_decode(bbP, &dataP->link_id);
    MIH_C_LINK_ADDR_LIST_decode(bbP, &dataP->link_addr_list);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_STATES_RSP2String(MIH_C_LINK_STATES_RSP_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    switch (dataP->choice) {
        case 0:
            buffer_index += sprintf(&bufP[buffer_index], "OPMODE=");
            buffer_index += MIH_C_OPMODE2String2(&dataP->_union.op_mode, &bufP[buffer_index]);
            break;
        case 1:
            buffer_index += sprintf(&bufP[buffer_index], "CHANNEL ID=");
            buffer_index += MIH_C_CHANNEL_ID2String(&dataP->_union.channel_id, &bufP[buffer_index]);
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "LINK_STATES_RSP UNITIALIZED");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_STATES_RSP_encode(Bit_Buffer_t* bbP, MIH_C_LINK_STATES_RSP_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_encode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:  MIH_C_OPMODE_encode(bbP, &dataP->_union.op_mode); break;
        case 1:  MIH_C_CHANNEL_ID_encode(bbP, &dataP->_union.channel_id); break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE OF MIH_C_LINK_STATES_RSP_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_STATES_RSP_decode(Bit_Buffer_t* bbP, MIH_C_LINK_STATES_RSP_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_decode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:  MIH_C_OPMODE_decode(bbP, &dataP->_union.op_mode); break;
        case 1:  MIH_C_CHANNEL_ID_decode(bbP, &dataP->_union.channel_id); break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE OF MIH_C_LINK_STATES_RSP_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_STATES_REQ2String2(MIH_C_LINK_STATES_REQ_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    if  (*dataP & MIH_C_BIT_LINK_STATES_REQ_OP_MODE) buffer_index += sprintf(&bufP[buffer_index], "OPMODE");
    if  (*dataP & MIH_C_BIT_LINK_STATES_REQ_CHANNEL_ID) buffer_index += sprintf(&bufP[buffer_index], " CHANNEL_ID");
    return buffer_index;
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_DESC_REQ2String2(MIH_C_LINK_DESC_REQ_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    if  (*dataP & MIH_C_BIT_NUMBER_OF_CLASSES_OF_SERVICE_SUPPORTED) buffer_index += sprintf(&bufP[buffer_index], "NUM_OF_COS_SUPPORTED");
    if  (*dataP & MIH_C_BIT_NUMBER_OF_QUEUES_SUPPORTED) buffer_index += sprintf(&bufP[buffer_index], " NUM_OF_QUEUES_SUPPORTED");
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_STATUS_REQ_encode(Bit_Buffer_t* bbP, MIH_C_LINK_STATUS_REQ_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_STATES_REQ_encode(bbP, &dataP->link_states_req);
    MIH_C_LINK_PARAM_TYPE_LIST_encode(bbP, &dataP->link_param_type_list);
    MIH_C_LINK_DESC_RSP_encode(bbP, &dataP->link_desc_rsp);
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_STATUS_REQ_decode(Bit_Buffer_t* bbP, MIH_C_LINK_STATUS_REQ_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_STATES_REQ_decode(bbP, &dataP->link_states_req);
    MIH_C_LINK_PARAM_TYPE_LIST_decode(bbP, &dataP->link_param_type_list);
    MIH_C_LINK_DESC_RSP_decode(bbP, &dataP->link_desc_rsp);
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_STATUS_RSP_encode(Bit_Buffer_t* bbP, MIH_C_LINK_STATUS_RSP_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_STATES_RSP_LIST_encode(bbP, &dataP->link_states_rsp_list);
    MIH_C_LINK_PARAM_LIST_encode(bbP, &dataP->link_param_list);
    MIH_C_LINK_DESC_RSP_LIST_encode(bbP, &dataP->link_desc_rsp_list);
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_STATUS_RSP_decode(Bit_Buffer_t* bbP, MIH_C_LINK_STATUS_RSP_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_STATES_RSP_LIST_decode(bbP, &dataP->link_states_rsp_list);
    MIH_C_LINK_PARAM_LIST_decode(bbP, &dataP->link_param_list);
    MIH_C_LINK_DESC_RSP_LIST_decode(bbP, &dataP->link_desc_rsp_list);
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_TUPLE_ID_encode(Bit_Buffer_t* bbP, MIH_C_LINK_TUPLE_ID_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_ID_encode(bbP, &dataP->link_id);
    MIH_C_CHOICE_encode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case MIH_C_LINK_TUPLE_ID_CHOICE_NULL:  break;
        case MIH_C_LINK_TUPLE_ID_CHOICE_LINK_ADDR:  MIH_C_LINK_ADDR_encode(bbP, &dataP->_union.link_addr); break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE OF MIH_C_LINK_TUPLE_ID_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_TUPLE_ID_decode(Bit_Buffer_t* bbP, MIH_C_LINK_TUPLE_ID_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_ID_decode(bbP, &dataP->link_id);
    MIH_C_CHOICE_decode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case MIH_C_LINK_TUPLE_ID_CHOICE_NULL:       break;
        case MIH_C_LINK_TUPLE_ID_CHOICE_LINK_ADDR:  MIH_C_LINK_ADDR_decode(bbP, &dataP->_union.link_addr); break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE OF MIH_C_LINK_TUPLE_ID_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_DET_INFO_encode(Bit_Buffer_t* bbP, MIH_C_LINK_DET_INFO_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_TUPLE_ID_encode(bbP,    &dataP->link_tuple_id);
    MIH_C_NETWORK_ID_encode(bbP,       &dataP->network_id);
    MIH_C_NET_AUX_ID_encode(bbP,       &dataP->net_aux_id);
    MIH_C_SIG_STRENGTH_encode(bbP,     &dataP->sig_strength);
    MIH_C_UNSIGNED_INT2_encode(bbP,    &dataP->sinr);
    MIH_C_LINK_DATA_RATE_encode(bbP,   &dataP->link_data_rate);
    MIH_C_LINK_MIHCAP_FLAG_encode(bbP, &dataP->link_mihcap_flag);
    MIH_C_NET_CAPS_encode(bbP,         &dataP->net_caps);

}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_DET_INFO_decode(Bit_Buffer_t* bbP, MIH_C_LINK_DET_INFO_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_TUPLE_ID_decode(bbP,    &dataP->link_tuple_id);
    MIH_C_NETWORK_ID_decode(bbP,       &dataP->network_id);
    MIH_C_NET_AUX_ID_decode(bbP,       &dataP->net_aux_id);
    MIH_C_SIG_STRENGTH_decode(bbP,     &dataP->sig_strength);
    MIH_C_UNSIGNED_INT2_decode(bbP,    &dataP->sinr);
    MIH_C_LINK_DATA_RATE_decode(bbP,   &dataP->link_data_rate);
    MIH_C_LINK_MIHCAP_FLAG_decode(bbP, &dataP->link_mihcap_flag);
    MIH_C_NET_CAPS_decode(bbP,         &dataP->net_caps);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_TH_ACTION2String2(MIH_C_TH_ACTION_T *actionP, char* bufP){
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    switch (*actionP) {
        case MIH_C_SET_NORMAL_THRESHOLD:   buffer_index += sprintf(&bufP[buffer_index], "SET_NORMAL_THRESHOLD");break;
        case MIH_C_SET_ONE_SHOT_THRESHOLD: buffer_index += sprintf(&bufP[buffer_index], "SET_ONE_SHOT_THRESHOLD");break;
        case MIH_C_CANCEL_THRESHOLD:       buffer_index += sprintf(&bufP[buffer_index], "CANCEL_THRESHOLD");break;
        default:                           buffer_index += sprintf(&bufP[buffer_index], "UNKNOWN");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_THRESHOLD_XDIR2String2(MIH_C_THRESHOLD_XDIR_T *valP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    switch(*valP) {
        case MIH_C_ABOVE_THRESHOLD:
            buffer_index += sprintf(&bufP[buffer_index], "ABOVE_THRESHOLD");
            break;
        case MIH_C_BELOW_THRESHOLD:
            buffer_index += sprintf(&bufP[buffer_index], "BELOW_THRESHOLD");
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "UNKNOWN");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_PARAM_GEN2String2(MIH_C_LINK_PARAM_GEN_T *valP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    switch(*valP) {
        case MIH_C_LINK_PARAM_GEN_DATA_RATE:
            buffer_index += sprintf(&bufP[buffer_index], "DATA_RATE");
            break;
        case MIH_C_LINK_PARAM_GEN_SIGNAL_STRENGTH:
            buffer_index += sprintf(&bufP[buffer_index], "SIGNAL_STRENGTH");
            break;
        case MIH_C_LINK_PARAM_GEN_SINR:
            buffer_index += sprintf(&bufP[buffer_index], "SINR");
            break;
        case MIH_C_LINK_PARAM_GEN_THROUGHPUT:
            buffer_index += sprintf(&bufP[buffer_index], "THROUGHPUT");
            break;
        case MIH_C_LINK_PARAM_GEN_PACKET_ERROR_RATE:
            buffer_index += sprintf(&bufP[buffer_index], "PACKET_ERROR_RATE");
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "UNKNOWN");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_AC_RESULT2String2(MIH_C_LINK_AC_RESULT_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    switch (*dataP) {
        case MIH_C_LINK_AC_RESULT_SUCCESS:
            buffer_index += sprintf(&bufP[buffer_index], "SUCCESS");
            break;
        case MIH_C_LINK_AC_RESULT_FAILURE:
            buffer_index += sprintf(&bufP[buffer_index], "FAILURE");
            break;
        case MIH_C_LINK_AC_RESULT_REFUSED:
            buffer_index += sprintf(&bufP[buffer_index], "REFUSED");
            break;
        case MIH_C_LINK_AC_RESULT_INCAPABLE:
            buffer_index += sprintf(&bufP[buffer_index], "INCAPABLE");
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "UNKNOWN");
    }
    return buffer_index;
}
