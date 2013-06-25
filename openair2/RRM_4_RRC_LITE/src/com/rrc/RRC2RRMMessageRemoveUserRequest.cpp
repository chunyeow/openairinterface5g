#include <iostream>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <stdio.h>
//----------------------------------------------------------------------------
#include "RRC2RRMMessageRemoveUserRequest.h"
#include "RRM2RRCMessageRemoveUserResponse.h"
#include "RRCMessageHandler.h"
#include "RadioResources.h"
#include "Exceptions.h"

//----------------------------------------------------------------------------
RRC2RRMMessageRemoveUserRequest::RRC2RRMMessageRemoveUserRequest(std::string ip_dest_strP, int port_destP,
                                                                                    cell_id_t cell_idP,
                                                                                     mobile_id_t mobile_idP,
                                                                                     transaction_id_t transaction_idP)
//----------------------------------------------------------------------------
{
    Message();
    m_is_ipv6 = Utility::isipv6(ip_dest_strP);
    m_cell_id                     = cell_idP;
    m_mobile_id                   = mobile_idP;
    m_transaction_id              = transaction_idP;
}
//----------------------------------------------------------------------------
RRC2RRMMessageRemoveUserRequest::RRC2RRMMessageRemoveUserRequest(tcpip::Storage& in_messageP, msg_length_t msg_lengthP, frame_t msg_frameP, struct sockaddr *sa_fromP, socklen_t sa_lenP):
Message(in_messageP)
//----------------------------------------------------------------------------
{
    ParseIpParameters(sa_fromP, sa_lenP);
    m_cell_id        = m_message_storage.readChar();
    m_mobile_id      = m_message_storage.readChar();
    m_transaction_id = m_message_storage.readChar();
    printf("----------------------------------------------------------------------------------------------------------\n");
    printf("RRC\t-------REMOVE USER REQUEST------->\tRRM\n");
    printf("----------------------------------------------------------------------------------------------------------\n");
    printf("cell id = %d mobile id = %d transaction id = %d \n", m_cell_id, m_mobile_id, m_transaction_id);
}
//----------------------------------------------------------------------------
void RRC2RRMMessageRemoveUserRequest::Forward()
//----------------------------------------------------------------------------
{
    try {
        Transaction* tx = RadioResources::Instance()->Request(*this);
        RRM2RRCMessageRemoveUserResponse  response(Message::STATUS_REMOVE_USER_SUCCESSFULL, Message::MSG_RESP_OK, m_cell_id, m_mobile_id, m_transaction_id, tx);
        RRCMessageHandler::Instance()->Send2Peer(this->m_ip_str_src, this->m_port_src, response.GetSerializedMessageBuffer(), response.GetSerializedMessageSize());
    } catch (mobile_already_connected_error & x ) {
        RRM2RRCMessageRemoveUserResponse  response(Message::STATUS_REMOVE_USER_FAILED, Message::MSG_RESP_PROTOCOL_ERROR, m_cell_id, m_mobile_id, m_transaction_id);
        RRCMessageHandler::Instance()->Send2Peer(this->m_ip_str_src, this->m_port_src, response.GetSerializedMessageBuffer(), response.GetSerializedMessageSize());
    } catch (std::exception const& e ) {
        RRM2RRCMessageRemoveUserResponse  response(Message::STATUS_REMOVE_USER_FAILED, Message::MSG_RESP_INTERNAL_ERROR, m_cell_id, m_mobile_id, m_transaction_id);
        RRCMessageHandler::Instance()->Send2Peer(this->m_ip_str_src, this->m_port_src, response.GetSerializedMessageBuffer(), response.GetSerializedMessageSize());
    }
}
//----------------------------------------------------------------------------
void RRC2RRMMessageRemoveUserRequest::Serialize()
//----------------------------------------------------------------------------
{
    m_message_storage.reset();
    m_message_storage.writeChar(Message::MESSAGE_REMOVE_USER_REQUEST);
    m_message_storage.writeShort(6u);
    m_message_storage.writeChar(m_cell_id);
    m_message_storage.writeChar(m_mobile_id);
    m_message_storage.writeChar(m_transaction_id);
}
//----------------------------------------------------------------------------
RRC2RRMMessageRemoveUserRequest::~RRC2RRMMessageRemoveUserRequest()
//----------------------------------------------------------------------------
{
}

