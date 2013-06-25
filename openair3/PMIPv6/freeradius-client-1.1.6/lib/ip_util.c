/*
 * $Id: ip_util.c,v 1.13 2007/07/11 17:29:29 cparker Exp $
 *
 * Copyright (C) 1995,1996,1997 Lars Fenneberg
 *
 * Copyright 1992 Livingston Enterprises, Inc.
 *
 * Copyright 1992,1993, 1994,1995 The Regents of the University of Michigan
 * and Merit Network, Inc. All Rights Reserved
 *
 * See the file COPYRIGHT for the respective terms and conditions.
 * If the file is missing contact me at lf@elemental.net
 * and I'll send you a copy.
 *
 */

#include <config.h>
#include <includes.h>
#include <freeradius-client.h>

#if !defined(SA_LEN)
#define SA_LEN(sa) \
  (((sa)->sa_family == AF_INET) ? \
    sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6))
#endif

/*
 * Function: rc_gethostbyname
 *
 * Purpose: threadsafe replacement for gethostbyname.
 *
 * Returns: NULL on failure, hostent pointer on success
 */

struct hostent *rc_gethostbyname(const char *hostname)
{
	struct hostent *hp;
	hp = gethostbyname2(hostname, AF_INET6);
	return hp;
}


/*
 * Function: rc_get_ipaddr
 *
 * Purpose: return an IP address in host long notation from a host
 *          name or address in dot notation.
 *
 * Returns: 0 on failure
 */

int rc_get_ipaddr (char *host, struct in6_addr* rval)
{

	struct 	hostent *hp;
	unsigned char buf[sizeof(struct in6_addr)];

	if (rc_good_ipaddr (host) == 0)
	{
		if ( inet_pton(AF_INET6, host, rval) <= 0) {
			rc_log(LOG_ERR,"rc_get_ipaddr: error inet_pton(%s)", host);
			return 0;
		}
	    //rc_log(LOG_NOTICE,"rc_get_ipaddr (inet_pton) %s->%x:%x:%x:%x:%x:%x:%x:%x\n", host, NIP6ADDR(rval));
		return 1;
	}

	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int             s, sfd;
	*rval = in6addr_any;


	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET6;	// Allow IPv4 or IPv6
	hints.ai_socktype = 0;			// SOCK_DGRAM; Datagram socket
	hints.ai_flags = 0;
	hints.ai_protocol = 0;          // Any protocol

	s = getaddrinfo(host, NULL, &hints, &result);
	if (s != 0) {
		rc_log(LOG_ERR,"rc_get_ipaddr(%s) getaddrinfo: %s", host, gai_strerror(s));
		return 0;
	}

	// getaddrinfo() returns a list of address structures.
	//Try each address until we successfully connect(2).
	//If socket(2) (or connect(2)) fails, we (close the socket
	//and) try the next address.

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		if (rp->ai_canonname) {
			;//rc_log(LOG_NOTICE,"rc_get_ipaddr(%s) getaddrinfo() Canonical name:\n%s\nAdresses:\n", host, rp->ai_canonname);
		}
		if (rp->ai_family == AF_INET6) {
			char dst[INET6_ADDRSTRLEN];


			sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
			if (sfd == -1) {
				continue;
			}
			memcpy(rval, &((struct sockaddr_in6 *)rp->ai_addr)->sin6_addr, sizeof(struct in6_addr));

            //rc_log(LOG_NOTICE,"rc_get_ipaddr(%s) try connect to address : %x:%x:%x:%x:%x:%x:%x:%x socktype %d protocol %d\n",
                   //host, NIP6ADDR(rval), rp->ai_socktype, rp->ai_protocol);
			if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {

				//memcpy( rval, rp->ai_addr, sizeof(struct in6_addr));
				//rc_log(LOG_NOTICE,"rc_get_ipaddr(%s) success : %x:%x:%x:%x:%x:%x:%x:%x\n", host, NIP6ADDR(rval));
				if (!inet_ntop(AF_INET6, (const void *) rval, dst, sizeof(dst))) {
					rc_log(LOG_ERR,"inet_ntop: %s\n", strerror(errno));
					break;
				}
				//rc_log(LOG_NOTICE,"rc_get_ipaddr() inet_ntop %s->%s\n", host, dst);
				close(sfd);
				break;                  // Success
			}
			close(sfd);
		}
	}

	if (rp == NULL) {               // No address succeeded
		rc_log(LOG_ERR,"rc_get_ipaddr(%s) Could not connect", host);
		return 0;
	}
	freeaddrinfo(result);           // No longer needed
	//rc_log(LOG_NOTICE,"rc_get_ipaddr (getaddrinfo) %s->%x:%x:%x:%x:%x:%x:%x:%x\t return 1", host, NIP6ADDR(rval));
	return 1;
}


/*
 * Function: rc_good_ipaddr
 *
 * Purpose: check for valid IP address in standard dot notation.
 *
 * Returns: 0 on success, -1 when failure
 *
 */

int rc_good_ipaddr (char *addr)
{
	int		qc = 0;
	int		qd = 0;
	int		str_index;
	char		ip_number_str[4];
	int		ip_number_index;
	size_t i;

	for (i = 0; i < strlen(addr); ++i)
	{
		qc += (addr[i] == ':') ? 1 : 0;
		qd += (addr[i] == '.') ? 1 : 0;
	}
	if (qc > 7)
	{
		//rc_log(LOG_ERR,"rc_good_ipaddr(%s): false @1", addr);
		return -1;
	}
	if (qd && qd != 3)
	{
		//rc_log(LOG_ERR,"rc_good_ipaddr(%s): false @2", addr);
		return -1;
	}
	if (qc == 0 && qd == 0)
	{
		//rc_log(LOG_ERR,"rc_good_ipaddr(%s): false @3", addr);
		return -1;
	}

	ip_number_index = 0;
	for ( str_index = 0 ; str_index < strlen(addr); str_index++) {
		if ((addr[str_index] != '.') && (addr[str_index] != ':')) {
			if (ip_number_index < 4) {
				ip_number_str[ip_number_index] = addr[str_index];
				ip_number_index = ip_number_index + 1;
			} else {
				//rc_log(LOG_ERR,"rc_good_ipaddr(%s): false @3", addr);
				return -1;
			}
		} else {
			if (ip_number_index != 0) {
				for (i = 0; i < ip_number_index; ++i)
				{
					if ((ip_number_str[i]) < '0' || (ip_number_str[i] > '9' && ip_number_str[i] < 'A') ||
						(ip_number_str[i] > 'F' && ip_number_str[i] < 'a') || ip_number_str[i] > 'f')
					{
						//rc_log(LOG_ERR,"rc_good_ipaddr(%s): false @4", addr);
						return -1;
					}
				}
			}
			ip_number_index = 0;
		}
	}
	//rc_log(LOG_ERR,"rc_good_ipaddr(%s): true", addr);
	return 0;
}

/*
 * Function: rc_ip_hostname
 *
 * Purpose: Return a printable host name (or IP address in dot notation)
 *	    for the supplied IP address.
 *
 */

const char *rc_ip_hostname (struct in6_addr* h_ipaddr)
{
/*
struct hostent  *hp;
uint32_t           n_ipaddr = htonl (h_ipaddr);

if ((hp = rc_gethostbyaddr ((char *) &n_ipaddr, sizeof (struct in_addr),
	AF_INET)) == NULL) {
		rc_log(LOG_ERR,"rc_ip_hostname: couldn't look up host by addr: %08lX", h_ipaddr);
}

return (hp == NULL) ? "unknown" : hp->h_name;
*/
	static char host[INET6_ADDRSTRLEN];
	if (inet_ntop(AF_INET6, h_ipaddr, host, INET6_ADDRSTRLEN) == NULL) {
		return "unknown";
	} else {
		return host;
	}

}

/*
 * Function: rc_getport
 *
 * Purpose: get the port number for the supplied request type
 *
 */

unsigned short rc_getport(int type)
{
	struct servent *svp;

	if ((svp = getservbyname ((type==AUTH)?"radius":"radacct", "udp")) == NULL)
	{
		return (type==AUTH) ? PW_AUTH_UDP_PORT : PW_ACCT_UDP_PORT;
	} else {
		return ntohs ((unsigned short) svp->s_port);
	}
}

/*
 * Function: rc_own_hostname
 *
 * Purpose: get the hostname of this machine
 *
 * Returns  -1 on failure, 0 on success
 *
 */

int
rc_own_hostname(char *hostname, int len)
{
#ifdef HAVE_UNAME
	struct	utsname uts;
#endif

#if defined(HAVE_UNAME)
	if (uname(&uts) < 0)
	{
		rc_log(LOG_ERR,"rc_own_hostname: couldn't get own hostname");
		return -1;
	}
	strncpy(hostname, uts.nodename, len);
#elif defined(HAVE_GETHOSTNAME)
	if (gethostname(hostname, len) < 0)
	{
		rc_log(LOG_ERR,"rc_own_hostname: couldn't get own hostname");
		return -1;
	}
#elif defined(HAVE_SYSINFO)
	if (sysinfo(SI_HOSTNAME, hostname, len) < 0)
	{
		rc_log(LOG_ERR,"rc_own_hostname: couldn't get own hostname");
		return -1;
	}
#else
	return -1;
#endif

	return 0;
}

/*
 * Function: rc_own_ipaddress
 *
 * Purpose: get the IP address of this host in host order
 *
 * Returns: IP address on success, 0 on failure
 *
 */

void rc_own_ipaddress(rc_handle *rh, struct in6_addr* rval)
{
	char hostname[256];
	*rval = in6addr_any;

	if (!IN6_ARE_ADDR_EQUAL(&rh->this_host_ipaddr, &in6addr_any)) {
		if (rc_conf_str(rh, "bindaddr") == NULL ||
		    strcmp(rc_conf_str(rh, "bindaddr"), "*") == 0) {
			if (rc_own_hostname(hostname, sizeof(hostname)) < 0)
				//rc_log(LOG_NOTICE,"rc_own_ipaddress %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(rval));
				return ;
		} else {
			strncpy(hostname, rc_conf_str(rh, "bindaddr"), sizeof(hostname));
			hostname[sizeof(hostname) - 1] = '\0';
		}
		if (rc_get_ipaddr (hostname, &rh->this_host_ipaddr) == 0) {
			rc_log(LOG_ERR, "rc_own_ipaddress: couldn't get own IP address");
			//rc_log(LOG_NOTICE,"rc_own_ipaddress %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(rval));
			return ;
		}
	}
	memcpy(rval, &rh->this_host_ipaddr, sizeof (struct in6_addr));
	//rc_log(LOG_NOTICE,"rc_own_ipaddress %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(rval));
}

/*
 * Function: rc_own_bind_ipaddress
 *
 * Purpose: get the IP address to be used as a source address
 *          for sending requests in host order
 *
 * Returns: IP address
 *
 */

void rc_own_bind_ipaddress(rc_handle *rh, struct in6_addr* rval)
{
	char hostname[256];

	if (rh->this_host_bind_ipaddr != NULL) {
		memcpy(rval, rh->this_host_bind_ipaddr, sizeof (struct in6_addr));
		//rc_log(LOG_NOTICE,"rc_own_bind_ipaddress %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(rval));
		return;
	}

	rh->this_host_bind_ipaddr = malloc(sizeof (struct in6_addr));
	if (rh->this_host_bind_ipaddr == NULL)
		rc_log(LOG_CRIT, "rc_own_bind_ipaddress: out of memory");
	if (rc_conf_str(rh, "bindaddr") == NULL ||
		strcmp(rc_conf_str(rh, "bindaddr"), "*") == 0) {
		*rval = in6addr_any;
		//rc_log(LOG_NOTICE,"rc_own_bind_ipaddress: in6addr_any\n");
	} else {
		strncpy(hostname, rc_conf_str(rh, "bindaddr"), sizeof(hostname));
		hostname[sizeof(hostname) - 1] = '\0';
		if (rc_get_ipaddr (hostname, rval) == 0) {
			rc_log(LOG_ERR, "rc_own_ipaddress: couldn't get IP address from bindaddr");
			*rval = in6addr_any;
			//rc_log(LOG_NOTICE,"rc_own_bind_ipaddress: in6addr_any\n");
		}
	}
	if (rh->this_host_bind_ipaddr != NULL) {
        memcpy(rh->this_host_bind_ipaddr, rval, sizeof (struct in6_addr));
        //rc_log(LOG_NOTICE,"rc_own_bind_ipaddress assign rh->this_host_bind_ipaddr to %x:%x:%x:%x:%x:%x:%x:%x\n", NIP6ADDR(rh->this_host_bind_ipaddr));
		//*rh->this_host_bind_ipaddr = *rval;
	}

}

/*
 * Function: rc_get_srcaddr
 *
 * Purpose: given remote address find local address which the
 *          system will use as a source address for sending
 *          datagrams to that remote address
 *
 * Returns: 0 in success, -1 on failure, address is filled into
 *          the first argument.
 *
 */
int
rc_get_srcaddr(struct sockaddr_in6 *lia, struct sockaddr_in6 *ria)
{
	int temp_sock;
	socklen_t namelen;

	temp_sock = socket(ria->sin6_family, SOCK_DGRAM, 0);
	if (temp_sock == -1) {
		rc_log(LOG_ERR, "rc_get_srcaddr: socket: %s", strerror(errno));
		return -1;
	}

	if (connect(temp_sock, (struct sockaddr*)ria, sizeof(struct sockaddr_in6)) != 0) {
		rc_log(LOG_ERR, "rc_get_srcaddr: connect: %s",
		    strerror(errno));
		close(temp_sock);
		return -1;
	}

	namelen = sizeof(struct sockaddr_in6);
	if (getsockname(temp_sock, (struct sockaddr*)lia, &namelen) != 0) {
		rc_log(LOG_ERR, "rc_get_srcaddr: getsockname: %s",
		    strerror(errno));
		close(temp_sock);
		return -1;
	}

	close(temp_sock);
	return 0;
}
