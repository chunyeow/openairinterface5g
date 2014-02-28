/*
                             pdcp_primitives.h
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr


 ***************************************************************************/
#ifndef __PDCP_PRIMITIVES_H__
#    define __PDCP_PRIMITIVES_H__

#    include "platform.h"
//----------------------------------------------------------
// primitives
//----------------------------------------------------------
#    define PDCP_DATA_REQ     0x01
#    define PDCP_DATA_IND     0x02
//----------------------------------------------------------
// control primitives
//----------------------------------------------------------
#    define CPDCP_CONFIG_REQ  0x04
#    define CPDCP_RELEASE_REQ 0x08
#    define CPDCP_SN_REQ      0x10
#    define CPDCP_RELOC_REQ   0x20
#    define CPDCP_RELOC_CNF   0x40
//----------------------------------------------------------
// primitives definition
//----------------------------------------------------------
struct pdcp_data_req {
  uint16_t             rb_id;
  uint16_t             data_size;
};
struct pdcp_data_ind {
  uint16_t             rb_id;
  uint16_t             data_size;
};

//----------------------------------------------------------
// control primitives definition
//----------------------------------------------------------
// TO DO
struct cpdcp_config_req {
  void           *rlc_sap;
  uint8_t              rlc_type_sap; // am, um, tr
  uint8_t              header_compression_type;
};
struct cpdcp_release_req {
  void           *rlc_sap;
};

struct cpdcp_sn_req {
  uint32_t             sn;
};

struct cpdcp_relloc_req {
  uint32_t             receive_sn;
};

struct cpdcp_relloc_conf {
  uint32_t             receive_sn;
  uint32_t             send_sn;
};

struct cpdcp_primitive {
  uint8_t              type;
  union {
    struct cpdcp_config_req config_req;
    struct cpdcp_release_req release_req;
    struct cpdcp_sn_req sn_req;
    struct cpdcp_relloc_req relloc_req;
    struct cpdcp_relloc_conf relloc_conf;
  } primitive;
};


#endif
