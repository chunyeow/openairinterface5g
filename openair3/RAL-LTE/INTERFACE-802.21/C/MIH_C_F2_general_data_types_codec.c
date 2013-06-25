#define MIH_C_INTERFACE
#define MIH_C_F2_GENERAL_DATA_TYPES_CODEC_C
#include "MIH_C_F2_general_data_types_codec.h"
//-----------------------------------------------------------------------------
unsigned int MIH_C_BOOLEAN2String(MIH_C_BOOLEAN_T* dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    if (*dataP != MIH_C_BOOLEAN_FALSE) {
        buffer_index += sprintf(&bufP[buffer_index], "TRUE");
    } else {
        buffer_index += sprintf(&bufP[buffer_index], "FALSE");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_STATUS2String(MIH_C_STATUS_T *statusP, char* bufP){
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    switch (*statusP) {
        case MIH_C_STATUS_SUCCESS:                 buffer_index += sprintf(&bufP[buffer_index], "STATUS_SUCCESS");break;
        case MIH_C_STATUS_UNSPECIFIED_FAILURE:     buffer_index += sprintf(&bufP[buffer_index], "STATUS_UNSPECIFIED_FAILURE");break;
        case MIH_C_STATUS_REJECTED:                buffer_index += sprintf(&bufP[buffer_index], "STATUS_REJECTED");break;
        case MIH_C_STATUS_AUTHORIZATION_FAILURE:   buffer_index += sprintf(&bufP[buffer_index], "STATUS_AUTHORIZATION_FAILURE");break;
        case MIH_C_STATUS_NETWORK_ERROR:           buffer_index += sprintf(&bufP[buffer_index], "STATUS_NETWORK_ERROR");break;
        default:                                   buffer_index += sprintf(&bufP[buffer_index], "UNKNOWN_STATUS");
    }
    return buffer_index;
}

