/*
                               pdcp_entity.h
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#ifndef __PDCP_ENTITY_H__
#    define __PDCP_ENTITY_H__

#    include "list.h"


struct pdcp_entity {
  unsigned short             id;           // for resource management purpose
  unsigned char              allocation;
  void           *rlc_sap;
  //-----------------------------
  // C-SAP
  //-----------------------------
  list            c_sap;
  //-----------------------------
  // tranmission
  //-----------------------------
  list            input_sdu_list;
  void           *rlc;
  //-----------------------------
  // receiver
  //-----------------------------
  //struct list     output_sdu_list;

#ifdef ROHC
  void            (*pdcp_process_input_sdus) (struct pdcp_entity *pdcpP, u16 data_sizeP, mem_block * sduP);
#else
  void            (*pdcp_process_input_sdus) (struct pdcp_entity *pdcpP);
#endif

  unsigned char              rb_id;        // for traces
  unsigned char              rlc_type_sap; // am, um, tr

  struct rb_dispatcher *rb_dispatch;
};
#endif
