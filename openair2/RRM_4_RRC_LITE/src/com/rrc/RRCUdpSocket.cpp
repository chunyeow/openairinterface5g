#include <stdio.h>

#include "RRCUdpSocket.h"
#include "RRCMessageHandler.h"


RRCUdpSocket::RRCUdpSocket(ISocketHandler& h,int ibufsz, bool ipv6)
:UdpSocket(h, ibufsz, ipv6)
{
}


void RRCUdpSocket::OnRawData(const char *in_bufferP,size_t size_dataP,struct sockaddr *sa_fromP,socklen_t sa_lenP)
{
	RRCMessageHandler::Instance()->NotifyRxData(in_bufferP, size_dataP, sa_fromP, sa_lenP);
}
