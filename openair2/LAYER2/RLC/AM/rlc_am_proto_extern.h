/***************************************************************************
                          rlc_am_proto_extern.h  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#    ifndef __RLC_AM_PROTO_EXTERN_H__
#        define __RLC_AM_PROTO_EXTERN_H__
//-----------------------------------------------------------------------------
#        include "platform_types.h"
#        include "platform_constants.h"
#        include "list.h"
#        include "rlc_am_entity.h"
#        include "mac_primitives.h"
#        include "mem_block.h"
//-----------------------------------------------------------------------------
extern void     display_protocol_vars_rlc_am (struct rlc_am_entity *rlcP);
extern u32_t      rlc_am_get_buffer_occupancy (struct rlc_am_entity *rlcP, u8_t logical_channelsP);
extern void     init_rlc_am (struct rlc_am_entity *rlcP);
extern void    *rlc_am_tx (void *argP);
extern void     rlc_am_rx (void *argP, struct mac_data_ind data_indP);
extern void    *init_code_rlc_am (void *t);
extern void     send_rlc_am_control_primitive (struct rlc_am_entity *rlcP, module_id_t module_idP, mem_block_t * cprimitiveP);
extern void     rlc_am_send_mac_data_request (void *macP, u8_t logical_channel_identityP, list_t * pduP);

extern struct mac_status_resp rlc_am_mac_status_indication (void *rlcP, u16_t no_tbP, u16 tb_sizeP, struct mac_status_ind tx_statusP);
extern struct mac_status_resp rlc_am_mac_status_indication_on_first_channel (void *rlcP, u16_t no_tbP, u16 tb_sizeP, struct mac_status_ind tx_statusP);
extern struct mac_status_resp rlc_am_mac_status_indication_on_second_channel (void *rlcP, u16_t no_tbP, u16 tb_sizeP, struct mac_status_ind tx_statusP);
extern struct mac_data_req rlc_am_mac_data_request (void *rlcP);
extern struct mac_data_req rlc_am_mac_data_request_on_first_channel (void *rlcP);
extern struct mac_data_req rlc_am_mac_data_request_on_second_channel (void *rlcP);
extern void     rlc_am_mac_data_indication (void *rlcP, struct mac_data_ind data_indP);
extern void     rlc_am_data_req (void *rlcP, mem_block_t * sduP);
#    endif
