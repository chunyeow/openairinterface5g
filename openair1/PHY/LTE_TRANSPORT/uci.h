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
  uint32_t padding:14;
  uint32_t pmi:14; 
  uint32_t cqi1:4;
} wideband_cqi_rank1_2A_5MHz ; 
#define sizeof_wideband_cqi_rank1_2A_5MHz 18

typedef struct __attribute__((packed)) {
  uint16_t padding:1;
  uint16_t pmi:7; 
  uint16_t cqi2:4;
  uint16_t cqi1:4;
} wideband_cqi_rank2_2A_5MHz ; 
#define sizeof_wideband_cqi_rank2_2A_5MHz 15

typedef struct __attribute__((packed)) { 
  uint32_t padding:14;
  uint32_t diffcqi1:14;
  uint32_t cqi1:4;
} HLC_subband_cqi_nopmi_5MHz;
#define sizeof_HLC_subband_cqi_nopmi_5MHz 18

typedef struct __attribute__((packed)) { 
  uint32_t padding:12;
  uint32_t pmi:2;
  uint32_t diffcqi1:14;   
  uint32_t cqi1:4;
} HLC_subband_cqi_rank1_2A_5MHz;
#define sizeof_HLC_subband_cqi_rank1_2A_5MHz 20

typedef struct __attribute__((packed)) { 
  uint64_t padding:27;
  uint64_t pmi:1;
  uint64_t diffcqi2:14;   
  uint64_t cqi2:4;
  uint64_t diffcqi1:14;   
  uint64_t cqi1:4;
} HLC_subband_cqi_rank2_2A_5MHz;
#define sizeof_HLC_subband_cqi_rank2_2A_5MHz 37

typedef struct __attribute__((packed)) { 
  uint32_t padding:14;
  uint32_t diffcqi1:14;   
  uint32_t cqi1:4;
} HLC_subband_cqi_modes123_5MHz;
#define sizeof_HLC_subband_cqi_modes123_5MHz 18

typedef struct __attribute__((packed)) { 
  uint32_t padding:12;
  uint32_t crnti:16;   
  uint32_t mcs:4;
} HLC_subband_cqi_mcs_CBA_5MHz;
#define sizeof_HLC_subband_cqi_mcs_CBA_5MHz 20



#define MAX_CQI_PAYLOAD (sizeof(HLC_subband_cqi_rank2_2A_5MHz)*8*20)
#define MAX_CQI_BITS (sizeof(HLC_subband_cqi_rank2_2A_5MHz)*8)
#define MAX_CQI_BYTES (sizeof(HLC_subband_cqi_rank2_2A_5MHz))
#define MAX_ACK_PAYLOAD 18
#define MAX_RI_PAYLOAD 6
