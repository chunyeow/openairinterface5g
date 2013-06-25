#include "PHY/types.h"



typedef enum {
  ue_selected,
  wideband_cqi_rank1_2A, //wideband_cqi_rank1_2A,
  wideband_cqi_rank2_2A, //wideband_cqi_rank2_2A,
  HLC_subband_cqi_nopmi, //HLC_subband_cqi_nopmi,
  HLC_subband_cqi_rank1_2A, //HLC_subband_cqi_rank1_2A,
  HLC_subband_cqi_rank2_2A, //HLC_subband_cqi_rank2_2A,
  HLC_subband_cqi_modes123, //HLC_subband_cqi_modes123
  HLC_subband_cqi_mcs_CBA,
  unknown_cqi// 
} UCI_format_t;

typedef struct __attribute__((packed)) {
  u32 padding:14;
  u32 pmi:14; 
  u32 cqi1:4;
} wideband_cqi_rank1_2A_5MHz ; 
#define sizeof_wideband_cqi_rank1_2A_5MHz 18

typedef struct __attribute__((packed)) {
  u16 padding:1;
  u16 pmi:7; 
  u16 cqi2:4;
  u16 cqi1:4;
} wideband_cqi_rank2_2A_5MHz ; 
#define sizeof_wideband_cqi_rank2_2A_5MHz 15

typedef struct __attribute__((packed)) { 
  u32 padding:14;
  u32 diffcqi1:14;
  u32 cqi1:4;
} HLC_subband_cqi_nopmi_5MHz;
#define sizeof_HLC_subband_cqi_nopmi_5MHz 18

typedef struct __attribute__((packed)) { 
  u32 padding:12;
  u32 pmi:2;
  u32 diffcqi1:14;   
  u32 cqi1:4;
} HLC_subband_cqi_rank1_2A_5MHz;
#define sizeof_HLC_subband_cqi_rank1_2A_5MHz 20

typedef struct __attribute__((packed)) { 
  u64 padding:27;
  u64 pmi:1;
  u64 diffcqi2:14;   
  u64 cqi2:4;
  u64 diffcqi1:14;   
  u64 cqi1:4;
} HLC_subband_cqi_rank2_2A_5MHz;
#define sizeof_HLC_subband_cqi_rank2_2A_5MHz 37

typedef struct __attribute__((packed)) { 
  u32 padding:14;
  u32 diffcqi1:14;   
  u32 cqi1:4;
} HLC_subband_cqi_modes123_5MHz;
#define sizeof_HLC_subband_cqi_modes123_5MHz 18

typedef struct __attribute__((packed)) { 
  u32 padding:12;
  u32 crnti:16;   
  u32 mcs:4;
} HLC_subband_cqi_mcs_CBA_5MHz;
#define sizeof_HLC_subband_cqi_mcs_CBA_5MHz 20



#define MAX_CQI_PAYLOAD (sizeof(HLC_subband_cqi_rank2_2A_5MHz)*8*20)
#define MAX_CQI_BITS (sizeof(HLC_subband_cqi_rank2_2A_5MHz)*8)
#define MAX_CQI_BYTES (sizeof(HLC_subband_cqi_rank2_2A_5MHz))
#define MAX_ACK_PAYLOAD 18
#define MAX_RI_PAYLOAD 6
