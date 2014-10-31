#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>

int ethernet_socket_init(int Mod_id, char *dest_ip,int dest_port);

int ethernet_write_data(int Mod_id, const void *buff, int nsamps,int cc);

int ethernet_read_data(int Mod_id,void *buff, int nsamps,int cc);
