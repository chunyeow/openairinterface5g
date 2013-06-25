#define MIH_C_INTERFACE
#define MIH_C_F3_DATA_TYPES_FOR_ADDRESS_CODEC_C
#include "MIH_C_F3_data_types_for_address_codec.h"

//-----------------------------------------------------------------------------
unsigned int  MIH_C_3GPP_2G_CELL_ID2String(MIH_C_3GPP_2G_CELL_ID_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    buffer_index += sprintf(&bufP[buffer_index], "PLMN_ID = ");
    buffer_index += MIH_C_PLMN_ID2String(&dataP->plmn_id, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "LAC = ");
    buffer_index += MIH_C_LAC2String(&dataP->lac, &bufP[buffer_index]);
    buffer_index += sprintf(&bufP[buffer_index], "CI = ");
    buffer_index += MIH_C_CI2String(&dataP->ci, &bufP[buffer_index]);
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_3GPP_2G_CELL_ID_encode(Bit_Buffer_t* bbP, MIH_C_3GPP_2G_CELL_ID_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_PLMN_ID_encode(bbP, &dataP->plmn_id);
    MIH_C_LAC_encode(bbP, &dataP->lac);
    MIH_C_CI_encode(bbP, &dataP->ci);
}
//-----------------------------------------------------------------------------
inline void MIH_C_3GPP_2G_CELL_ID_decode(Bit_Buffer_t* bbP, MIH_C_3GPP_2G_CELL_ID_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_PLMN_ID_decode(bbP, &dataP->plmn_id);
    MIH_C_LAC_decode(bbP, &dataP->lac);
    MIH_C_CI_decode(bbP, &dataP->ci);
}
//-----------------------------------------------------------------------------
unsigned int  MIH_C_3GPP_3G_CELL_ID2String(MIH_C_3GPP_3G_CELL_ID_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    buffer_index += MIH_C_PLMN_ID2String(&dataP->plmn_id, &bufP[buffer_index]);
    buffer_index += MIH_C_CELL_ID2String(&dataP->cell_id, &bufP[buffer_index]);
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_3GPP_3G_CELL_ID_encode(Bit_Buffer_t* bbP, MIH_C_3GPP_3G_CELL_ID_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_PLMN_ID_encode(bbP, &dataP->plmn_id);
    MIH_C_CELL_ID_encode(bbP, &dataP->cell_id);
}
//-----------------------------------------------------------------------------
inline void MIH_C_3GPP_3G_CELL_ID_decode(Bit_Buffer_t* bbP, MIH_C_3GPP_3G_CELL_ID_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_PLMN_ID_decode(bbP, &dataP->plmn_id);
    MIH_C_CELL_ID_decode(bbP, &dataP->cell_id);
}
//-----------------------------------------------------------------------------
unsigned int MIH_C_LINK_ADDR2String(MIH_C_LINK_ADDR_T *dataP, char* bufP) {
//-----------------------------------------------------------------------------
    unsigned int buffer_index = 0;
    switch (dataP->choice) {
        case MIH_C_CHOICE_MAC_ADDR:
            buffer_index += sprintf(&bufP[buffer_index], "MAC_ADDR = ");
            buffer_index += MIH_C_MAC_ADDR2String(&dataP->_union.mac_addr, &bufP[buffer_index]);
            break;
        case MIH_C_CHOICE_3GPP_3G_CELL_ID:
            buffer_index += sprintf(&bufP[buffer_index], "3GPP_3G_CELL_ID = ");
            buffer_index = MIH_C_3GPP_3G_CELL_ID2String(&dataP->_union._3gpp_3g_cell_id, &bufP[buffer_index]);
            break;
        case MIH_C_CHOICE_3GPP_2G_CELL_ID:
            buffer_index += sprintf(&bufP[buffer_index], "3GPP_2G_CELL_ID = ");
            buffer_index = MIH_C_3GPP_2G_CELL_ID2String(&dataP->_union._3gpp_2g_cell_id, &bufP[buffer_index]);
            break;
        case MIH_C_CHOICE_3GPP_ADDR:
            buffer_index += sprintf(&bufP[buffer_index], "3GPP_ADDR = ");
            buffer_index = MIH_C_3GPP_ADDR2String(&dataP->_union._3gpp_addr, &bufP[buffer_index]);
            break;
        case MIH_C_CHOICE_3GPP2_ADDR:
            buffer_index += sprintf(&bufP[buffer_index], "3GPP2_ADDR = ");
            buffer_index = MIH_C_3GPP2_ADDR2String(&dataP->_union._3gpp2_addr, &bufP[buffer_index]);
            break;
        case MIH_C_CHOICE_OTHER_L2_ADDR:
            buffer_index += sprintf(&bufP[buffer_index], "OTHER_L2_ADDR = ");
            buffer_index = MIH_C_OTHER_L2_ADDR2String(&dataP->_union.other_l2_addr, &bufP[buffer_index]);
            break;
        default:
            buffer_index = sprintf(&bufP[buffer_index], "UNINITIALIZED ADDR");
    }
    return buffer_index;
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_ADDR_encode(Bit_Buffer_t* bbP, MIH_C_LINK_ADDR_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_encode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case MIH_C_CHOICE_MAC_ADDR:
            MIH_C_MAC_ADDR_encode(bbP, &dataP->_union.mac_addr);
            break;
        case MIH_C_CHOICE_3GPP_3G_CELL_ID:
            MIH_C_3GPP_3G_CELL_ID_encode(bbP, &dataP->_union._3gpp_3g_cell_id);
            break;
        case MIH_C_CHOICE_3GPP_2G_CELL_ID:
            MIH_C_3GPP_2G_CELL_ID_encode(bbP, &dataP->_union._3gpp_2g_cell_id);
            break;
        case MIH_C_CHOICE_3GPP_ADDR:
            MIH_C_3GPP_ADDR_encode(bbP, &dataP->_union._3gpp_addr);
            break;
        case MIH_C_CHOICE_3GPP2_ADDR:
            MIH_C_3GPP2_ADDR_encode(bbP, &dataP->_union._3gpp2_addr);
            break;
        case MIH_C_CHOICE_OTHER_L2_ADDR:
            MIH_C_OTHER_L2_ADDR_encode(bbP, &dataP->_union.other_l2_addr);
            break;
        default:
            ERR("NO KNOWN VALUE FOR ENCODING CHOICE OF MIH_C_LINK_ADDR_T %d\n", dataP->choice);
    }
}
//-----------------------------------------------------------------------------
inline void MIH_C_LINK_ADDR_decode(Bit_Buffer_t* bbP, MIH_C_LINK_ADDR_T *dataP) {
//-----------------------------------------------------------------------------
    MIH_C_CHOICE_decode(bbP, &dataP->choice);
    switch (dataP->choice) {
        case MIH_C_CHOICE_MAC_ADDR:
            MIH_C_MAC_ADDR_decode(bbP, &dataP->_union.mac_addr);
            break;
        case MIH_C_CHOICE_3GPP_3G_CELL_ID:
            MIH_C_3GPP_3G_CELL_ID_decode(bbP, &dataP->_union._3gpp_3g_cell_id);
            break;
        case MIH_C_CHOICE_3GPP_2G_CELL_ID:
            MIH_C_3GPP_2G_CELL_ID_decode(bbP, &dataP->_union._3gpp_2g_cell_id);
            break;
        case MIH_C_CHOICE_3GPP_ADDR:
            MIH_C_3GPP_ADDR_decode(bbP, &dataP->_union._3gpp_addr);
            break;
        case MIH_C_CHOICE_3GPP2_ADDR:
            MIH_C_3GPP2_ADDR_decode(bbP, &dataP->_union._3gpp2_addr);
            break;
        case MIH_C_CHOICE_OTHER_L2_ADDR:
            MIH_C_OTHER_L2_ADDR_decode(bbP, &dataP->_union.other_l2_addr);
            break;
        default:
            ERR("NO KNOWN VALUE FOR DECODING CHOICE OF MIH_C_LINK_ADDR_T %d\n", dataP->choice);
    }
}




