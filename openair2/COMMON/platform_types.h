/*
                                 platform_types.h
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#ifndef __PLATFORM_TYPES_H__
#    define __PLATFORM_TYPES_H__

#ifndef USER_MODE
#else
#include "openair_types.h"
#endif

typedef unsigned char      u8_t;
typedef unsigned short int u16_t;
typedef unsigned int       u32_t;
typedef unsigned long  int u64_t;

typedef signed char        s8_t;
typedef signed short int   s16_t;
typedef signed int         s32_t;
typedef signed long  int   s64_t;

typedef unsigned int       config_action_t;

typedef unsigned int       sdu_size_t;
typedef unsigned int       tbs_size_t;
typedef unsigned int       tb_size_t;
typedef unsigned int       rb_id_t;
typedef unsigned int       module_id_t;
typedef unsigned int       mui_t;
typedef unsigned int       confirm_t;
typedef unsigned int       rb_type_t;
typedef rb_id_t            chan_id_t;
typedef unsigned int       num_tb_t;
typedef unsigned int       crc_t;
typedef unsigned int       rlc_tx_status_t;
typedef unsigned int       rlc_mode_t;

typedef signed int         rlc_op_status_t;

typedef unsigned char      crc8_t;
typedef unsigned short     crc16_t;
typedef unsigned int       crc32_t;

typedef signed char        boolean_t;


// just for integration
extern unsigned int    frame;

#endif
