/*****************************************************************************
 *   Eurecom OpenAirInterface 3
 *    Copyright(c) 2012 Eurecom
 *
 * Source mRAL_subscribe.c
 *
 * Version 0.1
 *
 * Date  11/27/2013
 *
 * Product MIH RAL LTE
 *
 * Subsystem
 *
 * Authors Lionel Gauthier
 *
 * Description
 *
 *****************************************************************************/
#define LTE_RAL_UE
#define LTE_RAL_UE_RRC_MSG_C
#include "lteRALue.h"

static int ueid2eui48(u8 *euiP, u8* ue_idP)
{
    // inspired by linux-source-3.2.0/net/ipv6/addrconf.c
    memcpy(euiP, ue_idP, 3);
    memcpy(euiP + 5, ue_idP + 3, 3);
    euiP[3] = 0xFF;
    euiP[4] = 0xFE;
    euiP[0] ^= 2;
    return 0;
}

//---------------------------------------------------------------------------------------------------------------------
void mRAL_rx_rrc_ral_system_information_indication(instance_t instanceP, MessageDef *msg_p)
//---------------------------------------------------------------------------------------------------------------------
{
    MIH_C_LINK_DET_INFO_T link_det_info;


    // link id
    link_det_info.link_tuple_id.link_id.link_type                                 = MIH_C_WIRELESS_LTE;
    link_det_info.link_tuple_id.link_id.link_addr.choice                          = MIH_C_CHOICE_3GPP_3G_CELL_ID;

    link_det_info.link_tuple_id.choice                                            = MIH_C_LINK_TUPLE_ID_CHOICE_LINK_ADDR;

    // preserve byte order of plmn id
    memcpy(link_det_info.link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val, &RRC_RAL_SYSTEM_INFORMATION_IND(msg_p).plmn_id, 3);

    link_det_info.link_tuple_id.link_id.link_addr.choice                          = MIH_C_CHOICE_3GPP_3G_CELL_ID;
    link_det_info.link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.cell_id = RRC_RAL_SYSTEM_INFORMATION_IND(msg_p).cell_id;


    MIH_C_NETWORK_ID_set(&link_det_info.network_id, (u_int8_t *)PREDEFINED_MIH_NETWORK_ID, strlen(PREDEFINED_MIH_NETWORK_ID));

    MIH_C_NET_AUX_ID_set(&link_det_info.net_aux_id, (u_int8_t *)PREDEFINED_MIH_NETAUX_ID, strlen(PREDEFINED_MIH_NETAUX_ID));

    link_det_info.sig_strength.choice     = MIH_C_SIG_STRENGTH_CHOICE_DBM;
    link_det_info.sig_strength._union.dbm = RRC_RAL_SYSTEM_INFORMATION_IND(msg_p).dbm;

    link_det_info.sinr                    = RRC_RAL_SYSTEM_INFORMATION_IND(msg_p).sinr;

    link_det_info.link_data_rate          = RRC_RAL_SYSTEM_INFORMATION_IND(msg_p).link_data_rate;

    link_det_info.link_mihcap_flag        = g_ue_ral_obj[instanceP].link_mihcap_flag;

    link_det_info.net_caps                = g_ue_ral_obj[instanceP].net_caps;

    mRAL_send_link_detected_indication(instanceP, &g_ue_ral_obj[instanceP].transaction_id, &link_det_info);

    g_ue_ral_obj[instanceP].transaction_id ++;

}
//---------------------------------------------------------------------------------------------------------------------
void mRAL_rx_rrc_ral_connection_establishment_indication(instance_t instanceP, MessageDef *msg_p)
//---------------------------------------------------------------------------------------------------------------------
{
    MIH_C_LINK_TUPLE_ID_T link_tuple_id;
    uint8_t               ue_id_array[MIH_C_3GPP_ADDR_LENGTH];
    uint8_t               mn_link_addr[MIH_C_3GPP_ADDR_LENGTH];
    uint64_t              ue_id; //EUI-64
    int                   i;

    // The LINK_ID contains the MN LINK_ADDR
    link_tuple_id.link_id.link_type                                 = MIH_C_WIRELESS_LTE;
    link_tuple_id.link_id.link_addr.choice                          = MIH_C_CHOICE_3GPP_ADDR;
    memset(ue_id_array, 0, MIH_C_3GPP_ADDR_LENGTH);
#warning "TO DO FIX UE_ID TYPE in rrc_ral_connection_establishment_ind_t"
    ue_id = (uint64_t)RRC_RAL_CONNECTION_ESTABLISHMENT_IND(msg_p).ue_id;
    for (i = 0; i < MIH_C_3GPP_ADDR_LENGTH; i++) {
        ue_id_array[MIH_C_3GPP_ADDR_LENGTH-1-i] = (ue_id & 0x00000000000000FF);
        ue_id = ue_id >> 8;
    }
    ueid2eui48(mn_link_addr, ue_id_array);
    MIH_C_3GPP_ADDR_set(&(link_tuple_id.link_id.link_addr._union._3gpp_addr), NULL, 8);

    //The optional LINK_ADDR may contains a link address of PoA.
    link_tuple_id.choice = MIH_C_LINK_TUPLE_ID_CHOICE_NULL;

    mRAL_send_link_up_indication(instanceP, &g_ue_ral_obj[instanceP].transaction_id,
            &link_tuple_id,
            NULL,   //MIH_C_LINK_ADDR_T       *old_arP,(Optional) Old Access Router link address.
            NULL,   //MIH_C_LINK_ADDR_T       *new_arP,(Optional) New Access Router link address.
            NULL,   //MIH_C_IP_RENEWAL_FLAG_T *flagP, (Optional) Indicates whether the MN needs to change IP Address in the new PoA.
            NULL);  //MIH_C_IP_MOB_MGMT_T     *mobil_mngtP, (Optional) Indicates the type of Mobility Management Protocol supported by the new PoA.

    g_ue_ral_obj[instanceP].transaction_id ++;

}
//---------------------------------------------------------------------------------------------------------------------
void mRAL_rx_rrc_ral_connection_reestablishment_indication(instance_t instanceP, MessageDef *msg_p)
//---------------------------------------------------------------------------------------------------------------------
{
    MIH_C_LINK_TUPLE_ID_T link_tuple_id;
    uint8_t               ue_id_array[MIH_C_3GPP_ADDR_LENGTH];
    uint8_t               mn_link_addr[MIH_C_3GPP_ADDR_LENGTH];
    uint64_t              ue_id; //EUI-64
    int                   i;

    // The LINK_ID contains the MN LINK_ADDR
    link_tuple_id.link_id.link_type                                 = MIH_C_WIRELESS_LTE;
    link_tuple_id.link_id.link_addr.choice                          = MIH_C_CHOICE_3GPP_ADDR;
    memset(ue_id_array, 0, MIH_C_3GPP_ADDR_LENGTH);
#warning "TO DO FIX UE_ID TYPE in rrc_ral_connection_establishment_ind_t"
    ue_id = (uint64_t)RRC_RAL_CONNECTION_ESTABLISHMENT_IND(msg_p).ue_id;
    for (i = 0; i < MIH_C_3GPP_ADDR_LENGTH; i++) {
        ue_id_array[MIH_C_3GPP_ADDR_LENGTH-1-i] = (ue_id & 0x00000000000000FF);
        ue_id = ue_id >> 8;
    }
    ueid2eui48(mn_link_addr, ue_id_array);
    MIH_C_3GPP_ADDR_set(&(link_tuple_id.link_id.link_addr._union._3gpp_addr), NULL, 8);

    //The optional LINK_ADDR may contains a link address of PoA.
    link_tuple_id.choice = MIH_C_LINK_TUPLE_ID_CHOICE_NULL;

    LOG_D(RAL_UE, "RRC_RAL_CONNECTION_ESTABLISHMENT_IND num srb %d num drb %d\n", RRC_RAL_CONNECTION_REESTABLISHMENT_IND(msg_p).num_srb,RRC_RAL_CONNECTION_REESTABLISHMENT_IND(msg_p).num_drb);
    if ((RRC_RAL_CONNECTION_REESTABLISHMENT_IND(msg_p).num_drb > 0) && (RRC_RAL_CONNECTION_REESTABLISHMENT_IND(msg_p).num_srb > 0)) {
        mRAL_send_link_up_indication(instanceP, &g_ue_ral_obj[instanceP].transaction_id,
            &link_tuple_id,
            NULL,   //MIH_C_LINK_ADDR_T       *old_arP,(Optional) Old Access Router link address.
            NULL,   //MIH_C_LINK_ADDR_T       *new_arP,(Optional) New Access Router link address.
            NULL,   //MIH_C_IP_RENEWAL_FLAG_T *flagP, (Optional) Indicates whether the MN needs to change IP Address in the new PoA.
            NULL);  //MIH_C_IP_MOB_MGMT_T     *mobil_mngtP, (Optional) Indicates the type of Mobility Management Protocol supported by the new PoA.
    } else {
        MIH_C_LINK_DN_REASON_T  reason_code = MIH_C_LINK_DOWN_REASON_EXPLICIT_DISCONNECT;

        mRAL_send_link_down_indication(instanceP, &g_ue_ral_obj[instanceP].transaction_id,
                &link_tuple_id,
                NULL,
                &reason_code);
    }
    g_ue_ral_obj[instanceP].transaction_id ++;}
//---------------------------------------------------------------------------------------------------------------------
void mRAL_rx_rrc_ral_connection_reconfiguration_indication(instance_t instanceP, MessageDef *msg_p)
//---------------------------------------------------------------------------------------------------------------------
{

}
//---------------------------------------------------------------------------------------------------------------------
void mRAL_rx_rrc_ral_connection_release_indication(instance_t instanceP, MessageDef *msg_p)
//---------------------------------------------------------------------------------------------------------------------
{

}






