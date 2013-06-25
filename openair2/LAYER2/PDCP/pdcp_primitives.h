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
  u16             rb_id;
  u16             data_size;
};
struct pdcp_data_ind {
  u16             rb_id;
  u16             data_size;
};

//----------------------------------------------------------
// control primitives definition
//----------------------------------------------------------
// TO DO
struct cpdcp_config_req {
  void           *rlc_sap;
  u8              rlc_type_sap; // am, um, tr
  u8              header_compression_type;
};
struct cpdcp_release_req {
  void           *rlc_sap;
};

struct cpdcp_sn_req {
  u32             sn;
};

struct cpdcp_relloc_req {
  u32             receive_sn;
};

struct cpdcp_relloc_conf {
  u32             receive_sn;
  u32             send_sn;
};

struct cpdcp_primitive {
  u8              type;
  union {
    struct cpdcp_config_req config_req;
    struct cpdcp_release_req release_req;
    struct cpdcp_sn_req sn_req;
    struct cpdcp_relloc_req relloc_req;
    struct cpdcp_relloc_conf relloc_conf;
  } primitive;
};


#endif
