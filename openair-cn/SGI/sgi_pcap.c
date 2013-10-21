#include <string.h>
#include "sgi.h"


//---------------------------------------------------------------------------------------------------------------------
void* sgi_pcap_fw_2_gtpv1u_thread(void* args_p)
{
    bpf_u_int32                       netaddr = 0, mask = 0;  // To store network address and netmask
    struct bpf_program                filter;  // Place to store the BPF filter program
    char                              errbuf[PCAP_ERRBUF_SIZE];  // Error buffer
    struct pcap_pkthdr                pkthdr;  // Packet information (timestamp, size...)
    const unsigned char              *packet = NULL; // Received raw data
    char                              filter_string[] = "";
    sgi_data_t                       *sgi_data_p;

    sgi_data_p = (sgi_data_t*)args_p;

    SGI_IF_DEBUG("[PCAP] Device is %s and iif is %d\n", sgi_data_p->interface_name, sgi_data_p->interface_index);

    memset(errbuf, 0, PCAP_ERRBUF_SIZE);

    sgi_data_p->pcap_descr = pcap_open_live(sgi_data_p->interface_name, SGI_PCAPMAXBYTES2CAPTURE, 0, SGI_PCAPTIMEDELAYKERNEL2USERSPACE, errbuf);
    if (sgi_data_p->pcap_descr == NULL)
    	SGI_IF_ERROR("Error %s", errbuf);
    else if (*errbuf)
    	SGI_IF_WARNING("Warning %s\n", errbuf);

// Look up info from the capture device
    if (pcap_lookupnet(sgi_data_p->interface_name, &netaddr, &mask, errbuf) == -1) {
    	SGI_IF_ERROR("Can't get netmask for device %s\n", sgi_data_p->interface_name);
        netaddr = 0;
        mask = 0;
        pthread_exit(NULL);
    }

// Compiles the filter expression into a BPF filter program
    if (pcap_compile(sgi_data_p->pcap_descr, &filter, filter_string, 0, mask) == -1) {
    	SGI_IF_ERROR("Couldn't parse filter : %s\n", pcap_geterr(sgi_data_p->pcap_descr));
        pthread_exit(NULL);
   }
// Load the filter program into the packet capture device
    if (pcap_setfilter(sgi_data_p->pcap_descr, &filter) == -1) {
    	SGI_IF_ERROR("Couldn't install filter: %s\n", pcap_geterr(sgi_data_p->pcap_descr));
        pthread_exit(NULL);
    }

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    sgi_data_p->thread_started = 1;
    while (1) {
        packet = pcap_next(sgi_data_p->pcap_descr, &pkthdr);    // Get one packet

        if ( ( pkthdr.len > 0 ) && ( packet != NULL ) ){
        	sgi_print_hex_octets(packet, pkthdr.len);
            if (packet[54] == 0x85) { // case ROUTER SOLICITATION WITH ETHERNET HEADER
            }
        }
    }
    pthread_exit(NULL);
}

