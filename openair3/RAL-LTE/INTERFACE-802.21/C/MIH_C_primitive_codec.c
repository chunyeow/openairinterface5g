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
#define MIH_C_PRIMITIVE_CODEC_C
#include <assert.h>
#include "MIH_C_primitive_codec.h"

//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Decode_Link_Capability_Discover_request(Bit_Buffer_t* bbP, MIH_C_Link_Capability_Discover_request_t *primitiveP) {
//-----------------------------------------------------------------------------
    return MIH_PRIMITIVE_DECODE_OK;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Link_Capability_Discover_request2String(MIH_C_Link_Capability_Discover_request_t *primitiveP, char* bufP) {
//-----------------------------------------------------------------------------
    return 0;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Decode_Link_Event_Subscribe_request(Bit_Buffer_t* bbP, MIH_C_Link_Event_Subscribe_request_t *primitiveP){
//-----------------------------------------------------------------------------

    MIH_C_TLV_TYPE_T   tlv;
    u_int16_t          length;

    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    MIH_C_LIST_LENGTH_decode(bbP, &length);
    MIH_C_LINK_EVENT_LIST_decode(bbP, &primitiveP->RequestedLinkEventList);

    #if MIH_C_DEBUG_DESERIALIZATION
    if (primitiveP->RequestedLinkEventList & MIH_C_BIT_LINK_DETECTED)            printf("SUBSCRIBE_REQUEST LINK_DETECTED\n");
    if (primitiveP->RequestedLinkEventList & MIH_C_BIT_LINK_UP)                  printf("SUBSCRIBE_REQUEST LINK_UP\n");
    if (primitiveP->RequestedLinkEventList & MIH_C_BIT_LINK_DOWN)                printf("SUBSCRIBE_REQUEST LINK_DOWN\n");
    if (primitiveP->RequestedLinkEventList & MIH_C_BIT_LINK_PARAMETERS_REPORT)   printf("SUBSCRIBE_REQUEST LINK_PARAMETERS_REPORT\n");
    if (primitiveP->RequestedLinkEventList & MIH_C_BIT_LINK_GOING_DOWN)          printf("SUBSCRIBE_REQUEST LINK_GOING_DOWN\n");
    if (primitiveP->RequestedLinkEventList & MIH_C_BIT_LINK_HANDOVER_IMMINENT)   printf("SUBSCRIBE_REQUEST LINK_HANDOVER_IMMINENT\n");
    if (primitiveP->RequestedLinkEventList & MIH_C_BIT_LINK_HANDOVER_COMPLETE)   printf("SUBSCRIBE_REQUEST LINK_HANDOVER_COMPLETE\n");
    if (primitiveP->RequestedLinkEventList & MIH_C_BIT_LINK_PDU_TRANSMIT_STATUS) printf("SUBSCRIBE_REQUEST LINK_PDU_TRANSMIT_STATUS\n");
    #endif

    if (BitBuffer_isCheckReadOverflowOK(bbP, 0) == BIT_BUFFER_TRUE) {
        return MIH_PRIMITIVE_DECODE_OK;
    } else {
        return MIH_PRIMITIVE_DECODE_TOO_SHORT;
    }
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Link_Event_Subscribe_request2String(MIH_C_Link_Event_Subscribe_request_t *primitiveP, char* bufP) {
//-----------------------------------------------------------------------------

    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "-     PRIMITIVE Link_Event_Subscribe.request -\n");
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------\n");
    buffer_index += MIH_C_LINK_EVENT_LIST2String(&primitiveP->RequestedLinkEventList, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\n");
    return buffer_index;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Decode_Link_Event_Unsubscribe_request(Bit_Buffer_t* bbP, MIH_C_Link_Event_Unsubscribe_request_t *primitiveP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_EVENT_LIST_decode(bbP, &primitiveP->RequestedLinkEventList);
    if (BitBuffer_isCheckReadOverflowOK(bbP, 0) == BIT_BUFFER_TRUE) {
        return MIH_PRIMITIVE_DECODE_OK;
    } else {
        return MIH_PRIMITIVE_DECODE_TOO_SHORT;
    }
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Link_Event_Unsubscribe_request2String(MIH_C_Link_Event_Unsubscribe_request_t *primitiveP, char* bufP) {
//-----------------------------------------------------------------------------

    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "-     PRIMITIVE Link_Event_Unsubscribe.request -\n");
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------\n");
    buffer_index += MIH_C_LINK_EVENT_LIST2String(&primitiveP->RequestedLinkEventList, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\n");
    return buffer_index;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Decode_Link_Get_Parameters_request(Bit_Buffer_t* bbP, MIH_C_Link_Get_Parameters_request_t *primitiveP) {
//-----------------------------------------------------------------------------
    MIH_C_TLV_TYPE_T   tlv;
    u_int16_t          length;

    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    assert(tlv == MIH_C_TLV_LINK_PARAM_TYPE_LIST);
    MIH_C_LIST_LENGTH_decode(bbP, &length);
    MIH_C_LINK_PARAM_TYPE_LIST_decode(bbP, &primitiveP->LinkParametersRequest_list);

    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    assert(tlv == MIH_C_TLV_LINK_STATES_REQ);
    MIH_C_LIST_LENGTH_decode(bbP, &length);
    MIH_C_LINK_STATES_REQ_decode(bbP, &primitiveP->LinkStatesRequest);


    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    assert(tlv == MIH_C_TLV_LINK_DESC_REQ);
    MIH_C_LIST_LENGTH_decode(bbP, &length);
    MIH_C_LINK_DESC_REQ_decode(bbP, &primitiveP->LinkDescriptorsRequest);
    if (BitBuffer_isCheckReadOverflowOK(bbP, 0) == BIT_BUFFER_TRUE) {
        return MIH_PRIMITIVE_DECODE_OK;
    } else {
        return MIH_PRIMITIVE_DECODE_TOO_SHORT;
    }
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Link_Get_Parameters_request2String(MIH_C_Link_Get_Parameters_request_t *primitiveP, char* bufP) {
//-----------------------------------------------------------------------------

    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "-     PRIMITIVE Link_Get_Parameters.request      -\n");
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "LINK_PARAM_TYPE_LIST = ");
    buffer_index += MIH_C_LINK_PARAM_TYPE_LIST2String(&primitiveP->LinkParametersRequest_list, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nLINK_STATES_REQ = ");
    buffer_index += MIH_C_LINK_STATES_REQ2String(&primitiveP->LinkStatesRequest, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nLINK_DESC_REQ = ");
    buffer_index += MIH_C_LINK_DESC_REQ2String(&primitiveP->LinkDescriptorsRequest, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\n");
    return buffer_index;
}

//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Decode_Link_Configure_Thresholds_request(Bit_Buffer_t* bbP, MIH_C_Link_Configure_Thresholds_request_t *primitiveP) {
//-----------------------------------------------------------------------------
    MIH_C_TLV_TYPE_T   tlv;
    u_int16_t          length;

    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    MIH_C_LIST_LENGTH_decode(bbP, &length);

    MIH_C_LINK_CFG_PARAM_LIST_decode(bbP, &primitiveP->LinkConfigureParameterList_list);
    if (BitBuffer_isCheckReadOverflowOK(bbP, 0) == BIT_BUFFER_TRUE) {
        return MIH_PRIMITIVE_DECODE_OK;
    } else {
        return MIH_PRIMITIVE_DECODE_TOO_SHORT;
    }
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Link_Configure_Thresholds_request2String(MIH_C_Link_Configure_Thresholds_request_t *primitiveP, char* bufP) {
//-----------------------------------------------------------------------------

    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "-     PRIMITIVE Link_Configure_Threshold.request -\n");
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------\n");
    buffer_index += MIH_C_LINK_CFG_PARAM_LIST2String(&primitiveP->LinkConfigureParameterList_list, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\n");
    return buffer_index;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Decode_Link_Action_request(Bit_Buffer_t* bbP, MIH_C_Link_Action_request_t *primitiveP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_ADDR_T  *PoALinkAddress = NULL;
    MIH_C_TLV_TYPE_T   tlv;
    u_int16_t          length;

    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    assert(tlv == MIH_C_TLV_LINK_ACTION);
    MIH_C_LIST_LENGTH_decode(bbP, &length);
    // Patch MW 03/05/2013
    if (length >2) {
    MIH_C_LINK_ACTION_decode(bbP, &primitiveP->LinkAction);
    } else {
    MIH_C_LINK_ACTION_short_decode(bbP, &primitiveP->LinkAction) ;
    }

    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    assert(tlv == MIH_C_TLV_LINK_TIME_INTERVAL);
    MIH_C_LIST_LENGTH_decode(bbP, &length);
    MIH_C_UNSIGNED_INT2_decode(bbP, &primitiveP->ExecutionDelay);

    if (BitBuffer_isCheckReadOverflowOK(bbP, 0) == BIT_BUFFER_FALSE) {
        return MIH_PRIMITIVE_DECODE_TOO_SHORT;
    }

    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    if (tlv == MIH_C_TLV_POA) {
        PoALinkAddress = malloc(sizeof(MIH_C_LINK_ADDR_T));
        MIH_C_LINK_ADDR_decode(bbP, PoALinkAddress);
        primitiveP->PoALinkAddress = PoALinkAddress;
    } else {
        primitiveP->PoALinkAddress = NULL;
    }
    if (BitBuffer_isCheckReadOverflowOK(bbP, 0) == BIT_BUFFER_FALSE) {
        free(PoALinkAddress);
        return MIH_PRIMITIVE_DECODE_TOO_SHORT;
    }
    return MIH_PRIMITIVE_DECODE_OK;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Link_Action_request2String(MIH_C_Link_Action_request_t *primitiveP, char* bufP) {
//-----------------------------------------------------------------------------

    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "-     PRIMITIVE Link_Action.request -\n");
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------\n");
    buffer_index += MIH_C_LINK_ACTION2String(&primitiveP->LinkAction, &bufP[buffer_index]);
    buffer_index += MIH_C_UNSIGNED_INT22String(&primitiveP->ExecutionDelay, &bufP[buffer_index]);
    if (primitiveP->PoALinkAddress != NULL) {
        buffer_index += MIH_C_LINK_ADDR2String(primitiveP->PoALinkAddress, &bufP[buffer_index]);
    }
    buffer_index += sprintf(&bufP[buffer_index], "\n");
    return buffer_index;
}
#ifdef MIH_C_MEDIEVAL_EXTENSIONS
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Encode_Link_Register_indication(Bit_Buffer_t* bbP, MIH_C_Link_Register_indication_t *primitiveP) {
//-----------------------------------------------------------------------------
    MIH_C_TLV_TYPE_T            tlv;
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    u_int16_t                   encode_length;

    // TYPE
    tlv = MIH_C_TLV_LINK_INTERFACE_TYPE_ADDR;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_LINK_ID_encode(bbP, &primitiveP->Link_Id);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    return MIH_PRIMITIVE_ENCODE_OK;
}
#endif
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Link_Register_indication2String(MIH_C_Link_Register_indication_t *primitiveP, char* bufP) {
//-----------------------------------------------------------------------------

    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "-     PRIMITIVE Link_Register.indication -\n");
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------\n");
    buffer_index += MIH_C_LINK_ID2String(&primitiveP->Link_Id, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\n");
    return buffer_index;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Encode_Link_Detected_indication(Bit_Buffer_t* bbP, MIH_C_Link_Detected_indication_t *primitiveP) {
//-----------------------------------------------------------------------------
    MIH_C_TLV_TYPE_T            tlv;
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    u_int16_t                   encode_length;

    // TYPE
    tlv = MIH_C_TLV_LINK_DET_INFO;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_LINK_DET_INFO_encode(bbP, &primitiveP->LinkDetectedInfo);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    return MIH_PRIMITIVE_ENCODE_OK;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Encode_Link_Up_indication(Bit_Buffer_t* bbP, MIH_C_Link_Up_indication_t *primitiveP) {
//-----------------------------------------------------------------------------
    MIH_C_TLV_TYPE_T            tlv;
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    u_int16_t                   encode_length;

    // TYPE
    tlv = MIH_C_TLV_LINK_IDENTIFIER;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_LINK_TUPLE_ID_encode(bbP, &primitiveP->LinkIdentifier);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    if (primitiveP->OldAccessRouter) {
        // TYPE
        tlv = MIH_C_TLV_OLD_ACCESS_ROUTER;
        MIH_C_TLV_TYPE_encode(bbP, &tlv);

        // VALUE
        saved_byte_position = BitBuffer_getPosition(bbP);
        MIH_C_LINK_ADDR_encode(bbP, primitiveP->OldAccessRouter);
        end_byte_position = BitBuffer_getPosition(bbP);

        // LENGTH
        encode_length = end_byte_position - saved_byte_position;
        hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
        BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
        BitBuffer_rewind_to(bbP, saved_byte_position);
        MIH_C_LIST_LENGTH_encode(bbP, encode_length);
        BitBuffer_rewind_to(bbP, end_byte_position + hole_size);
    }
    if (primitiveP->NewAccessRouter) {
        // TYPE
        tlv = MIH_C_TLV_NEW_ACCESS_ROUTER;
        MIH_C_TLV_TYPE_encode(bbP, &tlv);

        // VALUE
        saved_byte_position = BitBuffer_getPosition(bbP);
        MIH_C_LINK_ADDR_encode(bbP, primitiveP->NewAccessRouter);
        end_byte_position = BitBuffer_getPosition(bbP);

        // LENGTH
        encode_length = end_byte_position - saved_byte_position;
        hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
        BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
        BitBuffer_rewind_to(bbP, saved_byte_position);
        MIH_C_LIST_LENGTH_encode(bbP, encode_length);
        BitBuffer_rewind_to(bbP, end_byte_position + hole_size);
    }
    if (primitiveP->IPRenewalFlag) {
        // TYPE
        tlv = MIH_C_TLV_IP_RENEWAL_FLAG;
        MIH_C_TLV_TYPE_encode(bbP, &tlv);

        // VALUE
        saved_byte_position = BitBuffer_getPosition(bbP);
        MIH_C_IP_RENEWAL_FLAG_encode(bbP, primitiveP->IPRenewalFlag);
        end_byte_position = BitBuffer_getPosition(bbP);

        // LENGTH
        encode_length = end_byte_position - saved_byte_position;
        hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
        BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
        BitBuffer_rewind_to(bbP, saved_byte_position);
        MIH_C_LIST_LENGTH_encode(bbP, encode_length);
        BitBuffer_rewind_to(bbP, end_byte_position + hole_size);
    }
    if (primitiveP->MobilityManagementSupport) {
        // TYPE
        tlv = MIH_C_TLV_MOBILITY_MANAGEMENT_SUPPORT;
        MIH_C_TLV_TYPE_encode(bbP, &tlv);

        // VALUE
        saved_byte_position = BitBuffer_getPosition(bbP);
        MIH_C_IP_MOB_MGMT_encode(bbP, primitiveP->MobilityManagementSupport);
        end_byte_position = BitBuffer_getPosition(bbP);

        // LENGTH
        encode_length = end_byte_position - saved_byte_position;
        hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
        BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
        BitBuffer_rewind_to(bbP, saved_byte_position);
        MIH_C_LIST_LENGTH_encode(bbP, encode_length);
        BitBuffer_rewind_to(bbP, end_byte_position + hole_size);
    }

    return MIH_PRIMITIVE_ENCODE_OK;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Encode_Link_Parameters_Report_indication(Bit_Buffer_t* bbP, MIH_C_Link_Parameters_Report_indication_t *primitiveP) {
//-----------------------------------------------------------------------------
    MIH_C_TLV_TYPE_T            tlv;
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    u_int16_t                   encode_length;


    // ------- LinkIdentifier--------
    // TYPE
    tlv = MIH_C_TLV_LINK_IDENTIFIER;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_LINK_TUPLE_ID_encode(bbP, &primitiveP->LinkIdentifier);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    // ------- LinkParametersReportList_list--------
    // TYPE
    tlv = MIH_C_TLV_LINK_PARAMETER_REPORT_LIST;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_LINK_PARAM_RPT_LIST_encode(bbP, &primitiveP->LinkParametersReportList_list);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    return MIH_PRIMITIVE_ENCODE_OK;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Encode_Link_Going_Down_indication(Bit_Buffer_t* bbP, MIH_C_Link_Going_Down_indication_t *primitiveP) {
//-----------------------------------------------------------------------------
    MIH_C_TLV_TYPE_T            tlv;
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    u_int16_t                   encode_length;

    // ------- LinkIdentifier--------
    // TYPE
    tlv = MIH_C_TLV_LINK_IDENTIFIER;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_LINK_TUPLE_ID_encode(bbP, &primitiveP->LinkIdentifier);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    // ------- TimeInterval--------
    // TYPE
    tlv = MIH_C_TLV_LINK_TIME_INTERVAL;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    MIH_C_LIST_LENGTH_encode(bbP, sizeof(MIH_C_UNSIGNED_INT2_T));
    // VALUE
    MIH_C_UNSIGNED_INT2_encode(bbP, &primitiveP->TimeInterval);

    // ------- LinkGoingDownReason--------
    // TYPE
    tlv = MIH_C_TLV_LINK_GOING_DOWN_REASON_CODE;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    MIH_C_LIST_LENGTH_encode(bbP, sizeof(MIH_C_LINK_GD_REASON_T));
    // VALUE
    MIH_C_LINK_GD_REASON_encode(bbP, &primitiveP->LinkGoingDownReason);

    return MIH_PRIMITIVE_ENCODE_OK;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Encode_Link_Down_indication(Bit_Buffer_t* bbP, MIH_C_Link_Down_indication_t *primitiveP) {
//-----------------------------------------------------------------------------
    MIH_C_TLV_TYPE_T            tlv;
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    u_int16_t                   encode_length;

    // ------- LinkIdentifier--------
    // TYPE
    tlv = MIH_C_TLV_LINK_IDENTIFIER;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_LINK_TUPLE_ID_encode(bbP, &primitiveP->LinkIdentifier);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    // ------- OldAccessRouter--------
    if (primitiveP->OldAccessRouter != NULL) {
        // TYPE
        tlv = MIH_C_TLV_OLD_ACCESS_ROUTER;
        MIH_C_TLV_TYPE_encode(bbP, &tlv);

        // VALUE
        saved_byte_position = BitBuffer_getPosition(bbP);
        MIH_C_LINK_ADDR_encode(bbP, primitiveP->OldAccessRouter);
        end_byte_position = BitBuffer_getPosition(bbP);

        // LENGTH
        encode_length = end_byte_position - saved_byte_position;
        hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
        BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
        BitBuffer_rewind_to(bbP, saved_byte_position);
        MIH_C_LIST_LENGTH_encode(bbP, encode_length);
        BitBuffer_rewind_to(bbP, end_byte_position + hole_size);
    }

    // ------- ReasonCode--------
    // TYPE
    tlv = MIH_C_TLV_LINK_DOWN_REASON_CODE;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    MIH_C_LIST_LENGTH_encode(bbP, sizeof(MIH_C_LINK_DN_REASON_T));
    // VALUE
    MIH_C_LINK_DN_REASON_encode(bbP, &primitiveP->ReasonCode);

    return MIH_PRIMITIVE_ENCODE_OK;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Encode_Link_Capability_Discover_confirm(Bit_Buffer_t* bbP, MIH_C_Link_Capability_Discover_confirm_t *primitiveP) {
//-----------------------------------------------------------------------------
    MIH_C_TLV_TYPE_T            tlv;
    u_int16_t                   encode_length;

    //---------  Status ------------
    // TYPE
    tlv = MIH_C_TLV_STATUS;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);
    // LENGTH
    encode_length = sizeof(MIH_C_STATUS_T);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    // VALUE
    MIH_C_STATUS_encode(bbP, &primitiveP->Status);


    //---------  SupportedLinkEventList ------------
    if (primitiveP->SupportedLinkEventList != NULL) {
        // TYPE
        tlv = MIH_C_TLV_LINK_EVENT_LIST;
        MIH_C_TLV_TYPE_encode(bbP, &tlv);
        // LENGTH
        encode_length = sizeof(MIH_C_LINK_EVENT_LIST_T);
        MIH_C_LIST_LENGTH_encode(bbP, encode_length);
        // VALUE
        MIH_C_LINK_EVENT_LIST_encode(bbP, primitiveP->SupportedLinkEventList);
    }


    //---------  SupportedLinkCommandList ------------
    if (primitiveP->SupportedLinkCommandList != NULL) {
        // TYPE
        tlv = MIH_C_TLV_LINK_CMD_LIST;
        MIH_C_TLV_TYPE_encode(bbP, &tlv);
        // LENGTH
        encode_length = sizeof(MIH_C_LINK_CMD_LIST_T);
        MIH_C_LIST_LENGTH_encode(bbP, encode_length);
        // VALUE
        MIH_C_LINK_CMD_LIST_encode(bbP, primitiveP->SupportedLinkCommandList);
    }

    return MIH_PRIMITIVE_ENCODE_OK;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Link_Capability_Discover_confirm2String(MIH_C_Link_Capability_Discover_confirm_t *primitiveP, char* bufP) {
//-----------------------------------------------------------------------------

    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "-     PRIMITIVE Capability_Discover.confirm -\n");
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "STATUS          = ");
    buffer_index += MIH_C_STATUS2String(&primitiveP->Status, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nLINK_EVENT_LIST = ");
    buffer_index += MIH_C_LINK_EVENT_LIST2String(primitiveP->SupportedLinkEventList, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nLINK_CMD_LIST   = ");
    buffer_index += MIH_C_LINK_CMD_LIST2String(primitiveP->SupportedLinkEventList, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\n");
    return buffer_index;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Encode_Link_Event_Subscribe_confirm(Bit_Buffer_t* bbP, MIH_C_Link_Event_Subscribe_confirm_t *primitiveP) {
//-----------------------------------------------------------------------------
    MIH_C_TLV_TYPE_T            tlv;
    u_int16_t                   encode_length;

    //---------  Status ------------
    // TYPE
    tlv = MIH_C_TLV_STATUS;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);
    // LENGTH
    encode_length = sizeof(MIH_C_STATUS_T);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    // VALUE
    MIH_C_STATUS_encode(bbP, &primitiveP->Status);


    //---------  SupportedLinkEventList ------------
    if (primitiveP->ResponseLinkEventList != NULL) {
        // TYPE
        tlv = MIH_C_TLV_LINK_EVENT_LIST;
        MIH_C_TLV_TYPE_encode(bbP, &tlv);
        // LENGTH
        // Bitmap
        encode_length = sizeof(MIH_C_LINK_EVENT_LIST_T);
        MIH_C_LIST_LENGTH_encode(bbP, encode_length);
        // VALUE
        MIH_C_LINK_EVENT_LIST_encode(bbP, primitiveP->ResponseLinkEventList);
    }
    return MIH_PRIMITIVE_ENCODE_OK;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Encode_Link_Event_Unsubscribe_confirm(Bit_Buffer_t* bbP, MIH_C_Link_Event_Unsubscribe_confirm_t *primitiveP) {
//-----------------------------------------------------------------------------
    MIH_C_TLV_TYPE_T            tlv;
    u_int16_t                   encode_length;

    //---------  Status ------------
    // TYPE
    tlv = MIH_C_TLV_STATUS;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);
    // LENGTH
    encode_length = sizeof(MIH_C_STATUS_T);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    // VALUE
    MIH_C_STATUS_encode(bbP, &primitiveP->Status);


    //---------  SupportedLinkEventList ------------
    if (primitiveP->ResponseLinkEventList != NULL) {
        // TYPE
        tlv = MIH_C_TLV_LINK_EVENT_LIST;
        MIH_C_TLV_TYPE_encode(bbP, &tlv);
        // LENGTH
        // Bitmap
        encode_length = sizeof(MIH_C_LINK_EVENT_LIST_T);
        MIH_C_LIST_LENGTH_encode(bbP, encode_length);
        // VALUE
        MIH_C_LINK_EVENT_LIST_encode(bbP, primitiveP->ResponseLinkEventList);
    }
    return MIH_PRIMITIVE_ENCODE_OK;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Encode_Link_Configure_Thresholds_confirm(Bit_Buffer_t* bbP, MIH_C_Link_Configure_Thresholds_confirm_t *primitiveP) {
//-----------------------------------------------------------------------------
    MIH_C_TLV_TYPE_T            tlv;
    u_int16_t                   encode_length;

    //---------  Status ------------
    // TYPE
    tlv = MIH_C_TLV_STATUS;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);
    // LENGTH
    encode_length = sizeof(MIH_C_STATUS_T);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    // VALUE
    MIH_C_STATUS_encode(bbP, &primitiveP->Status);


    //---------  SupportedLinkEventList ------------
    if (primitiveP->LinkConfigureStatusList_list != NULL) {
        // TYPE
        tlv = MIH_C_TLV_CONFIGURE_RESPONSE_LIST;
        MIH_C_TLV_TYPE_encode(bbP, &tlv);
        // LENGTH
        encode_length = sizeof(MIH_C_LINK_CFG_STATUS_T) * primitiveP->LinkConfigureStatusList_list->length;
        MIH_C_LIST_LENGTH_encode(bbP, encode_length);
        // VALUE
        MIH_C_LINK_CFG_STATUS_LIST_encode(bbP, primitiveP->LinkConfigureStatusList_list);
    }
    return MIH_PRIMITIVE_ENCODE_OK;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Encode_Link_Get_Parameters_confirm(Bit_Buffer_t* bbP, MIH_C_Link_Get_Parameters_confirm_t *primitiveP) {
//-----------------------------------------------------------------------------
    MIH_C_TLV_TYPE_T            tlv;
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    u_int16_t                   encode_length;

    //---------  Status ------------
    // TYPE
    tlv = MIH_C_TLV_STATUS;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);
    // LENGTH
    encode_length = sizeof(MIH_C_STATUS_T);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    // VALUE
    MIH_C_STATUS_encode(bbP, &primitiveP->Status);

    //---------  LinkParametersStatusList_list ------------
    // TYPE
    tlv = MIH_C_TLV_LINK_PARAMETERS_STATUS_LIST;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);
    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_LINK_PARAM_LIST_encode(bbP, primitiveP->LinkParametersStatusList_list);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH (BECAUSE LinkParametersStatusList_list contains a list)
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    //---------  LinkStatesResponse_list ------------
    // TYPE
    tlv = MIH_C_TLV_LINK_STATES_RSP_LIST;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);
    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_LINK_STATES_RSP_LIST_encode(bbP, primitiveP->LinkStatesResponse_list);
    end_byte_position = BitBuffer_getPosition(bbP);
    // LENGTH (DONE AFTER BECAUSE CONTAINS CHOICES OF NOT EQUAL SIZES)
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    //---------  LinkDescriptorsResponse_list ------------
    // TYPE
    tlv = MIH_C_TLV_LINK_DESC_RSP_LIST;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);
    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_LINK_DESC_RSP_LIST_encode(bbP, primitiveP->LinkDescriptorsResponse_list);
    end_byte_position = BitBuffer_getPosition(bbP);
    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);


    return MIH_PRIMITIVE_ENCODE_OK;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Primitive_Encode_Link_Action_confirm(Bit_Buffer_t* bbP, MIH_C_Link_Action_confirm_t *primitiveP) {
//-----------------------------------------------------------------------------
    MIH_C_TLV_TYPE_T            tlv;
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    u_int16_t                   encode_length;

    //---------  Status ------------
    // TYPE
    tlv = MIH_C_TLV_STATUS;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);
    // LENGTH
    encode_length = sizeof(MIH_C_STATUS_T);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    // VALUE
    MIH_C_STATUS_encode(bbP, &primitiveP->Status);

    if (primitiveP->Status == MIH_C_STATUS_SUCCESS) {
        if (primitiveP->ScanResponseSet_list != NULL) {
            //---------  ScanResponseSet_list ------------
            // TYPE
            tlv = MIH_C_TLV_LINK_SCAN_RSP_LIST;
            MIH_C_TLV_TYPE_encode(bbP, &tlv);
            // VALUE
            saved_byte_position = BitBuffer_getPosition(bbP);
            MIH_C_LINK_SCAN_RSP_LIST_encode(bbP, primitiveP->ScanResponseSet_list);
            end_byte_position = BitBuffer_getPosition(bbP);

            // LENGTH
            encode_length = end_byte_position - saved_byte_position;
            hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
            BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
            BitBuffer_rewind_to(bbP, saved_byte_position);
            MIH_C_LIST_LENGTH_encode(bbP, encode_length);
            BitBuffer_rewind_to(bbP, end_byte_position + hole_size);
        }
        //---------  LinkActionResult ------------
        // TYPE
        tlv = MIH_C_TLV_LINK_AC_RESULT;
        MIH_C_TLV_TYPE_encode(bbP, &tlv);
        // VALUE
        saved_byte_position = BitBuffer_getPosition(bbP);
        MIH_C_LINK_AC_RESULT_encode(bbP, primitiveP->LinkActionResult);
        end_byte_position = BitBuffer_getPosition(bbP);

        // LENGTH
        encode_length = end_byte_position - saved_byte_position;
        hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
        BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
        BitBuffer_rewind_to(bbP, saved_byte_position);
        MIH_C_LIST_LENGTH_encode(bbP, encode_length);
        BitBuffer_rewind_to(bbP, end_byte_position + hole_size);
    }

    return MIH_PRIMITIVE_ENCODE_OK;
}
