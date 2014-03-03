/***************************************************************************
                         lteRALue_mih_msg.c  -  description
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
/*! \file lteRALue_mih_msg.c
 * \brief Interface for MIH primitives in LTE-RAL-UE
 * \author WETTERWALD Michelle, GAUTHIER Lionel, MAUREL Frederic
 * \date 2013
 * \company EURECOM
 * \email: michelle.wetterwald@eurecom.fr, lionel.gauthier@eurecom.fr, frederic.maurel@eurecom.fr
 */
/*******************************************************************************/
#define LTE_RAL_UE
#define LTERALUE_MIH_MSG_C
//-----------------------------------------------------------------------------
#include "lteRALue.h"
#include "LAYER2/MAC/extern.h"
//-----------------------------------------------------------------------------
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
int mRAL_send_to_mih(ral_ue_instance_t instanceP, u_int8_t  *buffer_pP, size_t lenP) {
//-----------------------------------------------------------------------------
    int          result;
    module_id_t  mod_id = instanceP - NB_eNB_INST;
    result = send(g_ue_ral_obj[mod_id].mih_sock_desc, (const void *)buffer_pP, lenP, 0);
    if (result != lenP) {
        LOG_E(RAL_UE, "send_to_mih %d bytes failed, returned %d: %s\n", lenP, result, strerror(errno));
    }
    return result;
}

//---------------------------------------------------------------------------
int mRAL_mihf_connect(ral_ue_instance_t instanceP){
//---------------------------------------------------------------------------
    struct addrinfo  info;  /* endpoint information  */
    struct addrinfo *addr, *rp; /* endpoint address  */
    int              rc;  /* returned error code  */
    int              optval;  /* socket option value  */
    module_id_t      mod_id = instanceP - NB_eNB_INST;

    unsigned char buf[sizeof(struct sockaddr_in6)];

    /*
     * Initialize the remote MIH-F endpoint address information
     */
    memset(&info, 0, sizeof(struct addrinfo));
    info.ai_family   = AF_UNSPEC; /* Allow IPv4 or IPv6 */
    info.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    info.ai_flags    = 0;
    info.ai_protocol = 0;  /* Any protocol  */

    rc = getaddrinfo(g_ue_ral_obj[mod_id].mihf_ip_address, g_ue_ral_obj[mod_id].mihf_remote_port, &info, &addr);
    if (rc != 0) {
        LOG_E(RAL_UE, " getaddrinfo: %s\n", gai_strerror(rc));
        return -1;
    }

    /*
     * getaddrinfo() returns a linked list of address structures.
     * Try each address until we successfully connect(2). If socket(2)
     * (or connect(2)) fails, we (close the socket and) try the next address.
     */
    for (rp = addr; rp != NULL; rp = rp->ai_next) {

        g_ue_ral_obj[mod_id].mih_sock_desc = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (g_ue_ral_obj[mod_id].mih_sock_desc < 0) {
            continue;
        }

        optval = 1;
        setsockopt(g_ue_ral_obj[mod_id].mih_sock_desc, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

        /*
         * Convert the RAL local network address
         */
        if (rp->ai_family == AF_INET) {
            /* IPv4 network address family */
            struct sockaddr_in  *addr4 = NULL;

            LOG_D(RAL_UE, " %s is an ipv4 address\n", g_ue_ral_obj[mod_id].mihf_ip_address);
            addr4             = (struct sockaddr_in *)(&buf[0]);
            addr4->sin_port   = htons(atoi(g_ue_ral_obj[mod_id].ral_listening_port));
            addr4->sin_family = AF_INET;
            rc = inet_pton(AF_INET, g_ue_ral_obj[mod_id].ral_ip_address, &addr4->sin_addr);
        }
        else if (rp->ai_family == AF_INET6) {
            /* IPv6 network address family */
            struct sockaddr_in6 *addr6 = NULL;

            LOG_D(RAL_UE, " %s is an ipv6 address\n", g_ue_ral_obj[mod_id].mihf_ip_address);
            addr6              = (struct sockaddr_in6 *)(&buf[0]);
            addr6->sin6_port   = htons(atoi(g_ue_ral_obj[mod_id].ral_listening_port));
            addr6->sin6_family = AF_INET6;
            rc = inet_pton(AF_INET, g_ue_ral_obj[mod_id].ral_ip_address, &addr6->sin6_addr);
        }
        else {
            LOG_E(RAL_UE, " %s is an unknown address format %d\n",
                    g_ue_ral_obj[mod_id].mihf_ip_address, rp->ai_family);
            return -1;
        }

        if (rc < 0) {
            /* The network address convertion failed */
            LOG_E(RAL_UE, " inet_pton(RAL IP address %s): %s\n",
                    g_ue_ral_obj[mod_id].ral_ip_address, strerror(rc));
            return -1;
        }
        else if (rc == 0) {
            /* The network address is not valid */
            LOG_E(RAL_UE, " RAL IP address %s is not valid\n", g_ue_ral_obj[mod_id].ral_ip_address);
            return -1;
        }

        /* Bind the socket to the local RAL network address */
        rc = bind(g_ue_ral_obj[mod_id].mih_sock_desc, (const struct sockaddr *)buf,
                sizeof(struct sockaddr_in));

        if (rc < 0) {
            LOG_E(RAL_UE, " bind(RAL IP address %s): %s\n",
                    g_ue_ral_obj[mod_id].ral_ip_address, strerror(errno));
            return -1;
        }

        /* Connect the socket to the remote MIH-F network address */
        if (connect(g_ue_ral_obj[mod_id].mih_sock_desc, rp->ai_addr, rp->ai_addrlen) == 0) {
            LOG_N(RAL_UE, " RAL [%s:%s] is now UDP-CONNECTED to MIH-F [%s:%s]\n",
                    g_ue_ral_obj[mod_id].ral_ip_address, g_ue_ral_obj[mod_id].ral_listening_port,
                    g_ue_ral_obj[mod_id].mihf_ip_address, g_ue_ral_obj[mod_id].mihf_remote_port);
            break;
        }
        /*
         * We failed to connect:
         * Close the socket file descriptor and try to connect to an other
         * address.
         */
        close(g_ue_ral_obj[mod_id].mih_sock_desc);
    }

    /*
     * Unable to connect to a network address
     */
    if (rp == NULL) {
        LOG_E(RAL_UE, " Could not connect to MIH-F\n");
        return -1;
    }

    freeaddrinfo(addr);

    return 0;
}


//-----------------------------------------------------------------------------
void MIH_C_3GPP_ADDR_load_3gpp_str_address(ral_ue_instance_t instanceP, MIH_C_3GPP_ADDR_T* _3gpp_addr_pP, u_int8_t* str_pP)
//-----------------------------------------------------------------------------
{
    int           i, l;
    u_int8_t      val_temp;
    unsigned char address_3gpp[32];
    unsigned char buf[3];
    u_int8_t      _3gpp_byte_address[8];
    module_id_t   mod_id = instanceP - NB_eNB_INST;

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


/***************************************************************************
     Transmission side
 ***************************************************************************/

//-----------------------------------------------------------------------------
void mRAL_send_link_register_indication(ral_ue_instance_t        instanceP,
                                        MIH_C_TRANSACTION_ID_T  *transaction_id_pP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Register_indication_t  message;
    Bit_Buffer_t                             *bb_p;
    int                                       message_total_length;
    module_id_t                               mod_id = instanceP - NB_eNB_INST;

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Register_indication_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    message.header.service_identifier   = (MIH_C_SID_T)1;
    message.header.operation_code       = (MIH_C_OPCODE_T)3;
    message.header.action_identifier    = (MIH_C_AID_T)6;
    message.header.transaction_id       = *transaction_id_pP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_ue_ral_obj[mod_id].link_id, strlen(g_ue_ral_obj[mod_id].link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_ue_ral_obj[mod_id].mihf_id, strlen(g_ue_ral_obj[mod_id].mihf_id));


    message.primitive.Link_Id.link_type        = MIH_C_WIRELESS_LTE; //MIH_C_WIRELESS_UMTS;
    #ifdef USE_3GPP_ADDR_AS_LINK_ADDR
    message.primitive.Link_Id.link_addr.choice = (MIH_C_CHOICE_T)MIH_C_CHOICE_3GPP_ADDR;
    MIH_C_3GPP_ADDR_load_3gpp_str_address(mod_id, &message.primitive.Link_Id.link_addr._union._3gpp_addr, (u_int8_t*)UE_DEFAULT_3GPP_ADDRESS);
    #else
    message.primitive.Link_Id.link_addr.choice = (MIH_C_CHOICE_T)MIH_C_CHOICE_3GPP_3G_CELL_ID;
    memcpy(message.primitive.Link_Id.link_addr._union._3gpp_3g_cell_id.plmn_id.val, &g_ue_ral_obj[mod_id].plmn_id, 3);
    message.primitive.Link_Id.link_addr._union._3gpp_3g_cell_id.cell_id = g_ue_ral_obj[mod_id].cell_id;
    #endif
    //MIH_C_3GPP_ADDR_set(&message.primitive.Link_Id.link_addr._union._3gpp_addr, (u_int8_t*)&(g_ue_ral_obj[instanceP].ipv6_l2id[0]), strlen(UE_DEFAULT_3GPP_ADDRESS));
    ////MIH_C_3GPP_ADDR_set(&message.primitive.Link_Id.link_addr._union._3gpp_addr, (u_int8_t*)UE_DEFAULT_3GPP_ADDRESS, strlen(UE_DEFAULT_3GPP_ADDRESS));
    ////MIH_C_3GPP_ADDR_load_3gpp_str_address(&message.primitive.Link_Id.link_addr._union._3gpp_addr, (u_int8_t*)UE_DEFAULT_3GPP_ADDRESS);


    message_total_length = MIH_C_Link_Message_Encode_Link_Register_indication(bb_p, &message);

    #ifdef MSCGEN_PYTOOL
    memset(g_msc_gen_buf, 0, MSC_GEN_BUF_SIZE);
    g_msc_gen_buffer_index = 0;
    MIH_C_LINK_ID2String(&message.primitive.Link_Id, g_msc_gen_buf);
    #endif

    if (mRAL_send_to_mih(instanceP, bb_p->m_buffer,message_total_length)<0){
        printf("ERROR RAL_UE, : Send Link_Register.indication\n");
        LOG_E(RAL_UE, ": Send Link_Register.indication\n");
    } else {
        printf("OK RAL_UE, : Send Link_Register.indication\n");
        LOG_D(RAL_UE, ": Sent Link_Register.indication\n");
    }
    free_BitBuffer(bb_p);
}
//-----------------------------------------------------------------------------
void mRAL_send_link_detected_indication(ral_ue_instance_t        instanceP,
                                        MIH_C_TRANSACTION_ID_T  *transaction_id_pP,
                                        MIH_C_LINK_DET_INFO_T   *link_detected_info_pP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Detected_indication_t  message;
    Bit_Buffer_t                             *bb_p;
    int                                       message_total_length;
    module_id_t                               mod_id = instanceP - NB_eNB_INST;

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Detected_indication_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    message.header.service_identifier   = (MIH_C_SID_T)2;
    message.header.operation_code       = (MIH_C_OPCODE_T)3;
    message.header.action_identifier    = (MIH_C_AID_T)1;
    message.header.transaction_id       = *transaction_id_pP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_ue_ral_obj[mod_id].link_id, strlen(g_ue_ral_obj[mod_id].link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_ue_ral_obj[mod_id].mihf_id, strlen(g_ue_ral_obj[mod_id].mihf_id));


    memcpy(&message.primitive.LinkDetectedInfo, link_detected_info_pP, sizeof(MIH_C_LINK_DET_INFO_T));

    message_total_length = MIH_C_Link_Message_Encode_Link_Detected_indication(bb_p, &message);

    if (mRAL_send_to_mih(instanceP, bb_p->m_buffer,message_total_length)<0){
        LOG_E(RAL_UE, ": Send Link_Detected.indication\n");
    } else {
        LOG_D(RAL_UE, ": Sent Link_Detected.indication\n");
    }
    free_BitBuffer(bb_p);
}
//-----------------------------------------------------------------------------
void mRAL_send_link_up_indication(ral_ue_instance_t          instanceP,
                                  MIH_C_TRANSACTION_ID_T    *transaction_id_pP,
                                  MIH_C_LINK_TUPLE_ID_T     *link_identifier_pP,
                                  MIH_C_LINK_ADDR_T         *old_access_router_pP,
                                  MIH_C_LINK_ADDR_T         *new_access_router_pP,
                                  MIH_C_IP_RENEWAL_FLAG_T   *ip_renewal_flag_pP,
                                  MIH_C_IP_MOB_MGMT_T       *mobility_management_support_pP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Up_indication_t  message;
    Bit_Buffer_t                             *bb_p;
    int                                       message_total_length;
    module_id_t                               mod_id = instanceP - NB_eNB_INST;

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Up_indication_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    message.header.service_identifier   = (MIH_C_SID_T)2;
    message.header.operation_code       = (MIH_C_OPCODE_T)3;
    message.header.action_identifier    = (MIH_C_AID_T)2;
    message.header.transaction_id       = *transaction_id_pP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_ue_ral_obj[mod_id].link_id, strlen(g_ue_ral_obj[mod_id].link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_ue_ral_obj[mod_id].mihf_id, strlen(g_ue_ral_obj[mod_id].mihf_id));


    memcpy(&message.primitive.LinkIdentifier, link_identifier_pP, sizeof(MIH_C_LINK_TUPLE_ID_T));

    message.primitive.OldAccessRouter            = old_access_router_pP;
    message.primitive.NewAccessRouter            = new_access_router_pP;
    message.primitive.IPRenewalFlag              = ip_renewal_flag_pP;
    message.primitive.MobilityManagementSupport  = mobility_management_support_pP;

    message_total_length = MIH_C_Link_Message_Encode_Link_Up_indication(bb_p, &message);

    if (mRAL_send_to_mih(instanceP, bb_p->m_buffer,message_total_length)<0){
        LOG_E(RAL_UE, ": Send Link_Up.indication\n");
    } else {
        LOG_D(RAL_UE, ": Sent Link_Up.indication\n");
    }
    free_BitBuffer(bb_p);
}
//-----------------------------------------------------------------------------
void mRAL_send_link_parameters_report_indication(ral_ue_instance_t            instanceP,
                                                 MIH_C_TRANSACTION_ID_T      *transaction_id_pP,
                                                 MIH_C_LINK_TUPLE_ID_T       *link_identifier_pP,
                                                 MIH_C_LINK_PARAM_RPT_LIST_T *link_parameters_report_list_pP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Parameters_Report_indication_t  message;
    Bit_Buffer_t                             *bb_p;
    int                                       message_total_length;
    module_id_t                               mod_id = instanceP - NB_eNB_INST;

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Parameters_Report_indication_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    message.header.service_identifier   = (MIH_C_SID_T)2;
    message.header.operation_code       = (MIH_C_OPCODE_T)3;
    message.header.action_identifier    = (MIH_C_AID_T)5;
    message.header.transaction_id       = *transaction_id_pP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_ue_ral_obj[mod_id].link_id, strlen(g_ue_ral_obj[mod_id].link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_ue_ral_obj[mod_id].mihf_id, strlen(g_ue_ral_obj[mod_id].mihf_id));


    memcpy(&message.primitive.LinkIdentifier, link_identifier_pP, sizeof(MIH_C_LINK_TUPLE_ID_T));
    memcpy(&message.primitive.LinkParametersReportList_list, link_parameters_report_list_pP, sizeof(MIH_C_LINK_PARAM_RPT_LIST_T));

    message_total_length = MIH_C_Link_Message_Encode_Link_Parameters_Report_indication(bb_p, &message);

    if (mRAL_send_to_mih(instanceP, bb_p->m_buffer,message_total_length)<0){
        LOG_E(RAL_UE, ": Send Link_Parameters_Report.indication\n");
    } else {
        LOG_D(RAL_UE, ": Sent Link_Parameters_Report.indication\n");
    }
    free_BitBuffer(bb_p);
}

//-----------------------------------------------------------------------------
void mRAL_send_link_going_down_indication(ral_ue_instance_t            instanceP,
                                          MIH_C_TRANSACTION_ID_T      *transaction_id_pP,
                                          MIH_C_LINK_TUPLE_ID_T       *link_identifier_pP,
                                          MIH_C_UNSIGNED_INT2_T       *time_interval_pP,
                                          MIH_C_LINK_GD_REASON_T      *link_going_down_reason_pP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Going_Down_indication_t  message;
    Bit_Buffer_t                             *bb_p;
    int                                       message_total_length;
    module_id_t                               mod_id = instanceP - NB_eNB_INST;

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Going_Down_indication_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    message.header.service_identifier   = (MIH_C_SID_T)2;
    message.header.operation_code       = (MIH_C_OPCODE_T)3;
    message.header.action_identifier    = (MIH_C_AID_T)6;
    message.header.transaction_id       = *transaction_id_pP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_ue_ral_obj[mod_id].link_id, strlen(g_ue_ral_obj[mod_id].link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_ue_ral_obj[mod_id].mihf_id, strlen(g_ue_ral_obj[mod_id].mihf_id));


    memcpy(&message.primitive.LinkIdentifier, link_identifier_pP, sizeof(MIH_C_LINK_TUPLE_ID_T));
    message.primitive.TimeInterval = *time_interval_pP;
    memcpy(&message.primitive.LinkGoingDownReason, link_going_down_reason_pP, sizeof(MIH_C_LINK_GD_REASON_T));


    message_total_length = MIH_C_Link_Message_Encode_Link_Going_Down_indication(bb_p, &message);

    if (mRAL_send_to_mih(instanceP, bb_p->m_buffer,message_total_length)<0){
        LOG_E(RAL_UE, ": Send Link_Going_Down.indication\n");
    } else {
        LOG_D(RAL_UE, ": Sent Link_Going_Down.indication\n");
    }
    free_BitBuffer(bb_p);
}

//-----------------------------------------------------------------------------
void mRAL_send_link_down_indication(ral_ue_instance_t            instanceP,
                                    MIH_C_TRANSACTION_ID_T      *transaction_id_pP,
                                    MIH_C_LINK_TUPLE_ID_T       *link_identifier_pP,
                                    MIH_C_LINK_ADDR_T           *old_access_router_pP,
                                    MIH_C_LINK_DN_REASON_T      *reason_code_pP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Down_indication_t      message;
    Bit_Buffer_t                             *bb_p;
    int                                       message_total_length;
    module_id_t                               mod_id = instanceP - NB_eNB_INST;

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Going_Down_indication_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    message.header.service_identifier   = (MIH_C_SID_T)2;
    message.header.operation_code       = (MIH_C_OPCODE_T)3;
    message.header.action_identifier    = (MIH_C_AID_T)3;
    message.header.transaction_id       = *transaction_id_pP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_ue_ral_obj[mod_id].link_id, strlen(g_ue_ral_obj[mod_id].link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_ue_ral_obj[mod_id].mihf_id, strlen(g_ue_ral_obj[mod_id].mihf_id));


    memcpy(&message.primitive.LinkIdentifier, link_identifier_pP, sizeof(MIH_C_LINK_TUPLE_ID_T));
    message.primitive.OldAccessRouter = old_access_router_pP;
    memcpy(&message.primitive.ReasonCode, reason_code_pP, sizeof(MIH_C_LINK_DN_REASON_T));


    message_total_length = MIH_C_Link_Message_Encode_Link_Down_indication(bb_p, &message);

    if (mRAL_send_to_mih(instanceP, bb_p->m_buffer,message_total_length)<0){
        LOG_E(RAL_UE, ": Send Link_Down.indication\n");
    } else {
        LOG_D(RAL_UE, ": Sent Link_Down.indication\n");
    }
    free_BitBuffer(bb_p);
}

//-----------------------------------------------------------------------------
void mRAL_send_link_action_confirm(ral_ue_instance_t           instanceP,
                                   MIH_C_TRANSACTION_ID_T     *transaction_id_pP,
                                   MIH_C_STATUS_T             *status_pP,
                                   MIH_C_LINK_SCAN_RSP_LIST_T *scan_response_set_pP,
                                   MIH_C_LINK_AC_RESULT_T     *link_action_result_pP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Action_confirm_t       message;
    Bit_Buffer_t                             *bb_p;
    int                                       message_total_length;
    module_id_t                               mod_id = instanceP - NB_eNB_INST;


    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Action_confirm_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    message.header.service_identifier   = (MIH_C_SID_T)3;
    message.header.operation_code       = (MIH_C_OPCODE_T)0;
    message.header.action_identifier    = (MIH_C_AID_T)3;
    message.header.transaction_id       = *transaction_id_pP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_ue_ral_obj[mod_id].link_id, strlen(g_ue_ral_obj[mod_id].link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_ue_ral_obj[mod_id].mihf_id, strlen(g_ue_ral_obj[mod_id].mihf_id));


    message.primitive.Status                       = *status_pP;
    message.primitive.ScanResponseSet_list         = scan_response_set_pP;
    message.primitive.LinkActionResult             = link_action_result_pP;


    message_total_length = MIH_C_Link_Message_Encode_Link_Action_confirm(bb_p, &message);

    if (mRAL_send_to_mih(instanceP, bb_p->m_buffer,message_total_length)<0){
        LOG_E(RAL_UE, ": Send Link_Action.confirm\n");
    } else {
        LOG_D(RAL_UE, ": Sent Link_Action.confirm\n");
    }
    free_BitBuffer(bb_p);
}

//-----------------------------------------------------------------------------
void mRAL_send_capability_discover_confirm(ral_ue_instance_t          instanceP,
                                             MIH_C_TRANSACTION_ID_T  *transaction_id_pP,
                                             MIH_C_STATUS_T          *status_pP,
                                             MIH_C_LINK_EVENT_LIST_T *supported_link_event_list_pP,
                                             MIH_C_LINK_CMD_LIST_T   *supported_link_command_list_pP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Capability_Discover_confirm_t  message;
    Bit_Buffer_t                             *bb_p;
    int                                       message_total_length;
    module_id_t                               mod_id = instanceP - NB_eNB_INST;

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Capability_Discover_confirm_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    message.header.service_identifier   = (MIH_C_SID_T)1;
    message.header.operation_code       = (MIH_C_OPCODE_T)0;
    message.header.action_identifier    = (MIH_C_AID_T)1;
    message.header.transaction_id       = *transaction_id_pP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_ue_ral_obj[mod_id].link_id, strlen(g_ue_ral_obj[mod_id].link_id));
    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_ue_ral_obj[mod_id].mihf_id, strlen(g_ue_ral_obj[mod_id].mihf_id));

    message.primitive.Status                   = *status_pP;
    message.primitive.SupportedLinkEventList   = supported_link_event_list_pP;
    message.primitive.SupportedLinkCommandList = supported_link_command_list_pP;

    message_total_length = MIH_C_Link_Message_Encode_Capability_Discover_confirm(bb_p, &message);

    if (mRAL_send_to_mih(instanceP, bb_p->m_buffer,message_total_length)<0){
        LOG_E(RAL_UE, ": Send Link_Capability_Discover.confirm\n");
    } else {
        LOG_D(RAL_UE, ": Sent Link_Capability_Discover.confirm\n");
    }
    free_BitBuffer(bb_p);
}

//-----------------------------------------------------------------------------
void mRAL_send_event_subscribe_confirm(ral_ue_instance_t          instanceP,
                                         MIH_C_TRANSACTION_ID_T  *transaction_id_pP,
                                         MIH_C_STATUS_T          *status_pP,
                                         MIH_C_LINK_EVENT_LIST_T *response_link_event_list_pP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Event_Subscribe_confirm_t  message;
    Bit_Buffer_t                                 *bb_p;
    int                                           message_total_length;
    module_id_t                                   mod_id = instanceP - NB_eNB_INST;

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Event_Subscribe_confirm_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    message.header.service_identifier   = (MIH_C_SID_T)1;
    message.header.operation_code       = (MIH_C_OPCODE_T)0;
    message.header.action_identifier    = (MIH_C_AID_T)4;
    message.header.transaction_id       = *transaction_id_pP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_ue_ral_obj[mod_id].link_id, strlen(g_ue_ral_obj[mod_id].link_id));
    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_ue_ral_obj[mod_id].mihf_id, strlen(g_ue_ral_obj[mod_id].mihf_id));

    message.primitive.Status                   = *status_pP;
    message.primitive.ResponseLinkEventList    =  response_link_event_list_pP;

    message_total_length = MIH_C_Link_Message_Encode_Event_Subscribe_confirm(bb_p, &message);

    if (mRAL_send_to_mih(instanceP, bb_p->m_buffer,message_total_length)<0){
        LOG_E(RAL_UE, ": Send Link_Event_Subscribe.confirm\n");
    } else {
        LOG_D(RAL_UE, ": Sent Link_Event_Subscribe.confirm\n");
    }
    free_BitBuffer(bb_p);
}

//-----------------------------------------------------------------------------
void mRAL_send_event_unsubscribe_confirm(ral_ue_instance_t          instanceP,
                                           MIH_C_TRANSACTION_ID_T  *transaction_id_pP,
                                           MIH_C_STATUS_T          *status_pP,
                                           MIH_C_LINK_EVENT_LIST_T *response_link_event_list_pP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Event_Unsubscribe_confirm_t  message;
    Bit_Buffer_t                                   *bb_p;
    int                                             message_total_length;
    module_id_t                                     mod_id = instanceP - NB_eNB_INST;

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Event_Unsubscribe_confirm_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    message.header.service_identifier   = (MIH_C_SID_T)1;
    message.header.operation_code       = (MIH_C_OPCODE_T)0;
    message.header.action_identifier    = (MIH_C_AID_T)5;
    message.header.transaction_id       = *transaction_id_pP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_ue_ral_obj[mod_id].link_id, strlen(g_ue_ral_obj[mod_id].link_id));
    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_ue_ral_obj[mod_id].mihf_id, strlen(g_ue_ral_obj[mod_id].mihf_id));

    message.primitive.Status                   = *status_pP;
    message.primitive.ResponseLinkEventList    =  response_link_event_list_pP;

    message_total_length = MIH_C_Link_Message_Encode_Event_Unsubscribe_confirm(bb_p, &message);

    if (mRAL_send_to_mih(instanceP, bb_p->m_buffer,message_total_length)<0){
        LOG_E(RAL_UE, ": Send Link_Event_Unsubscribe.confirm\n");
    } else {
        LOG_D(RAL_UE, ": Sent Link_Event_Unsubscribe.confirm\n");
    }
    free_BitBuffer(bb_p);
}

//-----------------------------------------------------------------------------
void mRAL_send_configure_thresholds_confirm(ral_ue_instance_t               instanceP,
                                              MIH_C_TRANSACTION_ID_T       *transaction_id_pP,
                                              MIH_C_STATUS_T               *status_pP,
                                              MIH_C_LINK_CFG_STATUS_LIST_T *link_configure_status_list_pP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Configure_Thresholds_confirm_t  message;
    Bit_Buffer_t                                      *bb_p;
    int                                                message_total_length;
    module_id_t                                        mod_id = instanceP - NB_eNB_INST;

    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Configure_Thresholds_confirm_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    message.header.service_identifier   = (MIH_C_SID_T)3;
    message.header.operation_code       = (MIH_C_OPCODE_T)0;
    message.header.action_identifier    = (MIH_C_AID_T)2;
    message.header.transaction_id       = *transaction_id_pP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_ue_ral_obj[mod_id].link_id, strlen(g_ue_ral_obj[mod_id].link_id));
    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_ue_ral_obj[mod_id].mihf_id, strlen(g_ue_ral_obj[mod_id].mihf_id));

    message.primitive.Status                   = *status_pP;
    message.primitive.LinkConfigureStatusList_list  =  link_configure_status_list_pP;

    message_total_length = MIH_C_Link_Message_Encode_Configure_Thresholds_confirm(bb_p, &message);

    if (mRAL_send_to_mih(instanceP, bb_p->m_buffer,message_total_length)<0){
        LOG_E(RAL_UE, ": Send Link_Configure_Threshold.confirm\n");
    } else {
        LOG_D(RAL_UE, ": Sent Link_Configure_Threshold.confirm\n");
    }
    free_BitBuffer(bb_p);
}

//-----------------------------------------------------------------------------
void mRAL_send_get_parameters_confirm     (ral_ue_instance_t               instanceP,
                                             MIH_C_TRANSACTION_ID_T       *transaction_id_pP,
                                             MIH_C_STATUS_T               *status_pP,
                                             MIH_C_LINK_PARAM_LIST_T      *link_parameters_status_list_pP,
                                             MIH_C_LINK_STATES_RSP_LIST_T *link_states_response_list_pP,
                                             MIH_C_LINK_DESC_RSP_LIST_T   *link_descriptors_response_list_pP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Get_Parameters_confirm_t  message;
    Bit_Buffer_t                                *bb_p;
    int                                          message_total_length;
    module_id_t                                  mod_id = instanceP - NB_eNB_INST;


    bb_p = new_BitBuffer_0();
    BitBuffer_wrap(bb_p, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Get_Parameters_confirm_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    message.header.service_identifier   = (MIH_C_SID_T)3;
    message.header.operation_code       = (MIH_C_OPCODE_T)0;
    message.header.action_identifier    = (MIH_C_AID_T)1;
    message.header.transaction_id       = *transaction_id_pP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_ue_ral_obj[mod_id].link_id, strlen(g_ue_ral_obj[mod_id].link_id));
    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_ue_ral_obj[mod_id].mihf_id, strlen(g_ue_ral_obj[mod_id].mihf_id));

    message.primitive.Status                        = *status_pP;
    message.primitive.LinkParametersStatusList_list = link_parameters_status_list_pP;
    message.primitive.LinkStatesResponse_list       = link_states_response_list_pP;
    message.primitive.LinkDescriptorsResponse_list  = link_descriptors_response_list_pP;

    message_total_length = MIH_C_Link_Message_Encode_Get_Parameters_confirm(bb_p, &message);

    if (mRAL_send_to_mih(instanceP, bb_p->m_buffer,message_total_length)<0){
        LOG_E(RAL_UE, ": Send Link_Get_Parameters.confirm\n");
    } else {
        LOG_D(RAL_UE, ": Sent Link_Get_Parameters.confirm\n");
    }
    free_BitBuffer(bb_p);
}

/***************************************************************************
     Reception side
 ***************************************************************************/

//-----------------------------------------------------------------------------
int mRAL_mih_link_msg_decode(ral_ue_instance_t        instanceP,
                             Bit_Buffer_t            *bbP,
                             MIH_C_Message_Wrapper_t *message_wrapperP) {
//-----------------------------------------------------------------------------
    int                                      status = MIH_MESSAGE_DECODE_FAILURE;
    MIH_C_HEADER_T                           header;
    MIH_C_STATUS_T                           mih_status;
    module_id_t                              mod_id = instanceP - NB_eNB_INST;


    if ((bbP != NULL) && (message_wrapperP != NULL)) {
        status = MIH_C_Link_Header_Decode(bbP, &header);
        if (status == MIH_HEADER_DECODE_TOO_SHORT) {
            return MIH_MESSAGE_DECODE_TOO_SHORT;
        } else if (status == MIH_HEADER_DECODE_FAILURE) {
            return MIH_MESSAGE_DECODE_FAILURE;
        } else if (status == MIH_HEADER_DECODE_BAD_PARAMETER) {
            return MIH_MESSAGE_DECODE_BAD_PARAMETER;
        }
        message_wrapperP->message_id     = MIH_C_MESSAGE_ID(header.service_identifier, header.operation_code, header.action_identifier);

        switch (message_wrapperP->message_id) {
            case MIH_C_MESSAGE_LINK_CAPABILITY_DISCOVER_REQUEST_ID:
                LOG_D(RAL_UE, " %s Received MIH_C_MESSAGE_LINK_CAPABILITY_DISCOVER_REQUEST TID %u\n", __FUNCTION__, header.transaction_id);
                memcpy(&message_wrapperP->_union_message.link_capability_discover_request.header, (const void *)&header, sizeof(MIH_C_HEADER_T));
                status = MIH_C_Link_Message_Decode_Link_Capability_Discover_request(bbP, &message_wrapperP->_union_message.link_capability_discover_request);
                if (status == MIH_MESSAGE_DECODE_OK) {
                    MIH_C_Link_Message_Link_Capability_Discover_request2String(&message_wrapperP->_union_message.link_capability_discover_request, g_msg_print_buffer);
                    LOG_D(RAL_UE, "%s", g_msg_print_buffer);

                    mih_status = MIH_C_STATUS_SUCCESS;
                    LOG_D(RAL_UE, "**\n");
                    LOG_D(RAL_UE, " %s Sending MIH_C_MESSAGE_LINK_CAPABILITY_DISCOVER_CONFIRM\n\n", __FUNCTION__);

                    mRAL_send_capability_discover_confirm(instanceP,
                                                            &message_wrapperP->_union_message.link_capability_discover_request.header.transaction_id,
                                                            &mih_status,
                                                            &g_ue_ral_obj[mod_id].mih_supported_link_event_list,
                                                            &g_ue_ral_obj[mod_id].mih_supported_link_command_list);
                } else {
                }
                break;

            case MIH_C_MESSAGE_LINK_EVENT_SUBSCRIBE_REQUEST_ID:
                LOG_D(RAL_UE, " %s Received MIH_C_MESSAGE_LINK_EVENT_SUBSCRIBE_REQUEST TID %u\n", __FUNCTION__, header.transaction_id);
                memcpy(&message_wrapperP->_union_message.link_event_subscribe_request.header, (const void *)&header, sizeof(MIH_C_HEADER_T));
                status = MIH_C_Link_Message_Decode_Link_Event_Subscribe_request(bbP, &message_wrapperP->_union_message.link_event_subscribe_request);
                if (status == MIH_MESSAGE_DECODE_OK) {
                    LOG_D(RAL_UE, "**\n");

                    mRAL_subscribe_request(instanceP, &message_wrapperP->_union_message.link_event_subscribe_request);
                } else {
                }
                break;

            case MIH_C_MESSAGE_LINK_EVENT_UNSUBSCRIBE_REQUEST_ID:
                LOG_D(RAL_UE, " %s Received MIH_C_MESSAGE_LINK_EVENT_UNSUBSCRIBE_REQUEST TID %u\n", __FUNCTION__, header.transaction_id);
                memcpy(&message_wrapperP->_union_message.link_event_unsubscribe_request.header, (const void *)&header, sizeof(MIH_C_HEADER_T));
                status = MIH_C_Link_Message_Decode_Link_Event_Unsubscribe_request(bbP, &message_wrapperP->_union_message.link_event_unsubscribe_request);
                if (status == MIH_MESSAGE_DECODE_OK) {
                    LOG_D(RAL_UE, "**\n");
                    mRAL_unsubscribe_request(instanceP, &message_wrapperP->_union_message.link_event_unsubscribe_request);
                } else {
                }
                break;

            case MIH_C_MESSAGE_LINK_GET_PARAMETERS_REQUEST_ID:
                LOG_D(RAL_UE, " %s Received MIH_C_MESSAGE_LINK_GET_PARAMETERS_REQUEST TID %u\n", __FUNCTION__, header.transaction_id);
                memcpy(&message_wrapperP->_union_message.link_get_parameters_request.header, (const void *)&header, sizeof(MIH_C_HEADER_T));
                status = MIH_C_Link_Message_Decode_Link_Get_Parameters_request(bbP, &message_wrapperP->_union_message.link_get_parameters_request);
                if (status == MIH_MESSAGE_DECODE_OK) {
                    LOG_D(RAL_UE, "**\n");
                     mRAL_get_parameters_request(instanceP, &message_wrapperP->_union_message.link_get_parameters_request);
                } else {
                }
                break;

            case MIH_C_MESSAGE_LINK_CONFIGURE_THRESHOLDS_REQUEST_ID:
                LOG_D(RAL_UE, " %s Received MIH_C_MESSAGE_LINK_CONFIGURE_THRESHOLDS_REQUEST TID %u\n", __FUNCTION__, header.transaction_id);
                memcpy(&message_wrapperP->_union_message.link_configure_thresholds_request.header, (const void *)&header, sizeof(MIH_C_HEADER_T));
                status = MIH_C_Link_Message_Decode_Link_Configure_Thresholds_request(bbP, &message_wrapperP->_union_message.link_configure_thresholds_request);
                if (status == MIH_MESSAGE_DECODE_OK) {
                    LOG_D(RAL_UE, "**\n");
                    mRAL_configure_thresholds_request(instanceP, &message_wrapperP->_union_message.link_configure_thresholds_request);
                } else {
                }
                break;

            case MIH_C_MESSAGE_LINK_ACTION_REQUEST_ID:
                LOG_D(RAL_UE, " %s Received MIH_C_MESSAGE_LINK_ACTION_REQUEST TID %u\n", __FUNCTION__, header.transaction_id);
                memcpy(&message_wrapperP->_union_message.link_action_request.header, (const void *)&header, sizeof(MIH_C_HEADER_T));
                status = MIH_C_Link_Message_Decode_Link_Action_request(bbP, &message_wrapperP->_union_message.link_action_request);
                if (status == MIH_MESSAGE_DECODE_OK) {
                    LOG_D(RAL_UE, "**\n");
                    mRAL_action_request(instanceP, &message_wrapperP->_union_message.link_action_request);
                } else {
                }
                break;

            default:
                LOG_W(RAL_UE, "UNKNOWN MESSAGE ID SID %d, OP_CODE %d, AID %d\n", header.service_identifier, header.operation_code, header.action_identifier);
                status = MIH_MESSAGE_DECODE_FAILURE;

            return status;
        }
    } else {
        return MIH_MESSAGE_DECODE_BAD_PARAMETER;
    }
    return status;
}

//-----------------------------------------------------------------------------
int mRAL_mih_link_process_message(ral_ue_instance_t instanceP){
//-----------------------------------------------------------------------------
    MIH_C_Message_Wrapper_t  message_wrapper;
    int                      nb_bytes_received ;
    int                      nb_bytes_decoded  ;
    int                      total_bytes_to_decode ;
    int                      status ;
    Bit_Buffer_t            *bb_p;
    struct sockaddr_in       udp_socket;
    socklen_t                sockaddr_len;
    module_id_t              mod_id = instanceP - NB_eNB_INST;

    total_bytes_to_decode = 0;
    nb_bytes_received     = 0;

    bb_p = new_BitBuffer_0();

    nb_bytes_received = recvfrom(g_ue_ral_obj[mod_id].mih_sock_desc,
                                 (void *)g_msg_codec_recv_buffer,
                                 MSG_CODEC_RECV_BUFFER_SIZE,
                                 0,
                                 (struct sockaddr *) &udp_socket,
                                 &sockaddr_len);

    if (nb_bytes_received > 0) {
        LOG_D(RAL_UE, " \n");
        LOG_D(RAL_UE, " %s: Received %d bytes from MIHF\n", __FUNCTION__, nb_bytes_received);
        total_bytes_to_decode += nb_bytes_received;
        BitBuffer_wrap(bb_p, g_msg_codec_recv_buffer, total_bytes_to_decode);
        status  = mRAL_mih_link_msg_decode(instanceP, bb_p, &message_wrapper);
        nb_bytes_decoded = BitBuffer_getPosition(bb_p);
        if (status == MIH_MESSAGE_DECODE_OK) {
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
        }
        LOG_D(RAL_UE, " \n");
    }
    free_BitBuffer(bb_p);
    return 0;
}
