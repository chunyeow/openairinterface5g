/***************************************************************************
                          rrc_proto_rrm.h
                          -------------------
    copyright            : (C) 2010 by Eurecom
    created by	         : Lionel.Gauthier@eurecom.fr	
    modified by          : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  prototypes of RRM functions
 **************************************************************************/
#ifndef __RRC_PROTO_RRM_H__
#define __RRC_PROTO_RRM_H__

extern int  rrc_rrm_main_proc (void);
extern void rrc_rg_rrm_connected_init (void);

void rrc_rg_init_check_qos_classes(void);
//void init_rrc_handler (void);
void rrc_rrm_rcve_config (u8 *serialized_configP, int lengthP);
//void rrc_rx_decode_buffer (void);
void rrc_rrm_decode_message (void);

/*--------------------------------------------------------------------*/
//void rrc_null (void *mP, int lengthP);
void rrc_connection_response (void *mP, int lengthP);
void rrc_add_user_response (void *mP, int lengthP);
void rrc_remove_user_response (void *mP, int lengthP);
void rrc_add_radio_access_bearer_response (void *mP, int lengthP);
void rrc_remove_radio_access_bearer_response (void *mP, int lengthP);
//void rrc_connection_close_ack (void *mP, int lengthP);
void rrc_measurement_request (void *mP, int lengthP);


#endif
