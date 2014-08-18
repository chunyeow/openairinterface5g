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
#define MIH_C_MSG_CODEC_C
#include "MIH_C_msg_codec.h"

//-----------------------------------------------------------------------------
MIH_C_TRANSACTION_ID_T MIH_C_get_new_transaction_id(void) {
//-----------------------------------------------------------------------------
    // no need now for mutex
    g_transaction_id_generator += 1;
    return g_transaction_id_generator;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Decode_Link_Capability_Discover_request(Bit_Buffer_t* bbP, MIH_C_Message_Link_Capability_Discover_request_t *messageP) {
//-----------------------------------------------------------------------------
    int                status;
    MIH_C_TLV_TYPE_T   tlv;
    u_int16_t          length;

    // SOURCE
    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    MIH_C_LIST_LENGTH_decode(bbP, &length);

    if (tlv == MIH_C_TLV_SOURCE_MIHF_ID) {
        MIH_C_MIHF_ID_decode(bbP, &messageP->source);
    } else {
        printf("[MIH_C] ERROR  %s() -> MIH_MESSAGE_DECODE_FAILURE @ MIH_C_TLV_SOURCE_MIHF_ID\n", __FUNCTION__);
        return MIH_MESSAGE_DECODE_FAILURE;
    }

    // DESTINATION
    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    MIH_C_LIST_LENGTH_decode(bbP, &length);
    if (tlv == MIH_C_TLV_DESTINATION_MIHF_ID) {
        MIH_C_MIHF_ID_decode(bbP, &messageP->destination);
    } else {
        printf("[MIH_C] ERROR  %s() -> MIH_MESSAGE_DECODE_FAILURE @ MIH_C_TLV_DESTINATION_MIHF_ID\n", __FUNCTION__);
        return MIH_MESSAGE_DECODE_FAILURE;
    }

    status = MIH_C_Link_Primitive_Decode_Link_Capability_Discover_request(bbP, &messageP->primitive);
    if (status == MIH_PRIMITIVE_DECODE_OK) {
        printf("[MIH_C] %s() -> MIH_MESSAGE_DECODE_OK \n", __FUNCTION__);
        return MIH_MESSAGE_DECODE_OK;
    }
    if (status == MIH_PRIMITIVE_DECODE_TOO_SHORT) {
        printf("[MIH_C] ERROR  %s() -> MIH_MESSAGE_DECODE_TOO_SHORT \n", __FUNCTION__);
        return MIH_MESSAGE_DECODE_TOO_SHORT;
    }
    if (status == MIH_PRIMITIVE_DECODE_FAILURE) {
        printf("[MIH_C] ERROR  %s() -> MIH_MESSAGE_DECODE_FAILURE \n", __FUNCTION__);
        return MIH_MESSAGE_DECODE_FAILURE;
    }
    if (status == MIH_PRIMITIVE_DECODE_BAD_PARAMETER) {
        printf("[MIH_C] ERROR  %s() -> MIH_MESSAGE_DECODE_BAD_PARAMETER \n", __FUNCTION__);
        return MIH_MESSAGE_DECODE_BAD_PARAMETER;
    }
    return status;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Link_Capability_Discover_request2String(MIH_C_Message_Link_Capability_Discover_request_t *messageP, char* bufP) {
//-----------------------------------------------------------------------------

    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "\n--------------------------------------------------------------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "-     MESSAGE Link_Capability_Discover.request                                       -\n");
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------------------------------------------\n");
    buffer_index += MIH_C_HEADER2String(&messageP->header, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "-------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "SOURCE:      ");
    buffer_index += MIH_C_MIHF_ID2String(&messageP->source, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nDESTINATION: ");
    buffer_index += MIH_C_MIHF_ID2String(&messageP->destination, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\n");
    buffer_index += MIH_C_Link_Primitive_Link_Capability_Discover_request2String(&messageP->primitive, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------------------------------------------\n");
    return buffer_index;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Decode_Link_Event_Subscribe_request(Bit_Buffer_t* bbP, MIH_C_Message_Link_Event_Subscribe_request_t *messageP){
//-----------------------------------------------------------------------------
    int                status;
    MIH_C_TLV_TYPE_T   tlv;
    u_int16_t          length;

    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    MIH_C_LIST_LENGTH_decode(bbP, &length);
    if (tlv == MIH_C_TLV_SOURCE_MIHF_ID) {
        MIH_C_MIHF_ID_decode(bbP, &messageP->source);
    } else {
        printf("[MIH_C] ERROR  %s() -> MIH_MESSAGE_DECODE_FAILURE @ MIH_C_TLV_SOURCE_MIHF_ID\n", __FUNCTION__);
        return MIH_MESSAGE_DECODE_FAILURE;
    }


    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    MIH_C_LIST_LENGTH_decode(bbP, &length);
    if (tlv == MIH_C_TLV_DESTINATION_MIHF_ID) {
        MIH_C_MIHF_ID_decode(bbP, &messageP->destination);
    } else {
        printf("[MIH_C] ERROR  %s() -> MIH_MESSAGE_DECODE_FAILURE @ MIH_C_TLV_DESTINATION_MIHF_ID\n", __FUNCTION__);
        return MIH_MESSAGE_DECODE_FAILURE;
    }

    status = MIH_C_Link_Primitive_Decode_Link_Event_Subscribe_request(bbP, &messageP->primitive);
    if (status == MIH_PRIMITIVE_DECODE_OK) {
        printf("[MIH_C] %s() -> MIH_MESSAGE_DECODE_OK \n", __FUNCTION__);
        return MIH_MESSAGE_DECODE_OK;
    }
    if (status == MIH_PRIMITIVE_DECODE_TOO_SHORT) {
        printf("[MIH_C] ERROR  %s() -> MIH_MESSAGE_DECODE_TOO_SHORT \n", __FUNCTION__);
        return MIH_MESSAGE_DECODE_TOO_SHORT;
    }
    if (status == MIH_PRIMITIVE_DECODE_FAILURE) {
        printf("[MIH_C] ERROR  %s() -> MIH_MESSAGE_DECODE_FAILURE \n", __FUNCTION__);
        return MIH_MESSAGE_DECODE_FAILURE;
    }
    if (status == MIH_PRIMITIVE_DECODE_BAD_PARAMETER) {
        printf("[MIH_C] ERROR  %s() -> MIH_MESSAGE_DECODE_BAD_PARAMETER \n", __FUNCTION__);
        return MIH_MESSAGE_DECODE_BAD_PARAMETER;
    }
    return status;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Link_Event_Subscribe_request2String(MIH_C_Message_Link_Event_Subscribe_request_t *messageP, char* bufP) {
//-----------------------------------------------------------------------------

    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "\n--------------------------------------------------------------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "-     MESSAGE Link_Event_Subscribe.request                                       -\n");
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------------------------------------------\n");
    buffer_index += MIH_C_HEADER2String(&messageP->header, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "-------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "SOURCE:      ");
    buffer_index += MIH_C_MIHF_ID2String(&messageP->source, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nDESTINATION: ");
    buffer_index += MIH_C_MIHF_ID2String(&messageP->destination, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\n");
    buffer_index += MIH_C_Link_Primitive_Link_Event_Subscribe_request2String(&messageP->primitive, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------------------------------------------\n");
    return buffer_index;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Decode_Link_Event_Unsubscribe_request(Bit_Buffer_t* bbP, MIH_C_Message_Link_Event_Unsubscribe_request_t *messageP) {
//-----------------------------------------------------------------------------
    int                status;
    MIH_C_TLV_TYPE_T   tlv;
    u_int16_t          length;

    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    MIH_C_LIST_LENGTH_decode(bbP, &length);
    if (tlv == MIH_C_TLV_SOURCE_MIHF_ID) {
        MIH_C_MIHF_ID_decode(bbP, &messageP->source);
    } else {
        printf("[MIH_C] ERROR  %s() -> MIH_MESSAGE_DECODE_FAILURE @ MIH_C_TLV_SOURCE_MIHF_ID\n", __FUNCTION__);
        return MIH_PRIMITIVE_DECODE_FAILURE;
    }


    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    MIH_C_LIST_LENGTH_decode(bbP, &length);
    if (tlv == MIH_C_TLV_DESTINATION_MIHF_ID) {
        MIH_C_MIHF_ID_decode(bbP, &messageP->destination);
    } else {
        printf("[MIH_C] ERROR  %s() -> MIH_MESSAGE_DECODE_FAILURE @ MIH_C_TLV_DESTINATION_MIHF_ID\n", __FUNCTION__);
        return MIH_PRIMITIVE_DECODE_FAILURE;
    }

    status = MIH_C_Link_Primitive_Decode_Link_Event_Unsubscribe_request(bbP, &messageP->primitive);
    if (status == MIH_PRIMITIVE_DECODE_OK) {
        printf("[MIH_C] %s() -> MIH_MESSAGE_DECODE_OK \n", __FUNCTION__);
        return MIH_MESSAGE_DECODE_OK;
    }
    if (status == MIH_PRIMITIVE_DECODE_TOO_SHORT) {
        printf("[MIH_C] ERROR  %s() -> MIH_MESSAGE_DECODE_TOO_SHORT \n", __FUNCTION__);
        return MIH_MESSAGE_DECODE_TOO_SHORT;
    }
    if (status == MIH_PRIMITIVE_DECODE_FAILURE) {
        printf("[MIH_C] ERROR  %s() -> MIH_MESSAGE_DECODE_FAILURE \n", __FUNCTION__);
        return MIH_MESSAGE_DECODE_FAILURE;
    }
    if (status == MIH_PRIMITIVE_DECODE_BAD_PARAMETER) {
        printf("[MIH_C] ERROR  %s() -> MIH_MESSAGE_DECODE_BAD_PARAMETER \n", __FUNCTION__);
        return MIH_MESSAGE_DECODE_BAD_PARAMETER;
    }
    return status;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Link_Event_Unsubscribe_request2String(MIH_C_Message_Link_Event_Unsubscribe_request_t *messageP, char* bufP) {
//-----------------------------------------------------------------------------

    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "\n--------------------------------------------------------------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "-     MESSAGE Link_Event_Unsubscribe.request                                       -\n");
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------------------------------------------\n");
    buffer_index += MIH_C_HEADER2String(&messageP->header, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "-------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "SOURCE:      ");
    buffer_index += MIH_C_MIHF_ID2String(&messageP->source, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nDESTINATION: ");
    buffer_index += MIH_C_MIHF_ID2String(&messageP->destination, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\n");
    buffer_index += MIH_C_Link_Primitive_Link_Event_Unsubscribe_request2String(&messageP->primitive, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------------------------------------------\n");
    return buffer_index;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Decode_Link_Get_Parameters_request(Bit_Buffer_t* bbP, MIH_C_Message_Link_Get_Parameters_request_t *messageP) {
//-----------------------------------------------------------------------------
    int                status;
    MIH_C_TLV_TYPE_T   tlv;
    u_int16_t          length;

    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    MIH_C_LIST_LENGTH_decode(bbP, &length);
    if (tlv == MIH_C_TLV_SOURCE_MIHF_ID) { MIH_C_MIHF_ID_decode(bbP, &messageP->source);} else {return MIH_PRIMITIVE_DECODE_FAILURE;}


    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    MIH_C_LIST_LENGTH_decode(bbP, &length);
    if (tlv == MIH_C_TLV_DESTINATION_MIHF_ID) { MIH_C_MIHF_ID_decode(bbP, &messageP->destination);} else {return MIH_PRIMITIVE_DECODE_FAILURE;}

    status = MIH_C_Link_Primitive_Decode_Link_Get_Parameters_request(bbP, &messageP->primitive);
    if (status == MIH_PRIMITIVE_DECODE_OK) {
        return MIH_MESSAGE_DECODE_OK;
    }
    if (status == MIH_PRIMITIVE_DECODE_TOO_SHORT) {
        return MIH_MESSAGE_DECODE_TOO_SHORT;
    }
    if (status == MIH_PRIMITIVE_DECODE_FAILURE) {
        return MIH_MESSAGE_DECODE_FAILURE;
    }
    if (status == MIH_PRIMITIVE_DECODE_BAD_PARAMETER) {
        return MIH_MESSAGE_DECODE_BAD_PARAMETER;
    }
    return status;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Link_Get_Parameters_request2String(MIH_C_Message_Link_Get_Parameters_request_t *messageP, char* bufP) {
//-----------------------------------------------------------------------------

    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "\n--------------------------------------------------------------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "-     MESSAGE Link_Get_Parameters.request                                       -\n");
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------------------------------------------\n");
    buffer_index += MIH_C_HEADER2String(&messageP->header, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "-------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "SOURCE:      ");
    buffer_index += MIH_C_MIHF_ID2String(&messageP->source, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nDESTINATION: ");
    buffer_index += MIH_C_MIHF_ID2String(&messageP->destination, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\n");
    buffer_index += MIH_C_Link_Primitive_Link_Get_Parameters_request2String(&messageP->primitive, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------------------------------------------\n");
    return buffer_index;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Decode_Link_Configure_Thresholds_request(Bit_Buffer_t* bbP, MIH_C_Message_Link_Configure_Thresholds_request_t *messageP) {
//-----------------------------------------------------------------------------
    int                status;
    MIH_C_TLV_TYPE_T   tlv;
    u_int16_t          length;

    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    MIH_C_LIST_LENGTH_decode(bbP, &length);
    if (tlv == MIH_C_TLV_SOURCE_MIHF_ID) { MIH_C_MIHF_ID_decode(bbP, &messageP->source);} else {return MIH_PRIMITIVE_DECODE_FAILURE;}


    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    MIH_C_LIST_LENGTH_decode(bbP, &length);
    if (tlv == MIH_C_TLV_DESTINATION_MIHF_ID) { MIH_C_MIHF_ID_decode(bbP, &messageP->destination);} else {return MIH_PRIMITIVE_DECODE_FAILURE;}

    status = MIH_C_Link_Primitive_Decode_Link_Configure_Thresholds_request(bbP, &messageP->primitive);
    if (status == MIH_PRIMITIVE_DECODE_OK) {
        return MIH_MESSAGE_DECODE_OK;
    }
    if (status == MIH_PRIMITIVE_DECODE_TOO_SHORT) {
        return MIH_MESSAGE_DECODE_TOO_SHORT;
    }
    if (status == MIH_PRIMITIVE_DECODE_FAILURE) {
        return MIH_MESSAGE_DECODE_FAILURE;
    }
    if (status == MIH_PRIMITIVE_DECODE_BAD_PARAMETER) {
        return MIH_MESSAGE_DECODE_BAD_PARAMETER;
    }
    return status;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Link_Configure_Thresholds_request2String(MIH_C_Message_Link_Configure_Thresholds_request_t *messageP, char* bufP) {
//-----------------------------------------------------------------------------

    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "\n--------------------------------------------------------------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "-     MESSAGE Link_Configure_Threshold.request                                       -\n");
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------------------------------------------\n");
    buffer_index += MIH_C_HEADER2String(&messageP->header, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "-------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "SOURCE:      ");
    buffer_index += MIH_C_MIHF_ID2String(&messageP->source, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nDESTINATION: ");
    buffer_index += MIH_C_MIHF_ID2String(&messageP->destination, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\n");
    buffer_index += MIH_C_Link_Primitive_Link_Configure_Thresholds_request2String(&messageP->primitive, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------------------------------------------\n");
    return buffer_index;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Decode_Link_Action_request(Bit_Buffer_t* bbP, MIH_C_Message_Link_Action_request_t *messageP) {
//-----------------------------------------------------------------------------
    int                status;
    MIH_C_TLV_TYPE_T   tlv;
    u_int16_t          length;

    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    MIH_C_LIST_LENGTH_decode(bbP, &length);
    if (tlv == MIH_C_TLV_SOURCE_MIHF_ID) { MIH_C_MIHF_ID_decode(bbP, &messageP->source);} else {return MIH_PRIMITIVE_DECODE_FAILURE;}


    MIH_C_TLV_TYPE_decode(bbP, &tlv);
    MIH_C_LIST_LENGTH_decode(bbP, &length);
    if (tlv == MIH_C_TLV_DESTINATION_MIHF_ID) { MIH_C_MIHF_ID_decode(bbP, &messageP->destination);} else {return MIH_PRIMITIVE_DECODE_FAILURE;}

    status = MIH_C_Link_Primitive_Decode_Link_Action_request(bbP, &messageP->primitive);
    if (status == MIH_PRIMITIVE_DECODE_OK) {
        return MIH_MESSAGE_DECODE_OK;
    }
    if (status == MIH_PRIMITIVE_DECODE_TOO_SHORT) {
        return MIH_MESSAGE_DECODE_TOO_SHORT;
    }
    if (status == MIH_PRIMITIVE_DECODE_FAILURE) {
        return MIH_MESSAGE_DECODE_FAILURE;
    }
    if (status == MIH_PRIMITIVE_DECODE_BAD_PARAMETER) {
        return MIH_MESSAGE_DECODE_BAD_PARAMETER;
    }
    return status;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Link_Action_request2String(MIH_C_Message_Link_Action_request_t *messageP, char* bufP) {
//-----------------------------------------------------------------------------

    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "\n--------------------------------------------------------------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "-     MESSAGE Link_Action.request                                       -\n");
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------------------------------------------\n");
    buffer_index += MIH_C_HEADER2String(&messageP->header, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "-------------------------------\n");
    buffer_index += sprintf(&bufP[buffer_index], "SOURCE:      ");
    buffer_index += MIH_C_MIHF_ID2String(&messageP->source, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nDESTINATION: ");
    buffer_index += MIH_C_MIHF_ID2String(&messageP->destination, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\n");
    buffer_index += MIH_C_Link_Primitive_Link_Action_request2String(&messageP->primitive, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "--------------------------------------------------------------------------------------\n");
    return buffer_index;
}
#ifdef MIH_C_MEDIEVAL_EXTENSIONS
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Encode_Link_Register_indication(Bit_Buffer_t* bbP, MIH_C_Message_Link_Register_indication_t *messageP) {
//-----------------------------------------------------------------------------
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    MIH_C_TLV_TYPE_T            tlv;
    u_int16_t                   encode_length;

    BitBuffer_rewind(bbP);
    bbP->m_byte_position                = MSG_MIH_HEADER_SIZE_IN_BYTES;

    // TYPE
    tlv = MIH_C_TLV_SOURCE_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->source);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);


    // TYPE
    tlv = MIH_C_TLV_DESTINATION_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->destination);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    MIH_C_Link_Primitive_Encode_Link_Register_indication(bbP, &messageP->primitive);

    saved_byte_position = BitBuffer_getPosition(bbP);
    bbP->m_byte_position = 0;
    messageP->header.payload_length = saved_byte_position - MSG_MIH_HEADER_SIZE_IN_BYTES;


    MIH_C_Link_Header_Encode(bbP, &messageP->header);

    bbP->m_byte_position = saved_byte_position;

    return saved_byte_position;
}
#endif
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Encode_Link_Detected_indication(Bit_Buffer_t* bbP, MIH_C_Message_Link_Detected_indication_t *messageP) {
//-----------------------------------------------------------------------------
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    MIH_C_TLV_TYPE_T            tlv;
    u_int16_t                   encode_length;

    BitBuffer_rewind(bbP);
    bbP->m_byte_position                = MSG_MIH_HEADER_SIZE_IN_BYTES;

    // TYPE
    tlv = MIH_C_TLV_SOURCE_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->source);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);


    // TYPE
    tlv = MIH_C_TLV_DESTINATION_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->destination);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    MIH_C_Link_Primitive_Encode_Link_Detected_indication(bbP, &messageP->primitive);

    saved_byte_position = BitBuffer_getPosition(bbP);
    bbP->m_byte_position = 0;
    messageP->header.payload_length = saved_byte_position - MSG_MIH_HEADER_SIZE_IN_BYTES;


    MIH_C_Link_Header_Encode(bbP, &messageP->header);

    bbP->m_byte_position = saved_byte_position;

    return saved_byte_position;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Encode_Link_Up_indication(Bit_Buffer_t* bbP, MIH_C_Message_Link_Up_indication_t *messageP) {
//-----------------------------------------------------------------------------
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    MIH_C_TLV_TYPE_T            tlv;
    u_int16_t                   encode_length;

    BitBuffer_rewind(bbP);
    bbP->m_byte_position                = MSG_MIH_HEADER_SIZE_IN_BYTES;

    // TYPE
    tlv = MIH_C_TLV_SOURCE_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->source);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);


    // TYPE
    tlv = MIH_C_TLV_DESTINATION_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->destination);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    MIH_C_Link_Primitive_Encode_Link_Up_indication(bbP, &messageP->primitive);

    saved_byte_position = BitBuffer_getPosition(bbP);
    bbP->m_byte_position = 0;
    messageP->header.payload_length = saved_byte_position - MSG_MIH_HEADER_SIZE_IN_BYTES;


    MIH_C_Link_Header_Encode(bbP, &messageP->header);

    bbP->m_byte_position = saved_byte_position;

    return saved_byte_position;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Encode_Link_Parameters_Report_indication(Bit_Buffer_t* bbP, MIH_C_Message_Link_Parameters_Report_indication_t *messageP) {
//-----------------------------------------------------------------------------
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    MIH_C_TLV_TYPE_T            tlv;
    u_int16_t                   encode_length;

    BitBuffer_rewind(bbP);
    bbP->m_byte_position                = MSG_MIH_HEADER_SIZE_IN_BYTES;

    // TYPE
    tlv = MIH_C_TLV_SOURCE_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->source);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);


    // TYPE
    tlv = MIH_C_TLV_DESTINATION_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->destination);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    MIH_C_Link_Primitive_Encode_Link_Parameters_Report_indication(bbP, &messageP->primitive);

    saved_byte_position = BitBuffer_getPosition(bbP);
    bbP->m_byte_position = 0;
    messageP->header.payload_length = saved_byte_position - MSG_MIH_HEADER_SIZE_IN_BYTES;


    MIH_C_Link_Header_Encode(bbP, &messageP->header);

    bbP->m_byte_position = saved_byte_position;

    return saved_byte_position;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Encode_Link_Going_Down_indication(Bit_Buffer_t* bbP, MIH_C_Message_Link_Going_Down_indication_t *messageP) {
//-----------------------------------------------------------------------------
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    MIH_C_TLV_TYPE_T            tlv;
    u_int16_t                   encode_length;

    BitBuffer_rewind(bbP);
    bbP->m_byte_position                = MSG_MIH_HEADER_SIZE_IN_BYTES;

    // TYPE
    tlv = MIH_C_TLV_SOURCE_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->source);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);


    // TYPE
    tlv = MIH_C_TLV_DESTINATION_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->destination);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    MIH_C_Link_Primitive_Encode_Link_Going_Down_indication(bbP, &messageP->primitive);

    saved_byte_position = BitBuffer_getPosition(bbP);
    bbP->m_byte_position = 0;
    messageP->header.payload_length = saved_byte_position - MSG_MIH_HEADER_SIZE_IN_BYTES;


    MIH_C_Link_Header_Encode(bbP, &messageP->header);

    bbP->m_byte_position = saved_byte_position;

    return saved_byte_position;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Encode_Link_Down_indication(Bit_Buffer_t* bbP, MIH_C_Message_Link_Down_indication_t *messageP) {
//-----------------------------------------------------------------------------
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    MIH_C_TLV_TYPE_T            tlv;
    u_int16_t                   encode_length;

    BitBuffer_rewind(bbP);
    bbP->m_byte_position                = MSG_MIH_HEADER_SIZE_IN_BYTES;

    // TYPE
    tlv = MIH_C_TLV_SOURCE_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->source);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);


    // TYPE
    tlv = MIH_C_TLV_DESTINATION_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->destination);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    MIH_C_Link_Primitive_Encode_Link_Down_indication(bbP, &messageP->primitive);

    saved_byte_position = BitBuffer_getPosition(bbP);
    bbP->m_byte_position = 0;
    messageP->header.payload_length = saved_byte_position - MSG_MIH_HEADER_SIZE_IN_BYTES;


    MIH_C_Link_Header_Encode(bbP, &messageP->header);

    bbP->m_byte_position = saved_byte_position;

    return saved_byte_position;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Encode_Link_Action_confirm(Bit_Buffer_t* bbP, MIH_C_Message_Link_Action_confirm_t *messageP) {
//-----------------------------------------------------------------------------
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    MIH_C_TLV_TYPE_T            tlv;
    u_int16_t                   encode_length;

    BitBuffer_rewind(bbP);
    bbP->m_byte_position                = MSG_MIH_HEADER_SIZE_IN_BYTES;

    // TYPE
    tlv = MIH_C_TLV_SOURCE_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->source);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);


    // TYPE
    tlv = MIH_C_TLV_DESTINATION_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->destination);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    MIH_C_Link_Primitive_Encode_Link_Action_confirm(bbP, &messageP->primitive);

    saved_byte_position = BitBuffer_getPosition(bbP);
    bbP->m_byte_position = 0;
    messageP->header.payload_length = saved_byte_position - MSG_MIH_HEADER_SIZE_IN_BYTES;


    MIH_C_Link_Header_Encode(bbP, &messageP->header);

    bbP->m_byte_position = saved_byte_position;

    return saved_byte_position;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Encode_Capability_Discover_confirm(Bit_Buffer_t* bbP, MIH_C_Message_Link_Capability_Discover_confirm_t *messageP) {
//-----------------------------------------------------------------------------
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    MIH_C_TLV_TYPE_T            tlv;
    u_int16_t                   encode_length;

    BitBuffer_rewind(bbP);
    bbP->m_byte_position                = MSG_MIH_HEADER_SIZE_IN_BYTES;

    // TYPE
    tlv = MIH_C_TLV_SOURCE_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->source);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);


    // TYPE
    tlv = MIH_C_TLV_DESTINATION_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->destination);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    //--------------------------------------------------------------------------
    MIH_C_Link_Primitive_Encode_Link_Capability_Discover_confirm(bbP, &messageP->primitive);

    saved_byte_position = BitBuffer_getPosition(bbP);
    bbP->m_byte_position = 0;
    messageP->header.payload_length = saved_byte_position - MSG_MIH_HEADER_SIZE_IN_BYTES;


    MIH_C_Link_Header_Encode(bbP, &messageP->header);

    bbP->m_byte_position = saved_byte_position;

    return saved_byte_position;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Encode_Event_Subscribe_confirm(Bit_Buffer_t* bbP, MIH_C_Message_Link_Event_Subscribe_confirm_t *messageP) {
//-----------------------------------------------------------------------------
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    MIH_C_TLV_TYPE_T            tlv;
    u_int16_t                   encode_length;

    BitBuffer_rewind(bbP);
    bbP->m_byte_position                = MSG_MIH_HEADER_SIZE_IN_BYTES;

    // TYPE
    tlv = MIH_C_TLV_SOURCE_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->source);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);


    // TYPE
    tlv = MIH_C_TLV_DESTINATION_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->destination);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    //--------------------------------------------------------------------------
    MIH_C_Link_Primitive_Encode_Link_Event_Subscribe_confirm(bbP, &messageP->primitive);

    saved_byte_position = BitBuffer_getPosition(bbP);
    bbP->m_byte_position = 0;
    messageP->header.payload_length = saved_byte_position - MSG_MIH_HEADER_SIZE_IN_BYTES;


    MIH_C_Link_Header_Encode(bbP, &messageP->header);

    bbP->m_byte_position = saved_byte_position;

    return saved_byte_position;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Encode_Event_Unsubscribe_confirm(Bit_Buffer_t* bbP, MIH_C_Message_Link_Event_Unsubscribe_confirm_t *messageP) {
//-----------------------------------------------------------------------------
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    MIH_C_TLV_TYPE_T            tlv;
    u_int16_t                   encode_length;

    BitBuffer_rewind(bbP);
    bbP->m_byte_position                = MSG_MIH_HEADER_SIZE_IN_BYTES;

    // TYPE
    tlv = MIH_C_TLV_SOURCE_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->source);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);


    // TYPE
    tlv = MIH_C_TLV_DESTINATION_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->destination);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    //--------------------------------------------------------------------------
    MIH_C_Link_Primitive_Encode_Link_Event_Unsubscribe_confirm(bbP, &messageP->primitive);

    saved_byte_position             = BitBuffer_getPosition(bbP);
    bbP->m_byte_position            = 0;
    messageP->header.payload_length = saved_byte_position - MSG_MIH_HEADER_SIZE_IN_BYTES;


    MIH_C_Link_Header_Encode(bbP, &messageP->header);

    bbP->m_byte_position = saved_byte_position;

    return saved_byte_position;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Encode_Configure_Thresholds_confirm(Bit_Buffer_t* bbP, MIH_C_Message_Link_Configure_Thresholds_confirm_t *messageP) {
//-----------------------------------------------------------------------------
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    MIH_C_TLV_TYPE_T            tlv;
    u_int16_t                   encode_length;

    BitBuffer_rewind(bbP);
    bbP->m_byte_position                = MSG_MIH_HEADER_SIZE_IN_BYTES;

    // TYPE
    tlv = MIH_C_TLV_SOURCE_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->source);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);


    // TYPE
    tlv = MIH_C_TLV_DESTINATION_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->destination);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    //--------------------------------------------------------------------------
    MIH_C_Link_Primitive_Encode_Link_Configure_Thresholds_confirm(bbP, &messageP->primitive);

    saved_byte_position = BitBuffer_getPosition(bbP);
    bbP->m_byte_position = 0;
    messageP->header.payload_length = saved_byte_position - MSG_MIH_HEADER_SIZE_IN_BYTES;


    MIH_C_Link_Header_Encode(bbP, &messageP->header);

    bbP->m_byte_position = saved_byte_position;

    return saved_byte_position;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Message_Encode_Get_Parameters_confirm(Bit_Buffer_t* bbP, MIH_C_Message_Link_Get_Parameters_confirm_t *messageP) {
//-----------------------------------------------------------------------------
    unsigned int                saved_byte_position;
    unsigned int                end_byte_position;
    unsigned int                hole_size;
    MIH_C_TLV_TYPE_T            tlv;
    u_int16_t                   encode_length;

    BitBuffer_rewind(bbP);
    bbP->m_byte_position                = MSG_MIH_HEADER_SIZE_IN_BYTES;

    // TYPE
    tlv = MIH_C_TLV_SOURCE_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->source);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);


    // TYPE
    tlv = MIH_C_TLV_DESTINATION_MIHF_ID;
    MIH_C_TLV_TYPE_encode(bbP, &tlv);

    // VALUE
    saved_byte_position = BitBuffer_getPosition(bbP);
    MIH_C_MIHF_ID_encode(bbP, &messageP->destination);
    end_byte_position = BitBuffer_getPosition(bbP);

    // LENGTH
    encode_length = end_byte_position - saved_byte_position;
    hole_size     = MIH_C_LIST_LENGTH_get_encode_num_bytes(encode_length);
    BitBuffer_write_shift_last_n_bytes_right(bbP, encode_length, hole_size);
    BitBuffer_rewind_to(bbP, saved_byte_position);
    MIH_C_LIST_LENGTH_encode(bbP, encode_length);
    BitBuffer_rewind_to(bbP, end_byte_position + hole_size);

    //--------------------------------------------------------------------------
    MIH_C_Link_Primitive_Encode_Link_Get_Parameters_confirm(bbP, &messageP->primitive);

    saved_byte_position = BitBuffer_getPosition(bbP);
    bbP->m_byte_position = 0;
    messageP->header.payload_length = saved_byte_position - MSG_MIH_HEADER_SIZE_IN_BYTES;


    MIH_C_Link_Header_Encode(bbP, &messageP->header);

    bbP->m_byte_position = saved_byte_position;

    return saved_byte_position;
}
