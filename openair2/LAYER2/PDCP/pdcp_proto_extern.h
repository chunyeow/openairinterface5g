/*
                             pdcp_proto_extern.h
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#ifndef __PDCP_PROTO_EXTERN_H__
#    define __PDCP_PROTO_EXTERN_H__

//#    include "pdcp_entity.h"
//#    include "rb_mux.h"
#    include "mem_block.h"

#ifdef ROHC
extern void     pdcp_data_ind (module_id_t module_idP, rb_id_t rab_idP, sdu_size_t data_sizeP, mem_block_t * sduP);
extern void     pdcp_data_req (struct pdcp_entity *pdcpP, mem_block * sduP);
extern void     send_pdcp_control_primitive (struct pdcp_entity *pdcpP, mem_block * cprimitiveP);
extern void     control_pdcp (struct pdcp_entity *pdcpP);
extern void	pdcp_process_input_sdus_high(struct pdcp_entity *pdcpP);
extern void     pdcp_process_input_sdus_am (struct pdcp_entity *pdcpP, u16 data_sizeP, mem_block * sduP);
extern void     pdcp_process_output_sdus (struct pdcp_entity *pdcpP, mem_block * sduP, u8 rb_idP);
extern void 	pdcp_process_output_sdus_high (struct pdcp_entity *pdcpP, mem_block * sduP, u16 data_sizeP,  u16 rb_idP);
extern void     pdcp_process_input_sdus_um (struct pdcp_entity *pdcpP, u16 data_sizeP, mem_block * sduP);
extern void     pdcp_process_input_sdus_tr (struct pdcp_entity *pdcpP, u16 data_sizeP, mem_block * sduP);
extern void     init_pdcp (struct pdcp_entity *pdcpP, struct rb_dispatcher *rbP, u8 rb_idP);
extern void    *pdcp_tx (struct pdcp_entity *pdcpP, u16 data_sizeP, mem_block * sduP);
extern int 	reception_from_rohc_mt(void);
extern int 	reception_from_rohc_bs(void);
#else
extern void     pdcp_data_ind (module_id_t module_idP, rb_id_t rab_idP, sdu_size_t data_sizeP, mem_block_t * sduP);
extern void     pdcp_data_req (struct pdcp_entity *pdcpP, mem_block * sduP);
extern void     send_pdcp_control_primitive (struct pdcp_entity *pdcpP, mem_block * cprimitiveP);
extern void     control_pdcp (struct pdcp_entity *pdcpP);
extern void     pdcp_process_input_sdus_am (struct pdcp_entity *pdcpP);
extern void     pdcp_process_output_sdus (struct pdcp_entity *pdcpP, mem_block * sduP, u8 rb_idP);
extern void     pdcp_process_input_sdus_um (struct pdcp_entity *pdcpP);
extern void     pdcp_process_input_sdus_tr (struct pdcp_entity *pdcpP);
extern void     init_pdcp (struct pdcp_entity *pdcpP, struct rb_dispatcher *rbP, u8 rb_idP);
extern void    *pdcp_tx (void *argP);
#endif
#endif
