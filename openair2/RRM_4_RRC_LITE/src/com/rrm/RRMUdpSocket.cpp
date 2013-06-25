#include <stdio.h>
//----------------------------------------------------------------------------
#include "RRMUdpSocket.h"
#include "RRMMessageHandler.h"


//----------------------------------------------------------------------------
RRMUdpSocket::RRMUdpSocket(ISocketHandler& h,int ibufsz, bool ipv6):UdpSocket(h, ibufsz, ipv6)
//----------------------------------------------------------------------------
{
}
//----------------------------------------------------------------------------
void RRMUdpSocket::OnRawData(const char *in_bufferP,size_t size_dataP,struct sockaddr *sa_fromP,socklen_t sa_lenP)
//----------------------------------------------------------------------------
{
	RRMMessageHandler::Instance()->NotifyRxData(in_bufferP, size_dataP, sa_fromP, sa_lenP);
}
