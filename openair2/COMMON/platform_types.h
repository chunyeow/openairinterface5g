/*
                                 platform_types.h
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#ifndef __PLATFORM_TYPES_H__
#    define __PLATFORM_TYPES_H__

#ifdef USER_MODE
# include "openair_types.h"
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

typedef u16_t              sdu_size_t;
typedef s16_t              sdu_ssize_t;
typedef unsigned int       tbs_size_t;
typedef unsigned int       tb_size_t;
typedef u16_t              rb_id_t;
typedef u16_t              srb_id_t;
typedef u32_t              frame_t;
typedef u32_t              sub_frame_t;
typedef u8_t               module_id_t;
typedef unsigned int       mui_t;
typedef unsigned int       confirm_t;
typedef unsigned int       rb_type_t;
typedef unsigned int       logical_chan_id_t;
typedef unsigned int       num_tb_t;
typedef unsigned int       crc_t;
typedef unsigned int       rlc_tx_status_t;
typedef unsigned int       rlc_mode_t;
typedef s16_t              rlc_sn_t;
typedef u16_t              rlc_usn_t;
typedef int                traffic_type_t;
typedef u32_t              mbms_session_id_t;
typedef u16_t              mbms_service_id_t;
typedef u16_t              rnti_t;

typedef signed int         rlc_op_status_t;

typedef unsigned char      crc8_t;
typedef unsigned short     crc16_t;
typedef unsigned int       crc32_t;

typedef signed char        boolean_t;

typedef enum MBMS_flag_e {
    MBMS_FLAG_NO       = 0,
    MBMS_FLAG_YES      = 1,
} MBMS_flag_t;

typedef enum eNB_flag_e {
    ENB_FLAG_NO       = 0,
    ENB_FLAG_YES      = 1,
} eNB_flag_t;

// just for integration
extern frame_t    frame;

#endif
