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
#define MIH_C_F9_DATA_TYPES_FOR_QOS_CODEC_C
#include "MIH_C_F9_data_types_for_qos_codec.h"

//-----------------------------------------------------------------------------
unsigned int MIH_C_MIN_PK_TX_DELAY2String(MIH_C_MIN_PK_TX_DELAY_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "COS = ");
    buffer_index += MIH_C_COS_ID2String(&dataP->cos_id, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "VAL = ");
    buffer_index += MIH_C_UNSIGNED_INT22String(&dataP->value, &bufP[buffer_index]);
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_MIN_PK_TX_DELAY_encode(Bit_Buffer_t* bbP, MIH_C_MIN_PK_TX_DELAY_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_COS_ID_encode(bbP, &dataP->cos_id);
    MIH_C_UNSIGNED_INT2_encode(bbP, &dataP->value);
}
//-----------------------------------------------------------------------------
inline void MIH_C_MIN_PK_TX_DELAY_decode(Bit_Buffer_t* bbP, MIH_C_MIN_PK_TX_DELAY_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_COS_ID_decode(bbP, &dataP->cos_id);
    MIH_C_UNSIGNED_INT2_decode(bbP, &dataP->value);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_AVG_PK_TX_DELAY2String(MIH_C_AVG_PK_TX_DELAY_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "COS = ");
    buffer_index += MIH_C_COS_ID2String(&dataP->cos_id, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "VAL = ");
    buffer_index += MIH_C_UNSIGNED_INT22String(&dataP->value, &bufP[buffer_index]);
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_AVG_PK_TX_DELAY_encode(Bit_Buffer_t* bbP, MIH_C_AVG_PK_TX_DELAY_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_COS_ID_encode(bbP, &dataP->cos_id);
    MIH_C_UNSIGNED_INT2_encode(bbP, &dataP->value);
}
//-----------------------------------------------------------------------------
inline void MIH_C_AVG_PK_TX_DELAY_decode(Bit_Buffer_t* bbP, MIH_C_AVG_PK_TX_DELAY_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_COS_ID_decode(bbP, &dataP->cos_id);
    MIH_C_UNSIGNED_INT2_decode(bbP, &dataP->value);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_MAX_PK_TX_DELAY2String(MIH_C_MAX_PK_TX_DELAY_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "COS = ");
    buffer_index += MIH_C_COS_ID2String(&dataP->cos_id, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "VAL = ");
    buffer_index += MIH_C_UNSIGNED_INT22String(&dataP->value, &bufP[buffer_index]);
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_MAX_PK_TX_DELAY_encode(Bit_Buffer_t* bbP, MIH_C_MAX_PK_TX_DELAY_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_COS_ID_encode(bbP, &dataP->cos_id);
    MIH_C_UNSIGNED_INT2_encode(bbP, &dataP->value);
}
//-----------------------------------------------------------------------------
inline void MIH_C_MAX_PK_TX_DELAY_decode(Bit_Buffer_t* bbP, MIH_C_MAX_PK_TX_DELAY_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_COS_ID_decode(bbP, &dataP->cos_id);
    MIH_C_UNSIGNED_INT2_decode(bbP, &dataP->value);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_PK_DELAY_JITTER2String(MIH_C_PK_DELAY_JITTER_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "COS = ");
    buffer_index += MIH_C_COS_ID2String(&dataP->cos_id, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "VAL = ");
    buffer_index += MIH_C_UNSIGNED_INT22String(&dataP->value, &bufP[buffer_index]);
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_PK_DELAY_JITTER_encode(Bit_Buffer_t* bbP, MIH_C_PK_DELAY_JITTER_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_COS_ID_encode(bbP, &dataP->cos_id);
    MIH_C_UNSIGNED_INT2_encode(bbP, &dataP->value);
}
//-----------------------------------------------------------------------------
inline void MIH_C_PK_DELAY_JITTER_decode(Bit_Buffer_t* bbP, MIH_C_PK_DELAY_JITTER_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_COS_ID_decode(bbP, &dataP->cos_id);
    MIH_C_UNSIGNED_INT2_decode(bbP, &dataP->value);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_PK_LOSS_RATE2String(MIH_C_PK_LOSS_RATE_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "COS = ");
    buffer_index += MIH_C_COS_ID2String(&dataP->cos_id, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "VAL = ");
    buffer_index += MIH_C_UNSIGNED_INT22String(&dataP->value, &bufP[buffer_index]);
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_PK_LOSS_RATE_encode(Bit_Buffer_t* bbP, MIH_C_PK_LOSS_RATE_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_COS_ID_encode(bbP, &dataP->cos_id);
    MIH_C_UNSIGNED_INT2_encode(bbP, &dataP->value);
}
//-----------------------------------------------------------------------------
inline void MIH_C_PK_LOSS_RATE_decode(Bit_Buffer_t* bbP, MIH_C_PK_LOSS_RATE_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_COS_ID_decode(bbP, &dataP->cos_id);
    MIH_C_UNSIGNED_INT2_decode(bbP, &dataP->value);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_QOS_PARAM_VAL2String(MIH_C_QOS_PARAM_VAL_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;

    switch (dataP->choice) {
        case 0:
            buffer_index += sprintf(&bufP[buffer_index], "NUM_QOS_TYPES = ");
            buffer_index += MIH_C_NUM_COS_TYPES2String(&dataP->_union.num_qos_types, &bufP[buffer_index]);
            break;
        case 1:
            buffer_index += sprintf(&bufP[buffer_index], "MIN_PK_TX_DELAY_LIST = ");
            buffer_index += MIH_C_MIN_PK_TX_DELAY_LIST2String(&dataP->_union.min_pk_tx_delay_list, &bufP[buffer_index]);
            break;
        case 2:
            buffer_index += sprintf(&bufP[buffer_index], "AVG_PK_TX_DELAY_LIST = ");
            buffer_index += MIH_C_AVG_PK_TX_DELAY_LIST2String(&dataP->_union.avg_pk_tx_delay_list, &bufP[buffer_index]);
            break;
        case 3:
            buffer_index += sprintf(&bufP[buffer_index], "MAX_PK_TX_DELAY_LIST = ");
            buffer_index += MIH_C_MAX_PK_TX_DELAY_LIST2String(&dataP->_union.max_pk_tx_delay_list, &bufP[buffer_index]);
            break;
        case 4:
            buffer_index += sprintf(&bufP[buffer_index], "PK_DELAY_JITTER_LIST = ");
            buffer_index += MIH_C_PK_DELAY_JITTER_LIST2String(&dataP->_union.pk_delay_jitter_list, &bufP[buffer_index]);
            break;
        case 5:
            buffer_index += sprintf(&bufP[buffer_index], "PK_LOSS_RATE_LIST = ");
            buffer_index += MIH_C_PK_LOSS_RATE_LIST2String(&dataP->_union.pk_loss_rate_list, &bufP[buffer_index]);
            break;
        default:
            buffer_index += sprintf(&bufP[buffer_index], "QOS_PARAM_VAL UNITIALIZED");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_QOS_PARAM_VAL_encode(Bit_Buffer_t* bbP, MIH_C_QOS_PARAM_VAL_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_encode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:  MIH_C_NUM_COS_TYPES_encode(bbP, &dataP->_union.num_qos_types); break;
        case 1:  MIH_C_MIN_PK_TX_DELAY_LIST_encode(bbP, &dataP->_union.min_pk_tx_delay_list); break;
        case 2:  MIH_C_AVG_PK_TX_DELAY_LIST_encode(bbP, &dataP->_union.avg_pk_tx_delay_list); break;
        case 3:  MIH_C_MAX_PK_TX_DELAY_LIST_encode(bbP, &dataP->_union.max_pk_tx_delay_list); break;
        case 4:  MIH_C_PK_DELAY_JITTER_LIST_encode(bbP, &dataP->_union.pk_delay_jitter_list); break;
        case 5:  MIH_C_PK_LOSS_RATE_LIST_encode(bbP, &dataP->_union.pk_loss_rate_list); break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR ENCODING CHOICE OF MIH_C_QOS_PARAM_VAL_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_QOS_PARAM_VAL_decode(Bit_Buffer_t* bbP, MIH_C_QOS_PARAM_VAL_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_decode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case 0:  MIH_C_NUM_COS_TYPES_encode(bbP, &dataP->_union.num_qos_types); break;
        case 1:  MIH_C_MIN_PK_TX_DELAY_LIST_encode(bbP, &dataP->_union.min_pk_tx_delay_list); break;
        case 2:  MIH_C_AVG_PK_TX_DELAY_LIST_decode(bbP, &dataP->_union.avg_pk_tx_delay_list); break;
        case 3:  MIH_C_MAX_PK_TX_DELAY_LIST_decode(bbP, &dataP->_union.max_pk_tx_delay_list); break;
        case 4:  MIH_C_PK_DELAY_JITTER_LIST_decode(bbP, &dataP->_union.pk_delay_jitter_list); break;
        case 5:  MIH_C_PK_LOSS_RATE_LIST_decode(bbP, &dataP->_union.pk_loss_rate_list); break;
        default:
            printf("[MIH_C] ERROR NO KNOWN VALUE FOR DECODING CHOICE OF MIH_C_QOS_PARAM_VAL_T %d\n", dataP->choice);
    }
}



