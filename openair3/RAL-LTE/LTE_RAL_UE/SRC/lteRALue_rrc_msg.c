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
#include "LAYER2/MAC/extern.h"

//---------------------------------------------------------------------------------------------------------------------
static int ueid2eui48(uint8_t *euiP, uint8_t* ue_idP)
//---------------------------------------------------------------------------------------------------------------------
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
void mRAL_rx_rrc_ral_scan_confirm(instance_t instanceP,
                                  MessageDef *msg_pP)
//---------------------------------------------------------------------------------------------------------------------
{
    MIH_C_STATUS_T             status;
    MIH_C_LINK_SCAN_RSP_LIST_T scan_rsp_list;
    MIH_C_LINK_AC_RESULT_T     ac_result;
    int                        i;

    status    = MIH_C_STATUS_SUCCESS;
    ac_result = MIH_C_LINK_AC_RESULT_SUCCESS;
    memset(&scan_rsp_list, 0, sizeof(MIH_C_LINK_SCAN_RSP_LIST_T));

    for (i = 0 ; i < RRC_RAL_SCAN_CONF (msg_pP).num_scan_resp; i++) {
        // TO DO
        memcpy(&scan_rsp_list.val[i].link_addr, &RRC_RAL_SCAN_CONF (msg_pP).link_scan_resp[i].link_addr, sizeof(MIH_C_LINK_ADDR_T));
        // TO DO
        memcpy(&scan_rsp_list.val[i].network_id, &RRC_RAL_SCAN_CONF (msg_pP).link_scan_resp[i].network_id, sizeof(MIH_C_NETWORK_ID_T));

        scan_rsp_list.val[i].sig_strength.choice = RRC_RAL_SCAN_CONF (msg_pP).link_scan_resp[i].sig_strength.choice;
        switch (scan_rsp_list.val[i].sig_strength.choice) {
            case RAL_SIG_STRENGTH_CHOICE_DBM:
                scan_rsp_list.val[i].sig_strength._union.dbm = RRC_RAL_SCAN_CONF (msg_pP).link_scan_resp[i].sig_strength._union.dbm;
                break;
            case RAL_SIG_STRENGTH_CHOICE_PERCENTAGE:
                scan_rsp_list.val[i].sig_strength._union.percentage = RRC_RAL_SCAN_CONF (msg_pP).link_scan_resp[i].sig_strength._union.percentage;
                break;
            default:
                LOG_E(RAL_UE, "INVALID RRC_RAL_SCAN_CONF field sig_strength.choice %d\n", scan_rsp_list.val[i].sig_strength.choice);
                status = MIH_C_STATUS_UNSPECIFIED_FAILURE;
        }
        scan_rsp_list.length += 1;
    }
    mRAL_send_link_action_confirm(instanceP,
            &RRC_RAL_SCAN_CONF (msg_pP).transaction_id,
            &status,
            &scan_rsp_list,
            &ac_result);
}


//---------------------------------------------------------------------------------------------------------------------
void mRAL_rx_rrc_ral_system_information_indication(instance_t  instanceP,
                                                   MessageDef *msg_pP)
//---------------------------------------------------------------------------------------------------------------------
{
    MIH_C_LINK_DET_INFO_T   link_det_info;
    int                     i;
    module_id_t             mod_id = instanceP - NB_eNB_INST;

    memset(&link_det_info, 0, sizeof(MIH_C_LINK_DET_INFO_T));

    // save cell parameters
    g_ue_ral_obj[mod_id].cell_id = RRC_RAL_SYSTEM_INFORMATION_IND(msg_pP).cell_id;
    memcpy(&g_ue_ral_obj[mod_id].plmn_id, &RRC_RAL_SYSTEM_INFORMATION_IND(msg_pP).plmn_id, sizeof(g_ue_ral_obj[mod_id].plmn_id));

    // link id
    link_det_info.link_tuple_id.link_id.link_type                                 = MIH_C_WIRELESS_LTE;
    #ifdef USE_3GPP_ADDR_AS_LINK_ADDR
    link_det_info.link_tuple_id.link_id.link_addr.choice = (MIH_C_CHOICE_T)MIH_C_CHOICE_3GPP_ADDR;
    MIH_C_3GPP_ADDR_load_3gpp_str_address(mod_id, &link_det_info.link_tuple_id.link_id.link_addr._union._3gpp_addr, (u_int8_t*)UE_DEFAULT_3GPP_ADDRESS);
    #else
    link_det_info.link_tuple_id.link_id.link_addr.choice                          = MIH_C_CHOICE_3GPP_3G_CELL_ID;

    // preserve byte order of plmn id
    memcpy(link_det_info.link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val, &RRC_RAL_SYSTEM_INFORMATION_IND(msg_pP).plmn_id, 3);
    link_det_info.link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.cell_id = RRC_RAL_SYSTEM_INFORMATION_IND(msg_pP).cell_id;

    LOG_D(RAL_UE, "PLMN ID %d.%d.%d\n", link_det_info.link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val[0],
            link_det_info.link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val[1],
            link_det_info.link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val[2]);
    LOG_D(RAL_UE, "CELL ID %d\n", link_det_info.link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.cell_id);
    #endif
    //The optional LINK_ADDR may contains a link address of PoA.
    link_det_info.link_tuple_id.choice                                            = MIH_C_LINK_TUPLE_ID_CHOICE_NULL;

    MIH_C_NETWORK_ID_set(&link_det_info.network_id, (u_int8_t *)PREDEFINED_MIH_NETWORK_ID, strlen(PREDEFINED_MIH_NETWORK_ID));

    MIH_C_NET_AUX_ID_set(&link_det_info.net_aux_id, (u_int8_t *)PREDEFINED_MIH_NETAUX_ID, strlen(PREDEFINED_MIH_NETAUX_ID));

    link_det_info.sig_strength.choice     = MIH_C_SIG_STRENGTH_CHOICE_DBM;
    link_det_info.sig_strength._union.dbm = RRC_RAL_SYSTEM_INFORMATION_IND(msg_pP).dbm;

    link_det_info.sinr                    = RRC_RAL_SYSTEM_INFORMATION_IND(msg_pP).sinr;

    link_det_info.link_data_rate          = RRC_RAL_SYSTEM_INFORMATION_IND(msg_pP).link_data_rate;

    link_det_info.link_mihcap_flag        = g_ue_ral_obj[mod_id].link_mihcap_flag;

    link_det_info.net_caps                = g_ue_ral_obj[mod_id].net_caps;

    mRAL_send_link_detected_indication(instanceP, &g_ue_ral_obj[mod_id].transaction_id, &link_det_info);

    g_ue_ral_obj[mod_id].transaction_id ++;

}
//---------------------------------------------------------------------------------------------------------------------
void mRAL_rx_rrc_ral_connection_establishment_indication(instance_t  instanceP,
                                                         MessageDef *msg_pP)
//---------------------------------------------------------------------------------------------------------------------
{
    MIH_C_LINK_TUPLE_ID_T link_tuple_id;
    module_id_t           mod_id = instanceP - NB_eNB_INST;

    memset(&link_tuple_id, 0, sizeof(MIH_C_LINK_TUPLE_ID_T));
    // The LINK_ID contains the MN LINK_ADDR
    link_tuple_id.link_id.link_type                                 = MIH_C_WIRELESS_LTE;
    #ifdef USE_3GPP_ADDR_AS_LINK_ADDR
    link_tuple_id.link_id.link_addr.choice = (MIH_C_CHOICE_T)MIH_C_CHOICE_3GPP_ADDR;
    MIH_C_3GPP_ADDR_load_3gpp_str_address(mod_id, &link_tuple_id.link_id.link_addr._union._3gpp_addr, (u_int8_t*)UE_DEFAULT_3GPP_ADDRESS);
    #else
    link_tuple_id.link_id.link_addr.choice                          = MIH_C_CHOICE_3GPP_3G_CELL_ID;
    memcpy(link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val, &g_ue_ral_obj[mod_id].plmn_id, 3);
    link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.cell_id = g_ue_ral_obj[mod_id].cell_id;
    LOG_D(RAL_UE, "PLMN ID %d.%d.%d\n", link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val[0],
            link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val[1],
            link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val[2]);
    LOG_D(RAL_UE, "CELL ID %d\n", link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.cell_id);
    #endif
    //The optional LINK_ADDR may contains a link address of PoA.
    link_tuple_id.choice                                            = MIH_C_LINK_TUPLE_ID_CHOICE_NULL;

    mRAL_send_link_up_indication(instanceP, &g_ue_ral_obj[mod_id].transaction_id,
            &link_tuple_id,
            NULL,   //MIH_C_LINK_ADDR_T       *old_arP,(Optional) Old Access Router link address.
            NULL,   //MIH_C_LINK_ADDR_T       *new_arP,(Optional) New Access Router link address.
            NULL,   //MIH_C_IP_RENEWAL_FLAG_T *flagP, (Optional) Indicates whether the MN needs to change IP Address in the new PoA.
            NULL);  //MIH_C_IP_MOB_MGMT_T     *mobil_mngtP, (Optional) Indicates the type of Mobility Management Protocol supported by the new PoA.

    g_ue_ral_obj[mod_id].transaction_id ++;

}
//---------------------------------------------------------------------------------------------------------------------
void mRAL_rx_rrc_ral_connection_reestablishment_indication(instance_t  instanceP,
                                                           MessageDef *msg_pP)
//---------------------------------------------------------------------------------------------------------------------
{
    MIH_C_LINK_TUPLE_ID_T link_tuple_id;
    module_id_t           mod_id = instanceP - NB_eNB_INST;

    memset(&link_tuple_id, 0, sizeof(MIH_C_LINK_TUPLE_ID_T));
    //The optional LINK_ADDR may contains a link address of PoA.
    link_tuple_id.choice                                            = MIH_C_LINK_TUPLE_ID_CHOICE_NULL;
    link_tuple_id.link_id.link_type                                 = MIH_C_WIRELESS_LTE;
    #ifdef USE_3GPP_ADDR_AS_LINK_ADDR
    link_tuple_id.link_id.link_addr.choice = (MIH_C_CHOICE_T)MIH_C_CHOICE_3GPP_ADDR;
    MIH_C_3GPP_ADDR_load_3gpp_str_address(mod_id, &link_tuple_id.link_id.link_addr._union._3gpp_addr, (u_int8_t*)UE_DEFAULT_3GPP_ADDRESS);
    #else
    link_tuple_id.link_id.link_addr.choice                          = MIH_C_CHOICE_3GPP_3G_CELL_ID;
    // preserve byte order of plmn id
    memcpy(link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val, &g_ue_ral_obj[mod_id].plmn_id, 3);
    link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.cell_id = g_ue_ral_obj[mod_id].cell_id;

    LOG_D(RAL_UE, "PLMN ID %d.%d.%d\n", link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val[0],
            link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val[1],
            link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val[2]);
    LOG_D(RAL_UE, "CELL ID %d\n", link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.cell_id);
    #endif

    LOG_D(RAL_UE, "RRC_RAL_CONNECTION_ESTABLISHMENT_IND num srb %d num drb %d\n", RRC_RAL_CONNECTION_REESTABLISHMENT_IND(msg_pP).num_srb,RRC_RAL_CONNECTION_REESTABLISHMENT_IND(msg_pP).num_drb);
    if ((RRC_RAL_CONNECTION_REESTABLISHMENT_IND(msg_pP).num_drb > 0) && (RRC_RAL_CONNECTION_REESTABLISHMENT_IND(msg_pP).num_srb > 0)) {
        mRAL_send_link_up_indication(instanceP, &g_ue_ral_obj[mod_id].transaction_id,
            &link_tuple_id,
            NULL,   //MIH_C_LINK_ADDR_T       *old_arP,(Optional) Old Access Router link address.
            NULL,   //MIH_C_LINK_ADDR_T       *new_arP,(Optional) New Access Router link address.
            NULL,   //MIH_C_IP_RENEWAL_FLAG_T *flagP, (Optional) Indicates whether the MN needs to change IP Address in the new PoA.
            NULL);  //MIH_C_IP_MOB_MGMT_T     *mobil_mngtP, (Optional) Indicates the type of Mobility Management Protocol supported by the new PoA.
    } else {
        MIH_C_LINK_DN_REASON_T  reason_code = MIH_C_LINK_DOWN_REASON_EXPLICIT_DISCONNECT;

        mRAL_send_link_down_indication(instanceP, &g_ue_ral_obj[mod_id].transaction_id,
                &link_tuple_id,
                NULL,
                &reason_code);
    }
    g_ue_ral_obj[mod_id].transaction_id ++;}
//---------------------------------------------------------------------------------------------------------------------
void mRAL_rx_rrc_ral_connection_reconfiguration_indication(instance_t  instanceP,
                                                           MessageDef *msg_pP)
//---------------------------------------------------------------------------------------------------------------------
{
    MIH_C_LINK_TUPLE_ID_T link_tuple_id;
    module_id_t           mod_id = instanceP - NB_eNB_INST;

    memset(&link_tuple_id, 0, sizeof(MIH_C_LINK_TUPLE_ID_T));
    //The optional LINK_ADDR may contains a link address of PoA.
    link_tuple_id.choice                                            = MIH_C_LINK_TUPLE_ID_CHOICE_NULL;
    link_tuple_id.link_id.link_type                                 = MIH_C_WIRELESS_LTE;
    #ifdef USE_3GPP_ADDR_AS_LINK_ADDR
    link_tuple_id.link_id.link_addr.choice = (MIH_C_CHOICE_T)MIH_C_CHOICE_3GPP_ADDR;
    MIH_C_3GPP_ADDR_load_3gpp_str_address(mod_id, &link_tuple_id.link_id.link_addr._union._3gpp_addr, (u_int8_t*)UE_DEFAULT_3GPP_ADDRESS);
    #else
    // preserve byte order of plmn id
    memcpy(link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val, &g_ue_ral_obj[mod_id].plmn_id, 3);
    link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.cell_id = g_ue_ral_obj[mod_id].cell_id;

    LOG_D(RAL_UE, "PLMN ID %d.%d.%d\n", link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val[0],
            link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val[1],
            link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val[2]);
    LOG_D(RAL_UE, "CELL ID %d\n", link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.cell_id);
    #endif

    LOG_D(RAL_UE, "RRC_RAL_CONNECTION_RECONFIGURATION_IND num srb %d num drb %d\n",
            RRC_RAL_CONNECTION_RECONFIGURATION_IND(msg_pP).num_srb,
            RRC_RAL_CONNECTION_RECONFIGURATION_IND(msg_pP).num_drb);

    if ((RRC_RAL_CONNECTION_RECONFIGURATION_IND(msg_pP).num_drb > 0) &&
            (RRC_RAL_CONNECTION_RECONFIGURATION_IND(msg_pP).num_srb > 0)) {
        mRAL_send_link_up_indication(instanceP, &g_ue_ral_obj[mod_id].transaction_id,
            &link_tuple_id,
            NULL,   //MIH_C_LINK_ADDR_T       *old_arP,(Optional) Old Access Router link address.
            NULL,   //MIH_C_LINK_ADDR_T       *new_arP,(Optional) New Access Router link address.
            NULL,   //MIH_C_IP_RENEWAL_FLAG_T *flagP, (Optional) Indicates whether the MN needs to change IP Address in the new PoA.
            NULL);  //MIH_C_IP_MOB_MGMT_T     *mobil_mngtP, (Optional) Indicates the type of Mobility Management Protocol supported by the new PoA.
    } else {
        MIH_C_LINK_DN_REASON_T  reason_code = MIH_C_LINK_DOWN_REASON_EXPLICIT_DISCONNECT;

        mRAL_send_link_down_indication(instanceP, &g_ue_ral_obj[mod_id].transaction_id,
                &link_tuple_id,
                NULL,
                &reason_code);
    }
    g_ue_ral_obj[mod_id].transaction_id ++;
}
//---------------------------------------------------------------------------------------------------------------------
void mRAL_rx_rrc_ral_connection_release_indication(instance_t instanceP, MessageDef *msg_pP)
//---------------------------------------------------------------------------------------------------------------------
{

}






