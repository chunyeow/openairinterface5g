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

  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.

 *******************************************************************************/
/*! \file sgi_util.c
* \brief
* \author Lionel Gauthier
* \company Eurecom
* \email: lionel.gauthier@eurecom.fr
*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include "sgi.h"

#define FW_2_PRINT_BUFFER_LEN 10000
static char fw_2_print_buffer[FW_2_PRINT_BUFFER_LEN];
//-----------------------------------------------------------------------------
void sgi_print_hex_octets(unsigned char* dataP, unsigned long sizeP)
//-----------------------------------------------------------------------------
{
  unsigned long octet_index = 0;
  unsigned long buffer_marker = 0;
  unsigned char aindex;
  struct timeval tv;
  struct timezone tz;
  char timeofday[64];
  unsigned int h,m,s;

  if (dataP == NULL) {
    return;
  }

  gettimeofday(&tv, &tz);
  h = (tv.tv_sec/3600) % 24;
  m = (tv.tv_sec / 60) % 60;
  s = tv.tv_sec % 60;
  snprintf(timeofday, 64, "%02d:%02d:%02d.%06d", h,m,s,tv.tv_usec);

  SGI_IF_DEBUG("%s------+-------------------------------------------------+\n",timeofday);
  SGI_IF_DEBUG("%s      |  0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f |\n",timeofday);
  SGI_IF_DEBUG("%s------+-------------------------------------------------+\n",timeofday);
  for (octet_index = 0; octet_index < sizeP; octet_index++) {
    if ((octet_index % 16) == 0){
      if (octet_index != 0) {
          buffer_marker+=snprintf(&fw_2_print_buffer[buffer_marker], FW_2_PRINT_BUFFER_LEN - buffer_marker, " |\n");
          SGI_IF_DEBUG("%s%s",timeofday, fw_2_print_buffer);
          buffer_marker = 0;
      }
      buffer_marker+=snprintf(&fw_2_print_buffer[buffer_marker], FW_2_PRINT_BUFFER_LEN - buffer_marker, " %04ld |", octet_index);
    }
    /*
     * Print every single octet in hexadecimal form
     */
    buffer_marker+=snprintf(&fw_2_print_buffer[buffer_marker], FW_2_PRINT_BUFFER_LEN - buffer_marker, " %02x", dataP[octet_index]);
    /*
     * Align newline and pipes according to the octets in groups of 2
     */
  }

  /*
   * Append enough spaces and put final pipe
   */
  for (aindex = octet_index; aindex < 16; ++aindex)
    buffer_marker+=snprintf(&fw_2_print_buffer[buffer_marker], FW_2_PRINT_BUFFER_LEN - buffer_marker, "   ");
    //SGI_IF_DEBUG("   ");
  buffer_marker+=snprintf(&fw_2_print_buffer[buffer_marker], FW_2_PRINT_BUFFER_LEN - buffer_marker, " |\n");
  SGI_IF_DEBUG("%s%s",timeofday,fw_2_print_buffer);
}

//-----------------------------------------------------------------------------
char* sgi_status_2_str(SGIStatus_t statusP)
//-----------------------------------------------------------------------------
{
	switch (statusP) {
	    case SGI_STATUS_OK:                           return "SGI_STATUS_OK";break;
	    case SGI_STATUS_ERROR_CONTEXT_ALREADY_EXIST:  return "SGI_STATUS_ERROR_CONTEXT_ALREADY_EXIST";break;
	    case SGI_STATUS_ERROR_CONTEXT_NOT_FOUND:      return "SGI_STATUS_ERROR_CONTEXT_NOT_FOUND";break;
	    case SGI_STATUS_ERROR_INVALID_MESSAGE_FORMAT: return "SGI_STATUS_ERROR_INVALID_MESSAGE_FORMAT";break;
	    case SGI_STATUS_ERROR_SERVICE_NOT_SUPPORTED:  return "SGI_STATUS_ERROR_SERVICE_NOT_SUPPORTED";break;
	    case SGI_STATUS_ERROR_SYSTEM_FAILURE:         return "SGI_STATUS_ERROR_SYSTEM_FAILURE";break;
	    case SGI_STATUS_ERROR_NO_RESOURCES_AVAILABLE: return "SGI_STATUS_ERROR_NO_RESOURCES_AVAILABLE";break;
	    case SGI_STATUS_ERROR_NO_MEMORY_AVAILABLE:    return "SGI_STATUS_ERROR_NO_MEMORY_AVAILABLE";break;
	    default: return "UNKNOWN_SGI_STATUS";
	}
}

//-----------------------------------------------------------------------------
char* sgi_arpopcode_2_str(unsigned short int opcodeP)
//-----------------------------------------------------------------------------
{
	switch (opcodeP) {
	    case ARPOP_REQUEST:    return "ARPOP_REQUEST";break;
	    case ARPOP_REPLY:      return "ARPOP_REPLY";break;
	    case ARPOP_RREQUEST:   return "ARPOP_RREQUEST";break;
	    case ARPOP_RREPLY:     return "ARPOP_RREPLY";break;
	    case ARPOP_InREQUEST:  return "ARPOP_InREQUEST";break;
	    case ARPOP_InREPLY:    return "ARPOP_InREPLY";break;
	    case ARPOP_NAK:        return "ARPOP_NAK";break;
	    default: return "UNKNOWN_ARP_OPCODE";
	}
}
//-----------------------------------------------------------------------------
unsigned short in_cksum(unsigned short *addr, int len)
//-----------------------------------------------------------------------------
{
    register int sum = 0;
    u_short answer = 0;
    register u_short *w = addr;
    register int nleft = len;
    /*
     * Our algorithm is simple, using a 32 bit accumulator (sum), we add
     * sequential 16 bit words to it, and at the end, fold back all the
     * carry bits from the top 16 bits into the lower 16 bits.
     */
    while (nleft > 1)
    {
      sum += *w++;
      nleft -= 2;
    }
    /* mop up an odd byte, if necessary */
    if (nleft == 1)
    {
      *(u_char *) (&answer) = *(u_char *) w;
      sum += answer;
    }
    /* add back carry outs from top 16 bits to low 16 bits */
    sum = (sum >> 16) + (sum & 0xffff);       /* add hi 16 to low 16 */
    sum += (sum >> 16);               /* add carry */
    answer = ~sum;              /* truncate to 16 bits */
    return (answer);
}
//-----------------------------------------------------------------------------
void sgi_send_arp_request(sgi_data_t *sgi_dataP, char* dst_ip_addrP)
//-----------------------------------------------------------------------------
{
    char                 buffer[100]; // ARP request is 60 bytes including ethernet
    struct arphdr_s    *arph   = (struct arphdr_s *)(buffer);
    struct hostent      *hp     = NULL;
    uint32_t             i;
    struct in_addr       ip_dst;
    struct ifreq         ifr;
    struct iovec         iov[2];

    sgi_dataP->eh.ether_dhost[0] = (u_int8_t)(0xFF);
    sgi_dataP->eh.ether_dhost[1] = (u_int8_t)(0xFF);
    sgi_dataP->eh.ether_dhost[2] = (u_int8_t)(0xFF);
    sgi_dataP->eh.ether_dhost[3] = (u_int8_t)(0xFF);
    sgi_dataP->eh.ether_dhost[4] = (u_int8_t)(0xFF);
    sgi_dataP->eh.ether_dhost[5] = (u_int8_t)(0xFF);

    // get IP address of the default virtual interface
    ifr.ifr_addr.sa_family = AF_INET;
    sprintf(ifr.ifr_name, "%s.%d",sgi_dataP->interface_name,SGI_MIN_EPS_BEARER_ID);
    if (ioctl(sgi_dataP->sd[0], SIOCGIFADDR, &ifr) != 0) {
        SGI_IF_ERROR("Error during IP ADDRESS RETRIEVAL ON SGI INTERFACE (%s:%d)\n",strerror(errno), errno);
        exit (-1);
    }

    if ((hp = gethostbyname(dst_ip_addrP)) == NULL)
    {
        SGI_IF_DEBUG("gethostbyname() is OK.\n");
        if ((ip_dst.s_addr = inet_addr(dst_ip_addrP)) == -1)
        {
        	SGI_IF_ERROR("%s: Can't resolve, unknown host.\n", dst_ip_addrP);
            exit(1);
        }
    }
    else
        bcopy(hp->h_addr_list[0], &ip_dst.s_addr, hp->h_length);

    arph->ar_hrd = htons(ARPHRD_ETHER);
    arph->ar_pro = htons(ETH_P_IP);
    arph->ar_hln = ETH_ALEN;
    arph->ar_pln = 4;
    arph->ar_op  = htons(ARPOP_REQUEST);

    // sources
    memcpy(arph->ar_sha, sgi_dataP->eh.ether_shost, ETH_ALEN);

    i = ((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr.s_addr;
    arph->ar_sip[3] = i >> 24;
    arph->ar_sip[2] = (i & 0x00FF0000) >> 16;
    arph->ar_sip[1] = (i & 0x0000FF00) >> 8;
    arph->ar_sip[0] = (i & 0x000000FF);

    // destinations
    memset(arph->ar_tha, 0, ETH_ALEN);

    i = ip_dst.s_addr;
    arph->ar_tip[3] = i >> 24;
    arph->ar_tip[2] = (i & 0x00FF0000) >> 16;
    arph->ar_tip[1] = (i & 0x0000FF00) >> 8;
    arph->ar_tip[0] = (i & 0x000000FF);
    // save ip address of router for comparing with ARP REPLY
    sgi_dataP->ipv4_addr_of_router = ip_dst.s_addr;

    iov[0].iov_base   = &sgi_dataP->eh;
    iov[0].iov_len    = sizeof(sgi_dataP->eh);
    iov[1].iov_base   = (void *)buffer;
    iov[1].iov_len    = sizeof (struct arphdr) + ETH_ALEN*2 + 4*2;
#ifdef VLAN8021Q
    sgi_data_p->eh.ether_vlan8021q.vlan_tpid = htons(0x8100);
    sgi_dataP->eh.ether_vlan8021q.vlan_tci  = htons(0x7000 | 0x0000 | SGI_MIN_EPS_BEARER_ID);
#endif
    sgi_dataP->eh.ether_type     = htons(ETH_P_ARP);

    sgi_print_hex_octets(iov[0].iov_base, iov[0].iov_len);
    sgi_print_hex_octets(iov[1].iov_base, iov[1].iov_len);
    if (writev(sgi_dataP->sd[0], (const struct iovec *)&iov, 2) < 0) {
        SGI_IF_ERROR("Error during send ARP to socket %d bearer id %d : (%s:%d)\n",
        		sgi_dataP->sd[0],
        		SGI_MIN_EPS_BEARER_ID,
        		strerror(errno),
        		errno);
        exit(1);
    }

}

