/*
                                 platform_constants.h
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/

#include "RRC/LITE/MESSAGES/asn1_constants.h"

#ifndef __PLATFORM_CONSTANTS_H__
#    define __PLATFORM_CONSTANTS_H__

#define NL_MAX_PAYLOAD 4096  /* this should cover the max mtu size*/

#ifdef USER_MODE
#ifdef LARGE_SCALE
#    define NB_MODULES_MAX 128
#    define NB_NODE_MAX    128
#else
#    define NB_MODULES_MAX 32
#    define NB_NODE_MAX    32
#endif
#else
#    define NB_MODULES_MAX 1
#    define NB_NODE_MAX    1
#endif //PHY_EMUL


#ifdef JUMBO_FRAME
#    define MAX_IP_PACKET_SIZE         9000
#else
# if defined(OAI_NW_DRIVER_TYPE_ETHERNET)
/* SR: When using ethernet network driver the packet size is 1512 :
 * 1500 bytes IP packet + 12 bytes ethernet header
 */
#   define MAX_IP_PACKET_SIZE          1512
# else
#   define MAX_IP_PACKET_SIZE          1500
# endif
#endif
// overwrite the previous deinitions

#    define MAX_MODULES                NB_MODULES_MAX

#ifdef LARGE_SCALE
#    define MAX_MOBILES_PER_RG         128
#    define MAX_RG                     2
#else
#    define MAX_MOBILES_PER_RG         16
#    define MAX_RG                     2
#endif

#    define MAX_MANAGED_RG_PER_MOBILE  2

#    define DEFAULT_RAB_ID             3
#    define NB_RB_MAX  11
#    define NB_RAB_MAX 8 // 4
#    define RAB_SHIFT1 9
#    define RAB_SHIFT2 3
#    define RAB_OFFSET  0x0007
#    define RAB_OFFSET1 0x7E00
#    define RAB_OFFSET2 0x01F8
#    define DIR_OFFSET  0x8000
#    define DIR_SHIFT   15
#    define CH_OFFSET   0x0004
#    define CH_SHIFT    2
#ifdef MESH

#    define MAX_RB_MOBILE              NB_RB_MAX  * ( MAX_MANAGED_RG_PER_MOBILE + MAX_MOBILES_PER_RG - 1 )
#    define MAX_RAB_MOBILE             NB_RAB_MAX * ( MAX_MANAGED_RG_PER_MOBILE + MAX_MOBILES_PER_RG - 1 )
#    define MAX_RB_RG                  MAX_RB_MOBILE //NB_RB_MAX  *  MAX_MOBILES_PER_RG
#    define MAX_RAB_RG                 (NB_RB_MAX+1) * (MAX_MOBILES_PER_RG + 1)
#    define MAX_RAB                    MAX_RAB_RG
#    define MAX_RB                     MAX_RB_RG
#else

#    define MAX_RB_MOBILE              NB_RB_MAX * MAX_MANAGED_RG_PER_MOBILE
#    define MAX_RAB_MOBILE             NB_RAB_MAX * MAX_MANAGED_RG_PER_MOBILE
#    define MAX_RB_RG                  NB_RB_MAX * MAX_MOBILES_PER_RG
#    define MAX_RAB_RG                 NB_RB_MAX * MAX_MOBILES_PER_RG
#    //ifdef NODE_RG
#        define MAX_RAB                MAX_RAB_RG
#        define MAX_RB                 MAX_RB_RG
#    //else
#      //  ifdef NODE_MT
#        //    define MAX_RAB            MAX_RAB_MOBILE
#          //  define MAX_RB             MAX_RB_MOBILE
#        //else
#          //error NODE_RG or NODE_MT must be defined
#        //endif
//#    endif
#endif //MESH

// RLC_MODE
#        define RLC_NONE     0
#        define RLC_MODE_AM  1
#        define RLC_MODE_TM  2
#        define RLC_MODE_UM  3

//E_R
#        define E_R_RLC_ER_RELEASE          1
#        define E_R_RLC_ER_MODIFICATION     2
#        define E_R_RLC_ER_RE_ESTABLISHMENT 4
#        define E_R_RLC_ER_STOP             8
#        define E_R_RLC_ER_CONTINUE         16
#        define E_R_RLC_ER_NONE             0

// RLC_AM_SEND_MRW
#        define SEND_MRW_OFF    15
#        define SEND_MRW_ON     240

// RLC_SEGMENTATION_INDICATION
#        define SEGMENTATION_NOT_ALLOWED  0
#        define SEGMENTATION_ALLOWED      1

// RLC_DELIVERY_OF_ERRONEOUS_SDU
#        define DELIVERY_OF_ERRONEOUS_SDU_NO           0
#        define DELIVERY_OF_ERRONEOUS_SDU_YES          1
#        define DELIVERY_OF_ERRONEOUS_SDU_NO_DETECT    2


// CBA constant
#define NUM_MAX_CBA_GROUP 4

#    ifndef __cplusplus
#        ifndef NULL
#            define NULL 0
#        endif
#        ifndef null
#            define null 0
#        endif
#    endif



#ifdef USER_MODE
#define printk printf
#endif

#endif
