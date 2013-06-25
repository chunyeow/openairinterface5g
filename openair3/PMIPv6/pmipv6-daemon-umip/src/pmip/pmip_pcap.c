/*
 * This file is part of the PMIP, Proxy Mobile IPv6 for Linux.
 *
 * Authors: OPENAIR3 <openair_tech@eurecom.fr>
 *
 * Copyright 2010-2011 EURECOM (Sophia-Antipolis, FRANCE)
 * 
 * Proxy Mobile IPv6 (or PMIPv6, or PMIP) is a network-based mobility 
 * management protocol standardized by IETF. It is a protocol for building 
 * a common and access technology independent of mobile core networks, 
 * accommodating various access technologies such as WiMAX, 3GPP, 3GPP2 
 * and WLAN based access architectures. Proxy Mobile IPv6 is the only 
 * network-based mobility management protocol standardized by IETF.
 * 
 * PMIP Proxy Mobile IPv6 for Linux has been built above MIPL free software;
 * which it involves that it is under the same terms of GNU General Public
 * License version 2. See MIPL terms condition if you need more details. 
 */
/*! \file pmip_pcap.c
* \brief Analyse of captured packets
* \author OpenAir3 Group
* \date 12th of October 2010
* \version 1.0
* \company Eurecom
* \project OpenAirInterface
* \email: openair_tech@eurecom.fr
*/
#define PMIP
#define PMIP_PCAP_C
#ifdef HAVE_CONFIG_H
#       include <config.h>
#endif
#include <string.h>
#include <ctype.h>
//---------------------------------------------------------------------------------------------------------------------
#include "pmip_fsm.h"
#include "pmip_hnp_cache.h"
#include "pmip_pcap.h"
#include "pmip_msgs.h"
#include "pmip_mag_proc.h"
//---------------------------------------------------------------------------------------------------------------------
#ifdef ENABLE_VT
#    include "vt.h"
#endif
#include "debug.h"
#include "conf.h"

static pthread_t pcap_listener;

//---------------------------------------------------------------------------------------------------------------------
/*
 * print data in rows of 16 bytes: offset   hex   ascii
 *
 * 00000   47 45 54 20 2f 20 48 54  54 50 2f 31 2e 31 0d 0a   GET / HTTP/1.1..
 */
void
pmip_pcap_print_hex_ascii_line(const u_char *payload, int len, int offset)
{

    int i;
    int gap;
    const u_char *ch;

    /* offset */
    printf("%05d   ", offset);

    /* hex */
    ch = payload;
    for(i = 0; i < len; i++) {
        printf("%02x ", *ch);
        ch++;
        /* print extra space after 8th byte for visual aid */
        if (i == 7)
            printf(" ");
    }
    /* print space to handle line less than 8 bytes */
    if (len < 8)
        printf(" ");

    /* fill hex gap with spaces if not full line */
    if (len < 16) {
        gap = 16 - len;
        for (i = 0; i < gap; i++) {
            printf("   ");
        }
    }
    printf("   ");

    /* ascii (if printable) */
    ch = payload;
    for(i = 0; i < len; i++) {
        if (isprint(*ch))
            printf("%c", *ch);
        else
            printf(".");
        ch++;
    }
    printf("\n");
}
//---------------------------------------------------------------------------------------------------------------------
/*
 * print packet payload data (avoid printing binary data)
 */
void
pmip_pcap_print_payload(const u_char *payload, int len)
{

    int len_rem = len;
    int line_width = 16;                        /* number of bytes per line */
    int line_len;
    int offset = 0;                                     /* zero-based offset counter */
    const u_char *ch = payload;

    if (len <= 0)
        return;

    /* data fits on one line */
    if (len <= line_width) {
        pmip_pcap_print_hex_ascii_line(ch, len, offset);
        return;
    }

    /* data spans multiple lines */
    for ( ;; ) {
        /* compute current line length */
        line_len = line_width % len_rem;
        /* print line */
        pmip_pcap_print_hex_ascii_line(ch, line_len, offset);
        /* compute total remaining */
        len_rem = len_rem - line_len;
        /* shift pointer to remaining bytes to print */
        ch = ch + line_len;
        /* add offset */
        offset = offset + line_width;
        /* check if we have line width chars or less */
        if (len_rem <= line_width) {
            /* print last line and get out */
            pmip_pcap_print_hex_ascii_line(ch, len_rem, offset);
            break;
        }
    }
}
//---------------------------------------------------------------------------------------------------------------------

/*!
*  Construct message event and send it to the MAG FSM
* \param
*/
void pmip_pcap_msg_handler_associate(struct in6_addr mn_iidP, int iifP)
{
    dbg("pmip_pcap_msg_handler_associate()\n");
    msg_info_t msg;
    memset(&msg, 0, sizeof(msg_info_t));
    msg.mn_iid = EUI48_to_EUI64(mn_iidP);
    msg.iif = iifP;
    msg.msg_event = hasWLCCP;
    mag_fsm(&msg);
}
//---------------------------------------------------------------------------------------------------------------------
void pmip_pcap_msg_handler_deassociate(struct in6_addr mn_iidP, int iifP)
{
    dbg("pmip_pcap_msg_handler_desassociate()\n");
    msg_info_t msg;
    memset(&msg, 0, sizeof(msg_info_t));
    msg.mn_iid = EUI48_to_EUI64(mn_iidP);
    msg.iif = iifP;
    msg.msg_event = hasDEREG;
    mag_fsm(&msg);
}
//---------------------------------------------------------------------------------------------------------------------
int pmip_pcap_loop_start(void)
{
    dbg("Getting ingress informations\n");
    mag_get_ingress_info(&g_pcap_iif, g_pcap_devname);

    if (pthread_create(&pcap_listener, NULL, pmip_pcap_loop, NULL))
                return -1;
    return 0;
}
//---------------------------------------------------------------------------------------------------------------------
void pmip_pcap_loop_stop(void)
{
#undef HAVE_PCAP_BREAKLOOP
//#define HAVE_PCAP_BREAKLOOP
#ifdef HAVE_PCAP_BREAKLOOP
    /*
    * We have "pcap_breakloop()"; use it, so that we do as little
    * as possible in the signal handler (it's probably not safe
    * to do anything with standard I/O streams in a signal handler -
    * the ANSI C standard doesn't say it is).
    */
    pcap_breakloop(pcap_descr);
#endif

    pthread_cancel(pcap_listener);
    pthread_join(pcap_listener, NULL);
}
//---------------------------------------------------------------------------------------------------------------------
void* pmip_pcap_loop(__attribute__ ((unused)) void *arg)
{
    bpf_u_int32 netaddr = 0, mask = 0;  // To store network address and netmask
    struct bpf_program filter;  // Place to store the BPF filter program
    char errbuf[PCAP_ERRBUF_SIZE];  // Error buffer
    struct pcap_pkthdr pkthdr;  // Packet information (timestamp, size...)
    const unsigned char *packet = NULL; // Received raw data
    struct in6_addr mn_iid;
    char filter_string[] = "udp port 514 or ip6 proto 58";
    char addrmac[16];
    char addrmacByte[16];
    char *p;
    int b, j, count;

    dbg("[PCAP] Device is %s and iif is %d\n", g_pcap_devname, g_pcap_iif);

    pcap_descr = NULL;
    memset(errbuf, 0, PCAP_ERRBUF_SIZE);
    memset(&mn_iid, 0, sizeof(struct in6_addr));
    pcap_descr = pcap_open_live(g_pcap_devname, PCAPMAXBYTES2CAPTURE, 0, PCAPTIMEDELAYKERNEL2USERSPACE, errbuf);
    if (pcap_descr == NULL)
        dbg("Error %s", errbuf);
    else if (*errbuf)
        dbg("Warning %s\n", errbuf);

// Look up info from the capture device
    if (pcap_lookupnet(g_pcap_devname, &netaddr, &mask, errbuf) == -1) {
        dbg("Can't get netmask for device %s\n", g_pcap_devname);
        netaddr = 0;
        mask = 0;
    }

// Compiles the filter expression into a BPF filter program
    if (pcap_compile(pcap_descr, &filter, filter_string, 0, mask) == -1) {
        dbg("Couldn't parse filter : %s\n", pcap_geterr(pcap_descr));
   }
// Load the filter program into the packet capture device
    if (pcap_setfilter(pcap_descr, &filter) == -1) {
        dbg("Couldn't install filter: %s\n", pcap_geterr(pcap_descr));
    }

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    while (1) {
        packet = pcap_next(pcap_descr, &pkthdr);    // Get one packet

        if ( ( pkthdr.len > 0 ) && ( packet != NULL ) ){
            //pmip_pcap_print_payload(packet, pkthdr.len);
            if (packet[54] == 0x85) { // case ROUTER SOLICITATION WITH ETHERNET HEADER
                dbg("[PCAP] RECEIVED PACKET SIZE: %d bytes caplen %d bytes\n", pkthdr.len, pkthdr.caplen);
                memset(&mn_iid, 0, sizeof(struct in6_addr));
                for (b=0; b< 8; b++) {
                    mn_iid.s6_addr[b+8] = packet[b+30];
                }
                mn_iid = EUI64_to_EUI48(mn_iid);
                dbg("[PCAP] RECEIVED ROUTER SOLLICITATION OVER ETHERNET MAC ADDR %x:%x:%x:%x:%x:%x:%x:%x dev %s iif %d\n", NIP6ADDR(&mn_iid), g_pcap_devname, g_pcap_iif);
                pmip_pcap_msg_handler_associate(mn_iid, g_pcap_iif);
            } else if (packet[56] == 0x85) {// case ROUTER SOLICITATION WITHOUT ETHERNET HEADER
                dbg("[PCAP] RECEIVED PACKET SIZE: %d bytes caplen %d bytes\n", pkthdr.len, pkthdr.caplen);
                memset(&mn_iid, 0, sizeof(struct in6_addr));
                for (b=0; b< 8; b++) {
                    mn_iid.s6_addr[b+8] = packet[b+32];
                }
                mn_iid = EUI64_to_EUI48(mn_iid);
                dbg("[PCAP] RECEIVED ROUTER SOLLICITATION OVER LTE MAC ADDR %x:%x:%x:%x:%x:%x:%x:%x dev %s iif %d\n", NIP6ADDR(&mn_iid), g_pcap_devname, g_pcap_iif);
                pmip_pcap_msg_handler_associate(mn_iid, g_pcap_iif);
            // approx 50 bytes for MAC + IP + UDP headers
            } else if ((p = strstr((const char *)(&packet[PCAP_CAPTURE_SYSLOG_MESSAGE_FRAME_OFFSET]), conf.PcapSyslogAssociationGrepString)) != NULL) {
                dbg("[PCAP] RECEIVED SYSLOG PACKET SIZE: %d bytes caplen %d bytes\n", pkthdr.len, pkthdr.caplen);
                p = p + strlen(conf.PcapSyslogAssociationGrepString);
                count = 0;
                j = 0;
                while ((count < 12) && (j < 32)) {
                    if (isxdigit(p[j])) {
                        addrmac[count++] = p[j] ;
                    }
                    j++;
                }
                addrmac[count] = '\0';
                dbg ("Before conversion : %s \n", addrmac);

                // convert ascii hex string to bin
                for (b = 0 ; b < 6; b++) {
                    strncpy(addrmacByte, addrmac+2*b, 2);
                    addrmacByte[2] = '\0';
                    // we can do (unsigned int*) because x86 processors are little-endian
                    // (done for suppressing a warning)
                    sscanf ((const char*)addrmacByte, "%02x", (unsigned int*)&mn_iid.s6_addr[10 + b]);
                }
                pmip_pcap_msg_handler_associate(mn_iid, g_pcap_iif);

            }  else if ((p = strstr((const char *)(&packet[PCAP_CAPTURE_SYSLOG_MESSAGE_FRAME_OFFSET]), conf.PcapSyslogDeAssociationGrepString)) != NULL) {
                dbg("[PCAP] RECEIVED PACKET SIZE: %d bytes caplen %d bytes\n", pkthdr.len, pkthdr.caplen);
                p = p + strlen(conf.PcapSyslogDeAssociationGrepString);
                count = 0;
                j = 0;
                while ((count < 12) && (j < 32)) {
                    if (isxdigit(p[j])) {
                        addrmac[count++] = p[j] ;
                    }
                    j++;
                }
                addrmac[count] = '\0';
                dbg ("Before conversion : %s \n", addrmac);

                // convert ascii hex string to bin
                for (b = 0 ; b < 6; b++) {
                    strncpy(addrmacByte, addrmac+2*b, 2);
                    addrmacByte[2] = '\0';
                    // we can do (unsigned int*) because x86 processors are little-endian
                    // (done for suppressing a warning)
                    sscanf ((const char*)addrmacByte, "%02x", (unsigned int*)&mn_iid.s6_addr[10 + b]);
                }
                pmip_pcap_msg_handler_deassociate(mn_iid, g_pcap_iif);
            }
        }
    }
    pthread_exit(NULL);
}

