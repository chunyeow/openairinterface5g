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

#ifdef MIH_C_MEDIEVAL_EXTENSIONS
#define MIH_C_INTERFACE
#define MIH_C_MEDIEVAL_EXTENSIONS_C
#include "MIH_C_Medieval_extensions.h"

//-----------------------------------------------------------------------------
unsigned int MIH_C_PROTO2String(MIH_C_PROTO_T *protoP, char* bufP){
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    switch (*protoP) {
        case MIH_C_PROTO_TCP:   buffer_index += sprintf(&bufP[buffer_index], "TCP");break;
        case MIH_C_PROTO_UDP:   buffer_index += sprintf(&bufP[buffer_index], "UDP");break;
        default:                buffer_index += sprintf(&bufP[buffer_index], "UNKNOWN_PROTO");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_IP_TUPLE2String(MIH_C_IP_TUPLE_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;

    buffer_index += sprintf(&bufP[buffer_index], "IP_ADDR = ");
    buffer_index += MIH_C_IP_ADDR2String(&dataP->ip_addr, &bufP[buffer_index]);

    buffer_index += sprintf(&bufP[buffer_index], "\nPORT = ");
    buffer_index += MIH_C_PORT2String(&dataP->port, &bufP[buffer_index]);

    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_IP_TUPLE_encode(Bit_Buffer_t* bbP, MIH_C_IP_TUPLE_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_IP_ADDR_encode(bbP, &dataP->ip_addr);
    MIH_C_PORT_encode(bbP, &dataP->port);
}
//-----------------------------------------------------------------------------
inline void MIH_C_IP_TUPLE_decode(Bit_Buffer_t* bbP, MIH_C_IP_TUPLE_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_IP_ADDR_decode(bbP, &dataP->ip_addr);
    MIH_C_PORT_decode(bbP, &dataP->port);
}
/*//-----------------------------------------------------------------------------
unsigned int MIH_C_FLOW_ID2String(MIH_C_FLOW_ID_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;

    buffer_index += sprintf(&bufP[buffer_index], "IP_TUPLE SRC = ");
    buffer_index += MIH_C_IP_TUPLE2String(&dataP->source_addr, &bufP[buffer_index]);

    buffer_index += sprintf(&bufP[buffer_index], "\nIP_TUPLE DEST = ");
    buffer_index += MIH_C_IP_TUPLE2String(&dataP->dest_addr, &bufP[buffer_index]);

    buffer_index += sprintf(&bufP[buffer_index], "\nPROTO = ");
    buffer_index += MIH_C_PROTO2String(&dataP->transport_protocol, &bufP[buffer_index]);
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_FLOW_ID_encode(Bit_Buffer_t* bbP, MIH_C_FLOW_ID_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_IP_TUPLE_encode(bbP, &dataP->source_addr);
    MIH_C_IP_TUPLE_encode(bbP, &dataP->dest_addr);
    MIH_C_PROTO_encode(bbP, &dataP->transport_protocol);
}
//-----------------------------------------------------------------------------
inline void MIH_C_FLOW_ID_decode(Bit_Buffer_t* bbP, MIH_C_FLOW_ID_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_IP_TUPLE_decode(bbP, &dataP->source_addr);
    MIH_C_IP_TUPLE_decode(bbP, &dataP->dest_addr);
    MIH_C_PROTO_decode(bbP, &dataP->transport_protocol);
}*/
//-----------------------------------------------------------------------------
unsigned int MIH_C_MARK2String(MIH_C_MARK_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;

    switch (dataP->choice) {
        case 0:
            buffer_index += sprintf(&bufP[buffer_index], "DSCP MASK = ");
            buffer_index += MIH_C_BITMAP82String(&dataP->_union.dscp_mask, &bufP[buffer_index]);
            break;
        case 1:
            buffer_index += sprintf(&bufP[buffer_index], "FLOW LABEL = ");
            buffer_index += MIH_C_BITMAP242String(&dataP->_union.flow_label_mask, &bufP[buffer_index]);
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "MARK UNINITIALIZED ");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_MARK_encode(Bit_Buffer_t* bbP, MIH_C_MARK_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_encode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            MIH_C_BITMAP8_encode(bbP, &dataP->_union.dscp_mask);
            break;
        case 1:
            MIH_C_BITMAP24_encode(bbP, &dataP->_union.flow_label_mask);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE OF MIH_C_MARK_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_MARK_decode(Bit_Buffer_t* bbP, MIH_C_MARK_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_decode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            MIH_C_BITMAP8_decode(bbP, &dataP->_union.dscp_mask);
            break;
        case 1:
            MIH_C_BITMAP24_decode(bbP, &dataP->_union.flow_label_mask);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE OF MIH_C_MARK_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_QOS2String(MIH_C_QOS_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;

    switch (dataP->choice) {
        case 0:
            buffer_index += sprintf(&bufP[buffer_index], "MAX_DELAY = ");
            buffer_index += MIH_C_MAX_DELAY2String(&dataP->_union.mark_qos.max_delay, &bufP[buffer_index]);

            buffer_index += sprintf(&bufP[buffer_index], "BITRATE = ");
            buffer_index += MIH_C_BITRATE2String(&dataP->_union.mark_qos.bitrate, &bufP[buffer_index]);

            buffer_index += sprintf(&bufP[buffer_index], "JITTER = ");
            buffer_index += MIH_C_JITTER2String(&dataP->_union.mark_qos.jitter, &bufP[buffer_index]);

            buffer_index += sprintf(&bufP[buffer_index], "PKT_LOSS_RATE = ");
            buffer_index += MIH_C_PKT_LOSS_RATE2String(&dataP->_union.mark_qos.pkt_loss, &bufP[buffer_index]);
            break;
        case 1:
            buffer_index += sprintf(&bufP[buffer_index], "COS = ");
            buffer_index += MIH_C_COS2String(&dataP->_union.cos, &bufP[buffer_index]);
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "QOS UNINITIALIZED ");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_QOS_encode(Bit_Buffer_t* bbP, MIH_C_QOS_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_encode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            MIH_C_MAX_DELAY_encode(bbP, &dataP->_union.mark_qos.max_delay);
            MIH_C_BITRATE_encode(bbP, &dataP->_union.mark_qos.bitrate);
            MIH_C_JITTER_encode(bbP, &dataP->_union.mark_qos.jitter);
            MIH_C_PKT_LOSS_RATE_encode(bbP, &dataP->_union.mark_qos.pkt_loss);
            break;
        case 1:
            MIH_C_COS_encode(bbP, &dataP->_union.cos);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE OF MIH_C_QOS_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_QOS_decode(Bit_Buffer_t* bbP, MIH_C_QOS_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_decode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            MIH_C_MAX_DELAY_decode(bbP, &dataP->_union.mark_qos.max_delay);
            MIH_C_BITRATE_decode(bbP, &dataP->_union.mark_qos.bitrate);
            MIH_C_JITTER_decode(bbP, &dataP->_union.mark_qos.jitter);
            MIH_C_PKT_LOSS_RATE_decode(bbP, &dataP->_union.mark_qos.pkt_loss);
            break;
        case 1:
            MIH_C_COS_decode(bbP, &dataP->_union.cos);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE OF MIH_C_QOS_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_RESOURCE_DESC2String(MIH_C_RESOURCE_DESC_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;

    buffer_index += sprintf(&bufP[buffer_index], "LINK_ID = ");
    buffer_index += MIH_C_LINK_ID2String(&dataP->link_id, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "\nFLOW_ID = ");
    buffer_index += MIH_C_FLOW_ID2String(&dataP->flow_id, &bufP[buffer_index]);
    switch (dataP->choice_link_data_rate) {
        case 0:
            buffer_index += sprintf(&bufP[buffer_index], "\nLINK_DATA_RATE = NULL");
            break;
        case 1:
            buffer_index += sprintf(&bufP[buffer_index], "\nLINK_DATA_RATE = ");
            buffer_index += MIH_C_LINK_DATA_RATE2String(&dataP->_union_link_data_rate.link_data_rate, &bufP[buffer_index]);
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "\nLINK_DATA_RATE UNINITIALIZED ");
    }
    switch (dataP->choice_qos) {
        case 0:
            buffer_index += sprintf(&bufP[buffer_index], "\nQOS = NULL");
            break;
        case 1:
            buffer_index += sprintf(&bufP[buffer_index], "\nQOS = ");
            buffer_index += MIH_C_QOS2String(&dataP->_union_qos.qos, &bufP[buffer_index]);
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "\nQOS UNINITIALIZED ");
    }
    switch (dataP->choice_jumbo_enable) {
        case 0:
            buffer_index += sprintf(&bufP[buffer_index], "\nJUMBO_ENABLE = NULL");
            break;
        case 1:
            buffer_index += sprintf(&bufP[buffer_index], "\nJUMBO_ENABLE = ");
            buffer_index += MIH_C_JUMBO_ENABLE2String(&dataP->_union_jumbo_enable.jumbo_enable, &bufP[buffer_index]);
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "\nJUMBO_ENABLE UNINITIALIZED ");
    }
    switch (dataP->choice_multicast_enable) {
        case 0:
            buffer_index += sprintf(&bufP[buffer_index], "\nMULTICAST_ENABLE = NULL");
            break;
        case 1:
            buffer_index += sprintf(&bufP[buffer_index], "\nMULTICAST_ENABLE = ");
            buffer_index += MIH_C_MULTICAST_ENABLE2String(&dataP->_union_multicast_enable.multicast_enable, &bufP[buffer_index]);
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "\nMULTICAST_ENABLE UNINITIALIZED ");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_RESOURCE_DESC_encode(Bit_Buffer_t* bbP, MIH_C_RESOURCE_DESC_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_ID_encode(bbP, &dataP->link_id);
    MIH_C_FLOW_ID_encode(bbP, &dataP->flow_id);

    MIH_C_CHOICE_encode(bbP, &dataP->choice_link_data_rate);
    switch (dataP->choice_link_data_rate) {
        case 0:
            break;
        case 1:
            MIH_C_LINK_DATA_RATE_encode(bbP, &dataP->_union_link_data_rate.link_data_rate);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE choice_link_data_rate OF MIH_C_RESOURCE_DESC_T %d\n", dataP->choice_link_data_rate);
    }
    MIH_C_CHOICE_encode(bbP, &dataP->choice_qos);
    switch (dataP->choice_qos) {
        case 0:
            break;
        case 1:
            MIH_C_QOS_encode(bbP, &dataP->_union_qos.qos);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE choice_qos OF MIH_C_RESOURCE_DESC_T %d\n", dataP->choice_qos);
    }
    MIH_C_CHOICE_encode(bbP, &dataP->choice_jumbo_enable);
    switch (dataP->choice_jumbo_enable) {
        case 0:
            break;
        case 1:
            MIH_C_JUMBO_ENABLE_encode(bbP, &dataP->_union_jumbo_enable.jumbo_enable);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE choice_jumbo_enable OF MIH_C_RESOURCE_DESC_T %d\n", dataP->choice_jumbo_enable);
    }
    MIH_C_CHOICE_encode(bbP, &dataP->choice_multicast_enable);
    switch (dataP->choice_multicast_enable) {
        case 0:
            break;
        case 1:
            MIH_C_MULTICAST_ENABLE_encode(bbP, &dataP->_union_multicast_enable.multicast_enable);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE choice_multicast_enable OF MIH_C_RESOURCE_DESC_T %d\n", dataP->choice_multicast_enable);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_RESOURCE_DESC_decode(Bit_Buffer_t* bbP, MIH_C_RESOURCE_DESC_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_LINK_ID_decode(bbP, &dataP->link_id);
    MIH_C_FLOW_ID_decode(bbP, &dataP->flow_id);

    MIH_C_CHOICE_decode(bbP, &dataP->choice_link_data_rate);
    switch (dataP->choice_link_data_rate) {
        case 0:
            break;
        case 1:
            MIH_C_LINK_DATA_RATE_decode(bbP, &dataP->_union_link_data_rate.link_data_rate);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE choice_link_data_rate OF MIH_C_RESOURCE_DESC_T %d\n", dataP->choice_link_data_rate);
    }
    MIH_C_CHOICE_decode(bbP, &dataP->choice_qos);
    switch (dataP->choice_qos) {
        case 0:
            break;
        case 1:
            MIH_C_QOS_decode(bbP, &dataP->_union_qos.qos);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE choice_qos OF MIH_C_RESOURCE_DESC_T %d\n", dataP->choice_qos);
    }
    MIH_C_CHOICE_decode(bbP, &dataP->choice_jumbo_enable);
    switch (dataP->choice_jumbo_enable) {
        case 0:
            break;
        case 1:
            MIH_C_JUMBO_ENABLE_decode(bbP, &dataP->_union_jumbo_enable.jumbo_enable);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE choice_jumbo_enable OF MIH_C_RESOURCE_DESC_T %d\n", dataP->choice_jumbo_enable);
    }
    MIH_C_CHOICE_decode(bbP, &dataP->choice_multicast_enable);
    switch (dataP->choice_multicast_enable) {
        case 0:
            break;
        case 1:
            MIH_C_MULTICAST_ENABLE_decode(bbP, &dataP->_union_multicast_enable.multicast_enable);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE choice_multicast_enable OF MIH_C_RESOURCE_DESC_T %d\n", dataP->choice_multicast_enable);
    }
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_FLOW_ATTRIBUTE2String(MIH_C_FLOW_ATTRIBUTE_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    switch (dataP->choice_multicast_enable) {
        case 0:
            buffer_index += sprintf(&bufP[buffer_index], "MULTICAST_ENABLE = NULL");
            break;
        case 1:
            buffer_index += sprintf(&bufP[buffer_index], "MULTICAST_ENABLE = ");
            buffer_index += MIH_C_MULTICAST_ENABLE2String(&dataP->_union_multicast_enable.multicast_enable, &bufP[buffer_index]);
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "MULTICAST_ENABLE UNINITIALIZED ");
    }
    switch (dataP->choice_mark_qos) {
        case 0:
            buffer_index += sprintf(&bufP[buffer_index], "\nMARK_QOS = NULL");
            break;
        case 1:
            buffer_index += sprintf(&bufP[buffer_index], "\nMARK = ");
            buffer_index += MIH_C_MARK2String(&dataP->_union_mark_qos.mark_qos.mark, &bufP[buffer_index]);
            buffer_index += sprintf(&bufP[buffer_index], "\nQOS = ");
            buffer_index += MIH_C_QOS2String(&dataP->_union_mark_qos.mark_qos.qos, &bufP[buffer_index]);
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "\nMARK_QOS UNINITIALIZED ");
    }
    switch (dataP->choice_mark_drop_eligibility) {
        case 0:
            buffer_index += sprintf(&bufP[buffer_index], "\nMARK_DROP_ELIGIBILITY = NULL");
            break;
        case 1:
            buffer_index += sprintf(&bufP[buffer_index], "\nMARK = ");
            buffer_index += MIH_C_MARK2String(&dataP->_union_mark_drop_eligibility.mark_drop_eligibility.mark, &bufP[buffer_index]);
            buffer_index += sprintf(&bufP[buffer_index], "\nDROP_ELIGIBILITY = ");
            buffer_index += MIH_C_DROP_ELIGIBILITY2String(&dataP->_union_mark_drop_eligibility.mark_drop_eligibility.drop_eligibility, &bufP[buffer_index]);
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "\nMARK_DROP_ELIGIBILITY UNINITIALIZED ");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_FLOW_ATTRIBUTE_encode(Bit_Buffer_t* bbP, MIH_C_FLOW_ATTRIBUTE_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_FLOW_ID_encode(bbP, &dataP->flow_id);

    MIH_C_CHOICE_encode(bbP, &dataP->choice_multicast_enable);
    switch (dataP->choice_multicast_enable) {
        case 0:
            break;
        case 1:
            MIH_C_MULTICAST_ENABLE_encode(bbP, &dataP->_union_multicast_enable.multicast_enable);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE choice_multicast_enable OF MIH_C_FLOW_ATTRIBUTE_T %d\n", dataP->choice_multicast_enable);
    }
    MIH_C_CHOICE_encode(bbP, &dataP->choice_mark_qos);
    switch (dataP->choice_mark_qos) {
        case 0:
            break;
        case 1:
            MIH_C_MARK_encode(bbP, &dataP->_union_mark_qos.mark_qos.mark);
            MIH_C_QOS_encode(bbP, &dataP->_union_mark_qos.mark_qos.qos);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE choice_mark_qos OF MIH_C_FLOW_ATTRIBUTE_T %d\n", dataP->choice_mark_qos);
    }
    MIH_C_CHOICE_encode(bbP, &dataP->choice_mark_drop_eligibility);
    switch (dataP->choice_mark_drop_eligibility) {
        case 0:
            break;
        case 1:
            MIH_C_MARK_encode(bbP, &dataP->_union_mark_drop_eligibility.mark_drop_eligibility.mark);
            MIH_C_DROP_ELIGIBILITY_encode(bbP, &dataP->_union_mark_drop_eligibility.mark_drop_eligibility.drop_eligibility);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE choice_mark_drop_eligibility OF MIH_C_FLOW_ATTRIBUTE_T %d\n", dataP->choice_mark_drop_eligibility);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_FLOW_ATTRIBUTE_decode(Bit_Buffer_t* bbP, MIH_C_FLOW_ATTRIBUTE_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_FLOW_ID_decode(bbP, &dataP->flow_id);

    MIH_C_CHOICE_decode(bbP, &dataP->choice_multicast_enable);
    switch (dataP->choice_multicast_enable) {
        case 0:
            break;
        case 1:
            MIH_C_MULTICAST_ENABLE_decode(bbP, &dataP->_union_multicast_enable.multicast_enable);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE choice_multicast_enable OF MIH_C_FLOW_ATTRIBUTE_T %d\n", dataP->choice_multicast_enable);
    }
    MIH_C_CHOICE_decode(bbP, &dataP->choice_mark_qos);
    switch (dataP->choice_mark_qos) {
        case 0:
            break;
        case 1:
            MIH_C_MARK_decode(bbP, &dataP->_union_mark_qos.mark_qos.mark);
            MIH_C_QOS_decode(bbP, &dataP->_union_mark_qos.mark_qos.qos);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE choice_mark_qos OF MIH_C_FLOW_ATTRIBUTE_T %d\n", dataP->choice_mark_qos);
    }
    MIH_C_CHOICE_decode(bbP, &dataP->choice_mark_drop_eligibility);
    switch (dataP->choice_mark_drop_eligibility) {
        case 0:
            break;
        case 1:
            MIH_C_MARK_decode(bbP, &dataP->_union_mark_drop_eligibility.mark_drop_eligibility.mark);
            MIH_C_DROP_ELIGIBILITY_decode(bbP, &dataP->_union_mark_drop_eligibility.mark_drop_eligibility.drop_eligibility);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE choice_mark_drop_eligibility OF MIH_C_FLOW_ATTRIBUTE_T %d\n", dataP->choice_mark_drop_eligibility);
    }
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_AC_PARAM2String(MIH_C_LINK_AC_PARAM_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    switch (dataP->choice) {
        case 0:
            buffer_index += sprintf(&bufP[buffer_index], "NULL");
            break;
        case 1:
            buffer_index += sprintf(&bufP[buffer_index], "FLOW_ATTRIBUTE=");
            buffer_index += MIH_C_FLOW_ATTRIBUTE2String(&dataP->_union.flow_attribute, &bufP[buffer_index]);
            break;
        case 2:
            buffer_index += sprintf(&bufP[buffer_index], "RESOURCE_DESC=");
            buffer_index += MIH_C_RESOURCE_DESC2String(&dataP->_union.resource_desc, &bufP[buffer_index]);
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "UNINITIALIZED ");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_AC_PARAM_encode(Bit_Buffer_t* bbP, MIH_C_LINK_AC_PARAM_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_encode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            break;
        case 1:
            MIH_C_FLOW_ATTRIBUTE_encode(bbP, &dataP->_union.flow_attribute);
            break;
        case 2:
            MIH_C_RESOURCE_DESC_encode(bbP, &dataP->_union.resource_desc);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE OF MIH_C_LINK_AC_PARAM_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_AC_PARAM_decode(Bit_Buffer_t* bbP, MIH_C_LINK_AC_PARAM_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_decode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:
            break;
        case 1:
            MIH_C_FLOW_ATTRIBUTE_decode(bbP, &dataP->_union.flow_attribute);
            break;
        case 2:
            MIH_C_RESOURCE_DESC_decode(bbP, &dataP->_union.resource_desc);
            break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE OF MIH_C_LINK_AC_PARAM_T %d\n", dataP->choice);
    }
}

#endif
