/***************************************************************************
                         lteRALenb_mih_msg.c  -  description
 ***************************************************************************
  Eurecom OpenAirInterface 3
  Copyright(c) 1999 - 2013 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 450 route des Chappes, 06410 Biot Sophia Antipolis, France
 *******************************************************************************/
/*! \file lteRALenb_mih_msg.c
 * \brief Interface for MIH primitives in LTE-RAL-ENB
 * \author WETTERWALD Michelle, GAUTHIER Lionel, MAUREL Frederic
 * \date 2013
 * \company EURECOM
 * \email: michelle.wetterwald@eurecom.fr, lionel.gauthier@eurecom.fr, frederic.maurel@eurecom.fr
 */
/*******************************************************************************/
#define LTE_RAL_ENB
#define LTE_RAL_ENB_MIH_MSG_C
#include "lteRALenb.h"

/****************************************************************************/
/*******************  G L O C A L    D E F I N I T I O N S  *****************/
/****************************************************************************/

#define MSG_CODEC_RECV_BUFFER_SIZE 16400
#define MSG_CODEC_SEND_BUFFER_SIZE 16400

/****************************************************************************/
/*******************  L O C A L    D E F I N I T I O N S  *******************/
/****************************************************************************/

static u_int8_t g_msg_codec_recv_buffer[MSG_CODEC_RECV_BUFFER_SIZE] = {};
static u_int8_t g_msg_codec_send_buffer[MSG_CODEC_SEND_BUFFER_SIZE] = {};

static char g_msg_print_buffer[8192] = {};
static char g_msg_codec_print_buffer[8192] = {};


//-----------------------------------------------------------------------------
void eRAL_MIH_C_3GPP_ADDR_load_3gpp_str_address(ral_enb_instance_t instanceP, MIH_C_3GPP_ADDR_T* _3gpp_addr_pP, u_int8_t* str_pP)
//-----------------------------------------------------------------------------
{
    int           i, l;
    u_int8_t      val_temp;
    unsigned char address_3gpp[32];
    unsigned char buf[3];
    u_int8_t      _3gpp_byte_address[8];
    module_id_t   mod_id = instanceP;

    strcpy((char *)address_3gpp, (char *)str_pP);
    for(l=0; l<8; l++)
    {
        i=l*2;
        buf[0]= address_3gpp[i];
        buf[1]= address_3gpp[i+1];
        buf[2]= '\0';
        //sscanf((const char *)buf,"%02x", &val_temp);
        sscanf((const char *)buf,"%hhx", &val_temp);
        _3gpp_byte_address[l] = val_temp;
    }
    _3gpp_byte_address[7] += mod_id;
    MIH_C_3GPP_ADDR_set(_3gpp_addr_pP, _3gpp_byte_address, 8);
}

/****************************************************************************/
/******************  E X P O R T E D    F U N C T I O N S  ******************/
/****************************************************************************/
/*
 * -------------------------------------------------------------------------
 * Functions used to send MIH link service management messages
 * -------------------------------------------------------------------------
 */

/****************************************************************************
 ** Name:  eRAL_send_link_register_indication()                         **
 ** Description: Sends a Link_Register.indication message to the MIHF.     **
 **   This message is ODTONE specific and not defined by the 802.21        **
 **   standard. It allows the Link SAP to send informations to the MIHF    **
 **   about which technology it supports and which interface it manages.   **
 **      Upon receiving this message the MIHF executes its Link SAPs       **
 **   discovery procedure in order to get the full link capabilities.      **
 ** Inputs:  tid_pP  Transaction identifier                                  **
 **     Others: g_enb_ral_obj[instanceP].link_id, g_enb_ral_obj[instanceP].mihf_id, ralpriv                              **
 ** Outputs:  None                                                         **
 **     Others: g_msg_codec_send_buffer                                    **
 ***************************************************************************/
void eRAL_send_link_register_indication(ral_enb_instance_t instanceP, MIH_C_TRANSACTION_ID_T  *tid_pP)
{
    MIH_C_Message_Link_Register_indication_t  message;
    Bit_Buffer_t                             *bb_p;
    int                                       message_total_length;
    module_id_t                               mod_id = instanceP;

    LOG_D(RAL_ENB, " Send MIH_C_MESSAGE_LINK_REGISTER_INDICATION\n");

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Register_indication_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    //message.header.ack_req            = 0;
    //message.header.ack_rsp            = 0;
    //message.header.uir                = 0;
    //message.header.more_fragment      = 0
    //message.header.fragment_number    = 0;
    message.header.service_identifier   = (MIH_C_SID_T)1;
    message.header.operation_code       = (MIH_C_OPCODE_T)3;
    message.header.action_identifier    = (MIH_C_AID_T)6;
    message.header.transaction_id       = *tid_pP;

    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_enb_ral_obj[mod_id].link_id, strlen(g_enb_ral_obj[mod_id].link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_enb_ral_obj[mod_id].mihf_id, strlen(g_enb_ral_obj[mod_id].mihf_id));

    message.primitive.Link_Id.link_type        = MIH_C_WIRELESS_LTE; //MIH_C_WIRELESS_UMTS;
    #ifdef USE_3GPP_ADDR_AS_LINK_ADDR
    message.primitive.Link_Id.link_addr.choice = (MIH_C_CHOICE_T)MIH_C_CHOICE_3GPP_ADDR;
    eRAL_MIH_C_3GPP_ADDR_load_3gpp_str_address(mod_id, &message.primitive.Link_Id.link_addr._union._3gpp_addr, (u_int8_t*)ENB_DEFAULT_3GPP_ADDRESS);
    #else
    message.primitive.Link_Id.link_addr.choice = (MIH_C_CHOICE_T)MIH_C_CHOICE_3GPP_3G_CELL_ID;
    memcpy(message.primitive.Link_Id.link_addr._union._3gpp_3g_cell_id.plmn_id.val, &g_enb_ral_obj[mod_id].plmn_id, 3);
    message.primitive.Link_Id.link_addr._union._3gpp_3g_cell_id.cell_id = g_enb_ral_obj[mod_id].cell_id;
    //Bit_Buffer_t *plmn = new_BitBuffer_0();
    //BitBuffer_wrap(plmn, (unsigned char*) &g_enb_ral_obj[instanceP].plmn_id, DEFAULT_PLMN_SIZE);
    //MIH_C_PLMN_ID_decode(plmn, &message.primitive.Link_Id.link_addr._union._3gpp_3g_cell_id.plmn_id);
    //message.primitive.Link_Id.link_addr._union._3gpp_3g_cell_id.cell_id = g_enb_ral_obj[instanceP].cell_id;
    //free_BitBuffer(plmn);
    #endif

    message_total_length = MIH_C_Link_Message_Encode_Link_Register_indication(bb_p, &message);

    if (eRAL_send_to_mih( instanceP, bb_p->m_buffer, message_total_length) < 0) {
        LOG_E(RAL_ENB, ": Send Link_Register.indication\n");
    } else {
        LOG_D(RAL_ENB, ": Sent Link_Register.indication\n");
    }
    free_BitBuffer(bb_p);
}

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_send_capability_discover_confirm()                **
 **                                                                        **
 ** Description: Sends capability discover service management response to  **
 **   the MIH-F.                                                **
 **                                                                        **
 ** Inputs:  tid_pP  Transaction identifier                     **
 **     statusP: Status of operation                        **
 **     levt_listP: List of events supported by the link layer **
 **     lcmd_listP: List of commands supported by the link     **
 **    layer                                      **
 **     Others: g_enb_ral_obj[instanceP].link_id, g_enb_ral_obj[instanceP].mihf_id                       **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: None                                       **
 **     Others: g_msg_codec_send_buffer                    **
 **                                                                        **
 ***************************************************************************/
void eRAL_send_capability_discover_confirm(ral_enb_instance_t instanceP, MIH_C_TRANSACTION_ID_T  *tid_pP,
        MIH_C_STATUS_T          *statusP,
        MIH_C_LINK_EVENT_LIST_T *link_evt_listP,
        MIH_C_LINK_CMD_LIST_T   *link_cmd_listP)
{
    MIH_C_Message_Link_Capability_Discover_confirm_t  message;
    Bit_Buffer_t                                      *bb_p;
    int                                               message_total_length;

    LOG_D(RAL_ENB, " Send MIH_C_MESSAGE_LINK_CAPABILITY_DISCOVER_CONFIRM\n");

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Capability_Discover_confirm_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    //message.header.ack_req            = 0;
    //message.header.ack_rsp            = 0;
    //message.header.uir                = 0;
    //message.header.more_fragment      = 0
    //message.header.fragment_number    = 0;
    message.header.service_identifier   = (MIH_C_SID_T)1;
    message.header.operation_code       = (MIH_C_OPCODE_T)0;
    message.header.action_identifier    = (MIH_C_AID_T)1;
    message.header.transaction_id       = *tid_pP;

    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_enb_ral_obj[instanceP].link_id, strlen(g_enb_ral_obj[instanceP].link_id));
    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_enb_ral_obj[instanceP].mihf_id, strlen(g_enb_ral_obj[instanceP].mihf_id));

    message.primitive.Status                   = *statusP;
    message.primitive.SupportedLinkEventList   = link_evt_listP;
    message.primitive.SupportedLinkCommandList = link_cmd_listP;

    message_total_length = MIH_C_Link_Message_Encode_Capability_Discover_confirm(bb_p, &message);

    if (eRAL_send_to_mih( instanceP, bb_p->m_buffer, message_total_length) < 0) {
        LOG_E(RAL_ENB, ": Send Link_Capability_Discover.confirm\n");
    } else {
        LOG_D(RAL_ENB, ": Sent Link_Capability_Discover.confirm\n");
    }
    free_BitBuffer(bb_p);
}

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_send_event_subscribe_confirm()                    **
 **                                                                        **
 ** Description: Sends a Link_Event_Subscribe.confirm message to the MIHF. **
 **                                                                        **
 **   This primitive is generated in response to a Link_Event_  **
 **   Subscribe.request.                                        **
 **                                                                        **
 ** Inputs:  tid_pP  Transaction identifier                     **
 **     statusP: Status of operation                        **
 **     levt_listP: List of successfully subscribed link events**
 **     Others: g_enb_ral_obj[instanceP].link_id, g_enb_ral_obj[instanceP].mihf_id                       **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: None                                       **
 **     Others: g_msg_codec_send_buffer                    **
 **                                                                        **
 ***************************************************************************/
void eRAL_send_event_subscribe_confirm(ral_enb_instance_t instanceP, MIH_C_TRANSACTION_ID_T  *tid_pP,
        MIH_C_STATUS_T          *statusP,
        MIH_C_LINK_EVENT_LIST_T *levt_listP)
{
    MIH_C_Message_Link_Event_Subscribe_confirm_t  message;
    Bit_Buffer_t                                 *bb_p;
    int                                           message_total_length;

    LOG_D(RAL_ENB, " Send MIH_C_MESSAGE_LINK_EVENT_SUBSCRIBE_CONFIRM\n");

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Event_Subscribe_confirm_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    //message.header.ack_req            = 0;
    //message.header.ack_rsp            = 0;
    //message.header.uir                = 0;
    //message.header.more_fragment      = 0
    //message.header.fragment_number    = 0;
    message.header.service_identifier   = (MIH_C_SID_T)1;
    message.header.operation_code       = (MIH_C_OPCODE_T)0;
    message.header.action_identifier    = (MIH_C_AID_T)4;
    message.header.transaction_id       = *tid_pP;

    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_enb_ral_obj[instanceP].link_id, strlen(g_enb_ral_obj[instanceP].link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_enb_ral_obj[instanceP].mihf_id, strlen(g_enb_ral_obj[instanceP].mihf_id));

    message.primitive.Status                   = *statusP;
    message.primitive.ResponseLinkEventList    = levt_listP;

    message_total_length = MIH_C_Link_Message_Encode_Event_Subscribe_confirm(bb_p, &message);

    if (eRAL_send_to_mih( instanceP, bb_p->m_buffer, message_total_length) < 0) {
        LOG_E(RAL_ENB, ": Send Link_Event_Subscribe.confirm\n");
    } else {
        LOG_D(RAL_ENB, ": Sent Link_Event_Subscribe.confirm\n");
    }
    free_BitBuffer(bb_p);
}

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_send_event_unsubscribe_confirm()                  **
 **                                                                        **
 ** Description: Sends a Link_Event_Unsubscribe.confirm message to the     **
 **   MIHF.                                                     **
 **                                                                        **
 **   This primitive is generated in response to a Link_Event_  **
 **   Unsubscribe.request.                                      **
 **                                                                        **
 ** Inputs:  tid_pP  Transaction identifier                     **
 **     statusP: Status of operation                        **
 **     levt_listP: List of successfully subscribed link events**
 **     Others: g_enb_ral_obj[instanceP].link_id, g_enb_ral_obj[instanceP].mihf_id                       **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: None                                       **
 **     Others: g_msg_codec_send_buffer                    **
 **                                                                        **
 ***************************************************************************/
void eRAL_send_event_unsubscribe_confirm(ral_enb_instance_t instanceP, MIH_C_TRANSACTION_ID_T  *tid_pP,
        MIH_C_STATUS_T          *statusP,
        MIH_C_LINK_EVENT_LIST_T *levt_listP)
{
    MIH_C_Message_Link_Event_Unsubscribe_confirm_t  message;
    Bit_Buffer_t                                   *bb_p;
    int                                             message_total_length;

    LOG_D(RAL_ENB, " Send MIH_C_MESSAGE_LINK_EVENT_UNSUBSCRIBE_CONFIRM\n");

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Event_Unsubscribe_confirm_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    //message.header.ack_req            = 0;
    //message.header.ack_rsp            = 0;
    //message.header.uir                = 0;
    //message.header.more_fragment      = 0
    //message.header.fragment_number    = 0;
    message.header.service_identifier   = (MIH_C_SID_T)1;
    message.header.operation_code       = (MIH_C_OPCODE_T)0;
    message.header.action_identifier    = (MIH_C_AID_T)5;
    message.header.transaction_id       = *tid_pP;

    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_enb_ral_obj[instanceP].link_id, strlen(g_enb_ral_obj[instanceP].link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_enb_ral_obj[instanceP].mihf_id, strlen(g_enb_ral_obj[instanceP].mihf_id));

    message.primitive.Status                   = *statusP;
    message.primitive.ResponseLinkEventList    = levt_listP;

    message_total_length = MIH_C_Link_Message_Encode_Event_Unsubscribe_confirm(bb_p, &message);

    if (eRAL_send_to_mih( instanceP, bb_p->m_buffer, message_total_length) < 0) {
        LOG_E(RAL_ENB, ": Send Link_Event_Unsubscribe.confirm\n");
    } else {
        LOG_D(RAL_ENB, ": Sent Link_Event_Unsubscribe.confirm\n");
    }
    free_BitBuffer(bb_p);
}

/*
 * -------------------------------------------------------------------------
 *  Functions used to send MIH link event messages
 * -------------------------------------------------------------------------
 */

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_send_link_detected_indication()                   **
 **                                                                        **
 ** Description: Sends a Link_Detected.indication message to the MIHF.     **
 **                                                                        **
 **   The Link Detected event is generated on the MN when the   **
 **   first PoA of an access network is detected. This event is **
 **   not generated when subsequent PoAs of the same access     **
 **   network are discovered during the active connection on    **
 **   that link.                                                **
 **                                                                        **
 ** Inputs:  tid_pP  Transaction identifier                     **
 **     linfoP: Information of the detected link           **
 **     Others: g_enb_ral_obj[instanceP].link_id, g_enb_ral_obj[instanceP].mihf_id                       **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: None                                       **
 **     Others: g_msg_codec_send_buffer                    **
 **                                                                        **
 ***************************************************************************/
void eRAL_send_link_detected_indication(ral_enb_instance_t instanceP, MIH_C_TRANSACTION_ID_T  *tid_pP, MIH_C_LINK_DET_INFO_T   *linfoP){
    MIH_C_Message_Link_Detected_indication_t  message;
    Bit_Buffer_t                             *bb_p;
    int                                       message_total_length;

    if (!(g_enb_ral_obj[instanceP].mih_supported_link_event_list & MIH_C_BIT_LINK_DETECTED)) {
        return;
    }

    LOG_D(RAL_ENB, " Send MIH_C_MESSAGE_LINK_DETECTED_INDICATION\n");

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Detected_indication_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    //message.header.ack_req            = 0;
    //message.header.ack_rsp            = 0;
    //message.header.uir                = 0;
    //message.header.more_fragment      = 0
    //message.header.fragment_number    = 0;
    message.header.service_identifier   = (MIH_C_SID_T)2;
    message.header.operation_code       = (MIH_C_OPCODE_T)3;
    message.header.action_identifier    = (MIH_C_AID_T)1;
    message.header.transaction_id       = *tid_pP;

    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_enb_ral_obj[instanceP].link_id, strlen(g_enb_ral_obj[instanceP].link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_enb_ral_obj[instanceP].mihf_id, strlen(g_enb_ral_obj[instanceP].mihf_id));

    memcpy(&message.primitive.LinkDetectedInfo, linfoP,
            sizeof(MIH_C_LINK_DET_INFO_T));

    message_total_length = MIH_C_Link_Message_Encode_Link_Detected_indication(bb_p, &message);

    if (eRAL_send_to_mih( instanceP, bb_p->m_buffer, message_total_length) < 0) {
        LOG_E(RAL_ENB, ": Send Link_Detected.indication\n");
    } else {
        LOG_D(RAL_ENB, ": Sent Link_Detected.indication\n");
    }
    free_BitBuffer(bb_p);
}

/****************************************************************************
 ** Name:  eRAL_send_link_up_indication()                               **
 ** Description: Sends a Link_Up.indication message to the MIHF.           **
 **                                                                        **
 **   This notification is generated when a layer 2 connection is          **
 **   established for the specified link interface.                        **
 ** Inputs:  tid_pP  Transaction identifier                                  **
 **          lid_pP:  Link identifier                                        **
 **          old_ar_pP: Old access router link address                       **
 **          new_arP: New access router link address                       **
 **          flagP:  Indicates whether the MN needs to change IP Address   **
 **                  in the new PoA                                        **
 **          mobil_mngtP: Indicates the type of Mobility Management        **
 **                       Protocol supported by the new PoA                **
 ***************************************************************************/
void eRAL_send_link_up_indication(ral_enb_instance_t instanceP, MIH_C_TRANSACTION_ID_T  *tid_pP,
        MIH_C_LINK_TUPLE_ID_T   *lid_pP,
        MIH_C_LINK_ADDR_T       *old_ar_pP,
        MIH_C_LINK_ADDR_T       *new_arP,
        MIH_C_IP_RENEWAL_FLAG_T *flagP,
        MIH_C_IP_MOB_MGMT_T     *mobil_mngtP)
{
    MIH_C_Message_Link_Up_indication_t  message;
    Bit_Buffer_t                       *bb_p = NULL;
    int                                 message_total_length;
    module_id_t                         mod_id = instanceP;

    if (!(g_enb_ral_obj[mod_id].mih_supported_link_event_list & MIH_C_BIT_LINK_UP)) {
        return;
    }

    LOG_D(RAL_ENB, " Send MIH_C_MESSAGE_LINK_UP_INDICATION\n");

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Up_indication_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    //message.header.ack_req            = 0;
    //message.header.ack_rsp            = 0;
    //message.header.uir                = 0;
    //message.header.more_fragment      = 0
    //message.header.fragment_number    = 0;
    message.header.service_identifier   = (MIH_C_SID_T)2;
    message.header.operation_code       = (MIH_C_OPCODE_T)3;
    message.header.action_identifier    = (MIH_C_AID_T)2;
    message.header.transaction_id       = *tid_pP;

    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_enb_ral_obj[mod_id].link_id, strlen(g_enb_ral_obj[instanceP].link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_enb_ral_obj[mod_id].mihf_id, strlen(g_enb_ral_obj[instanceP].mihf_id));

    memcpy(&message.primitive.LinkIdentifier, lid_pP, sizeof(MIH_C_LINK_TUPLE_ID_T));

    message.primitive.OldAccessRouter            = old_ar_pP;
    message.primitive.NewAccessRouter            = new_arP;
    message.primitive.IPRenewalFlag              = flagP;
    message.primitive.MobilityManagementSupport  = mobil_mngtP;

    message_total_length = MIH_C_Link_Message_Encode_Link_Up_indication(bb_p, &message);

    if (eRAL_send_to_mih( instanceP, bb_p->m_buffer, message_total_length) < 0) {
        LOG_E(RAL_ENB, ": Send Link_Up.indication\n");
    } else {
        LOG_D(RAL_ENB, ": Sent Link_Up.indication\n");
    }
    free_BitBuffer(bb_p);
}

/****************************************************************************
 ** Name:  eRAL_send_link_down_indication()                             **
 ** Description: Sends a Link_Down.indication message to the MIHF.         **
 **                                                                        **
 **   This notification is generated when layer 2 connectivity is lost.    **
 **   Layer 2 connectivity is lost explicitly in cases where the MN        **
 **   initiates detach type procedures. In other cases, the MN can infer   **
 **   loss of link connectivity due to successive time-outs for            **
 **   acknowledgements of retransmitted packets along with loss  of        **
 **   reception of broadcast frames.                                       **
 ** Inputs:  tid_pP  Transaction identifier                                  **
 **          lid_pP:  Link identifier                                        **
 **          old_ar_pP: Old access router link address                       **
 **          reason_codeP: Reason why the link went down                   **
 ***************************************************************************/
void eRAL_send_link_down_indication(ral_enb_instance_t instanceP, MIH_C_TRANSACTION_ID_T *tid_pP,
        MIH_C_LINK_TUPLE_ID_T  *lid_pP,
        MIH_C_LINK_ADDR_T      *old_ar_pP,
        MIH_C_LINK_DN_REASON_T *reason_codeP)
{
    MIH_C_Message_Link_Down_indication_t      message;
    Bit_Buffer_t                             *bb_p = NULL;
    int                                       message_total_length;

    if (!(g_enb_ral_obj[instanceP].mih_supported_link_event_list & MIH_C_BIT_LINK_DOWN)) {
        return;
    }

    LOG_D(RAL_ENB, " Send MIH_C_MESSAGE_LINK_DOWN_INDICATION\n");

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Going_Down_indication_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    message.header.service_identifier   = (MIH_C_SID_T)2;
    message.header.operation_code       = (MIH_C_OPCODE_T)3;
    message.header.action_identifier    = (MIH_C_AID_T)3;
    message.header.transaction_id       = *tid_pP;

    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_enb_ral_obj[instanceP].link_id, strlen(g_enb_ral_obj[instanceP].link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_enb_ral_obj[instanceP].mihf_id, strlen(g_enb_ral_obj[instanceP].mihf_id));

    memcpy(&message.primitive.LinkIdentifier, lid_pP, sizeof(MIH_C_LINK_TUPLE_ID_T));
    message.primitive.OldAccessRouter = old_ar_pP;
    memcpy(&message.primitive.ReasonCode, reason_codeP, sizeof(MIH_C_LINK_DN_REASON_T));

    message_total_length = MIH_C_Link_Message_Encode_Link_Down_indication(bb_p, &message);

    if (eRAL_send_to_mih( instanceP, bb_p->m_buffer, message_total_length) < 0) {
        LOG_E(RAL_ENB, ": Send Link_Down.indication\n");
    } else {
        LOG_D(RAL_ENB, ": Sent Link_Down.indication\n");
    }
    free_BitBuffer(bb_p);
}

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_send_link_parameters_report_indication()          **
 **                                                                        **
 ** Description: Sends a Link_Parameters_Report.indication message to the  **
 **   MIHF.                                                     **
 **                                                                        **
 **   For each specified parameter, this notification is gene-  **
 **   rated either at a predefined regular interval determined  **
 **   by a user configurable timer or when it crosses a confi-  **
 **   gured threshold.                                          **
 **                                                                        **
 ** Inputs:  tid_pP  Transaction identifier                     **
 **     lid_pP:  Link identifier                            **
 **     lparam_listP: List of link parameter reports             **
 **     Others: g_enb_ral_obj[instanceP].link_id, g_enb_ral_obj[instanceP].mihf_id                       **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: None                                       **
 **     Others: g_msg_codec_send_buffer                    **
 **                                                                        **
 ***************************************************************************/
void eRAL_send_link_parameters_report_indication(ral_enb_instance_t instanceP, MIH_C_TRANSACTION_ID_T *tid_pP,
        MIH_C_LINK_TUPLE_ID_T       *lid_pP,
        MIH_C_LINK_PARAM_RPT_LIST_T *lparam_listP)
{
    MIH_C_Message_Link_Parameters_Report_indication_t  message;
    Bit_Buffer_t                                       *bb_p;
    int                                                message_total_length;

    if (!(g_enb_ral_obj[instanceP].mih_supported_link_event_list & MIH_C_BIT_LINK_PARAMETERS_REPORT)) {
        return;
    }

    LOG_D(RAL_ENB, " Send MIH_C_MESSAGE_LINK_PARAMETERS_REPORT_INDICATION\n");

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Parameters_Report_indication_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    //message.header.ack_req            = 0;
    //message.header.ack_rsp            = 0;
    //message.header.uir                = 0;
    //message.header.more_fragment      = 0
    //message.header.fragment_number    = 0;
    message.header.service_identifier   = (MIH_C_SID_T)2;
    message.header.operation_code       = (MIH_C_OPCODE_T)3;
    message.header.action_identifier    = (MIH_C_AID_T)5;
    message.header.transaction_id       = *tid_pP;

    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_enb_ral_obj[instanceP].link_id, strlen(g_enb_ral_obj[instanceP].link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_enb_ral_obj[instanceP].mihf_id, strlen(g_enb_ral_obj[instanceP].mihf_id));


    memcpy(&message.primitive.LinkIdentifier, lid_pP, sizeof(MIH_C_LINK_TUPLE_ID_T));
    memcpy(&message.primitive.LinkParametersReportList_list, lparam_listP, sizeof(MIH_C_LINK_PARAM_RPT_LIST_T));

    message_total_length = MIH_C_Link_Message_Encode_Link_Parameters_Report_indication(bb_p, &message);

    if (eRAL_send_to_mih( instanceP, bb_p->m_buffer, message_total_length) < 0) {
        LOG_E(RAL_ENB, ": Send Link_Parameters_Report.indication\n");
    } else {
        LOG_D(RAL_ENB, ": Sent Link_Parameters_Report.indication\n");
    }
    free_BitBuffer(bb_p);
}

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_send_link_going_down_indication()                 **
 **                                                                        **
 ** Description: Sends a Link_Going_Down.indication message to the MIHF.   **
 **                                                                        **
 **   A Link_Going_Down event implies that a Link_Down is immi- **
 **   nent within a certain time interval. If Link_Down is NOT  **
 **   received within specified time interval then actions due  **
 **   to previous Link_Going_Down are ignored.                  **
 **                                                                        **
 ** Inputs:  tid_pP  Transaction identifier                     **
 **     lid_pP:  Link identifier                            **
 **     timeP:  The time interval (ms) at which the link   **
 **    is expected to go down (0 if unknown)      **
 **     lreasonP: Reason why the link is going to be down    **
 **     Others: g_enb_ral_obj[instanceP].link_id, g_enb_ral_obj[instanceP].mihf_id                       **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: None                                       **
 **     Others: g_msg_codec_send_buffer                    **
 **                                                                        **
 ***************************************************************************/
void eRAL_send_link_going_down_indication(ral_enb_instance_t instanceP, MIH_C_TRANSACTION_ID_T *tid_pP,
        MIH_C_LINK_TUPLE_ID_T  *lid_pP,
        MIH_C_UNSIGNED_INT2_T  *timeP,
        MIH_C_LINK_GD_REASON_T *lreasonP)
{
    MIH_C_Message_Link_Going_Down_indication_t  message;
    Bit_Buffer_t                                *bb_p;
    int                                         message_total_length;

    if (!(g_enb_ral_obj[instanceP].mih_supported_link_event_list & MIH_C_BIT_LINK_GOING_DOWN)) {
        return;
    }

    LOG_D(RAL_ENB, " Send MIH_C_MESSAGE_LINK_GOING_DOWN_INDICATION\n");

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Going_Down_indication_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    //message.header.ack_req            = 0;
    //message.header.ack_rsp            = 0;
    //message.header.uir                = 0;
    //message.header.more_fragment      = 0
    //message.header.fragment_number    = 0;
    message.header.service_identifier   = (MIH_C_SID_T)2;
    message.header.operation_code       = (MIH_C_OPCODE_T)3;
    message.header.action_identifier    = (MIH_C_AID_T)6;
    message.header.transaction_id       = *tid_pP;

    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_enb_ral_obj[instanceP].link_id, strlen(g_enb_ral_obj[instanceP].link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_enb_ral_obj[instanceP].mihf_id, strlen(g_enb_ral_obj[instanceP].mihf_id));


    memcpy(&message.primitive.LinkIdentifier, lid_pP, sizeof(MIH_C_LINK_TUPLE_ID_T));
    message.primitive.TimeInterval = *timeP;
    memcpy(&message.primitive.LinkGoingDownReason, lreasonP,
            sizeof(MIH_C_LINK_GD_REASON_T));

    message_total_length = MIH_C_Link_Message_Encode_Link_Going_Down_indication(bb_p, &message);

    if (eRAL_send_to_mih( instanceP, bb_p->m_buffer, message_total_length) < 0) {
        LOG_E(RAL_ENB, ": Send Link_Going_Down.indication\n");
    } else {
        LOG_D(RAL_ENB, ": Sent Link_Going_Down.indication\n");
    }
    free_BitBuffer(bb_p);
}

/*
 * -------------------------------------------------------------------------
 *  Functions used to send MIH link command messages
 * -------------------------------------------------------------------------
 */

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_send_get_parameters_confirm()                     **
 **                                                                        **
 ** Description: Sends a Link_Get_Parameters.confirm message to the MIHF.  **
 **                                                                        **
 **   This primitive is generated in response to a Link_Get_    **
 **   Parameters.request.                                       **
 **                                                                        **
 ** Inputs:  tid_pP  Transaction identifier                     **
 **     statusP: Status of operation                        **
 **     lparam_listP: List of measurable link parameters and     **
 **    their current values                       **
 **     lstates_listP: List of current link state information     **
 **     ldesc_listP: List of link descriptors                   **
 **     Others: g_enb_ral_obj[instanceP].link_id, g_enb_ral_obj[instanceP].mihf_id                       **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: None                                       **
 **     Others: g_msg_codec_send_buffer                    **
 **                                                                        **
 ***************************************************************************/
void eRAL_send_get_parameters_confirm(ral_enb_instance_t instanceP, MIH_C_TRANSACTION_ID_T       *tid_pP,
        MIH_C_STATUS_T               *statusP,
        MIH_C_LINK_PARAM_LIST_T      *lparam_listP,
        MIH_C_LINK_STATES_RSP_LIST_T *lstates_listP,
        MIH_C_LINK_DESC_RSP_LIST_T   *ldesc_listP)
{
    MIH_C_Message_Link_Get_Parameters_confirm_t  message;
    Bit_Buffer_t                                *bb_p;
    int                                          message_total_length;

    LOG_D(RAL_ENB, " Send MIH_C_MESSAGE_LINK_GET_PARAMETERS_CONFIRM\n");

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Get_Parameters_confirm_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    //message.header.ack_req            = 0;
    //message.header.ack_rsp            = 0;
    //message.header.uir                = 0;
    //message.header.more_fragment      = 0
    //message.header.fragment_number    = 0;
    message.header.service_identifier   = (MIH_C_SID_T)3;
    message.header.operation_code       = (MIH_C_OPCODE_T)0;
    message.header.action_identifier    = (MIH_C_AID_T)1;
    message.header.transaction_id       = *tid_pP;

    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_enb_ral_obj[instanceP].link_id, strlen(g_enb_ral_obj[instanceP].link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_enb_ral_obj[instanceP].mihf_id, strlen(g_enb_ral_obj[instanceP].mihf_id));

    message.primitive.Status                        = *statusP;
    message.primitive.LinkParametersStatusList_list = lparam_listP;
    message.primitive.LinkStatesResponse_list       = lstates_listP;
    message.primitive.LinkDescriptorsResponse_list  = ldesc_listP;

    message_total_length = MIH_C_Link_Message_Encode_Get_Parameters_confirm(bb_p, &message);

    if (eRAL_send_to_mih( instanceP, bb_p->m_buffer, message_total_length) < 0) {
        LOG_E(RAL_ENB, ": Send Link_Get_Parameters.confirm\n");
    } else {
        LOG_D(RAL_ENB, ": Sent Link_Get_Parameters.confirm\n");
    }
    free_BitBuffer(bb_p);
}

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_send_configure_thresholds_confirm()               **
 **                                                                        **
 ** Description: Sends a Link_Configure_Thresholds.confirm message to the  **
 **   MIHF.                                                     **
 **                                                                        **
 **   This primitive is generated in response to a Link_        **
 **   Configure_Thresholds.request.                             **
 **                                                                        **
 ** Inputs:  tid_pP  Transaction identifier                     **
 **     statusP: Status of operation                        **
 **     lstatus_listP: List of link configure status              **
 **     Others: g_enb_ral_obj[instanceP].link_id, g_enb_ral_obj[instanceP].mihf_id                       **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: None                                       **
 **     Others: g_msg_codec_send_buffer                    **
 **                                                                        **
 ***************************************************************************/
void eRAL_send_configure_thresholds_confirm(ral_enb_instance_t instanceP, MIH_C_TRANSACTION_ID_T *tid_pP,
        MIH_C_STATUS_T               *statusP,
        MIH_C_LINK_CFG_STATUS_LIST_T *lstatus_listP)
{
    MIH_C_Message_Link_Configure_Thresholds_confirm_t  message;
    Bit_Buffer_t                                      *bb_p;
    int                                                message_total_length;

    LOG_D(RAL_ENB, " Send MIH_C_MESSAGE_LINK_CONFIGURE_THRESHOLDS_CONFIRM\n");

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Configure_Thresholds_confirm_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    //message.header.ack_req            = 0;
    //message.header.ack_rsp            = 0;
    //message.header.uir                = 0;
    //message.header.more_fragment      = 0
    //message.header.fragment_number    = 0;
    message.header.service_identifier   = (MIH_C_SID_T)3;
    message.header.operation_code       = (MIH_C_OPCODE_T)0;
    message.header.action_identifier    = (MIH_C_AID_T)2;
    message.header.transaction_id       = *tid_pP;

    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_enb_ral_obj[instanceP].link_id, strlen(g_enb_ral_obj[instanceP].link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_enb_ral_obj[instanceP].mihf_id, strlen(g_enb_ral_obj[instanceP].mihf_id));

    message.primitive.Status                        = *statusP;
    message.primitive.LinkConfigureStatusList_list  =  lstatus_listP;

    message_total_length = MIH_C_Link_Message_Encode_Configure_Thresholds_confirm(bb_p, &message);

    if (eRAL_send_to_mih( instanceP, bb_p->m_buffer, message_total_length) < 0) {
        LOG_E(RAL_ENB, ": Send Link_Configure_Threshold.confirm\n");
    } else {
        LOG_D(RAL_ENB, ": Sent Link_Configure_Threshold.confirm\n");
    }
    free_BitBuffer(bb_p);
}

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_send_link_action_confirm()                        **
 **                                                                        **
 ** Description: Sends a Link_Action.confirm message to the MIHF.          **
 **                                                                        **
 **   This primitive is generated to communicate the result of  **
 **   the action executed on the link-layer connection.         **
 **                                                                        **
 ** Inputs:  tid_pP  Transaction identifier                     **
 **     statusP: Status of operation                        **
 **     response_setP: List of discovered links and related       **
 **    information                                **
 **     action_resultP: Specifies whether the link action was     **
 **     successful                                **
 **     Others: g_enb_ral_obj[instanceP].link_id, g_enb_ral_obj[instanceP].mihf_id                       **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: None                                       **
 **     Others: g_msg_codec_send_buffer                    **
 **                                                                        **
 ***************************************************************************/
void eRAL_send_link_action_confirm(
        ral_enb_instance_t instanceP,
        MIH_C_TRANSACTION_ID_T     *tid_pP,
        MIH_C_STATUS_T             *statusP,
        MIH_C_LINK_SCAN_RSP_LIST_T *response_setP,
        MIH_C_LINK_AC_RESULT_T     *action_resultP)
{
    MIH_C_Message_Link_Action_confirm_t       message;
    Bit_Buffer_t                             *bb_p;
    int                                       message_total_length;

    LOG_D(RAL_ENB, " Send MIH_C_MESSAGE_LINK_ACTION_CONFIRM\n");

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Action_confirm_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    //message.header.ack_req            = 0;
    //message.header.ack_rsp            = 0;
    //message.header.uir                = 0;
    //message.header.more_fragment      = 0
    //message.header.fragment_number    = 0;
    message.header.service_identifier   = (MIH_C_SID_T)3;
    message.header.operation_code       = (MIH_C_OPCODE_T)0;
    message.header.action_identifier    = (MIH_C_AID_T)3;
    message.header.transaction_id       = *tid_pP;

    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_enb_ral_obj[instanceP].link_id, strlen(g_enb_ral_obj[instanceP].link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_enb_ral_obj[instanceP].mihf_id, strlen(g_enb_ral_obj[instanceP].mihf_id));

    message.primitive.Status                       = *statusP;
    message.primitive.ScanResponseSet_list         = response_setP;
    message.primitive.LinkActionResult             = action_resultP;

    message_total_length = MIH_C_Link_Message_Encode_Link_Action_confirm(bb_p, &message);

    if (eRAL_send_to_mih( instanceP, bb_p->m_buffer, message_total_length) < 0) {
        LOG_E(RAL_ENB, ": Send Link_Action.confirm\n");
    } else {
        LOG_D(RAL_ENB, ": Sent Link_Action.confirm\n");
    }
    free_BitBuffer(bb_p);
}

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_mihf_connect(ral_enb_instance_t instanceP)                 **
 **                                                                        **
 ** Description: Connects the RAL to the remote MIH Function.              **
 **   The calling process should exit upon connection failure              **
 **   in order to properly close the MIH-F socket file                     **
 **   descriptor and free the automatically allocated addrinfo             **
 **   structure.                                                           **
 **                                                                        **
 ** Inputs:  None                                                          **
 **     Others: g_enb_ral_obj[instanceP].mihf_ip_address, g_enb_ral_obj[instanceP].mihf_remote_port                      **
 **    g_enb_ral_obj[instanceP].ral_ip_address,                                                   **
 **    g_enb_ral_obj[instanceP].ral_listening_port                                       **
 **                                                                        **
 ** Outputs:  None                                                         **
 **   Return: 0 on success, -1 on failure                                  **
 **     Others: g_sockd_mihf                                               **
 **                                                                        **
 ****************************************************************************/
int eRAL_mihf_connect(ral_enb_instance_t instanceP)
{
    struct addrinfo info;  /* endpoint information  */
    struct addrinfo *addr, *rp; /* endpoint address  */
    int   rc;  /* returned error code  */
    int   optval;  /* socket option value  */

    unsigned char buf[sizeof(struct sockaddr_in6)];

    /*
     * Initialize the remote MIH-F endpoint address information
     */
    memset(&info, 0, sizeof(struct addrinfo));
    info.ai_family   = AF_UNSPEC; /* Allow IPv4 or IPv6 */
    info.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    info.ai_flags    = 0;
    info.ai_protocol = 0;  /* Any protocol  */

    rc = getaddrinfo(g_enb_ral_obj[instanceP].mihf_ip_address, g_enb_ral_obj[instanceP].mihf_remote_port, &info, &addr);
    if (rc != 0) {
        LOG_E(RAL_ENB, " getaddrinfo: %s\n", gai_strerror(rc));
        return -1;
    }

    /*
     * getaddrinfo() returns a linked list of address structures.
     * Try each address until we successfully connect(2). If socket(2)
     * (or connect(2)) fails, we (close the socket and) try the next address.
     */
    for (rp = addr; rp != NULL; rp = rp->ai_next) {

        g_enb_ral_obj[instanceP].mih_sock_desc = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (g_enb_ral_obj[instanceP].mih_sock_desc < 0) {
            continue;
        }

        optval = 1;
        setsockopt(g_enb_ral_obj[instanceP].mih_sock_desc, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

        /*
         * Convert the RAL local network address
         */
        if (rp->ai_family == AF_INET) {
            /* IPv4 network address family */
            struct sockaddr_in  *addr4 = NULL;

            LOG_D(RAL_ENB, " %s is an ipv4 address\n", g_enb_ral_obj[instanceP].mihf_ip_address);
            addr4             = (struct sockaddr_in *)(&buf[0]);
            addr4->sin_port   = htons(atoi(g_enb_ral_obj[instanceP].ral_listening_port));
            addr4->sin_family = AF_INET;
            rc = inet_pton(AF_INET, g_enb_ral_obj[instanceP].ral_ip_address, &addr4->sin_addr);
        }
        else if (rp->ai_family == AF_INET6) {
            /* IPv6 network address family */
            struct sockaddr_in6 *addr6 = NULL;

            LOG_D(RAL_ENB, " %s is an ipv6 address\n", g_enb_ral_obj[instanceP].mihf_ip_address);
            addr6              = (struct sockaddr_in6 *)(&buf[0]);
            addr6->sin6_port   = htons(atoi(g_enb_ral_obj[instanceP].ral_listening_port));
            addr6->sin6_family = AF_INET6;
            rc = inet_pton(AF_INET, g_enb_ral_obj[instanceP].ral_ip_address, &addr6->sin6_addr);
        }
        else {
            LOG_E(RAL_ENB, " %s is an unknown address format %d\n",
                    g_enb_ral_obj[instanceP].mihf_ip_address, rp->ai_family);
            return -1;
        }

        if (rc < 0) {
            /* The network address convertion failed */
            LOG_E(RAL_ENB, " inet_pton(RAL IP address %s): %s\n",
                    g_enb_ral_obj[instanceP].ral_ip_address, strerror(rc));
            return -1;
        }
        else if (rc == 0) {
            /* The network address is not valid */
            LOG_E(RAL_ENB, " RAL IP address %s is not valid\n", g_enb_ral_obj[instanceP].ral_ip_address);
            return -1;
        }

        /* Bind the socket to the local RAL network address */
        rc = bind(g_enb_ral_obj[instanceP].mih_sock_desc, (const struct sockaddr *)buf,
                sizeof(struct sockaddr_in));

        if (rc < 0) {
            LOG_E(RAL_ENB, " bind(RAL IP address %s): %s\n",
                    g_enb_ral_obj[instanceP].ral_ip_address, strerror(errno));
            return -1;
        }

        /* Connect the socket to the remote MIH-F network address */
        if (connect(g_enb_ral_obj[instanceP].mih_sock_desc, rp->ai_addr, rp->ai_addrlen) == 0) {
            LOG_N(RAL_ENB, " RAL [%s:%s] is now UDP-CONNECTED to MIH-F [%s:%s]\n",
                    g_enb_ral_obj[instanceP].ral_ip_address, g_enb_ral_obj[instanceP].ral_listening_port,
                    g_enb_ral_obj[instanceP].mihf_ip_address, g_enb_ral_obj[instanceP].mihf_remote_port);
            break;
        }
        /*
         * We failed to connect:
         * Close the socket file descriptor and try to connect to an other
         * address.
         */
        close(g_enb_ral_obj[instanceP].mih_sock_desc);
    }

    /*
     * Unable to connect to a network address
     */
    if (rp == NULL) {
        LOG_E(RAL_ENB, " Could not connect to MIH-F\n");
        return -1;
    }

    freeaddrinfo(addr);

    return 0;
}

/****************************************************************************
 ** Name:  eRAL_mih_link_process_message()                              **
 ** Description: Processes messages received from the MIH-F.               **
 **     Others: g_sockd_mihf                                               **
 **   Return: Always return 0                                              **
 **     Others: g_msg_codec_recv_buffer                                    **
 ***************************************************************************/
int eRAL_mih_link_process_message(ral_enb_instance_t instanceP){
    MIH_C_Message_Wrapper_t  message_wrapper;
    int                      nb_bytes_received ;
    int                      nb_bytes_decoded  ;
    int                      total_bytes_to_decode ;
    int                      status ;
    Bit_Buffer_t            *bb_p;
    struct sockaddr_in       udp_socket;
    socklen_t                sockaddr_len;

    LOG_D(RAL_ENB,"HEEERRRE process mesage\n");

    total_bytes_to_decode = 0;
    nb_bytes_received     = 0;

    bb_p = new_BitBuffer_0();

    nb_bytes_received = recvfrom(g_enb_ral_obj[instanceP].mih_sock_desc,
            (void *)g_msg_codec_recv_buffer,
            MSG_CODEC_RECV_BUFFER_SIZE,
            0,
            (struct sockaddr *) &udp_socket,
            &sockaddr_len);

    if (nb_bytes_received > 0) {
        LOG_D(RAL_ENB, " \n");
        LOG_D(RAL_ENB, " %s Received %d bytes\n", __FUNCTION__, nb_bytes_received);
        eRAL_print_buffer(g_msg_codec_recv_buffer, nb_bytes_received);
        total_bytes_to_decode += nb_bytes_received;
        BitBuffer_wrap(bb_p, g_msg_codec_recv_buffer, total_bytes_to_decode);
        /* Decode the message received from the MIHF */
        status  = eRAL_mih_link_msg_decode(instanceP, bb_p, &message_wrapper);
        if (status == MIH_MESSAGE_DECODE_OK) {
            nb_bytes_decoded = BitBuffer_getPosition(bb_p);
            if (nb_bytes_decoded > 0) {
                total_bytes_to_decode = total_bytes_to_decode - nb_bytes_decoded;
                // if remaining bytes to decode
                if (total_bytes_to_decode > 0) {
                    //shift left bytes in buffer
                    memcpy(g_msg_codec_recv_buffer, &g_msg_codec_recv_buffer[nb_bytes_decoded], nb_bytes_decoded);
                    //shift left again bytes in buffer
                    if (total_bytes_to_decode > nb_bytes_decoded) {
                        memcpy(&g_msg_codec_recv_buffer[nb_bytes_decoded], &g_msg_codec_recv_buffer[nb_bytes_decoded], total_bytes_to_decode - nb_bytes_decoded);
                    }
                    // not necessary
                    memset(&g_msg_codec_recv_buffer[total_bytes_to_decode], 0 , MSG_CODEC_RECV_BUFFER_SIZE - total_bytes_to_decode);
                }
            }
            // data could not be decoded
        } else if (status == MIH_MESSAGE_DECODE_FAILURE) {
            memset(g_msg_codec_recv_buffer, 0, MSG_CODEC_RECV_BUFFER_SIZE);
            total_bytes_to_decode = 0;
        } else if (status == MIH_MESSAGE_DECODE_TOO_SHORT) {
        } else if (status == MIH_MESSAGE_DECODE_BAD_PARAMETER) {
        }
    }
    free_BitBuffer(bb_p);
    return 0;
}

/****************************************************************************/
/*********************  L O C A L    F U N C T I O N S  *********************/
/****************************************************************************/

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_print_buffer()                                    **
 **                                                                        **
 ** Description: Print the content of a buffer in hexadecimal.             **
 **                                                                        **
 ** Inputs:  buffer_pP: Pointer to the buffer to print             **
 **     lengthP: Length of the buffer to print              **
 **     Others: g_msg_codec_print_buffer                   **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: None                                       **
 **     Others: None                                       **
 **                                                                        **
 ***************************************************************************/
void eRAL_print_buffer(const u_int8_t * buffer_pP, int lenP)
{
    char          c;
    unsigned int  buffer_index = 0;
    unsigned int  index;
    unsigned int  octet_index  = 0;
    unsigned long char_index   = 0;

    if (buffer_pP == NULL) {
        return;
    }

    buffer_index += sprintf(&g_msg_codec_print_buffer[buffer_index],"\n------+-------------------------------------------------+------------------+\n");
    buffer_index += sprintf(&g_msg_codec_print_buffer[buffer_index], "      |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef |\n");
    buffer_index += sprintf(&g_msg_codec_print_buffer[buffer_index], "------+-------------------------------------------------+------------------+\n");
    for (octet_index = 0; octet_index < lenP; octet_index++) {
        if ((octet_index % 16) == 0) {
            if (octet_index != 0) {
                buffer_index += sprintf(&g_msg_codec_print_buffer[buffer_index], " | ");
                for (char_index = octet_index - 16; char_index < octet_index; char_index++) {
                    c = (char) buffer_pP[char_index] & 0177;
                    if (iscntrl(c) || isspace(c)) {
                        buffer_index += sprintf(&g_msg_codec_print_buffer[buffer_index], " ");
                    } else {
                        buffer_index += sprintf(&g_msg_codec_print_buffer[buffer_index], "%c", c);
                    }
                }
                buffer_index += sprintf(&g_msg_codec_print_buffer[buffer_index], " |\n");
            }
            buffer_index += sprintf(&g_msg_codec_print_buffer[buffer_index], " %04d |", octet_index);
        }
        /*
         * Print every single octet in hexadecimal form
         */
        buffer_index += sprintf(&g_msg_codec_print_buffer[buffer_index], " %02x", (u_int8_t)(buffer_pP[octet_index] & 0x00FF));
    }

    /*
     * Append enough spaces and put final pipe
     */
    if ((lenP % 16) > 0) {
        for (index = (octet_index % 16); index < 16; ++index) {
            buffer_index += sprintf(&g_msg_codec_print_buffer[buffer_index], "   ");
        }
    }
    buffer_index += sprintf(&g_msg_codec_print_buffer[buffer_index], " | ");
    for (char_index = (octet_index / 16) * 16; char_index < octet_index; char_index++)
    {
        c = (char) buffer_pP[char_index] & 0177;
        if (iscntrl(c) || isspace(c)) {
            buffer_index += sprintf(&g_msg_codec_print_buffer[buffer_index], " ");
        } else {
            buffer_index += sprintf(&g_msg_codec_print_buffer[buffer_index], "%c", c);
        }
    }
    if ((lenP % 16) > 0) {
        for (index = (octet_index % 16); index < 16; ++index) {
            buffer_index += sprintf(&g_msg_codec_print_buffer[buffer_index], " ");
        }
    }
    buffer_index += sprintf(&g_msg_codec_print_buffer[buffer_index], " |\n");
    buffer_index += sprintf(&g_msg_codec_print_buffer[buffer_index], "------+-------------------------------------------------+------------------+\n");
    LOG_D(RAL_ENB, g_msg_codec_print_buffer);
}

/****************************************************************************
 **                                                                        **
 ** Name:  eRAL_send_to_mih( instanceP, )                                     **
 **                                                                        **
 ** Description: Sends a buffered message to the MIH-F.                    **
 **                                                                        **
 ** Inputs:  buffer_pP: Pointer to the buffered buffer to send     **
 **     lenP:  Length of the buffered buffer to send      **
 **     Others: g_sockd_mihf                               **
 **                                                                        **
 ** Outputs:  None                                                      **
 **   Return: The number of bytes actually sent          **
 **     Others: None                                       **
 **                                                                        **
 ***************************************************************************/
int eRAL_send_to_mih(ral_enb_instance_t instanceP, const u_int8_t *buffer_pP, int lenP)
{
    int result;
    eRAL_print_buffer(buffer_pP, lenP);
    result = send(g_enb_ral_obj[instanceP].mih_sock_desc, (const void *)buffer_pP, lenP, 0);
    if (result != lenP) {
        LOG_E(RAL_ENB, " %s : %d bytes failed, returned %d: %s\n",
                __FUNCTION__, lenP, result, strerror(errno));
    }
    return result;
}

/****************************************************************************
 ** Name:  eRAL_mih_link_msg_decode()                                   **
 ** Description: Decode messages received from the MIH-F.                  **
 ** Inputs:  bb_pP:  Pointer to the buffer to decode                         **
 ** Outputs:  message_wrapper_pP:                                            **
 **    Pointer to the message wrapper                                      **
 **   Return: < 0 on failure, 0 otherwise                                  **
 ***************************************************************************/
int eRAL_mih_link_msg_decode(ral_enb_instance_t instanceP, Bit_Buffer_t* bb_pP, MIH_C_Message_Wrapper_t *message_wrapper_pP){
    //---------------------------------------------------------------------------
    int            status = MIH_MESSAGE_DECODE_FAILURE;
    MIH_C_HEADER_T header;
    MIH_C_STATUS_T mih_status;
    
    LOG_D(RAL_ENB,"HEEERRRE decode\n");

    if ((bb_pP != NULL) && (message_wrapper_pP != NULL)){
        /*
         * Decode MIH protocol header
         */
        status = MIH_C_Link_Header_Decode(bb_pP, &header);
        if (status == MIH_HEADER_DECODE_TOO_SHORT) {
            return MIH_MESSAGE_DECODE_TOO_SHORT;
        } else if (status == MIH_HEADER_DECODE_FAILURE) {
            return MIH_MESSAGE_DECODE_FAILURE;
        } else if (status == MIH_HEADER_DECODE_BAD_PARAMETER) {
            return MIH_MESSAGE_DECODE_BAD_PARAMETER;
        }
        message_wrapper_pP->message_id = MIH_C_MESSAGE_ID(header.service_identifier, header.operation_code, header.action_identifier);

        /*
         * Decode MIH primitives
         */
        
        switch (message_wrapper_pP->message_id) {

            case MIH_C_MESSAGE_LINK_CAPABILITY_DISCOVER_REQUEST_ID:
                /*
                 * This primitive is generated by the MIHF when it needs to
                 * receive link-layer event notifications and learn about which
                 * link-layer commands the lower layer can support.
                 * The recipient responds immediately with Link_Capability_
                 * Discover.confirm primitive.
                 */
                LOG_D(RAL_ENB, " %s Received MIH_C_MESSAGE_LINK_CAPABILITY_DISCOVER_REQUEST\n", __FUNCTION__);
                memcpy(&message_wrapper_pP->_union_message.link_capability_discover_request.header, (const void *)&header, sizeof(MIH_C_HEADER_T));

                /* Decode Link_Capability_Discover.request */
                status = MIH_C_Link_Message_Decode_Link_Capability_Discover_request(bb_pP, &message_wrapper_pP->_union_message.link_capability_discover_request);
                if (status == MIH_MESSAGE_DECODE_OK) {

                    /* Process Link_Capability_Discover.request */
                    MIH_C_Link_Message_Link_Capability_Discover_request2String(&message_wrapper_pP->_union_message.link_capability_discover_request, g_msg_print_buffer);
                    LOG_D(RAL_ENB, " %s", g_msg_print_buffer);
                    mih_status = MIH_C_STATUS_SUCCESS;
                    LOG_D(RAL_ENB, "**\n");
                    /* Send Link_Capability_Discover.confirm */
                    eRAL_send_capability_discover_confirm(instanceP, &message_wrapper_pP->_union_message.link_capability_discover_request.header.transaction_id,
                            &mih_status,
                            &g_enb_ral_obj[instanceP].mih_supported_link_event_list,
                            &g_enb_ral_obj[instanceP].mih_supported_link_command_list);
                } else {
                }
                break;

            case MIH_C_MESSAGE_LINK_EVENT_SUBSCRIBE_REQUEST_ID:
                /*
                 * This primitive is generated by a subscriber such as the MIHF
                 * that is seeking to receive event indications from different
                 * link-layer technologies.
                 * The recipient responds immediately with Link_Event_Subscribe.
                 * confirm primitive.
                 */
                LOG_D(RAL_ENB, " %s Received MIH_C_MESSAGE_LINK_EVENT_SUBSCRIBE_REQUEST\n", __FUNCTION__);
                memcpy(&message_wrapper_pP->_union_message.link_event_subscribe_request.header, (const void *)&header, sizeof(MIH_C_HEADER_T));

                /* Decode Link_Event_Subscribe.request */
                status = MIH_C_Link_Message_Decode_Link_Event_Subscribe_request(bb_pP, &message_wrapper_pP->_union_message.link_event_subscribe_request);
                if (status == MIH_MESSAGE_DECODE_OK) {
                    /* Process Link_Event_Subscribe.request */
                    LOG_D(RAL_ENB, "**\n");
                    eRAL_subscribe_request(instanceP, &message_wrapper_pP->_union_message.link_event_subscribe_request);

                } else {
                }
                break;

            case MIH_C_MESSAGE_LINK_EVENT_UNSUBSCRIBE_REQUEST_ID:
                /*
                 * This primitive is generated by a subscriber such as the MIHF
                 * that is seeking to unsubscribe from an already subscribed set
                 * of events.
                 * The recipient responds immediately with Link_Event_
                 * Unsubscribe.confirm primitive.
                 */
                LOG_D(RAL_ENB, " %s Received MIH_C_MESSAGE_LINK_EVENT_UNSUBSCRIBE_REQUEST\n", __FUNCTION__);
                memcpy(&message_wrapper_pP->_union_message.link_event_unsubscribe_request.header, (const void *)&header, sizeof(MIH_C_HEADER_T));

                /* Decode Link_Event_Unsubscribe.request */
                status = MIH_C_Link_Message_Decode_Link_Event_Unsubscribe_request(bb_pP, &message_wrapper_pP->_union_message.link_event_unsubscribe_request);
                if (status == MIH_MESSAGE_DECODE_OK) {
                    LOG_D(RAL_ENB, "**\n");
                    /* Process Link_Event_Unsubscribe.request */
                    eRAL_unsubscribe_request(instanceP, &message_wrapper_pP->_union_message.link_event_unsubscribe_request);

                } else {
                }
                break;

            case MIH_C_MESSAGE_LINK_GET_PARAMETERS_REQUEST_ID:
                /*
                 * This primitive is generated by the MIHF to obtain the current
                 * value of a set of link parameters from a link.
                 * The recipient link responds with Link_Get_Parameters.confirm
                 * primitive.
                 */
                LOG_D(RAL_ENB, " %s Received MIH_C_MESSAGE_LINK_GET_PARAMETERS_REQUEST\n", __FUNCTION__);
                memcpy(&message_wrapper_pP->_union_message.link_get_parameters_request.header, (const void *)&header, sizeof(MIH_C_HEADER_T));

                /* Decode Link_Get_Parameters.request */
                status = MIH_C_Link_Message_Decode_Link_Get_Parameters_request(bb_pP, &message_wrapper_pP->_union_message.link_get_parameters_request);
                if (status == MIH_MESSAGE_DECODE_OK) {
                    LOG_D(RAL_ENB, "**\n");
                    /* Process Link_Get_Parameters.request */
                    eRAL_get_parameters_request(instanceP, &message_wrapper_pP->_union_message.link_get_parameters_request);
                } else {
                }
                break;

            case MIH_C_MESSAGE_LINK_CONFIGURE_THRESHOLDS_REQUEST_ID:
                /*
                 * This primitive is generated by an MIHF that needs to set
                 * threshold values for different link parameters.
                 * The recipient responds immediately with Link_Configure_
                 * Thresholds.confirm primitive.
                 */
                LOG_D(RAL_ENB, " %s Received MIH_C_MESSAGE_LINK_CONFIGURE_THRESHOLDS_REQUEST\n", __FUNCTION__);
                memcpy(&message_wrapper_pP->_union_message.link_configure_thresholds_request.header, (const void *)&header, sizeof(MIH_C_HEADER_T));

                /* Decode Link_Configure_Thresholds.request */
                status = MIH_C_Link_Message_Decode_Link_Configure_Thresholds_request(bb_pP, &message_wrapper_pP->_union_message.link_configure_thresholds_request);
                if (status == MIH_MESSAGE_DECODE_OK) {
                    LOG_D(RAL_ENB, "**\n");
                    /* Process Link_Configure_Thresholds.request */
                    eRAL_configure_thresholds_request(instanceP, &message_wrapper_pP->_union_message.link_configure_thresholds_request);
                } else {
                }
                break;

            case MIH_C_MESSAGE_LINK_ACTION_REQUEST_ID:
                /*
                 * This primitive is used by the MIHF to request an action on a
                 * link-layer connection to enable optimal handling of link-
                 * layer resources for the purpose of handovers.
                 * The MIHF generates this primitive upon request from the MIH
                 * user to perform an action on a pre-defined link-layer
                 * connection.
                 */
                LOG_D(RAL_ENB, " %s Received MIH_C_MESSAGE_LINK_ACTION_REQUEST\n", __FUNCTION__);
                memcpy(&message_wrapper_pP->_union_message.link_action_request.header, (const void *)&header, sizeof(MIH_C_HEADER_T));

                /* Decode Link_Action.request */
                status = MIH_C_Link_Message_Decode_Link_Action_request(bb_pP, &message_wrapper_pP->_union_message.link_action_request);
                if (status == MIH_MESSAGE_DECODE_OK) {
                    LOG_D(RAL_ENB, "**\n");
                    /* Process Link_Action.request */
                    eRAL_action_request(instanceP, &message_wrapper_pP->_union_message.link_action_request);
                } else {
                }
                break;
            default:
                LOG_W(RAL_ENB, " UNKNOWN MESSAGE ID SID %d, OP_CODE %d, AID %d\n", header.service_identifier, header.operation_code, header.action_identifier);
                status = MIH_MESSAGE_DECODE_FAILURE;
                
                return status;
        }
    }
    else {
        status = MIH_MESSAGE_DECODE_BAD_PARAMETER;
    }
    return status;
}
