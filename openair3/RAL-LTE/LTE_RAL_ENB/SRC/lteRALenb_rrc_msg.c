/*****************************************************************************
 *   Eurecom OpenAirInterface 3
 *    Copyright(c) 2012 Eurecom
 *
 * Source eRAL_subscribe.c
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
#define LTE_RAL_ENB
#define LTE_RAL_ENB_RRC_MSG_C
#include "lteRALenb.h"

static int ueid2eui48(uint8_t *euiP, uint8_t* ue_idP)
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
void eRAL_rx_rrc_ral_system_configuration_indication(instance_t instanceP, MessageDef *msg_pP)
//---------------------------------------------------------------------------------------------------------------------
{
    module_id_t           mod_id = instanceP;
    g_enb_ral_obj[mod_id].plmn_id = RRC_RAL_SYSTEM_CONFIGURATION_IND(msg_pP).plmn_id;
    g_enb_ral_obj[mod_id].cell_id = RRC_RAL_SYSTEM_CONFIGURATION_IND(msg_pP).cell_id;
}

//---------------------------------------------------------------------------------------------------------------------
void eRAL_rx_rrc_ral_connection_establishment_indication(instance_t instanceP, MessageDef *msg_pP)
//---------------------------------------------------------------------------------------------------------------------
{
    MIH_C_LINK_TUPLE_ID_T link_tuple_id;
#ifdef USE_3GPP_ADDR_AS_LINK_ADDR
    uint8_t               ue_id_array[MIH_C_3GPP_ADDR_LENGTH];
    uint8_t               mn_link_addr[MIH_C_3GPP_ADDR_LENGTH];
    uint64_t              ue_id; //EUI-64
    int                   i;
#endif
    module_id_t           mod_id = instanceP;

    // The LINK_ID contains the MN LINK_ADDR
    link_tuple_id.choice                                            = MIH_C_LINK_TUPLE_ID_CHOICE_LINK_ADDR;
    link_tuple_id.link_id.link_type                                 = MIH_C_WIRELESS_LTE;
#ifdef USE_3GPP_ADDR_AS_LINK_ADDR
    link_tuple_id.link_id.link_addr.choice = (MIH_C_CHOICE_T)MIH_C_CHOICE_3GPP_ADDR;

    memset(ue_id_array, 0, MIH_C_3GPP_ADDR_LENGTH);
    ue_id = (uint64_t)RRC_RAL_CONNECTION_ESTABLISHMENT_IND(msg_pP).ue_id;
    for (i = 0; i < MIH_C_3GPP_ADDR_LENGTH; i++) {
        ue_id_array[MIH_C_3GPP_ADDR_LENGTH-1-i] = (ue_id & 0x00000000000000FF);
        ue_id = ue_id >> 8;
    }
    ueid2eui48(mn_link_addr, ue_id_array);
    MIH_C_3GPP_ADDR_set(&(link_tuple_id.link_id.link_addr._union._3gpp_addr), NULL, 8);

    // The optional LINK_ADDR contains a link address of PoA.
    eRAL_MIH_C_3GPP_ADDR_load_3gpp_str_address(mod_id, &link_tuple_id._union.link_addr._union._3gpp_addr, (uint8_t*)ENB_DEFAULT_3GPP_ADDRESS);
#else
#warning TO DO UE ID
    // preserve byte order of plmn id
    memcpy(link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val, &g_enb_ral_obj[mod_id].plmn_id, 3);
    link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.cell_id = g_enb_ral_obj[mod_id].cell_id;

    LOG_D(RAL_ENB, "PLMN ID %d.%d.%d\n",
        link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val[0],
        link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val[1],
        link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.plmn_id.val[2]);

    LOG_D(RAL_ENB, "CELL ID %d\n",
        link_tuple_id.link_id.link_addr._union._3gpp_3g_cell_id.cell_id);
#endif


    eRAL_send_link_up_indication(instanceP, &g_enb_ral_obj[instanceP].transaction_id,
            &link_tuple_id,
            NULL,   //MIH_C_LINK_ADDR_T       *old_arP,(Optional) Old Access Router link address.
            NULL,   //MIH_C_LINK_ADDR_T       *new_arP,(Optional) New Access Router link address.
            NULL,   //MIH_C_IP_RENEWAL_FLAG_T *flagP, (Optional) Indicates whether the MN needs to change IP Address in the new PoA.
            NULL);  //MIH_C_IP_MOB_MGMT_T     *mobil_mngtP, (Optional) Indicates the type of Mobility Management Protocol supported by the new PoA.

    g_enb_ral_obj[instanceP].transaction_id ++;

}
//---------------------------------------------------------------------------------------------------------------------
void eRAL_rx_rrc_ral_connection_reestablishment_indication(instance_t instanceP, MessageDef *msg_pP)
//---------------------------------------------------------------------------------------------------------------------
{
  module_id_t           mod_id = instanceP;
#warning "TO DO ral_rx_rrc_ral_connection_reestablishment_indication"
}
//---------------------------------------------------------------------------------------------------------------------
void eRAL_rx_rrc_ral_connection_reconfiguration_indication(instance_t instanceP, MessageDef *msg_pP)
//---------------------------------------------------------------------------------------------------------------------
{
  MIH_C_LINK_TUPLE_ID_T link_tuple_id;
  uint8_t               ue_id_array[MIH_C_3GPP_ADDR_LENGTH];
  uint8_t               mn_link_addr[MIH_C_3GPP_ADDR_LENGTH];
  uint64_t              ue_id; //EUI-64
  int                   i;
  module_id_t           mod_id = instanceP;

  // The LINK_ID contains the MN LINK_ADDR
  link_tuple_id.link_id.link_type                                 = MIH_C_WIRELESS_LTE;
  link_tuple_id.link_id.link_addr.choice                          = MIH_C_CHOICE_3GPP_ADDR;
  memset(ue_id_array, 0, MIH_C_3GPP_ADDR_LENGTH);
#warning "TO DO FIX UE_ID TYPE in rrc_ral_connection_establishment_ind_t"
  ue_id = (uint64_t)RRC_RAL_CONNECTION_ESTABLISHMENT_IND(msg_pP).ue_id;
  for (i = 0; i < MIH_C_3GPP_ADDR_LENGTH; i++) {
      ue_id_array[MIH_C_3GPP_ADDR_LENGTH-1-i] = (ue_id & 0x00000000000000FF);
      ue_id = ue_id >> 8;
  }
  ueid2eui48(mn_link_addr, ue_id_array);
  MIH_C_3GPP_ADDR_set(&(link_tuple_id.link_id.link_addr._union._3gpp_addr), NULL, 8);

  //The optional LINK_ADDR may contains a link address of PoA.
  link_tuple_id.choice = MIH_C_LINK_TUPLE_ID_CHOICE_NULL;

  eRAL_send_link_up_indication(instanceP, &g_enb_ral_obj[instanceP].transaction_id,
          &link_tuple_id,
          NULL,   //MIH_C_LINK_ADDR_T       *old_arP,(Optional) Old Access Router link address.
          NULL,   //MIH_C_LINK_ADDR_T       *new_arP,(Optional) New Access Router link address.
          NULL,   //MIH_C_IP_RENEWAL_FLAG_T *flagP, (Optional) Indicates whether the MN needs to change IP Address in the new PoA.
          NULL);  //MIH_C_IP_MOB_MGMT_T     *mobil_mngtP, (Optional) Indicates the type of Mobility Management Protocol supported by the new PoA.

  g_enb_ral_obj[mod_id].transaction_id ++;


}
//---------------------------------------------------------------------------------------------------------------------
void eRAL_rx_rrc_ral_measurement_report_indication(instance_t instanceP, MessageDef *msg_pP)
//---------------------------------------------------------------------------------------------------------------------
{
  module_id_t           mod_id = instanceP;

}
//---------------------------------------------------------------------------------------------------------------------
void eRAL_rx_rrc_ral_connection_release_indication(instance_t instanceP, MessageDef *msg_pP)
//---------------------------------------------------------------------------------------------------------------------
{
  module_id_t           mod_id = instanceP;

}






