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
// Add a new classifier rule (send direction)
struct classifier_entity *oai_nw_drv_class_add_send_classifier(struct cx_entity *cx, u8 dscp, u16 classref){
  //---------------------------------------------------------------------------
  struct classifier_entity *classifier;

  #ifdef OAI_DRV_DEBUG_CLASS
  printk("OAI_NW_DRV_CLASS_ADD_SCLASSIFIER: begin for dscp %d, classref %d\n", dscp,classref);
  #endif
  if (cx==NULL){
      #ifdef OAI_DRV_DEBUG_CLASS
      printk("OAI_NW_DRV_CLASS_ADD_SCLASSIFIER - input parameter cx is NULL \n");
      #endif
      return NULL;
  }
  for (classifier=cx->sclassifier[dscp]; classifier!=NULL; classifier=classifier->next){
      if (classifier->classref==classref){
          #ifdef OAI_DRV_DEBUG_CLASS
          printk("OAI_NW_DRV_CLASS_ADD_SCLASSIFIER: classifier already exist for dscp %d, classref %d\n",dscp,classref);
          #endif
          return classifier;
      }
  }
  classifier=(struct classifier_entity *)kmalloc(sizeof(struct classifier_entity), GFP_KERNEL);
  if (classifier==NULL)
      return NULL;
  classifier->next=cx->sclassifier[dscp];
  classifier->classref=classref;
  cx->sclassifier[dscp]=classifier;
  ++cx->nsclassifier;
  #ifdef OAI_DRV_DEBUG_CLASS
  printk("OAI_NW_DRV_CLASS_ADD_SCLASSIFIER: classifier created for dscp %d, classref %d\n",dscp,classref);
  #endif
  return classifier;
}
//---------------------------------------------------------------------------
// Add a new classifier rule (receive direction)
struct classifier_entity *oai_nw_drv_class_add_recv_classifier(u8 dscp,
                            u16 classref,
                            struct oai_nw_drv_priv *gpriv){
  //---------------------------------------------------------------------------
  struct classifier_entity *classifier;
#ifdef OAI_DRV_DEBUG_CLASS
  printk("OAI_NW_DRV_CLASS_ADD_RCLASSIFIER: begin\n");
#endif
  for (classifier=gpriv->rclassifier[dscp]; classifier!=NULL; classifier=classifier->next)
    {
      if (classifier->classref==classref){
#ifdef OAI_DRV_DEBUG_CLASS
    printk("OAI_NW_DRV_CLASS_ADD_RCLASSIFIER: classifier already exist for dscp %d, classref %d\n",dscp,classref);
#endif
    return classifier;
      }
    }
  classifier=(struct classifier_entity *)kmalloc(sizeof(struct classifier_entity), GFP_KERNEL);
  if (classifier==NULL)
    return NULL;
  classifier->next=gpriv->rclassifier[dscp];
  classifier->classref=classref;
  gpriv->rclassifier[dscp]=classifier;
  ++gpriv->nrclassifier;
#ifdef OAI_DRV_DEBUG_CLASS
  printk("OAI_NW_DRV_CLASS_ADD_RCLASSIFIER: classifier created for dscp %d, classref %d\n",dscp,classref);
#endif
  return classifier;
}

//---------------------------------------------------------------------------
// Add a new classifier rule (forwarding)
struct classifier_entity *oai_nw_drv_class_add_fwd_classifier(struct cx_entity *cx, u8 dscp, u16 classref){
  //---------------------------------------------------------------------------
  struct classifier_entity *classifier;
#ifdef OAI_DRV_DEBUG_CLASS
  printk("OAI_NW_DRV_CLASS_ADD_FCLASSIFIER: begin for dscp %d, classref %d\n", dscp,classref);
#endif
  if (cx==NULL){
#ifdef OAI_DRV_DEBUG_CLASS
    printk("OAI_NW_DRV_CLASS_ADD_FCLASSIFIER - input parameter cx is NULL \n");
#endif
    return NULL;
  }
  for (classifier=cx->fclassifier[dscp]; classifier!=NULL; classifier=classifier->next){
    if (classifier->classref==classref){
#ifdef OAI_DRV_DEBUG_CLASS
      printk("OAI_NW_DRV_CLASS_ADD_SCLASSIFIER: classifier already exist for dscp %d, classref %d\n",dscp,classref);
#endif
      return classifier;
    }
  }
  classifier=(struct classifier_entity *)kmalloc(sizeof(struct classifier_entity), GFP_KERNEL);
  if (classifier==NULL)
    return NULL;
  classifier->next=cx->fclassifier[dscp];
  classifier->classref=classref;
  cx->fclassifier[dscp]=classifier;
  ++cx->nfclassifier;
#ifdef OAI_DRV_DEBUG_CLASS
  printk("OAI_NW_DRV_CLASS_ADD_FCLASSIFIER: classifier created for dscp %d, classref %d\n",dscp,classref);
#endif
  return classifier;
}


//---------------------------------------------------------------------------
void oai_nw_drv_class_flush_send_classifier(struct cx_entity *cx){
  //---------------------------------------------------------------------------
  u8 dscpi;
  struct classifier_entity *classifier;
#ifdef OAI_DRV_DEBUG_CLASS
  printk("OAI_NW_DRV_CLASS_FLUSH_SCLASSIFIER: begin\n");
#endif
  if (cx==NULL){
#ifdef OAI_DRV_DEBUG_CLASS
    printk("OAI_NW_DRV_CLASS_FLUSH_SCLASSIFIER - input parameter cx is NULL \n");
#endif
    return;
  }
  //
  for (dscpi=0; dscpi<OAI_NW_DRV_DSCP_MAX; ++dscpi)
    {
      for (classifier=cx->sclassifier[dscpi]; classifier!=NULL; classifier=cx->sclassifier[dscpi])
    {
      cx->sclassifier[dscpi]=classifier->next;
      kfree(classifier);
    }
    }
  cx->nsclassifier=0;
#ifdef OAI_DRV_DEBUG_CLASS
  printk("OAI_NW_DRV_CLASS_FLUSH_SCLASSIFIER: end\n");
#endif
}
//---------------------------------------------------------------------------
void oai_nw_drv_class_flush_fwd_classifier(struct cx_entity *cx){
  //---------------------------------------------------------------------------
  u8 dscpi;
  struct classifier_entity *classifier;
#ifdef OAI_DRV_DEBUG_CLASS
  printk("OAI_NW_DRV_CLASS_FLUSH_FCLASSIFIER: begin\n");
#endif
  if (cx==NULL){
#ifdef OAI_DRV_DEBUG_CLASS
    printk("OAI_NW_DRV_CLASS_FLUSH_FCLASSIFIER - input parameter cx is NULL \n");
#endif
    return;
  }
  //
  for (dscpi=0; dscpi<OAI_NW_DRV_DSCP_MAX; ++dscpi)
    {
      for (classifier=cx->fclassifier[dscpi]; classifier!=NULL; classifier=cx->fclassifier[dscpi])
    {
      cx->fclassifier[dscpi]=classifier->next;
      kfree(classifier);
    }
    }
  cx->nfclassifier=0;
#ifdef OAI_DRV_DEBUG_CLASS
  printk("OAI_NW_DRV_CLASS_FLUSH_FCLASSIFIER: end\n");
#endif
}

//---------------------------------------------------------------------------
void oai_nw_drv_class_flush_recv_classifier(struct oai_nw_drv_priv *gpriv){
  //---------------------------------------------------------------------------
  u8 dscpi;
  struct classifier_entity *classifier;
#ifdef OAI_DRV_DEBUG_CLASS
  printk("OAI_NW_DRV_CLASS_FLUSH_RCLASSIFIER: begin\n");
#endif
  for (dscpi=0; dscpi<OAI_NW_DRV_DSCP_MAX; ++dscpi)
    {
      for (classifier=gpriv->rclassifier[dscpi]; classifier!=NULL; classifier=gpriv->rclassifier[dscpi])
    {
      gpriv->rclassifier[dscpi]=classifier->next;
      kfree(classifier);
    }
    }
  gpriv->nrclassifier=0;
#ifdef OAI_DRV_DEBUG_CLASS
  printk("OAI_NW_DRV_CLASS_FLUSH_RCLASSIFIER: end\n");
#endif
}

//---------------------------------------------------------------------------
// Delete a classifier rule (send direction)
void oai_nw_drv_class_del_send_classifier(struct cx_entity *cx, u8 dscp, u16 classref){
  //---------------------------------------------------------------------------
  struct classifier_entity *pclassifier,*np;
#ifdef OAI_DRV_DEBUG_CLASS
  printk("OAI_NW_DRV_CLASS_DEL_SCLASSIFIER: begin\n");
#endif
  if (cx==NULL){
#ifdef OAI_DRV_DEBUG_CLASS
    printk("OAI_NW_DRV_CLASS_DEL_SCLASSIFIER - input parameter cx is NULL \n");
#endif
    return;
  }
  //
  pclassifier=cx->sclassifier[dscp];
  if (pclassifier==NULL)
    return;
  if (pclassifier->classref==classref)
    {
      cx->sclassifier[dscp]=pclassifier->next;
      kfree(pclassifier);
      --cx->nsclassifier;
      return;
    }
  for (np=pclassifier->next; np!=NULL; pclassifier=np)
    {
      if (np->classref==classref)
    {
      pclassifier->next=np->next;
      kfree(np);
      --cx->nsclassifier;
      return;
    }
    }
#ifdef OAI_DRV_DEBUG_CLASS
  printk("OAI_NW_DRV_CLASS_DEL_SCLASSIFIER: end\n");
#endif
}

//---------------------------------------------------------------------------
// Delete a classifier rule (send direction)
void oai_nw_drv_class_del_fwd_classifier(struct cx_entity *cx, u8 dscp, u16 classref){
  //---------------------------------------------------------------------------
  struct classifier_entity *pclassifier,*np;
#ifdef OAI_DRV_DEBUG_CLASS
  printk("OAI_NW_DRV_CLASS_DEL_FCLASSIFIER: begin\n");
#endif
  if (cx==NULL){
#ifdef OAI_DRV_DEBUG_CLASS
    printk("OAI_NW_DRV_CLASS_DEL_FCLASSIFIER - input parameter cx is NULL \n");
#endif
    return;
  }
  //
  pclassifier=cx->fclassifier[dscp];
  if (pclassifier==NULL)
    return;
  if (pclassifier->classref==classref)
    {
      cx->fclassifier[dscp]=pclassifier->next;
      kfree(pclassifier);
      --cx->nfclassifier;
      return;
    }
  for (np=pclassifier->next; np!=NULL; pclassifier=np)
    {
      if (np->classref==classref)
    {
      pclassifier->next=np->next;
      kfree(np);
      --cx->nfclassifier;
      return;
    }
    }
#ifdef OAI_DRV_DEBUG_CLASS
  printk("OAI_NW_DRV_CLASS_DEL_FCLASSIFIER: end\n");
#endif
}

//---------------------------------------------------------------------------
// Delete a classifier rule (receive direction)
void oai_nw_drv_class_del_recv_classifier(u8 dscp, u16 classref,struct oai_nw_drv_priv *gpriv){
  //---------------------------------------------------------------------------
  struct classifier_entity *pclassifier,*np;
#ifdef OAI_DRV_DEBUG_CLASS
  printk("OAI_NW_DRV_CLASS_DEL_RCLASSIFIER: begin\n");
#endif
  pclassifier=gpriv->rclassifier[dscp];
  if (pclassifier==NULL)
    return;
  if (pclassifier->classref==classref)
    {
      gpriv->rclassifier[dscp]=pclassifier->next;
      kfree(pclassifier);
      --gpriv->nrclassifier;
      return;
    }
  for (np=pclassifier->next; np!=NULL; pclassifier=np)
    {
      if (np->classref==classref)
    {
      pclassifier->next=np->next;
      kfree(np);
      --gpriv->nrclassifier;
      return;
    }
    }
#ifdef OAI_DRV_DEBUG_CLASS
  printk("OAI_NW_DRV_CLASS_DEL_RCLASSIFIER: end\n");
#endif
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

//---------------------------------------------------------------------------
// Search the entity with the IPv4 address 'addr'
struct cx_entity *oai_nw_drv_find_cx4(struct sk_buff  *skb,
                                unsigned char    dscp,
                                struct oai_nw_drv_priv *gpriv,
                                int              inst,
                                int             *paddr_type,
                                unsigned char   *cx_searcher) {
  //---------------------------------------------------------------------------
  unsigned char             cxi;
  u32                       daddr;
  struct cx_entity         *default_ip=NULL;
  struct classifier_entity *pclassifier=NULL;
  struct in_addr            masked_addr;

  //  if (inst >0)
  //    return(gpriv->cx);  //dump to clusterhead

    if (skb!=NULL) {
        daddr = ((struct iphdr*)(skb_network_header(skb)))->daddr;
        if (daddr != INADDR_ANY) {

            if (ipv4_is_multicast(ip_hdr(skb)->daddr)) {
                // TO BE CHECKED
                *paddr_type = OAI_NW_DRV_IPV4_ADDR_TYPE_MC_SIGNALLING;
                 #ifdef OAI_DRV_DEBUG_CLASS
                 printk("SOURCE ADDR %d.%d.%d.%d",NIPADDR(ip_hdr(skb)->saddr));
                 printk("    DEST   ADDR %d.%d.%d.%d MULTICAST\n",NIPADDR(ip_hdr(skb)->daddr));
                 #endif
            	return NULL;
            } else if (ipv4_is_lbcast(ip_hdr(skb)->daddr)) {
                // TO BE CHECKED
                *paddr_type = OAI_NW_DRV_IPV4_ADDR_TYPE_BROADCAST;
                #ifdef OAI_DRV_DEBUG_CLASS
                printk("SOURCE ADDR %d.%d.%d.%d",NIPADDR(ip_hdr(skb)->saddr));
                printk("    DEST   ADDR %d.%d.%d.%d LOCAL BROADCAST\n",NIPADDR(ip_hdr(skb)->daddr));
                #endif
            	return NULL;
            } else if (IN_CLASSA(htonl(ip_hdr(skb)->daddr)) ||
                       IN_CLASSB(htonl(ip_hdr(skb)->daddr)) ||
                       IN_CLASSC(htonl(ip_hdr(skb)->daddr))) {

                if ( ((ip_hdr(skb)->daddr & 0xFF000000) >> 24) == 0x000000FF) {
                    *paddr_type = OAI_NW_DRV_IPV4_ADDR_TYPE_BROADCAST;
                    #ifdef OAI_DRV_DEBUG_CLASS
                    printk("SOURCE ADDR %d.%d.%d.%d",NIPADDR(ip_hdr(skb)->saddr));
                    printk("    DEST   ADDR %d.%d.%d.%d BROADCAST\n",NIPADDR(ip_hdr(skb)->daddr));
                    #endif
                	return NULL;
                }
                *paddr_type = OAI_NW_DRV_IPV4_ADDR_TYPE_UNICAST;
                #ifdef OAI_DRV_DEBUG_CLASS
                printk("SOURCE ADDR %d.%d.%d.%d",NIPADDR(ip_hdr(skb)->saddr));
                printk("    DEST   ADDR %d.%d.%d.%d UNICAST\n",NIPADDR(ip_hdr(skb)->daddr));
                #endif

                for (cxi=*cx_searcher; cxi<OAI_NW_DRV_CX_MAX; ++cxi) {

                    (*cx_searcher)++;
                    pclassifier = gpriv->cx[cxi].sclassifier[dscp];

                    while (pclassifier!=NULL) {
                        if ((pclassifier->ip_version == OAI_NW_DRV_IP_VERSION_4)  || (pclassifier->ip_version == OAI_NW_DRV_IP_VERSION_ALL)) {   // verify that this is an IPv4 rule
                            oai_nw_drv_create_mask_ipv4_addr(&masked_addr, pclassifier->dplen);
                            if (IN_ARE_ADDR_MASKED_EQUAL(&ip_hdr(skb)->daddr, &(pclassifier->daddr.ipv4), &masked_addr)) {
                                #ifdef OAI_DRV_DEBUG_CLASS
                                printk("oai_nw_drv_find_cx4: IP MASK MATCHED: found cx %d: %d.%d.%d.%d/%d\n",cxi, NIPADDR(pclassifier->daddr.ipv4), pclassifier->dplen);
                                #endif //OAI_DRV_DEBUG_CLASS
                                return &gpriv->cx[cxi];
                                //return gpriv->cx+cxi;
                            }
                        }
                        // goto to next classification rule for the connection
                        pclassifier = pclassifier->next;
                    }
                }
            } else {
                *paddr_type = OAI_NW_DRV_IPV4_ADDR_TYPE_UNKNOWN;
                #ifdef OAI_DRV_DEBUG_CLASS
                printk("SOURCE ADDR %d.%d.%d.%d",NIPADDR(ip_hdr(skb)->saddr));
                printk("    DEST   ADDR %d.%d.%d.%d TYPE UNKNOWN\n",NIPADDR(ip_hdr(skb)->daddr));
                #endif
            }
        }
    }
    #ifdef OAI_DRV_DEBUG_CLASS
    printk("oai_nw_drv_find_cx4 NOT FOUND: %d.%d.%d.%d\n",NIPADDR(ip_hdr(skb)->daddr));
    #endif //OAI_DRV_DEBUG_CLASS
    return default_ip;
}

#ifdef MPLS
//---------------------------------------------------------------------------
// Search the entity with the mpls label and given exp
struct cx_entity *oai_nw_drv_find_cx_mpls(struct sk_buff *skb,
                unsigned char exp,
                struct oai_nw_drv_priv *gpriv,
                int inst,
                unsigned char *cx_searcher){
  //---------------------------------------------------------------------------
    unsigned char             cxi;
    struct cx_entity         *default_label  = NULL;
    struct classifier_entity *pclassifier    = NULL;

    //  if (inst >0)
    //return(gpriv->cx);  //dump to clusterhead

    #ifdef OAI_DRV_DEBUG_CLASS
    printk("[NAS][CLASS][MPLS] Searching for label %d\n",MPLSCB(skb)->label);
    #endif

    for (cxi=*cx_searcher; cxi<OAI_NW_DRV_CX_MAX; ++cxi) {

        (*cx_searcher)++;
        pclassifier = gpriv->cx[cxi].sclassifier[exp];

        while (pclassifier!=NULL) {
            if (pclassifier->ip_version == OAI_NW_DRV_MPLS_VERSION_CODE) {   // verify that this is an MPLS rule

                #ifdef OAI_DRV_DEBUG_CLASS
                printk("cx %d : label %d\n",cxi,pclassifier->daddr.mpls_label);
                #endif //OAI_DRV_DEBUG_CLASS

                if (pclassifier->daddr.mpls_label==MPLSCB(skb)->label){
                    #ifdef OAI_DRV_DEBUG_CLASS
                    printk("found cx %d: label %d, RB %d\n",cxi,
                                                    MPLSCB(skb)->label,
                                                        pclassifier->rab_id);
                    #endif //OAI_DRV_DEBUG_CLASS
                    return gpriv->cx+cxi;
                }
            /*
            else if (gpriv->cx[cxi].sclassifier[dscp]->daddr.ipv4==OAI_NW_DRV_DEFAULT_IPV4_ADDR) {
            #ifdef OAI_DRV_DEBUG_CLASS
            printk("found default_ip rule\n");
            #endif //OAI_DRV_DEBUG_CLASS
            default_ip = gpriv->cx+cxi;
            }
            */
            }
            // goto to next classification rule for the connection
            pclassifier = pclassifier->next;
        }
    }
    return default_label;
}
#endif

//---------------------------------------------------------------------------
// Search the sending function
void oai_nw_drv_class_send(struct sk_buff *skb,int inst){
  //---------------------------------------------------------------------------
    struct classifier_entity  *pclassifier = NULL, *sp = NULL;
    struct sk_buff            *skb_cloned = NULL;
    u8                        *protocolh = NULL;
    u8                         version;
    u8                         protocol, dscp;
    u16                        classref;
    struct cx_entity          *cx        = NULL;
    unsigned int               i;
    struct net_device         *dev        = oai_nw_drv_dev[inst];
    struct oai_nw_drv_priv    *gpriv      = netdev_priv(dev);
    unsigned char              cx_searcher,no_connection;
    int                        addr_type;
    struct in6_addr            masked6_addr;
    struct in_addr             masked_addr;
    // RARP vars
    struct arphdr             *rarp = NULL;
    unsigned char             *rarp_ptr = NULL;
    __be32                     sip, tip;
    unsigned char              *sha = NULL, *tha = NULL;         /* s for "source", t for "target" */

    #ifdef OAI_DRV_DEBUG_CLASS
    printk("[NAS][%s] begin - inst %d\n",__FUNCTION__, inst);
    #endif
    if (skb==NULL){
        #ifdef OAI_DRV_DEBUG_SEND
        printk("[NAS][%s] - input parameter skb is NULL \n",__FUNCTION__);
        #endif
        return;
    }

    #ifdef OAI_DRV_DEBUG_SEND
    printk("[NAS][%s] Got packet from kernel:\n",__FUNCTION__);
    for (i=0;i < skb->len;i++)
        printk("%2x ",((unsigned char *)skb->data)[i]);
    printk("\n");
    #endif

    // find all connections related to socket
    cx_searcher   = 0;
    no_connection = 1;

    // Address classification
    switch (ntohs(skb->protocol)) {
        case ETH_P_IPV6:
            version   = 6;
            addr_type = OAI_NW_DRV_IPV6_ADDR_TYPE_UNKNOWN;
            protocolh = oai_nw_drv_TOOL_get_protocol6(ipv6_hdr(skb), &protocol);
            dscp      = oai_nw_drv_TOOL_get_dscp6    (ipv6_hdr(skb));
            cx        = oai_nw_drv_find_cx6         (skb, dscp, gpriv, inst, &addr_type, &cx_searcher);

            #ifdef OAI_DRV_DEBUG_CLASS
            printk("[NAS][%s] ETH_P_IPV6 skb %p dscp %d gpriv %p inst %d cx_searcher %p \n",__FUNCTION__,skb, dscp, gpriv, inst, &cx_searcher);
            #endif
            // find in default DSCP a valid classification
            if (cx == NULL) {
                switch (addr_type) {
                    case OAI_NW_DRV_IPV6_ADDR_TYPE_MC_SIGNALLING:
                        for (i=0; i<OAI_NW_DRV_CX_MAX; i++){
                            pclassifier=(&gpriv->cx[i])->sclassifier[OAI_NW_DRV_DSCP_DEFAULT];
                            while (pclassifier!=NULL) {
                                if ((pclassifier->ip_version == OAI_NW_DRV_IP_VERSION_6) || (pclassifier->ip_version == OAI_NW_DRV_IP_VERSION_ALL)) {
                                    // ok found default classifier for this packet
                                    oai_nw_drv_create_mask_ipv6_addr(&masked6_addr, pclassifier->dplen);
                                    #ifdef OAI_DRV_DEBUG_CLASS
                                    printk("[NAS][%s] IPv6 MULTICAST CX %u TRYING default DSCP classifier IP Version %d, Dest ADDR %X:%X:%X:%X:%X:%X:%X:%X/%u Mask %X:%X:%X:%X:%X:%X:%X:%X/%u \n",
                                    		__FUNCTION__, i, pclassifier->ip_version,
                                    		NIP6ADDR(&(pclassifier->daddr.ipv6)), pclassifier->dplen,
                                    		NIP6ADDR(&masked6_addr), pclassifier->dplen);
                                    #endif
                                    if (IN6_ARE_ADDR_MASKED_EQUAL(&pclassifier->daddr.ipv6, &ipv6_hdr(skb)->daddr, &masked6_addr)) {
                                        cx = &gpriv->cx[i];
                                        #ifdef OAI_DRV_DEBUG_CLASS
                                        printk("[NAS][%s] IPv6 MULTICAST CX %u ETH_P_IPV6 FOUND OAI_NW_DRV_DSCP_DEFAULT with IN6_ARE_ADDR_MASKED_EQUAL(%d bits)\n",__FUNCTION__, i, pclassifier->dplen);
                                        #endif
                                        skb_cloned =  skb_clone (skb, 0);
                                        if (skb_cloned) {
                                            pclassifier->fct(skb_cloned, cx, pclassifier,inst);
                                            dev_kfree_skb(skb_cloned);
                                        } else {
                                            printk("[NAS][%s] IPv6 MULTICAST CX %u could not send packet, skb_clone() failed)\n",__FUNCTION__, i);
                                        }
                                        break;
                                    } else if(IN6_IS_ADDR_UNSPECIFIED(&pclassifier->daddr.ipv6)) {
                                        cx = &gpriv->cx[i];
                                        #ifdef OAI_DRV_DEBUG_CLASS
                                        printk("[NAS][%s] IPv6 MULTICAST CX %u ETH_P_IPV6 FOUND OAI_NW_DRV_DSCP_DEFAULT with IN6_IS_ADDR_UNSPECIFIED\n",__FUNCTION__, i);
                                        #endif
                                        skb_cloned =  skb_clone (skb, 0);
                                        if (skb_cloned) {
                                            pclassifier->fct(skb_cloned, cx, pclassifier,inst);
                                            dev_kfree_skb(skb_cloned);
                                        } else {
                                            printk("[NAS][%s] IPv6 MULTICAST CX %u could not send packet, skb_clone() failed)\n",__FUNCTION__, i);
                                        }
                                        break;
                                    }
                                }
                                pclassifier = pclassifier->next;
                            }
                        }
                        cx = NULL;
                        break;

                    case OAI_NW_DRV_IPV6_ADDR_TYPE_UNICAST:

                        for (i=0; i<OAI_NW_DRV_CX_MAX; i++){
                            pclassifier=(&gpriv->cx[i])->sclassifier[OAI_NW_DRV_DSCP_DEFAULT];
                            while (pclassifier!=NULL) {
                                #ifdef OAI_DRV_DEBUG_CLASS
                                printk("[NAS][%s] TRYING default DSCP classifier IP Version %d, Dest ADDR %X:%X:%X:%X:%X:%X:%X:%X/%u\n",__FUNCTION__, pclassifier->ip_version, NIP6ADDR(&(pclassifier->daddr.ipv6)), pclassifier->dplen);
                                #endif
                                if ((pclassifier->ip_version == OAI_NW_DRV_IP_VERSION_6) || (pclassifier->ip_version == OAI_NW_DRV_IP_VERSION_ALL)) {
                                    // ok found default classifier for this packet
                                    oai_nw_drv_create_mask_ipv6_addr(&masked6_addr, pclassifier->dplen);
                                    if (IN6_ARE_ADDR_MASKED_EQUAL(&pclassifier->daddr.ipv6, &ipv6_hdr(skb)->daddr, &masked6_addr)) {
                                        // then force dscp
                                        cx = &gpriv->cx[i];
                                        #ifdef OAI_DRV_DEBUG_CLASS
                                        printk("[NAS][%s] ETH_P_IPV6 FOUND OAI_NW_DRV_DSCP_DEFAULT with IN6_ARE_ADDR_MASKED_EQUAL(%d bits)\n",__FUNCTION__, pclassifier->dplen);
                                        #endif
                                        dscp = OAI_NW_DRV_DSCP_DEFAULT;
                                        i = OAI_NW_DRV_CX_MAX;
                                        break;
                                    } else if(IN6_IS_ADDR_UNSPECIFIED(&pclassifier->daddr.ipv6)) {
                                        cx = &gpriv->cx[i];
                                        #ifdef OAI_DRV_DEBUG_CLASS
                                        printk("[NAS][%s] ETH_P_IPV6 FOUND OAI_NW_DRV_DSCP_DEFAULT with IN6_IS_ADDR_UNSPECIFIED\n",__FUNCTION__);
                                        #endif
                                        dscp = OAI_NW_DRV_DSCP_DEFAULT;
                                        i = OAI_NW_DRV_CX_MAX;
                                        break;
                                    } else {
                                        printk("[NAS][%s] TRYING default DSCP classifier: NO MATCH\n",__FUNCTION__);
                                    }
                                }
                                pclassifier = pclassifier->next;
                            }
                        }
                        break;

                    case OAI_NW_DRV_IPV6_ADDR_TYPE_MC_MBMS:
                        break;

                      // should have found a valid classification rule
                    case OAI_NW_DRV_IPV6_ADDR_TYPE_UNKNOWN:
                    default:
                      ;
                }
            }
            break;

        case ETH_P_ARP:
            version   = 4;
            addr_type = OAI_NW_DRV_IPV4_ADDR_TYPE_BROADCAST;
            dscp      = 0;
            cx        = NULL;
            /* Basic sanity checks can be done without the lock.  */
            //rarp = (struct arphdr *)skb_transport_header(skb);
            rarp = (struct arphdr *)skb_network_header(skb);
            if (rarp) {
                if (rarp->ar_hln != dev->addr_len || dev->type != ntohs(rarp->ar_hrd)) {
                    printk("[NAS][%s] ARP PACKET WRONG ADDR LEN or WRONG ARP HEADER TYPE\n",__FUNCTION__);
                    break;
                }
            } else {
                printk("[NAS][%s] ARP HEADER POINTER IS NULL\n",__FUNCTION__);
                break;
            }

            /* If it's not Ethernet, delete it. */
            if (rarp->ar_pro != htons(ETH_P_IP)) {
                printk("[NAS][%s] ARP PACKET PROTOCOL IS NOT ETHERNET\n",__FUNCTION__);
                break;
            }
            rarp_ptr = (unsigned char *) (rarp + 1);
            sha = rarp_ptr;
            rarp_ptr += dev->addr_len;
            memcpy(&sip, rarp_ptr, 4);
            rarp_ptr += 4;
            tha = rarp_ptr;
            rarp_ptr += dev->addr_len;
            memcpy(&tip, rarp_ptr, 4);

            #ifdef OAI_DRV_DEBUG_CLASS
            printk("[NAS][%s] ARP DEST IP transport IP = %d.%d.%d.%d\n",__FUNCTION__, NIPADDR(tip));
            #endif
            for (i=0; i<OAI_NW_DRV_CX_MAX; i++){
                pclassifier=(&gpriv->cx[i])->sclassifier[OAI_NW_DRV_DSCP_DEFAULT];
                while (pclassifier!=NULL) {
                    if ((pclassifier->ip_version == OAI_NW_DRV_IP_VERSION_4) || (pclassifier->ip_version == OAI_NW_DRV_IP_VERSION_ALL)) {
                        // ok found default classifier for this packet
                        oai_nw_drv_create_mask_ipv4_addr(&masked_addr, pclassifier->dplen);
                        #ifdef OAI_DRV_DEBUG_CLASS
                        printk("[NAS][%s] MASK = %d.%d.%d.%d\n",__FUNCTION__, NIPADDR(masked_addr.s_addr));
                        #endif

                        if (IN_ARE_ADDR_MASKED_EQUAL(&pclassifier->daddr.ipv4, &tip, &masked_addr.s_addr)) {
                            // then force dscp
                            cx = &gpriv->cx[i];
                            #ifdef OAI_DRV_DEBUG_CLASS
                            printk("[NAS][%s] ETH_P_ARP FOUND OAI_NW_DRV_DSCP_DEFAULT with IN_ARE_ADDR_MASKED_EQUAL(%d bits)\n",__FUNCTION__, pclassifier->dplen);
                            #endif
                            dscp = OAI_NW_DRV_DSCP_DEFAULT;
                            i = OAI_NW_DRV_CX_MAX;
                            break;
                        } else if(INADDR_ANY == pclassifier->daddr.ipv4) {
                            cx = &gpriv->cx[i];
                            #ifdef OAI_DRV_DEBUG_CLASS
                            printk("[NAS][%s] ETH_P_ARP FOUND OAI_NW_DRV_DSCP_DEFAULT with INADDR_ANY\n",__FUNCTION__);
                            #endif
                            dscp = OAI_NW_DRV_DSCP_DEFAULT;
                            i = OAI_NW_DRV_CX_MAX;
                            break;
                        }
                    }
                    pclassifier = pclassifier->next;
                }
            }

            break;

        case ETH_P_IP:
            version   = 4;
            addr_type = OAI_NW_DRV_IPV4_ADDR_TYPE_UNKNOWN;
            dscp      = oai_nw_drv_TOOL_get_dscp4((struct iphdr *)(skb_network_header(skb)));
            cx        = oai_nw_drv_find_cx4(skb, dscp, gpriv, inst, &addr_type, &cx_searcher);
            protocolh = oai_nw_drv_TOOL_get_protocol4((struct iphdr *)(skb_network_header(skb)), &protocol);
            // find in default DSCP a valid classification
            if (cx == NULL) {
                switch (addr_type) {
                    case OAI_NW_DRV_IPV4_ADDR_TYPE_MC_SIGNALLING:
                    case OAI_NW_DRV_IPV4_ADDR_TYPE_BROADCAST:
                        for (i=0; i<OAI_NW_DRV_CX_MAX; i++){
                            pclassifier=(&gpriv->cx[i])->sclassifier[OAI_NW_DRV_DSCP_DEFAULT];
                            while (pclassifier!=NULL) {
                                if ((pclassifier->ip_version == OAI_NW_DRV_IP_VERSION_4) || (pclassifier->ip_version == OAI_NW_DRV_IP_VERSION_ALL)) {
                                    // ok found default classifier for this packet
                                    oai_nw_drv_create_mask_ipv4_addr(&masked_addr, pclassifier->dplen);
                                    #ifdef OAI_DRV_DEBUG_CLASS
                                    printk("[NAS][%s] IPv4 MULTICAST CX %u TRYING default DSCP classifier IP Version %d, Dest ADDR %u.%u.%u.%u/%u Mask %u.%u.%u.%u/%u \n",
                                    		__FUNCTION__, i, pclassifier->ip_version,
                                    		NIPADDR(pclassifier->daddr.ipv4), pclassifier->dplen,
                                    		NIPADDR(masked_addr.s_addr), pclassifier->dplen);
                                    #endif
                                    if (IN_ARE_ADDR_MASKED_EQUAL(&pclassifier->daddr.ipv4, &ip_hdr(skb)->daddr, &masked_addr.s_addr)) {
                                        cx = &gpriv->cx[i];
                                        #ifdef OAI_DRV_DEBUG_CLASS
                                        printk("[NAS][%s][CX %u] ETH_P_IP FOUND OAI_NW_DRV_DSCP_DEFAULT with IN_ARE_ADDR_MASKED_EQUAL(IP CLASS %d.%d.%d.%d, IP DEST %d.%d.%d.%d, MASK = %d.%d.%d.%d %d bits)\n",
                                               __FUNCTION__, i, NIPADDR(pclassifier->daddr.ipv4), NIPADDR(ip_hdr(skb)->daddr), NIPADDR(masked_addr.s_addr), pclassifier->dplen);
                                        #endif
                                        skb_cloned =  skb_clone (skb, 0);
                                        if (skb_cloned) {
                                            pclassifier->fct(skb_cloned, cx, pclassifier,inst);
                                            dev_kfree_skb(skb_cloned);
                                        } else {
                                            printk("[NAS][%s] IPv4 MULTICAST CX %u could not send packet, skb_clone() failed)\n",__FUNCTION__, i);
                                        }
                                        break;
                                    } else if(INADDR_ANY == pclassifier->daddr.ipv4) {
                                        cx = &gpriv->cx[i];
                                        #ifdef OAI_DRV_DEBUG_CLASS
                                        printk("[NAS][%s][CX %u] ETH_P_IP FOUND OAI_NW_DRV_DSCP_DEFAULT with INADDR_ANY\n",__FUNCTION__, i);
                                        #endif
                                        skb_cloned =  skb_clone (skb, 0);
                                        if (skb_cloned) {
                                            pclassifier->fct(skb_cloned, cx, pclassifier,inst);
                                            dev_kfree_skb(skb_cloned);
                                        } else {
                                            printk("[NAS][%s] IPv6 MULTICAST CX %u could not send packet, skb_clone() failed)\n",__FUNCTION__, i);
                                        }
                                        break;
                                    }
                                }
                                pclassifier = pclassifier->next;
                            }
                        }
                        cx = NULL;
                        break;
                    case OAI_NW_DRV_IPV4_ADDR_TYPE_UNICAST:

                        for (i=0; i<OAI_NW_DRV_CX_MAX; i++){
                            //if ((pclassifier=cx->sclassifier[OAI_NW_DRV_DSCP_DEFAULT])!=NULL) {
                            pclassifier=(&gpriv->cx[i])->sclassifier[OAI_NW_DRV_DSCP_DEFAULT];
                            while (pclassifier != NULL) {
                                if ((pclassifier->ip_version == OAI_NW_DRV_IP_VERSION_4) || (pclassifier->ip_version == OAI_NW_DRV_IP_VERSION_ALL)) {
                                    // ok found default classifier for this packet
                                    oai_nw_drv_create_mask_ipv4_addr(&masked_addr, pclassifier->dplen);
                                    #ifdef OAI_DRV_DEBUG_CLASS
                                    printk("[NAS][%s] MASK = %d.%d.%d.%d\n",__FUNCTION__, NIPADDR(masked_addr.s_addr));
                                    #endif
                                    if (IN_ARE_ADDR_MASKED_EQUAL(&pclassifier->daddr.ipv4, &ip_hdr(skb)->daddr, &masked_addr.s_addr)) {
                                        // then force dscp
                                        cx = &gpriv->cx[i];
                                        #ifdef OAI_DRV_DEBUG_CLASS
                                        printk("[NAS][%s][CX %d] ETH_P_IP FOUND OAI_NW_DRV_DSCP_DEFAULT with IN_ARE_ADDR_MASKED_EQUAL(IP CLASS %d.%d.%d.%d, IP DEST %d.%d.%d.%d, MASK = %d.%d.%d.%d %d bits)\n",
                                               __FUNCTION__, i, NIPADDR(pclassifier->daddr.ipv4), NIPADDR(ip_hdr(skb)->daddr), NIPADDR(masked_addr.s_addr), pclassifier->dplen);
                                        #endif
                                        dscp = OAI_NW_DRV_DSCP_DEFAULT;
                                        i = OAI_NW_DRV_CX_MAX;
                                        break;
                                    } else if(INADDR_ANY == pclassifier->daddr.ipv4) {
                                        cx = &gpriv->cx[i];
                                        #ifdef OAI_DRV_DEBUG_CLASS
                                        printk("[NAS][%s] ETH_P_IP FOUND OAI_NW_DRV_DSCP_DEFAULT with INADDR_ANY\n",__FUNCTION__);
                                        #endif
                                        dscp = OAI_NW_DRV_DSCP_DEFAULT;
                                        i = OAI_NW_DRV_CX_MAX;
                                        break;
                                    }
                                }
                                pclassifier = pclassifier->next;
                            }
                        }
                        break;

                      // should have found a valid classification rule
                    case OAI_NW_DRV_IPV4_ADDR_TYPE_UNKNOWN:
                    default:
                      ;
                }
            }
            #ifdef OAI_DRV_DEBUG_CLASS
            printk("[NAS][%s] ETH_P_IP Got IPv4 packet (%02X), dscp = %d, cx = %p\n",__FUNCTION__,ntohs(skb->protocol),dscp,cx);
            #endif
            break;

        #ifdef MPLS
        case ETH_P_MPLS_UC:
            cx=oai_nw_drv_find_cx_mpls(skb,MPLSCB(skb)->exp,gpriv,inst,&cx_searcher);
            #ifdef OAI_DRV_DEBUG_CLASS
            printk("[NAS][%s] ETH_P_MPLS_UC Got MPLS unicast packet, exp = %d, label = %d, cx = %p\n",__FUNCTION__,MPLSCB(skb)->exp,MPLSCB(skb)->label,cx);
            #endif

            dscp = MPLSCB(skb)->exp;
            version = OAI_NW_DRV_MPLS_VERSION_CODE;
            protocol = version;
            break;
        #endif

        default:
            printk("[NAS][%s] Unknown protocol\n",__FUNCTION__);
            version = 0;
            return;
    }



    // If a valid connection for the DSCP/EXP with destination address
    // is found scan all protocol-based classification rules
    if (cx != NULL) {
        classref = 0;
        sp       = NULL;

        #ifdef OAI_DRV_DEBUG_CLASS
        printk("[NAS][%s] DSCP/EXP %d : looking for classifier entry\n",__FUNCTION__,dscp);
        #endif
        for (pclassifier=cx->sclassifier[dscp]; pclassifier!=NULL; pclassifier=pclassifier->next) {
            #ifdef OAI_DRV_DEBUG_CLASS
            printk("[NAS][%s] DSCP %d p->classref=%d,p->protocol=%d,p->version=%d\n",__FUNCTION__,dscp,pclassifier->classref,pclassifier->protocol,pclassifier->ip_version);
            #endif
            // Check if transport protocol/message match
            /*
            if ((pclassifier->protocol == protocol))
                if ((protocol == OAI_NW_DRV_PROTOCOL_ICMP6) && (version == 6))
                    if (pclassifier->protocol_message_type == (pclassifier->protocol_message_type )) {
                        printk("[GRAAL][CLASSIFIER] Router advertisement\n");
                    }
            */
            // normal rule checks that network protocol version matches

            if ((pclassifier->ip_version == version)  || (pclassifier->ip_version == OAI_NW_DRV_IP_VERSION_ALL)){
                sp=pclassifier;
                classref=sp->classref;
                break;
            }
        }

        if (sp!=NULL) {
            #ifdef OAI_DRV_DEBUG_CLASS
            char sfct[10], sprotocol[10];
            if (sp->fct==oai_nw_drv_common_ip2wireless)
                strcpy(sfct, "qos");
            if (sp->fct==oai_nw_drv_CTL_send)
                strcpy(sfct, "ctl");
            if (sp->fct==oai_nw_drv_class_del_send_classifier)
                strcpy(sfct, "del");

            switch(protocol) {
                case OAI_NW_DRV_PROTOCOL_UDP:strcpy(sprotocol, "udp");printk("udp packet\n");break;
                case OAI_NW_DRV_PROTOCOL_TCP:strcpy(sprotocol, "tcp");printk("tcp packet\n");break;
                case OAI_NW_DRV_PROTOCOL_ICMP4:strcpy(sprotocol, "icmp4");printk("icmp4 packet\n");break;
                case OAI_NW_DRV_PROTOCOL_ICMP6:strcpy(sprotocol, "icmp6");print_TOOL_pk_icmp6((struct icmp6hdr*)protocolh);break;
                case OAI_NW_DRV_PROTOCOL_ARP:strcpy(sprotocol, "arp");printk("arp packet\n");break;
                #ifdef MPLS
                case OAI_NW_DRV_MPLS_VERSION_CODE:strcpy(sprotocol,"mpls");break;
                #endif
                default:strcpy(sprotocol, "unknown");

            }
            printk("[NAS][%s] (dscp %u, %s) received, (classref %u, fct %s, rab_id %u) classifier rule\n",__FUNCTION__,
                  dscp, sprotocol, sp->classref, sfct, sp->rab_id);
            #endif

            sp->fct(skb, cx, sp,inst);
        } // if classifier entry match found
        else {
            printk("[NAS][%s] no corresponding item in the classifier, so the message is dropped\n",__FUNCTION__);
            //  oai_nw_drv_COMMON_del_send(skb, cx, NULL,inst);
        }

        no_connection = 0;
    }   // if connection found
    //#ifdef OAI_DRV_DEBUG_CLASS
    if (no_connection == 1)
        printk("[NAS][%s] no corresponding connection, so the message is dropped\n",__FUNCTION__);
    //#endif
    //  }   // while loop over connections
    #ifdef OAI_DRV_DEBUG_CLASS
    printk("[NAS][%s] end\n",__FUNCTION__);
    #endif
}
