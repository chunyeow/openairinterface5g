#include <stdio.h>

#include "RRC2RRMMessageAddRadioAccessBearerConfirm.h"
#include "RRCMessageHandler.h"
#include "RadioResources.h"
#include "Exceptions.h"

//----------------------------------------------------------------------------
RRC2RRMMessageAddRadioAccessBearerConfirm::RRC2RRMMessageAddRadioAccessBearerConfirm(std::string ip_dest_strP, int port_destP,
                                                                                    cell_id_t cell_idP,
                                                                                     mobile_id_t mobile_idP,
                                                                                     rb_id_t            radio_bearer_idP,
                                                                                     transaction_id_t transaction_idP)
//----------------------------------------------------------------------------
{
    Message();
    m_is_ipv6 = Utility::isipv6(ip_dest_strP);
    m_cell_id                     = cell_idP;
    m_mobile_id                   = mobile_idP;
    m_radio_bearer_id             = radio_bearer_idP;
    m_transaction_id              = transaction_idP;
}
//----------------------------------------------------------------------------
RRC2RRMMessageAddRadioAccessBearerConfirm::RRC2RRMMessageAddRadioAccessBearerConfirm(tcpip::Storage& in_messageP, msg_length_t msg_lengthP, frame_t msg_frameP, struct sockaddr *sa_fromP, socklen_t sa_lenP):
Message(in_messageP)
//----------------------------------------------------------------------------
{
    ParseIpParameters(sa_fromP, sa_lenP);
/*    if (m_is_ipv6) {
        printf("[RRM][RRC2RRMMessageAddRadioAccessBearerConfirm] (IPv6) Received %d bytes from: %s:%d\n",  msg_lengthP, m_ip_char_src, m_port_src);
    } else {
        printf("[RRM][RRC2RRMMessageAddRadioAccessBearerConfirm] (IPv4) Received %d bytes from: %s:%d\n",  msg_lengthP, m_ip_char_src, m_port_src);
    }*/
    m_cell_id                     = m_message_storage.readChar();
    m_mobile_id                   = m_message_storage.readChar();
    m_radio_bearer_id             = m_message_storage.readChar();
    m_transaction_id              = m_message_storage.readChar();
    printf("----------------------------------------------------------------------------------------------------------\n");
    printf("RRC\t-------ADD DATA RADIO BEARER CONFIRM------->\tRRM\n");
    printf("----------------------------------------------------------------------------------------------------------\n");
    printf("cell id = %d mobile id = %d radio bearer id = %d transaction id = %d \n", m_cell_id, m_mobile_id, m_radio_bearer_id, m_transaction_id);
}
//----------------------------------------------------------------------------
void RRC2RRMMessageAddRadioAccessBearerConfirm::Forward()
//----------------------------------------------------------------------------
{
    try {
        Transaction* tx = RadioResources::Instance()->Request(*this);
        if (tx != NULL) {
            delete tx;
        }
    } catch (std::exception const& e ) {
    }
}
//----------------------------------------------------------------------------
void RRC2RRMMessageAddRadioAccessBearerConfirm::Serialize()
//----------------------------------------------------------------------------
{
    m_message_storage.reset();
    m_message_storage.writeChar(Message::MESSAGE_ADD_RADIO_ACCESS_BEARER_CONFIRM);
    m_message_storage.writeShort(7u);
    m_message_storage.writeChar(m_cell_id);
    m_message_storage.writeChar(m_mobile_id);
    m_message_storage.writeChar(m_radio_bearer_id);
    m_message_storage.writeChar(m_transaction_id);
}
//----------------------------------------------------------------------------
RRC2RRMMessageAddRadioAccessBearerConfirm::~RRC2RRMMessageAddRadioAccessBearerConfirm()
//----------------------------------------------------------------------------
{
}

