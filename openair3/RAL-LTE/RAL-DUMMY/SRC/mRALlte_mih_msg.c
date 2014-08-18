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
#define MRAL_MODULE
#define MRALLTE_MIH_MSG_C
//-----------------------------------------------------------------------------
#include "mRALlte_mih_msg.h"
#include "mRALlte_subscribe.h"
#include "mRALlte_variables.h"
#include "mRALlte_parameters.h"
#include "mRALlte_thresholds.h"
#include "mRALlte_action.h"
//-----------------------------------------------------------------------------

static char  g_msg_codec_tmp_print_buffer[8192];
#ifdef MSCGEN_PYTOOL
#define MSC_GEN_BUF_SIZE                         1024
// global instead of poisonning the stack
static char                                      g_msc_gen_buf[MSC_GEN_BUF_SIZE];
static unsigned int                              g_msc_gen_buffer_index;
#endif


//-----------------------------------------------------------------------------
int mRALlte_send_to_mih(u_int8_t  *bufferP, size_t lenP) {
//-----------------------------------------------------------------------------
    int result;
    mRALlte_print_buffer((char*)bufferP, lenP);
    result = send(g_sockd_mihf, (const void *)bufferP, lenP, 0);
    if (result != lenP) {
        ERR("send_to_mih %d bytes failed, returned %d: %s\n", lenP, result, strerror(errno));
    }
    return result;
}
//-----------------------------------------------------------------------------
// Print the content of a buffer in hexadecimal
void mRALlte_print_buffer(char * bufferP, int lengthP) {
//-----------------------------------------------------------------------------
    char          c;
    unsigned int  buffer_index = 0;
    unsigned int  index;
    unsigned int  octet_index  = 0;
    unsigned long char_index   = 0;

    if (bufferP == NULL) {
        return;
    }


    buffer_index += sprintf(&g_msg_codec_tmp_print_buffer[buffer_index], "\n------+-------------------------------------------------+------------------+\n");
    buffer_index += sprintf(&g_msg_codec_tmp_print_buffer[buffer_index], "      |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f | 0123456789abcdef |\n");
    buffer_index += sprintf(&g_msg_codec_tmp_print_buffer[buffer_index], "------+-------------------------------------------------+------------------+\n");
    for (octet_index = 0; octet_index < lengthP; octet_index++) {
        if ((octet_index % 16) == 0){
            if (octet_index != 0) {
                buffer_index += sprintf(&g_msg_codec_tmp_print_buffer[buffer_index], " | ");
                for (char_index = octet_index - 16; char_index < octet_index; char_index++) {
                    c = (char) bufferP[char_index] & 0177;
                    if (iscntrl(c) || isspace(c)) {
                        buffer_index += sprintf(&g_msg_codec_tmp_print_buffer[buffer_index], " ");
                    } else {
                        buffer_index += sprintf(&g_msg_codec_tmp_print_buffer[buffer_index], "%c", c);
                    }
                }
                buffer_index += sprintf(&g_msg_codec_tmp_print_buffer[buffer_index], " |\n");
            }
            buffer_index += sprintf(&g_msg_codec_tmp_print_buffer[buffer_index], " %04d |", octet_index);
        }
        /*
        * Print every single octet in hexadecimal form
        */
        buffer_index += sprintf(&g_msg_codec_tmp_print_buffer[buffer_index], " %02x", (u_int8_t)(bufferP[octet_index] & 0x00FF));
        /*
        * Align newline and pipes according to the octets in groups of 2
        */
    }

    /*
    * Append enough spaces and put final pipe
    */
    if ((lengthP % 16) > 0) {
        for (index = (octet_index % 16); index < 16; ++index) {
            buffer_index += sprintf(&g_msg_codec_tmp_print_buffer[buffer_index], "   ");
        }
    }
    buffer_index += sprintf(&g_msg_codec_tmp_print_buffer[buffer_index], " | ");
    for (char_index = (octet_index / 16) * 16; char_index < octet_index; char_index++) {
        c = (char) bufferP[char_index] & 0177;
        if (iscntrl(c) || isspace(c)) {
            buffer_index += sprintf(&g_msg_codec_tmp_print_buffer[buffer_index], " ");
        } else {
            buffer_index += sprintf(&g_msg_codec_tmp_print_buffer[buffer_index], "%c", c);
        }
    }
    if ((lengthP % 16) > 0) {
        for (index = (octet_index % 16); index < 16; ++index) {
            buffer_index += sprintf(&g_msg_codec_tmp_print_buffer[buffer_index], " ");
        }
    }
    buffer_index += sprintf(&g_msg_codec_tmp_print_buffer[buffer_index], " |\n");
    buffer_index += sprintf(&g_msg_codec_tmp_print_buffer[buffer_index], "------+-------------------------------------------------+------------------+\n");
    DEBUG(g_msg_codec_tmp_print_buffer);
}
//---------------------------------------------------------------------------
int mRALlte_mihf_connect(void){
//---------------------------------------------------------------------------
    struct addrinfo      hints;
    struct addrinfo     *result, *rp;
    int                  s, on;
    struct sockaddr_in  *addr  = NULL;
    struct sockaddr_in6 *addr6 = NULL;
    unsigned char        buf[sizeof(struct sockaddr_in6)];


    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family   = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_DGRAM;   /* Datagram socket */
    hints.ai_flags    = 0;
    hints.ai_protocol = 0;            /* Any protocol */

    s = getaddrinfo(g_mihf_ip_address, g_mihf_remote_port, &hints, &result);
    if (s != 0) {
        ERR(" getaddrinfo: %s\n", gai_strerror(s));
        freeaddrinfo(result);
        return -1;
    }

    /* getaddrinfo() returns a list of address structures.
        Try each address until we successfully connect(2).
        If socket(2) (or connect(2)) fails, we (close the socket
        and) try the next address. */

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        g_sockd_mihf = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (g_sockd_mihf == -1)
            continue;

        on = 1;
        setsockopt( g_sockd_mihf, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

        if(rp->ai_family == AF_INET) {
            DEBUG(" %s is an ipv4 address\n",g_mihf_ip_address);
            addr             = (struct sockaddr_in *)(&buf[0]);
            addr->sin_port   = htons(atoi(g_ral_listening_port_for_mihf));
            addr->sin_family = AF_INET;
            s = inet_pton(AF_INET, g_ral_ip_address, &addr->sin_addr);
            if (s <= 0) {
                if (s == 0) {
                    ERR(" IP RAL address should be a IPv4 ADDR - But found not in presentation format : %s\n", g_ral_ip_address);
                } else {
                    ERR(" %s - inet_pton(RAL IPv4 ADDR %s): %s\n", __FUNCTION__, g_ral_ip_address, strerror(s));
                }
                return -1;
            }

            s = bind(g_sockd_mihf, (const struct sockaddr *)addr, sizeof(struct sockaddr_in));
            if (s == -1) {
                ERR(" RAL IPv4 Address Bind: %s\n", strerror(errno));
                return -1;
            }
            // sockd_mihf is of type SOCK_DGRAM, rp->ai_addr is the address to which datagrams are sent by default
            if (connect(g_sockd_mihf, rp->ai_addr, rp->ai_addrlen) != -1) {
                NOTICE(" RAL is now UDP-CONNECTED to MIH-F\n");
                freeaddrinfo(result);
                return 0;
            } else {
                close(g_sockd_mihf);
            }
        } else if (rp->ai_family == AF_INET6) {
            DEBUG(" %s is an ipv6 address\n",g_mihf_ip_address);
            addr6              = (struct sockaddr_in6 *)(&buf[0]);
            addr6->sin6_port   = htons(atoi(g_ral_listening_port_for_mihf));
            addr6->sin6_family = AF_INET6;
            s = inet_pton(AF_INET, g_ral_ip_address, &addr6->sin6_addr);
            if (s <= 0) {
                if (s == 0) {
                    ERR(" IP RAL address should be a IPv6 ADDR, But found not in presentation format : %s\n", g_ral_ip_address);
                } else {
                    ERR(" %s - inet_pton(RAL IPv6 ADDR %s): %s\n", __FUNCTION__, g_ral_ip_address, strerror(s));
                }
                return -1;
            }

            s = bind(g_sockd_mihf, (const struct sockaddr *)addr6, sizeof(struct sockaddr_in));
            if (s == -1) {
                ERR(" RAL IPv6 Address Bind: %s\n", strerror(errno));
                return -1;
            }
            if (connect(g_sockd_mihf, rp->ai_addr, rp->ai_addrlen) != -1) {
                NOTICE(" RAL is now UDP-CONNECTED to MIH-F\n");
                freeaddrinfo(result);
                return 0;
            } else {
                close(g_sockd_mihf);
            }
        } else {
            ERR(" %s is an unknown address format %d\n",g_mihf_ip_address,rp->ai_family);
        }
        close(g_sockd_mihf);
    }

    if (rp == NULL) {   /* No address succeeded */
        ERR(" Could not connect to MIH-F\n");
        return -1;
    }
    return -1;
}
//-----------------------------------------------------------------------------
void mRALlte_send_link_register_indication(MIH_C_TRANSACTION_ID_T  *transaction_idP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Register_indication_t  message;
    Bit_Buffer_t                             *bb;
    int                                       message_total_length;

    bb = new_BitBuffer_0();
    BitBuffer_wrap(bb, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

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
    message.header.transaction_id       = *transaction_idP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_link_id, strlen(g_link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_mihf_id, strlen(g_mihf_id));


    message.primitive.Link_Id.link_type        = MIH_C_WIRELESS_UMTS;
    message.primitive.Link_Id.link_addr.choice = (MIH_C_CHOICE_T)MIH_C_CHOICE_3GPP_ADDR;

    MIH_C_3GPP_ADDR_set(&message.primitive.Link_Id.link_addr._union._3gpp_addr, (u_int8_t*)DEFAULT_ADDRESS_3GPP, strlen(DEFAULT_ADDRESS_3GPP));
    //MIH_C_3GPP_ADDR_load_3gpp_str_address(&message.primitive.Link_Id.link_addr._union._3gpp_addr, (u_int8_t*)DEFAULT_ADDRESS_3GPP);

    message_total_length = MIH_C_Link_Message_Encode_Link_Register_indication(bb, &message);

    #ifdef MSCGEN_PYTOOL
    memset(g_msc_gen_buf, 0, MSC_GEN_BUF_SIZE);
    g_msc_gen_buffer_index = 0;
    MIH_C_LINK_ID2String(&message.primitive.Link_Id, g_msc_gen_buf);
    #endif

    if (mRALlte_send_to_mih(bb->m_buffer,message_total_length)<0){
        ERR(": Send Link_Register.indication\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Register.indication\\n%s ---x][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_msc_gen_buf,
            g_mihf_id);
        #endif
    } else {
        DEBUG(": Sent Link_Register.indication\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Register.indication\\n%s --->][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_msc_gen_buf,
            g_mihf_id);
        #endif
    }
    free_BitBuffer(bb);
}
//-----------------------------------------------------------------------------
void mRALlte_send_link_detected_indication(MIH_C_TRANSACTION_ID_T  *transaction_idP, MIH_C_LINK_DET_INFO_T   *link_detected_infoP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Detected_indication_t  message;
    Bit_Buffer_t                             *bb;
    int                                       message_total_length;

    bb = new_BitBuffer_0();
    BitBuffer_wrap(bb, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

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
    message.header.transaction_id       = *transaction_idP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_link_id, strlen(g_link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_mihf_id, strlen(g_mihf_id));


    memcpy(&message.primitive.LinkDetectedInfo, link_detected_infoP, sizeof(MIH_C_LINK_DET_INFO_T));

    message_total_length = MIH_C_Link_Message_Encode_Link_Detected_indication(bb, &message);

    if (mRALlte_send_to_mih(bb->m_buffer,message_total_length)<0){
        ERR(": Send Link_Detected.indication\n");
        #ifdef MSCGEN_PYTOOL
        g_msc_gen_buffer_index = 0;
        memset(g_msc_gen_buf, 0, MSC_GEN_BUF_SIZE);
        g_msc_gen_buffer_index = MIH_C_SIG_STRENGTH2String(&link_detected_infoP->sig_strength, &g_msc_gen_buf[g_msc_gen_buffer_index]);
        g_msc_gen_buffer_index = sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], "\\nSINR %d\\nData rate %d", link_detected_infoP->sinr, link_detected_infoP->link_data_rate);
        NOTICE("[MSC_MSG][%s][%s][--- Link_Detected.indication\\n%s ---x][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_msc_gen_buf,
            g_mihf_id);
        #endif
    } else {
        DEBUG(": Sent Link_Detected.indication\n");
        #ifdef MSCGEN_PYTOOL
        g_msc_gen_buffer_index = 0;
        memset(g_msc_gen_buf, 0, MSC_GEN_BUF_SIZE);
        g_msc_gen_buffer_index = MIH_C_SIG_STRENGTH2String(&link_detected_infoP->sig_strength, &g_msc_gen_buf[g_msc_gen_buffer_index]);
        g_msc_gen_buffer_index = sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], "\\nSINR %d\\nData rate %d", link_detected_infoP->sinr, link_detected_infoP->link_data_rate);

        NOTICE("[MSC_MSG][%s][%s][--- Link_Detected.indication\\n%s --->][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_msc_gen_buf,
            g_mihf_id);
        #endif
    }
    free_BitBuffer(bb);
}
//-----------------------------------------------------------------------------
void mRALlte_send_link_up_indication(MIH_C_TRANSACTION_ID_T    *transaction_idP,
                                     MIH_C_LINK_TUPLE_ID_T     *link_identifierP,
                                     MIH_C_LINK_ADDR_T         *old_access_routerP,
                                     MIH_C_LINK_ADDR_T         *new_access_routerP,
                                     MIH_C_IP_RENEWAL_FLAG_T   *ip_renewal_flagP,
                                     MIH_C_IP_MOB_MGMT_T       *mobility_management_supportP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Up_indication_t  message;
    Bit_Buffer_t                             *bb;
    int                                       message_total_length;

    bb = new_BitBuffer_0();
    BitBuffer_wrap(bb, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

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
    message.header.transaction_id       = *transaction_idP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_link_id, strlen(g_link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_mihf_id, strlen(g_mihf_id));


    memcpy(&message.primitive.LinkIdentifier, link_identifierP, sizeof(MIH_C_LINK_TUPLE_ID_T));

    message.primitive.OldAccessRouter            = old_access_routerP;
    message.primitive.NewAccessRouter            = new_access_routerP;
    message.primitive.IPRenewalFlag              = ip_renewal_flagP;
    message.primitive.MobilityManagementSupport  = mobility_management_supportP;

    message_total_length = MIH_C_Link_Message_Encode_Link_Up_indication(bb, &message);

    if (mRALlte_send_to_mih(bb->m_buffer,message_total_length)<0){
        ERR(": Send Link_Up.indication\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Up.indication ---x][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_mihf_id);
        #endif
    } else {
        DEBUG(": Sent Link_Up.indication\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Up.indication --->][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_mihf_id);
        #endif
    }
    free_BitBuffer(bb);
}
//-----------------------------------------------------------------------------
void mRALlte_send_link_parameters_report_indication(MIH_C_TRANSACTION_ID_T      *transaction_idP,
                                                    MIH_C_LINK_TUPLE_ID_T       *link_identifierP,
                                                    MIH_C_LINK_PARAM_RPT_LIST_T *link_parameters_report_listP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Parameters_Report_indication_t  message;
    Bit_Buffer_t                             *bb;
    int                                       message_total_length;
    #ifdef MSCGEN_PYTOOL
    unsigned int                              index;
    #endif

    bb = new_BitBuffer_0();
    BitBuffer_wrap(bb, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

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
    message.header.transaction_id       = *transaction_idP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_link_id, strlen(g_link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_mihf_id, strlen(g_mihf_id));


    memcpy(&message.primitive.LinkIdentifier, link_identifierP, sizeof(MIH_C_LINK_TUPLE_ID_T));
    memcpy(&message.primitive.LinkParametersReportList_list, link_parameters_report_listP, sizeof(MIH_C_LINK_PARAM_RPT_LIST_T));

    message_total_length = MIH_C_Link_Message_Encode_Link_Parameters_Report_indication(bb, &message);

    if (mRALlte_send_to_mih(bb->m_buffer,message_total_length)<0){
        ERR(": Send Link_Parameters_Report.indication\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Parameters_Report.indication ---x][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_mihf_id);
        #endif
    } else {
        DEBUG(": Sent Link_Parameters_Report.indication\n");
        #ifdef MSCGEN_PYTOOL
        g_msc_gen_buffer_index = 0;
        memset(g_msc_gen_buf, 0, MSC_GEN_BUF_SIZE);
        g_msc_gen_buffer_index += MIH_C_LINK_ID2String(&link_identifierP->link_id, &g_msc_gen_buf[g_msc_gen_buffer_index]);
        for (index = 0; index < link_parameters_report_listP->length; index++) {
            g_msc_gen_buffer_index += sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], "\\n");
            g_msc_gen_buffer_index += MIH_C_LINK_PARAM_RPT2String(&link_parameters_report_listP->val[index], &g_msc_gen_buf[g_msc_gen_buffer_index]);
        }
        NOTICE("[MSC_MSG][%s][%s][--- Link_Parameters_Report.indication\\n%s --->][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_msc_gen_buf,
            g_mihf_id);
        #endif
    }
    free_BitBuffer(bb);
}
//-----------------------------------------------------------------------------
void mRALlte_send_link_going_down_indication(MIH_C_TRANSACTION_ID_T      *transaction_idP,
                                                    MIH_C_LINK_TUPLE_ID_T       *link_identifierP,
                                                    MIH_C_UNSIGNED_INT2_T       *time_intervalP,
                                                    MIH_C_LINK_GD_REASON_T      *link_going_down_reasonP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Going_Down_indication_t  message;
    Bit_Buffer_t                             *bb;
    int                                       message_total_length;

    bb = new_BitBuffer_0();
    BitBuffer_wrap(bb, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

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
    message.header.transaction_id       = *transaction_idP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_link_id, strlen(g_link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_mihf_id, strlen(g_mihf_id));


    memcpy(&message.primitive.LinkIdentifier, link_identifierP, sizeof(MIH_C_LINK_TUPLE_ID_T));
    message.primitive.TimeInterval = *time_intervalP;
    memcpy(&message.primitive.LinkGoingDownReason, link_going_down_reasonP, sizeof(MIH_C_LINK_GD_REASON_T));


    message_total_length = MIH_C_Link_Message_Encode_Link_Going_Down_indication(bb, &message);

    if (mRALlte_send_to_mih(bb->m_buffer,message_total_length)<0){
        ERR(": Send Link_Going_Down.indication\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Going_Down.indication ---x][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_mihf_id);
        #endif
    } else {
        DEBUG(": Sent Link_Going_Down.indication\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Going_Down.indication --->][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_mihf_id);
        #endif
    }
    free_BitBuffer(bb);
}
//-----------------------------------------------------------------------------
void mRALlte_send_link_down_indication(MIH_C_TRANSACTION_ID_T      *transaction_idP,
                                       MIH_C_LINK_TUPLE_ID_T       *link_identifierP,
                                       MIH_C_LINK_ADDR_T           *old_access_routerP,
                                       MIH_C_LINK_DN_REASON_T      *reason_codeP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Down_indication_t      message;
    Bit_Buffer_t                             *bb;
    int                                       message_total_length;

    bb = new_BitBuffer_0();
    BitBuffer_wrap(bb, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

    memset(&message, 0, sizeof (MIH_C_Message_Link_Going_Down_indication_t));

    message.header.version              = (MIH_C_VERSION_T)MIH_C_PROTOCOL_VERSION;
    //message.header.ack_req            = 0;
    //message.header.ack_rsp            = 0;
    //message.header.uir                = 0;
    //message.header.more_fragment      = 0
    //message.header.fragment_number    = 0;
    message.header.service_identifier   = (MIH_C_SID_T)2;
    message.header.operation_code       = (MIH_C_OPCODE_T)3;
    message.header.action_identifier    = (MIH_C_AID_T)3;
    message.header.transaction_id       = *transaction_idP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_link_id, strlen(g_link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_mihf_id, strlen(g_mihf_id));


    memcpy(&message.primitive.LinkIdentifier, link_identifierP, sizeof(MIH_C_LINK_TUPLE_ID_T));
    message.primitive.OldAccessRouter = old_access_routerP;
    memcpy(&message.primitive.ReasonCode, reason_codeP, sizeof(MIH_C_LINK_DN_REASON_T));


    message_total_length = MIH_C_Link_Message_Encode_Link_Down_indication(bb, &message);

    if (mRALlte_send_to_mih(bb->m_buffer,message_total_length)<0){
        ERR(": Send Link_Down.indication\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Down.indication ---x][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_mihf_id);
        #endif
    } else {
        DEBUG(": Sent Link_Down.indication\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Down.indication --->][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_mihf_id);
        #endif
    }
    free_BitBuffer(bb);
}
//-----------------------------------------------------------------------------
void mRALlte_send_link_action_confirm(MIH_C_TRANSACTION_ID_T     *transaction_idP,
                                      MIH_C_STATUS_T             *statusP,
                                      MIH_C_LINK_SCAN_RSP_LIST_T *scan_response_setP,
                                      MIH_C_LINK_AC_RESULT_T     *link_action_resultP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Action_confirm_t       message;
    Bit_Buffer_t                             *bb;
    int                                       message_total_length;
#ifdef MSCGEN_PYTOOL
    unsigned int                              index;
#endif

    bb = new_BitBuffer_0();
    BitBuffer_wrap(bb, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

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
    message.header.transaction_id       = *transaction_idP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_link_id, strlen(g_link_id));

    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_mihf_id, strlen(g_mihf_id));


    message.primitive.Status                       = *statusP;
    message.primitive.ScanResponseSet_list         = scan_response_setP;
    message.primitive.LinkActionResult             = link_action_resultP;


    message_total_length = MIH_C_Link_Message_Encode_Link_Action_confirm(bb, &message);

    #ifdef MSCGEN_PYTOOL
    memset(g_msc_gen_buf, 0, MSC_GEN_BUF_SIZE);
    g_msc_gen_buffer_index = 0;
    g_msc_gen_buffer_index += MIH_C_STATUS2String(&message.primitive.Status, &g_msc_gen_buf[g_msc_gen_buffer_index]);
    if (scan_response_setP) {
        for (index = 0; index < scan_response_setP->length; index++) {
            g_msc_gen_buffer_index += sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], "\\nScan resp:");
            g_msc_gen_buffer_index += MIH_C_LINK_SCAN_RSP2String(&scan_response_setP->val[index], &g_msc_gen_buf[g_msc_gen_buffer_index]);
        }
    }
    if (link_action_resultP) {
            g_msc_gen_buffer_index += sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], "\\nAction result:");
            g_msc_gen_buffer_index += MIH_C_LINK_AC_RESULT2String2(link_action_resultP, &g_msc_gen_buf[g_msc_gen_buffer_index]);
    }
    #endif
    if (mRALlte_send_to_mih(bb->m_buffer,message_total_length)<0){
        ERR(": Send Link_Action.confirm\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Action.confirm\\nstatus=%s ---x][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_msc_gen_buf,
            g_mihf_id);
        #endif
    } else {
        DEBUG(": Sent Link_Action.confirm\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Action.confirm\\nstatus=%s --->][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_msc_gen_buf,
            g_mihf_id);
        #endif
    }
    free_BitBuffer(bb);
}

//-----------------------------------------------------------------------------
void mRALte_send_capability_discover_confirm(MIH_C_TRANSACTION_ID_T  *transaction_idP,
                                             MIH_C_STATUS_T          *statusP,
                                             MIH_C_LINK_EVENT_LIST_T *supported_link_event_listP,
                                             MIH_C_LINK_CMD_LIST_T   *supported_link_command_listP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Capability_Discover_confirm_t  message;
    Bit_Buffer_t                             *bb;
    int                                       message_total_length;

    bb = new_BitBuffer_0();
    BitBuffer_wrap(bb, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

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
    message.header.transaction_id       = *transaction_idP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_link_id, strlen(g_link_id));
    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_mihf_id, strlen(g_mihf_id));

    message.primitive.Status                   = *statusP;
    message.primitive.SupportedLinkEventList   = supported_link_event_listP;
    message.primitive.SupportedLinkCommandList = supported_link_command_listP;

    message_total_length = MIH_C_Link_Message_Encode_Capability_Discover_confirm(bb, &message);

    #ifdef MSCGEN_PYTOOL
    memset(g_msc_gen_buf, 0, MSC_GEN_BUF_SIZE);
    g_msc_gen_buffer_index = 0;
    g_msc_gen_buffer_index += MIH_C_STATUS2String(&message.primitive.Status, &g_msc_gen_buf[g_msc_gen_buffer_index]);
    g_msc_gen_buffer_index += sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], "\\n\\nsupported link events=");
    g_msc_gen_buffer_index += MIH_C_LINK_EVENT_LIST2String2(message.primitive.SupportedLinkEventList, &g_msc_gen_buf[g_msc_gen_buffer_index]);
    g_msc_gen_buffer_index += sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], "\\nsupported commands=");
    g_msc_gen_buffer_index += MIH_C_LINK_CMD_LIST2String2(message.primitive.SupportedLinkCommandList, &g_msc_gen_buf[g_msc_gen_buffer_index]);
    #endif
    if (mRALlte_send_to_mih(bb->m_buffer,message_total_length)<0){
        ERR(": Send Link_Capability_Discover.confirm\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Capability_Discover.confirm\\nstatus=%s ---x][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_msc_gen_buf,
            g_mihf_id);
        #endif
    } else {
        DEBUG(": Sent Link_Capability_Discover.confirm\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Capability_Discover.confirm\\nstatus=%s --->][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_msc_gen_buf,
            g_mihf_id);
        #endif
    }
    free_BitBuffer(bb);
}
//-----------------------------------------------------------------------------
void mRALte_send_event_subscribe_confirm(MIH_C_TRANSACTION_ID_T  *transaction_idP,
                                         MIH_C_STATUS_T          *statusP,
                                         MIH_C_LINK_EVENT_LIST_T *response_link_event_listP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Event_Subscribe_confirm_t  message;
    Bit_Buffer_t                                 *bb;
    int                                           message_total_length;

    bb = new_BitBuffer_0();
    BitBuffer_wrap(bb, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

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
    message.header.transaction_id       = *transaction_idP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_link_id, strlen(g_link_id));
    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_mihf_id, strlen(g_mihf_id));

    message.primitive.Status                   = *statusP;
    message.primitive.ResponseLinkEventList    =  response_link_event_listP;

    message_total_length = MIH_C_Link_Message_Encode_Event_Subscribe_confirm(bb, &message);

    #ifdef MSCGEN_PYTOOL
    memset(g_msc_gen_buf, 0, MSC_GEN_BUF_SIZE);
    g_msc_gen_buffer_index = 0;
    g_msc_gen_buffer_index += MIH_C_STATUS2String(&message.primitive.Status, &g_msc_gen_buf[g_msc_gen_buffer_index]);
    g_msc_gen_buffer_index += sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], "\\nlink event list=");
    g_msc_gen_buffer_index += MIH_C_LINK_EVENT_LIST2String2(message.primitive.ResponseLinkEventList, &g_msc_gen_buf[g_msc_gen_buffer_index]);
    #endif
    if (mRALlte_send_to_mih(bb->m_buffer,message_total_length)<0){
        ERR(": Send Link_Event_Subscribe.confirm\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Event_Subscribe.confirm\\nstatus=%s ---x][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_msc_gen_buf,
            g_mihf_id);
        #endif
    } else {
        DEBUG(": Sent Link_Event_Subscribe.confirm\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Event_Subscribe.confirm\\nstatus=%s --->][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_msc_gen_buf,
            g_mihf_id);
        #endif
    }
    free_BitBuffer(bb);
}
//-----------------------------------------------------------------------------
void mRALte_send_event_unsubscribe_confirm(MIH_C_TRANSACTION_ID_T  *transaction_idP,
                                           MIH_C_STATUS_T          *statusP,
                                           MIH_C_LINK_EVENT_LIST_T *response_link_event_listP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Event_Unsubscribe_confirm_t  message;
    Bit_Buffer_t                                   *bb;
    int                                             message_total_length;

    bb = new_BitBuffer_0();
    BitBuffer_wrap(bb, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

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
    message.header.transaction_id       = *transaction_idP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_link_id, strlen(g_link_id));
    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_mihf_id, strlen(g_mihf_id));

    message.primitive.Status                   = *statusP;
    message.primitive.ResponseLinkEventList    =  response_link_event_listP;

    message_total_length = MIH_C_Link_Message_Encode_Event_Unsubscribe_confirm(bb, &message);

    #ifdef MSCGEN_PYTOOL
    memset(g_msc_gen_buf, 0, MSC_GEN_BUF_SIZE);
    g_msc_gen_buffer_index = 0;
    g_msc_gen_buffer_index += MIH_C_STATUS2String(&message.primitive.Status, &g_msc_gen_buf[g_msc_gen_buffer_index]);
    g_msc_gen_buffer_index += sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], "\\nlink event list=");
    g_msc_gen_buffer_index += MIH_C_LINK_EVENT_LIST2String2(message.primitive.ResponseLinkEventList, &g_msc_gen_buf[g_msc_gen_buffer_index]);
    #endif
    if (mRALlte_send_to_mih(bb->m_buffer,message_total_length)<0){
        ERR(": Send Link_Event_Unsubscribe.confirm\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Event_Unsubscribe.confirm\\nstatus=%s ---x][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_msc_gen_buf,
            g_mihf_id);
        #endif
    } else {
        DEBUG(": Sent Link_Event_Unsubscribe.confirm\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Event_Unsubscribe.confirm\\nstatus=%s --->][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_msc_gen_buf,
            g_mihf_id);
        #endif
    }
    free_BitBuffer(bb);
}
 //-----------------------------------------------------------------------------
void mRALte_send_configure_thresholds_confirm(MIH_C_TRANSACTION_ID_T   *transaction_idP,
                                         MIH_C_STATUS_T               *statusP,
                                         MIH_C_LINK_CFG_STATUS_LIST_T *link_configure_status_listP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Configure_Thresholds_confirm_t  message;
    Bit_Buffer_t                                      *bb;
    int                                                message_total_length;
    #ifdef MSCGEN_PYTOOL
    int                                                i;
    #endif

    bb = new_BitBuffer_0();
    BitBuffer_wrap(bb, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

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
    message.header.transaction_id       = *transaction_idP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_link_id, strlen(g_link_id));
    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_mihf_id, strlen(g_mihf_id));

    message.primitive.Status                   = *statusP;
    message.primitive.LinkConfigureStatusList_list  =  link_configure_status_listP;

    message_total_length = MIH_C_Link_Message_Encode_Configure_Thresholds_confirm(bb, &message);

    #ifdef MSCGEN_PYTOOL
    memset(g_msc_gen_buf, 0, MSC_GEN_BUF_SIZE);
    g_msc_gen_buffer_index = 0;
    g_msc_gen_buffer_index += MIH_C_STATUS2String(&message.primitive.Status, &g_msc_gen_buf[g_msc_gen_buffer_index]);
    g_msc_gen_buffer_index += sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], "\\n");
    for (i = 0; i < link_configure_status_listP->length; i++) {
        g_msc_gen_buffer_index += MIH_C_LINK_CFG_STATUS2String(&link_configure_status_listP->val[i], &g_msc_gen_buf[g_msc_gen_buffer_index]);
    }
    #endif
    if (mRALlte_send_to_mih(bb->m_buffer,message_total_length)<0){
        ERR(": Send Link_Configure_Threshold.confirm\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Configure_Thresholds.confirm\\nstatus=%s ---x][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_msc_gen_buf,
            g_mihf_id);
        #endif
    } else {
        DEBUG(": Sent Link_Configure_Threshold.confirm\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Configure_Thresholds.confirm\\nstatus=%s --->][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_msc_gen_buf,
            g_mihf_id);
        #endif
    }
    free_BitBuffer(bb);
}
//-----------------------------------------------------------------------------
void mRALte_send_get_parameters_confirm     (MIH_C_TRANSACTION_ID_T       *transaction_idP,
                                             MIH_C_STATUS_T               *statusP,
                                             MIH_C_LINK_PARAM_LIST_T      *link_parameters_status_listP,
                                             MIH_C_LINK_STATES_RSP_LIST_T *link_states_response_listP,
                                             MIH_C_LINK_DESC_RSP_LIST_T   *link_descriptors_response_listP) {
//-----------------------------------------------------------------------------
    MIH_C_Message_Link_Get_Parameters_confirm_t  message;
    Bit_Buffer_t                                *bb;
    int                                          message_total_length;
#ifdef MSCGEN_PYTOOL
    char                                     msg_src[32];
    char                                     msg_dst[32];
    int                                      i;
#endif

    bb = new_BitBuffer_0();
    BitBuffer_wrap(bb, g_msg_codec_send_buffer, (unsigned int)MSG_CODEC_SEND_BUFFER_SIZE);

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
    message.header.transaction_id       = *transaction_idP;


    MIH_C_MIHF_ID_set(&message.source, (u_int8_t*)g_link_id, strlen(g_link_id));
    MIH_C_MIHF_ID_set(&message.destination, (u_int8_t*)g_mihf_id, strlen(g_mihf_id));

    message.primitive.Status                        = *statusP;
    message.primitive.LinkParametersStatusList_list = link_parameters_status_listP;
    message.primitive.LinkStatesResponse_list       = link_states_response_listP;
    message.primitive.LinkDescriptorsResponse_list  = link_descriptors_response_listP;

    message_total_length = MIH_C_Link_Message_Encode_Get_Parameters_confirm(bb, &message);

    if (mRALlte_send_to_mih(bb->m_buffer,message_total_length)<0){
        ERR(": Send Link_Get_Parameters.confirm\n");
        #ifdef MSCGEN_PYTOOL
        NOTICE("[MSC_MSG][%s][%s][--- Link_Get_Parameters.confirm ---x][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_mihf_id);
        #endif
    } else {
        DEBUG(": Sent Link_Get_Parameters.confirm\n");
        #ifdef MSCGEN_PYTOOL
        MIH_C_MIHF_ID2String(&message.source, msg_src);
        MIH_C_MIHF_ID2String(&message.destination, msg_dst);
        memset(g_msc_gen_buf, 0, MSC_GEN_BUF_SIZE);
        g_msc_gen_buffer_index = 0;
        g_msc_gen_buffer_index += MIH_C_STATUS2String(&message.primitive.Status, &g_msc_gen_buf[g_msc_gen_buffer_index]);
        if (link_parameters_status_listP) {
            g_msc_gen_buffer_index += sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], "\\n LinkParametersStatusList=");
            for (i = 0; i < link_parameters_status_listP->length; i++) {
                g_msc_gen_buffer_index += MIH_C_LINK_PARAM2String(&link_parameters_status_listP->val[i], &g_msc_gen_buf[g_msc_gen_buffer_index]);
                g_msc_gen_buffer_index += sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], ", ");
            }
        }
        if (link_states_response_listP) {
            g_msc_gen_buffer_index += sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], "\\n LinkStatesResponseList=");
            for (i = 0; i < link_states_response_listP->length; i++) {
                g_msc_gen_buffer_index += MIH_C_LINK_STATES_RSP2String(&link_states_response_listP->val[i], &g_msc_gen_buf[g_msc_gen_buffer_index]);
                g_msc_gen_buffer_index += sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], ", ");
            }
        }
        if (link_descriptors_response_listP) {
            g_msc_gen_buffer_index += sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], "\\n LinkDescriptorsResponseList=");
            for (i = 0; i < link_descriptors_response_listP->length; i++) {
                g_msc_gen_buffer_index += MIH_C_LINK_DESC_RSP2String(&link_descriptors_response_listP->val[i], &g_msc_gen_buf[g_msc_gen_buffer_index]);
                g_msc_gen_buffer_index += sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], ", ");
            }
        }
        NOTICE("[MSC_MSG][%s][%s][--- Link_Get_Parameters.confirm\\n%s --->][%s]\n",
            getTimeStamp4Log(),
            g_link_id,
            g_msc_gen_buf,
            g_mihf_id);
        #endif
    }
    free_BitBuffer(bb);
}
//-----------------------------------------------------------------------------
int mRALlte_mih_link_msg_decode(Bit_Buffer_t* bbP, MIH_C_Message_Wrapper_t *message_wrapperP) {
//-----------------------------------------------------------------------------
    int                                      status = MIH_MESSAGE_DECODE_FAILURE;
    MIH_C_HEADER_T                           header;
    MIH_C_STATUS_T                           mih_status;
#ifdef MSCGEN_PYTOOL
    char                                     msg_src[32];
    char                                     msg_dst[32];
    int                                      i;
#endif

    if ((bbP != NULL) && (message_wrapperP != NULL)) {
        status = MIH_C_Link_Header_Decode(bbP, &header);
        if (status == MIH_HEADER_DECODE_TOO_SHORT) {
            return MIH_MESSAGE_DECODE_TOO_SHORT;
        } else if (status == MIH_HEADER_DECODE_FAILURE) {
            return MIH_MESSAGE_DECODE_FAILURE;
        } else if (status == MIH_HEADER_DECODE_BAD_PARAMETER) {
            return MIH_MESSAGE_DECODE_BAD_PARAMETER;
        }
        message_wrapperP->message_id = MIH_C_MESSAGE_ID(header.service_identifier, header.operation_code, header.action_identifier);

        switch (message_wrapperP->message_id) {
            case MIH_C_MESSAGE_LINK_CAPABILITY_DISCOVER_REQUEST_ID:
                DEBUG(" %s Received MIH_C_MESSAGE_LINK_CAPABILITY_DISCOVER_REQUEST\n", __FUNCTION__);
                memcpy(&message_wrapperP->_union_message.link_capability_discover_request.header, (const void *)&header, sizeof(MIH_C_HEADER_T));
                status = MIH_C_Link_Message_Decode_Link_Capability_Discover_request(bbP, &message_wrapperP->_union_message.link_capability_discover_request);
                if (status == MIH_MESSAGE_DECODE_OK) {
                    #ifdef MSCGEN_PYTOOL
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_capability_discover_request.source), msg_src);
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_capability_discover_request.destination), msg_dst);
                    NOTICE("[MSC_MSG][%s][%s][--- Link_Capability_Discover.request --->][%s]\n",
                        getTimeStamp4Log(),
                        msg_src,
                        msg_dst);
                    #endif
                    MIH_C_Link_Message_Link_Capability_Discover_request2String(&message_wrapperP->_union_message.link_capability_discover_request, g_msg_print_buffer);
                    DEBUG("%s", g_msg_print_buffer);

                    mih_status = MIH_C_STATUS_SUCCESS;
                    DEBUG("**\n");
                    DEBUG(" %s Sending MIH_C_MESSAGE_LINK_CAPABILITY_DISCOVER_CONFIRM\n\n", __FUNCTION__);

                    mRALte_send_capability_discover_confirm(&message_wrapperP->_union_message.link_capability_discover_request.header.transaction_id,
                                                            &mih_status,
                                                            &ralpriv->mih_supported_link_event_list,
                                                            &ralpriv->mih_supported_link_command_list);
                } else {
                    #ifdef MSCGEN_PYTOOL
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_capability_discover_request.source), msg_src);
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_capability_discover_request.destination), msg_dst);
                    NOTICE("[MSC_MSG][%s][%s][--- Link_Capability_Discover.request\\nERR DECODE ---x][%s]\n",
                        getTimeStamp4Log(),
                        msg_src,
                        msg_dst);
                    #endif
                }
                break;

            case MIH_C_MESSAGE_LINK_EVENT_SUBSCRIBE_REQUEST_ID:
                DEBUG(" %s Received MIH_C_MESSAGE_LINK_EVENT_SUBSCRIBE_REQUEST\n", __FUNCTION__);
                memcpy(&message_wrapperP->_union_message.link_event_subscribe_request.header, (const void *)&header, sizeof(MIH_C_HEADER_T));
                status = MIH_C_Link_Message_Decode_Link_Event_Subscribe_request(bbP, &message_wrapperP->_union_message.link_event_subscribe_request);
                if (status == MIH_MESSAGE_DECODE_OK) {
                    #ifdef MSCGEN_PYTOOL
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_event_subscribe_request.source), msg_src);
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_event_subscribe_request.destination), msg_dst);
                    memset(g_msc_gen_buf, 0, MSC_GEN_BUF_SIZE);
                    MIH_C_LINK_EVENT_LIST2String2(&message_wrapperP->_union_message.link_event_subscribe_request.primitive.RequestedLinkEventList, g_msc_gen_buf);
                    NOTICE("[MSC_MSG][%s][%s][--- Link_Event_Subscribe.request\\n%s --->][%s]\n",
                        getTimeStamp4Log(),
                        msg_src,
                        g_msc_gen_buf,
                        msg_dst);
                    #endif
                    DEBUG("**\n");

                    mRALlte_subscribe_request(&message_wrapperP->_union_message.link_event_subscribe_request);
                } else {
                    #ifdef MSCGEN_PYTOOL
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_event_subscribe_request.source), msg_src);
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_event_subscribe_request.destination), msg_dst);
                    memset(g_msc_gen_buf, 0, MSC_GEN_BUF_SIZE);
                    MIH_C_LINK_EVENT_LIST2String2(&message_wrapperP->_union_message.link_event_subscribe_request.primitive.RequestedLinkEventList, g_msc_gen_buf);
                    NOTICE("[MSC_MSG][%s][%s][--- Link_Event_Subscribe.request\\nERR DECODE ---x][%s]\n",
                        getTimeStamp4Log(),
                        msg_src,
                        msg_dst);
                    #endif
                }
                break;

            case MIH_C_MESSAGE_LINK_EVENT_UNSUBSCRIBE_REQUEST_ID:
                DEBUG(" %s Received MIH_C_MESSAGE_LINK_EVENT_UNSUBSCRIBE_REQUEST\n", __FUNCTION__);
                memcpy(&message_wrapperP->_union_message.link_event_unsubscribe_request.header, (const void *)&header, sizeof(MIH_C_HEADER_T));
                status = MIH_C_Link_Message_Decode_Link_Event_Unsubscribe_request(bbP, &message_wrapperP->_union_message.link_event_unsubscribe_request);
                if (status == MIH_MESSAGE_DECODE_OK) {
                    #ifdef MSCGEN_PYTOOL
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_event_unsubscribe_request.source), msg_src);
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_event_unsubscribe_request.destination), msg_dst);
                    memset(g_msc_gen_buf, 0, MSC_GEN_BUF_SIZE);
                    MIH_C_LINK_EVENT_LIST2String2(&message_wrapperP->_union_message.link_event_unsubscribe_request.primitive.RequestedLinkEventList, g_msc_gen_buf);
                    NOTICE("[MSC_MSG][%s][%s][--- Link_Event_Unsubscribe.request\\n%s --->][%s]\n",
                        getTimeStamp4Log(),
                        msg_src,
                        g_msc_gen_buf,
                        msg_dst);
                    #endif
                    DEBUG("**\n");
                    mRALlte_unsubscribe_request(&message_wrapperP->_union_message.link_event_unsubscribe_request);
                } else {
                    #ifdef MSCGEN_PYTOOL
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_event_unsubscribe_request.source), msg_src);
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_event_unsubscribe_request.destination), msg_dst);
                    memset(g_msc_gen_buf, 0, MSC_GEN_BUF_SIZE);
                    MIH_C_LINK_EVENT_LIST2String2(&message_wrapperP->_union_message.link_event_unsubscribe_request.primitive.RequestedLinkEventList, g_msc_gen_buf);
                    NOTICE("[MSC_MSG][%s][%s][--- Link_Event_Unsubscribe.request\\nERR DECODE ---x][%s]\n",
                        getTimeStamp4Log(),
                        msg_src,
                        msg_dst);
                    #endif
                }
                break;

            case MIH_C_MESSAGE_LINK_GET_PARAMETERS_REQUEST_ID:
                DEBUG(" %s Received MIH_C_MESSAGE_LINK_GET_PARAMETERS_REQUEST\n", __FUNCTION__);
                memcpy(&message_wrapperP->_union_message.link_get_parameters_request.header, (const void *)&header, sizeof(MIH_C_HEADER_T));
                status = MIH_C_Link_Message_Decode_Link_Get_Parameters_request(bbP, &message_wrapperP->_union_message.link_get_parameters_request);
                if (status == MIH_MESSAGE_DECODE_OK) {
                    #ifdef MSCGEN_PYTOOL
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_get_parameters_request.source), msg_src);
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_get_parameters_request.destination), msg_dst);
                    memset(g_msc_gen_buf, 0, MSC_GEN_BUF_SIZE);
                    g_msc_gen_buffer_index = 0;
                    for (i = 0; i < message_wrapperP->_union_message.link_get_parameters_request.primitive.LinkParametersRequest_list.length; i++) {
                        g_msc_gen_buffer_index += MIH_C_LINK_PARAM_TYPE2String(&message_wrapperP->_union_message.link_get_parameters_request.primitive.LinkParametersRequest_list.val[i], &g_msc_gen_buf[g_msc_gen_buffer_index]);
                    }
                    g_msc_gen_buffer_index += sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], "\\n Link states request=");
                    g_msc_gen_buffer_index += MIH_C_LINK_STATES_REQ2String2(&message_wrapperP->_union_message.link_get_parameters_request.primitive.LinkStatesRequest, &g_msc_gen_buf[g_msc_gen_buffer_index]);
                    g_msc_gen_buffer_index += sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], "\\n Link desc request=");
                    g_msc_gen_buffer_index += MIH_C_LINK_DESC_REQ2String2(&message_wrapperP->_union_message.link_get_parameters_request.primitive.LinkDescriptorsRequest, &g_msc_gen_buf[g_msc_gen_buffer_index]);

                    NOTICE("[MSC_MSG][%s][%s][--- Link_Get_Parameters.request\\n%s --->][%s]\n",
                        getTimeStamp4Log(),
                        msg_src,
                        g_msc_gen_buf,
                        msg_dst);
                    #endif
                    DEBUG("**\n");
                     mRALlte_get_parameters_request(&message_wrapperP->_union_message.link_get_parameters_request);
                } else {
                    #ifdef MSCGEN_PYTOOL
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_get_parameters_request.source), msg_src);
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_get_parameters_request.destination), msg_dst);
                    NOTICE("[MSC_MSG][%s][%s][--- Link_Get_Parameters.request\\nERR DECODE ---x][%s]\n",
                        getTimeStamp4Log(),
                        msg_src,
                        msg_dst);
                    #endif
                }
                break;

            case MIH_C_MESSAGE_LINK_CONFIGURE_THRESHOLDS_REQUEST_ID:
                DEBUG(" %s Received MIH_C_MESSAGE_LINK_CONFIGURE_THRESHOLDS_REQUEST\n", __FUNCTION__);
                memcpy(&message_wrapperP->_union_message.link_configure_thresholds_request.header, (const void *)&header, sizeof(MIH_C_HEADER_T));
                status = MIH_C_Link_Message_Decode_Link_Configure_Thresholds_request(bbP, &message_wrapperP->_union_message.link_configure_thresholds_request);
                if (status == MIH_MESSAGE_DECODE_OK) {
                    #ifdef MSCGEN_PYTOOL
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_configure_thresholds_request.source), msg_src);
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_configure_thresholds_request.destination), msg_dst);
                    memset(g_msc_gen_buf, 0, MSC_GEN_BUF_SIZE);
                    g_msc_gen_buffer_index = 0;
                    for (i = 0; i < message_wrapperP->_union_message.link_configure_thresholds_request.primitive.LinkConfigureParameterList_list.length; i++) {
                        g_msc_gen_buffer_index += MIH_C_LINK_CFG_PARAM2String(&message_wrapperP->_union_message.link_configure_thresholds_request.primitive.LinkConfigureParameterList_list.val[i], &g_msc_gen_buf[g_msc_gen_buffer_index]);
                    }
                    NOTICE("[MSC_MSG][%s][%s][--- Link_Configure_Thresholds.request\\n%s --->][%s]\n",
                        getTimeStamp4Log(),
                        msg_src,
                        g_msc_gen_buf,
                        msg_dst);
                    #endif
                    DEBUG("**\n");
                    mRALlte_configure_thresholds_request(&message_wrapperP->_union_message.link_configure_thresholds_request);
                } else {
                    #ifdef MSCGEN_PYTOOL
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_configure_thresholds_request.source), msg_src);
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_configure_thresholds_request.destination), msg_dst);
                    NOTICE("[MSC_MSG][%s][%s][--- Link_Configure_Thresholds.request\\nERR DECODE ---x][%s]\n",
                        getTimeStamp4Log(),
                        msg_src,
                        msg_dst);
                    #endif
                }
                break;

            case MIH_C_MESSAGE_LINK_ACTION_REQUEST_ID:
                DEBUG(" %s Received MIH_C_MESSAGE_LINK_ACTION_REQUEST\n", __FUNCTION__);
                memcpy(&message_wrapperP->_union_message.link_action_request.header, (const void *)&header, sizeof(MIH_C_HEADER_T));
                status = MIH_C_Link_Message_Decode_Link_Action_request(bbP, &message_wrapperP->_union_message.link_action_request);
                if (status == MIH_MESSAGE_DECODE_OK) {
                    #ifdef MSCGEN_PYTOOL
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_action_request.source), msg_src);
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_action_request.destination), msg_dst);
                    memset(g_msc_gen_buf, 0, MSC_GEN_BUF_SIZE);
                    g_msc_gen_buffer_index = 0;
                    g_msc_gen_buffer_index += MIH_C_LINK_ACTION2String(&message_wrapperP->_union_message.link_action_request.primitive.LinkAction, g_msc_gen_buf);
                    g_msc_gen_buffer_index += sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], "\\nExecution Delay=%d", message_wrapperP->_union_message.link_action_request.primitive.ExecutionDelay);
                    if (message_wrapperP->_union_message.link_action_request.primitive.PoALinkAddress != NULL) {
                        g_msc_gen_buffer_index += sprintf(&g_msc_gen_buf[g_msc_gen_buffer_index], "\\nPoALinkAddress=");
                        g_msc_gen_buffer_index += MIH_C_LINK_ADDR2String(message_wrapperP->_union_message.link_action_request.primitive.PoALinkAddress, &g_msc_gen_buf[g_msc_gen_buffer_index]);
                    }
                    NOTICE("[MSC_MSG][%s][%s][--- Link_Action.request\\n%s --->][%s]\n",
                        getTimeStamp4Log(),
                        msg_src,
                        g_msc_gen_buf,
                        msg_dst);
                    #endif
                    DEBUG("**\n");
                    mRALlte_action_request(&message_wrapperP->_union_message.link_action_request);
                } else {
                    #ifdef MSCGEN_PYTOOL
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_action_request.source), msg_src);
                    MIH_C_MIHF_ID2String(&(message_wrapperP->_union_message.link_action_request.destination), msg_dst);
                    NOTICE("[MSC_MSG][%s][%s][--- Link_Action.request\\nERR DECODE --->][%s]\n",
                        getTimeStamp4Log(),
                        msg_src,
                        msg_dst);
                    #endif
                }
                break;

            default:
                WARNING("UNKNOWN MESSAGE ID SID %d, OP_CODE %d, AID %d\n", header.service_identifier, header.operation_code, header.action_identifier);
                status = MIH_MESSAGE_DECODE_FAILURE;

            return status;
        }
    } else {
        return MIH_MESSAGE_DECODE_BAD_PARAMETER;
    }
    return status;
}
//-----------------------------------------------------------------------------
int mRALlte_mih_link_process_message(void){
//-----------------------------------------------------------------------------
    MIH_C_Message_Wrapper_t  message_wrapper;
    int                      nb_bytes_received ;
    int                      nb_bytes_decoded  ;
    int                      total_bytes_to_decode ;
    int                      status ;
    Bit_Buffer_t            *bb;
    struct sockaddr_in       udp_socket;
    socklen_t                sockaddr_len;


    total_bytes_to_decode = 0;
    nb_bytes_received     = 0;

    bb = new_BitBuffer_0();

    nb_bytes_received = recvfrom(g_sockd_mihf,
                                 (void *)g_msg_codec_recv_buffer,
                                 MSG_CODEC_RECV_BUFFER_SIZE,
                                 0,
                                 (struct sockaddr *) &udp_socket,
                                 &sockaddr_len);

    if (nb_bytes_received > 0) {
        DEBUG(" \n");
        DEBUG(" %s Received %d bytes from MIHF\n", __FUNCTION__, nb_bytes_received);
        mRALlte_print_buffer((char*)g_msg_codec_recv_buffer, nb_bytes_received);
        total_bytes_to_decode += nb_bytes_received;
        BitBuffer_wrap(bb, g_msg_codec_recv_buffer, total_bytes_to_decode);
        status  = mRALlte_mih_link_msg_decode(bb, &message_wrapper);
        nb_bytes_decoded = BitBuffer_getPosition(bb);
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
        } else if ((status == MIH_MESSAGE_DECODE_TOO_SHORT)) {
        }
        DEBUG(" \n");
    }
    free_BitBuffer(bb);
    return 0;
}
