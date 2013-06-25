/*
                             rlc_um_proto_extern.h
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
 ***************************************************************************/
#    ifndef __RLC_UM_PROTO_EXTERN_H__
#        define __RLC_UM_PROTO_EXTERN_H__

#        include "rlc_um_entity.h"
#        include "mem_block.h"

extern void     rlc_um_stat_req     (struct rlc_um_entity *rlcP, 
							  unsigned int* tx_pdcp_sdu,
							  unsigned int* tx_pdcp_sdu_discarded,
							  unsigned int* tx_data_pdu,
							  unsigned int* rx_sdu,
							  unsigned int* rx_error_pdu,  
							  unsigned int* rx_data_pdu,
							  unsigned int* rx_data_pdu_out_of_window);
extern void     rlc_um_get_pdus (void *argP);
extern void     rlc_um_rx (void *argP, struct mac_data_ind data_indP);
extern struct mac_status_resp rlc_um_mac_status_indication (void *rlcP, u16_t no_tbP, u16 tb_sizeP, struct mac_status_ind tx_statusP);
extern struct mac_data_req rlc_um_mac_data_request (void *rlcP);
extern void     rlc_um_mac_data_indication (void *rlcP, struct mac_data_ind data_indP);
extern void     rlc_um_data_req (void *rlcP, mem_block_t *sduP);
#    endif
