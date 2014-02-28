/***************************************************************************
                          nas_ue_ioctl.h  -  description
                             -------------------
    copyright            : (C) 2005 by Eurecom
    email                : michelle.wetterwald@eurecom.fr
                           yan.moret@eurecom.fr
 ***************************************************************************

 ***************************************************************************/
#ifndef _IAL_IOCTL_H
#define _IAL_IOCTL_H

#include <sys/ioctl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <net/if.h>
//LG#include <linux/ipv6.h>
//LG#include <linux/in.h>
//LG#include <linux/in6.h>
//#include <linux/netdevice.h>

//#include <graal_constant.h>
//#define GRAAL_STATE_IDLE 			0
//#define GRAAL_STATE_CONNECTED			1
//#define GRAAL_STATE_ESTABLISHMENT_REQUEST	2
//#define GRAAL_STATE_ESTABLISHMENT_FAILURE	3
//#define GRAAL_STATE_RELEASE_FAILURE		4

//typedef unsigned char uint8_t;
//typedef unsigned short uint16_t;
//typedef unsigned int uint32_t;

// Temp constants = arguments gioctl
//arg[0]
#define IO_OBJ_STATS 0
#define IO_OBJ_CNX   1
#define IO_OBJ_RB    2
#define IO_OBJ_MEAS  3
#define IO_OBJ_IMEI  4
//arg[1]
#define IO_CMD_ADD   0
#define IO_CMD_DEL   1
#define IO_CMD_LIST  2



//LGextern int inet_pton(int af, const char *src, void *dst);
//LGextern char *inet_ntop(int af, const void *src, char *dst, size_t sise);

#endif
