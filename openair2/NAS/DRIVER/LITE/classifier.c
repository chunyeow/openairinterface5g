/*******************************************************************************

  Eurecom OpenAirInterface 2
  Copyright(c) 1999 - 2010 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fsr/openairinterface
  Address      : Eurecom, 2229, route des crÃªtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/

/*! \file classifier.c
* \brief Classify IP packets
* \author  yan moret(no longer valid),  Michelle Wetterwald, Raymond knopp, Navid Nikaein, Lionel GAUTHIER
* \company Eurecom
* \email:  michelle.wetterwald@eurecom.fr, knopp@eurecom.fr, navid.nikaein@eurecom.fr, lionel.gauthier@eurecom.fr
*/


#include "local.h"
#include "proto_extern.h"
#include <net/ip6_fib.h>
#include <net/route.h>
#ifdef MPLS
#include <net/mpls.h>
#endif


#define IN_CLASSA(a)            ((((long int) (a)) & 0x80000000) == 0)
#define IN_CLASSB(a)            ((((long int) (a)) & 0xc0000000) == 0x80000000)
#define IN_CLASSC(a)            ((((long int) (a)) & 0xe0000000) == 0xc0000000)
#define IN_CLASSD(a)            ((((long int) (a)) & 0xf0000000) == 0xe0000000)
#define IN_MULTICAST(a)         IN_CLASSD(a)
#define IN_EXPERIMENTAL(a)      ((((long int) (a)) & 0xf0000000) == 0xf0000000)
#define IN_BADCLASS(a)          IN_EXPERIMENTAL((a))
/* Address to accept any incoming messages. */
#define INADDR_ANY              ((unsigned long int) 0x00000000)
/* Address to send to all hosts. */
#define INADDR_BROADCAST        ((unsigned long int) 0xffffffff)
/* Address indicating an error return. */
#define INADDR_NONE             ((unsigned long int) 0xffffffff)


#define NIPADDR(addr) \
        (uint8_t)(addr & 0x000000FF), \
        (uint8_t)((addr & 0x0000FF00) >> 8), \
        (uint8_t)((addr & 0x00FF0000) >> 16), \
        (uint8_t)((addr & 0xFF000000) >> 24)

#define NIP6ADDR(addr) \
        ntohs((addr)->s6_addr16[0]), \
        ntohs((addr)->s6_addr16[1]), \
        ntohs((addr)->s6_addr16[2]), \
        ntohs((addr)->s6_addr16[3]), \
        ntohs((addr)->s6_addr16[4]), \
        ntohs((addr)->s6_addr16[5]), \
        ntohs((addr)->s6_addr16[6]), \
        ntohs((addr)->s6_addr16[7])


#define IN6_IS_ADDR_UNSPECIFIED(a) \
        (((__const uint32_t *) (a))[0] == 0                                   \
         && ((__const uint32_t *) (a))[1] == 0                                \
         && ((__const uint32_t *) (a))[2] == 0                                \
         && ((__const uint32_t *) (a))[3] == 0)

#define IN6_IS_ADDR_LOOPBACK(a) \
        (((__const uint32_t *) (a))[0] == 0                                   \
         && ((__const uint32_t *) (a))[1] == 0                                \
         && ((__const uint32_t *) (a))[2] == 0                                \
         && ((__const uint32_t *) (a))[3] == htonl (1))

#define IN6_IS_ADDR_MULTICAST(a) (((__const uint8_t *) (a))[0] == 0xff)

#define IN6_IS_ADDR_LINKLOCAL(a) \
        ((((__const uint32_t *) (a))[0] & htonl (0xffc00000))                 \
         == htonl (0xfe800000))

#define IN6_IS_ADDR_SITELOCAL(a) \
        ((((__const uint32_t *) (a))[0] & htonl (0xffc00000))                 \
         == htonl (0xfec00000))

#define IN6_IS_ADDR_V4MAPPED(a) \
        ((((__const uint32_t *) (a))[0] == 0)                                 \
         && (((__const uint32_t *) (a))[1] == 0)                              \
         && (((__const uint32_t *) (a))[2] == htonl (0xffff)))

#define IN6_IS_ADDR_V4COMPAT(a) \
        ((((__const uint32_t *) (a))[0] == 0)                                 \
         && (((__const uint32_t *) (a))[1] == 0)                              \
         && (((__const uint32_t *) (a))[2] == 0)                              \
         && (ntohl (((__const uint32_t *) (a))[3]) > 1))

#define IN6_ARE_ADDR_EQUAL(a,b) \
        ((((__const uint32_t *) (a))[0] == ((__const uint32_t *) (b))[0])     \
         && (((__const uint32_t *) (a))[1] == ((__const uint32_t *) (b))[1])  \
         && (((__const uint32_t *) (a))[2] == ((__const uint32_t *) (b))[2])  \
         && (((__const uint32_t *) (a))[3] == ((__const uint32_t *) (b))[3]))

#define IN6_IS_ADDR_MC_NODELOCAL(a) \
        (IN6_IS_ADDR_MULTICAST(a)                                             \
         && ((((__const uint8_t *) (a))[1] & 0xf) == 0x1))

#define IN6_IS_ADDR_MC_LINKLOCAL(a) \
        (IN6_IS_ADDR_MULTICAST(a)                                             \
         && ((((__const uint8_t *) (a))[1] & 0xf) == 0x2))

#define IN6_IS_ADDR_MC_SITELOCAL(a) \
        (IN6_IS_ADDR_MULTICAST(a)                                             \
         && ((((__const uint8_t *) (a))[1] & 0xf) == 0x5))

#define IN6_IS_ADDR_MC_ORGLOCAL(a) \
        (IN6_IS_ADDR_MULTICAST(a)                                             \
         && ((((__const uint8_t *) (a))[1] & 0xf) == 0x8))

#define IN6_IS_ADDR_MC_GLOBAL(a) \
        (IN6_IS_ADDR_MULTICAST(a)                                             \
         && ((((__const uint8_t *) (a))[1] & 0xf) == 0xe))

#define IN6_ARE_ADDR_MASKED_EQUAL(a,b,m) \
           (((((__const uint32_t *) (a))[0] & (((__const uint32_t *) (m))[0])) == (((__const uint32_t *) (b))[0] & (((__const uint32_t *) (m))[0])))  \
         && ((((__const uint32_t *) (a))[1] & (((__const uint32_t *) (m))[1])) == (((__const uint32_t *) (b))[1] & (((__const uint32_t *) (m))[1])))  \
         && ((((__const uint32_t *) (a))[2] & (((__const uint32_t *) (m))[2])) == (((__const uint32_t *) (b))[2] & (((__const uint32_t *) (m))[2])))  \
         && ((((__const uint32_t *) (a))[3] & (((__const uint32_t *) (m))[3])) == (((__const uint32_t *) (b))[3] & (((__const uint32_t *) (m))[3]))))

#define IN_ARE_ADDR_MASKED_EQUAL(a,b,m) \
           (((((__const uint8_t *) (a))[0] & (((__const uint8_t *) (m))[0])) == (((__const uint8_t *) (b))[0] & (((__const uint8_t *) (m))[0])))  \
         && ((((__const uint8_t *) (a))[1] & (((__const uint8_t *) (m))[1])) == (((__const uint8_t *) (b))[1] & (((__const uint8_t *) (m))[1])))  \
         && ((((__const uint8_t *) (a))[2] & (((__const uint8_t *) (m))[2])) == (((__const uint8_t *) (b))[2] & (((__const uint8_t *) (m))[2])))  \
         && ((((__const uint8_t *) (a))[3] & (((__const uint8_t *) (m))[3])) == (((__const uint8_t *) (b))[3] & (((__const uint8_t *) (m))[3]))))


//#define OAI_DRV_DEBUG_CLASS
//#define OAI_DRV_DEBUG_SEND
//---------------------------------------------------------------------------
void oai_nw_drv_create_mask_ipv6_addr(struct in6_addr *masked_addrP, int prefix_len){
  //---------------------------------------------------------------------------
  int   u6_addr8_index;
  int   u6_addr1_index;
  int   index;

  masked_addrP->s6_addr32[0] = 0xFFFFFFFF;
  masked_addrP->s6_addr32[1] = 0xFFFFFFFF;
  masked_addrP->s6_addr32[2] = 0xFFFFFFFF;
  masked_addrP->s6_addr32[3] = 0xFFFFFFFF;

  switch (prefix_len) {
  case 128:
	  return;
  case 112:
	  masked_addrP->s6_addr32[3] = htonl(0xFFFF0000);
	  return;
  case 96:
	  masked_addrP->s6_addr32[3] = 0x00000000;
	  return;
  case 80:
	  masked_addrP->s6_addr32[2] = htonl(0xFFFF0000);
	  masked_addrP->s6_addr32[3] = 0x00000000;
	  return;
  case 64:
	  masked_addrP->s6_addr32[2] = 0x00000000;
	  masked_addrP->s6_addr32[3] = 0x00000000;
	  return;
  case 48:
	  masked_addrP->s6_addr32[1] = htonl(0xFFFF0000);
	  masked_addrP->s6_addr32[2] = 0x00000000;
	  masked_addrP->s6_addr32[3] = 0x00000000;
	  return;
  case 32:
	  masked_addrP->s6_addr32[1] = 0x00000000;
	  masked_addrP->s6_addr32[2] = 0x00000000;
	  masked_addrP->s6_addr32[3] = 0x00000000;
	  return;
  case 16:
	  masked_addrP->s6_addr32[0] = htonl(0xFFFF0000);
	  masked_addrP->s6_addr32[1] = 0x00000000;
	  masked_addrP->s6_addr32[2] = 0x00000000;
	  masked_addrP->s6_addr32[3] = 0x00000000;
	  return;
  default:
      u6_addr8_index = prefix_len >> 3;
      u6_addr1_index = prefix_len & 0x07;

      for (index = u6_addr8_index ; index < 16; index++) {
          masked_addrP->s6_addr[index] = 0;
      }
      if (u6_addr1_index > 0) {
          masked_addrP->s6_addr[u6_addr8_index+1] = htons(0xFF << (8-u6_addr1_index));
      }
      for (index = 0 ; index < 4; index++) {
    	  masked_addrP->s6_addr32[index] = htonl(masked_addrP->s6_addr32[index]);
      }
  }

}
//---------------------------------------------------------------------------
void oai_nw_drv_create_mask_ipv4_addr(struct in_addr *masked_addrP, int prefix_len){
  //---------------------------------------------------------------------------
  if (prefix_len > 32) {
      prefix_len = 32;
  }
  masked_addrP->s_addr = htonl(0xFFFFFFFF << (32 - prefix_len));
  return;
}


//---------------------------------------------------------------------------
// Search the entity with the IPv6 address 'addr'
// Navid: the ipv6 classifier is not fully tested
struct cx_entity *oai_nw_drv_find_cx6(struct sk_buff  *skb,
                                unsigned char    dscp,
                                struct oai_nw_drv_priv *gpriv,
                                int              inst,
                                int             *paddr_type,
                                unsigned char   *cx_searcher) {
  //---------------------------------------------------------------------------
  unsigned char             cxi;
  struct cx_entity         *cx = NULL;
  struct classifier_entity *sclassifier= NULL;
  u32                       mc_addr_hdr;
  struct in6_addr           masked_addr;

  if (skb!=NULL) {
      #ifdef OAI_DRV_DEBUG_CLASS
      printk("SOURCE ADDR %X:%X:%X:%X:%X:%X:%X:%X",NIP6ADDR(&(ipv6_hdr(skb)->saddr)));
      printk("    DEST   ADDR %X:%X:%X:%X:%X:%X:%X:%X\n",NIP6ADDR(&(ipv6_hdr(skb)->daddr)));
      #endif
      mc_addr_hdr = ntohl(ipv6_hdr(skb)->daddr.in6_u.u6_addr32[0]);
      //printk("   mc_addr_hdr  %08X\n",mc_addr_hdr);
      // First check if multicast [1st octet is FF]
      if ((mc_addr_hdr & 0xFF000000) == 0xFF000000) {
          // packet type according to the scope of the multicast packet
          // we don't consider RPT bits in second octet [maybe done later if needed]
          switch(mc_addr_hdr & 0x000F0000) {
              case (0x00020000):
                  *paddr_type = OAI_NW_DRV_IPV6_ADDR_TYPE_MC_SIGNALLING;
                  #ifdef OAI_DRV_DEBUG_CLASS
                  printk("nasrg_CLASS_cx6: multicast packet - signalling \n");
                  #endif
                  break;
              case (0x000E0000):
                  *paddr_type = OAI_NW_DRV_IPV6_ADDR_TYPE_MC_MBMS;
                  //*pmbms_ix = 0;
                  //cx=gpriv->cx;  // MBMS associate to Mobile 0
                  #ifdef OAI_DRV_DEBUG_CLASS
                  printk("nasrg_CLASS_cx6: multicast packet - MBMS data \n");
                  #endif
                  break;
          default:
                  printk("nasrg_CLASS_cx6: default \n");
                  *paddr_type = OAI_NW_DRV_IPV6_ADDR_TYPE_UNKNOWN;
                  //*pmbms_ix = NASRG_MBMS_SVCES_MAX;
          }
      } else {
          *paddr_type = OAI_NW_DRV_IPV6_ADDR_TYPE_UNICAST;

          for (cxi=*cx_searcher; cxi<OAI_NW_DRV_CX_MAX; cxi++) {

              (*cx_searcher)++;
              sclassifier = gpriv->cx[cxi].sclassifier[dscp];

              while (sclassifier!=NULL) {
                  if ((sclassifier->ip_version == OAI_NW_DRV_IP_VERSION_6) || (sclassifier->ip_version == OAI_NW_DRV_IP_VERSION_ALL)) {   // verify that this is an IPv6 rule
                      /*LGif (IN6_IS_ADDR_UNSPECIFIED(&(sclassifier->daddr.ipv6))) {
                          printk("oai_nw_drv_find_cx6: addr is null \n");
                          sclassifier = sclassifier->next;
                          continue;
                      }*/
                      #ifdef OAI_DRV_DEBUG_CLASS
                      printk("cx %d : DSCP %d %X:%X:%X:%X:%X:%X:%X:%X\n",cxi, dscp, NIP6ADDR(&(sclassifier->daddr.ipv6)));
                      #endif //OAI_DRV_DEBUG_CLASS
                      //if ((dst = (unsigned int*)&(((struct rt6_info *)skbdst)->rt6i_gateway)) == 0){
                      // LG: STRANGE
                      if (IN6_IS_ADDR_UNSPECIFIED(&ipv6_hdr(skb)->daddr)) {
                          printk("oai_nw_drv_find_cx6: dst addr is null \n");
                          sclassifier = sclassifier->next;
                          continue;
                      }

                      oai_nw_drv_create_mask_ipv6_addr(&masked_addr, sclassifier->dplen);
                      if (IN6_ARE_ADDR_MASKED_EQUAL(&ipv6_hdr(skb)->daddr, &(sclassifier->daddr.ipv6), &masked_addr)) {
                              #ifdef OAI_DRV_DEBUG_CLASS
                              printk("oai_nw_drv_find_cx6: found cx %d: %X:%X:%X:%X:%X:%X:%X:%X\n",cxi, NIP6ADDR(&(sclassifier->daddr.ipv6)));
                              #endif //OAI_DRV_DEBUG_CLASS
                              return &gpriv->cx[cxi];
                      }
                  }
                  // Go to next classifier entry for connection
                  sclassifier = sclassifier->next;
              }
          }
      }
  }
  printk("oai_nw_drv_find_cx6 NOT FOUND: %X:%X:%X:%X:%X:%X:%X:%X\n",NIP6ADDR(&ipv6_hdr(skb)->daddr));
  return cx;
}

