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
#define MIH_C_HEADER_CODEC_C
#include "MIH_C_header_codec.h"
//-----------------------------------------------------------------------------
int MIH_C_HEADER2String(MIH_C_HEADER_T* headerP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "HEADER.version:             ");
    buffer_index += MIH_C_VERSION2String(&headerP->version, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nHEADER.ack_req:             ");
    buffer_index += MIH_C_ACK_REQ2String(&headerP->ack_req, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nHEADER.ack_rsp:             ");
    buffer_index += MIH_C_ACK_RSP2String(&headerP->ack_rsp, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nHEADER.uir:                 ");
    buffer_index += MIH_C_UIR2String(&headerP->uir, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nHEADER.more_fragment:       ");
    buffer_index += MIH_C_M2String(&headerP->more_fragment, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nHEADER.fragment_number:     ");
    buffer_index += MIH_C_FN2String(&headerP->more_fragment, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nHEADER.service_identifier:  ");
    buffer_index += MIH_C_SID2String(&headerP->service_identifier, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nHEADER.operation_code:      ");
    buffer_index += MIH_C_OPCODE2String(&headerP->operation_code, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nHEADER.action_identifier:   ");
    buffer_index += MIH_C_AID2String(&headerP->action_identifier, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nHEADER.transaction_id:      ");
    buffer_index += MIH_C_TRANSACTION_ID2String(&headerP->transaction_id, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nHEADER.payload_length:      ");
    buffer_index += MIH_C_VARIABLE_PAYLOAD_LENGTH2String(&headerP->payload_length, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\n");
    return buffer_index;
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Header_Encode(Bit_Buffer_t* bbP, MIH_C_HEADER_T* headerP) {
//-----------------------------------------------------------------------------
    u_int8_t                 reserved = 0;
    int                      status ;
    if ((bbP != NULL) && (headerP != NULL)) {
        status                       = MIH_HEADER_ENCODE_OK;
        BitBuffer_write8b    (bbP, headerP->version,            4);
        BitBuffer_writeBool  (bbP, headerP->ack_req);
        BitBuffer_writeBool  (bbP, headerP->ack_rsp);
        BitBuffer_writeBool  (bbP, headerP->uir);
        BitBuffer_writeBool  (bbP, headerP->more_fragment);
        BitBuffer_write8b    (bbP, headerP->fragment_number,    7);
        BitBuffer_writeBool  (bbP, reserved);
        BitBuffer_write8b    (bbP, headerP->service_identifier, 4);
        BitBuffer_write8b    (bbP, headerP->operation_code,     2);
        BitBuffer_write16b   (bbP, headerP->action_identifier, 10);
        BitBuffer_write8b    (bbP, reserved,                    4);
        BitBuffer_write16b   (bbP, headerP->transaction_id,    12);
        BitBuffer_write16    (bbP, headerP->payload_length);
        /*DEBUG_ENCODE("%s: version                0x%02X\n", __FUNCTION__, headerP->version);
        DEBUG_ENCODE("%s: ack_req                0x%02X\n", __FUNCTION__, headerP->ack_req);
        DEBUG_ENCODE("%s: ack_rsp                0x%02X\n", __FUNCTION__, headerP->ack_rsp);
        DEBUG_ENCODE("%s: uir                    0x%02X\n", __FUNCTION__, headerP->uir);
        DEBUG_ENCODE("%s: more_fragment          0x%02X\n", __FUNCTION__, headerP->more_fragment);
        DEBUG_ENCODE("%s: reserved               0x%02X\n", __FUNCTION__, reserved);
        DEBUG_ENCODE("%s: service_identifier     0x%02X\n", __FUNCTION__, headerP->service_identifier);
        DEBUG_ENCODE("%s: operation_code         0x%02X\n", __FUNCTION__, headerP->operation_code);
        DEBUG_ENCODE("%s: action_identifier      0x%04X\n", __FUNCTION__, headerP->action_identifier);
        DEBUG_ENCODE("%s: reserved               0x%02X\n", __FUNCTION__, reserved);
        DEBUG_ENCODE("%s: transaction_id         0x%04X\n", __FUNCTION__, headerP->transaction_id);
        DEBUG_ENCODE("%s: payload_length         0x%02X\n", __FUNCTION__, headerP->payload_length);*/

        // may be check for validity of header field -> set MIH_HEADER_DECODE_FAILURE
        if (BitBuffer_isCheckWriteOverflowOK(bbP, 0) == BIT_BUFFER_FALSE) {
            memset(headerP, 0, sizeof(MIH_C_HEADER_T));
            return MIH_HEADER_ENCODE_TOO_SHORT;
        } else if (status == MIH_HEADER_ENCODE_FAILURE) {
            memset(headerP, 0, sizeof(MIH_C_HEADER_T));
            return MIH_HEADER_ENCODE_FAILURE;
        } else {
            return MIH_HEADER_ENCODE_OK;
        }
    } else {
        memset(headerP, 0, sizeof(MIH_C_HEADER_T));
        return MIH_HEADER_ENCODE_BAD_PARAMETER;
    }
}
//-----------------------------------------------------------------------------
int MIH_C_Link_Header_Decode(Bit_Buffer_t* bbP, MIH_C_HEADER_T* headerP) {
//-----------------------------------------------------------------------------
    u_int8_t                 reserved ;
    int                      status ;
    if ((bbP != NULL) && (headerP != NULL)) {
        status                       = MIH_HEADER_DECODE_OK;
        headerP->version             = (MIH_C_VERSION_T)BitBuffer_read(bbP, 4);
        headerP->ack_req             = (MIH_C_ACK_REQ_T)BitBuffer_readBool(bbP);
        headerP->ack_rsp             = (MIH_C_ACK_RSP_T)BitBuffer_readBool(bbP);
        headerP->uir                 = (MIH_C_UIR_T)BitBuffer_readBool(bbP);
        headerP->more_fragment       = (MIH_C_M_T)BitBuffer_readBool(bbP);
        headerP->fragment_number     = (MIH_C_FN_T)BitBuffer_read(bbP, 7);
        reserved                     = BitBuffer_readBool(bbP);
        headerP->service_identifier  = (MIH_C_SID_T)                    BitBuffer_read(bbP, 4);
        headerP->operation_code      = (MIH_C_OPCODE_T)                 BitBuffer_read(bbP, 2);
        headerP->action_identifier   = (MIH_C_AID_T)                    BitBuffer_read(bbP, 10);
        reserved                     =                                  BitBuffer_read(bbP, 4);
        headerP->transaction_id      = (MIH_C_TRANSACTION_ID_T)         BitBuffer_read(bbP, 12);
        headerP->payload_length      = (MIH_C_VARIABLE_PAYLOAD_LENGTH_T)BitBuffer_read16(bbP);

        /*DEBUG_DECODE("%s: version                0x%02X\n", __FUNCTION__, headerP->version);
        DEBUG_DECODE("%s: ack_req                0x%02X\n", __FUNCTION__, headerP->ack_req);
        DEBUG_DECODE("%s: ack_rsp                0x%02X\n", __FUNCTION__, headerP->ack_rsp);
        DEBUG_DECODE("%s: uir                    0x%02X\n", __FUNCTION__, headerP->uir);
        DEBUG_DECODE("%s: more_fragment          0x%02X\n", __FUNCTION__, headerP->more_fragment);
        DEBUG_DECODE("%s: reserved               0x%02X\n", __FUNCTION__, reserved);
        DEBUG_DECODE("%s: service_identifier     0x%02X\n", __FUNCTION__, headerP->service_identifier);
        DEBUG_DECODE("%s: operation_code         0x%02X\n", __FUNCTION__, headerP->operation_code);
        DEBUG_DECODE("%s: action_identifier      0x%04X\n", __FUNCTION__, headerP->action_identifier);
        DEBUG_DECODE("%s: reserved               0x%02X\n", __FUNCTION__, reserved);
        DEBUG_DECODE("%s: transaction_id         0x%04X\n", __FUNCTION__, headerP->transaction_id);
        DEBUG_DECODE("%s: payload_length         0x%02X\n", __FUNCTION__, headerP->payload_length);*/
        // may be check for validity of header field -> set MIH_HEADER_DECODE_FAILURE

        if (BitBuffer_isCheckReadOverflowOK(bbP, 0) == BIT_BUFFER_FALSE) {
            memset(headerP, 0, sizeof(MIH_C_HEADER_T));
            return MIH_HEADER_DECODE_TOO_SHORT;
        } else if (status == MIH_HEADER_DECODE_FAILURE) {
            memset(headerP, 0, sizeof(MIH_C_HEADER_T));
            return MIH_HEADER_DECODE_FAILURE;
        } else {
            return MIH_HEADER_DECODE_OK;
        }
    } else {
        memset(headerP, 0, sizeof(MIH_C_HEADER_T));
        return MIH_HEADER_DECODE_BAD_PARAMETER;
    }
}
