/***************************************************************************
                          rrc_rg_rrm_process.c
                          -------------------
    copyright            : (C) 2009, 2010 by Eurecom
    created by	         : Lionel.Gauthier@eurecom.fr	
    modified by          : Michelle.Wetterwald@eurecom.fr
 **************************************************************************
  main process for RRM interface
 **************************************************************************/
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"
#include "rtos_header.h"

/********************
// RRC definitions
 ********************/
#include "rrc_rg_vars.h"
#include "rrc_rrm_sap.h"
//#include "rrc_mbms_constant.h"
#include "rrc_rrm_primitives.h"
//#include "rrm_config_structs.h"
//#include "rrc_rg_rrm_intf.h"
//-----------------------------------------------------------------------------
#include "rrc_proto_int.h"
#include "rrc_proto_fsm.h"
#include "rrc_proto_intf.h"
#include "rrc_proto_bch.h"
#include "rrc_proto_mbms.h"
#include "rrc_proto_rrm.h"
//-----------------------------------------------------------------------------
// For FIFOS interface
#ifdef USER_MODE
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif


//protocol_bs->rrc.rc_rrm.
#ifdef USER_MODE
//-----------------------------------------------------------------------------
// Create and initialize FIFOs for RG RRM-RRC SAPs
void rrc_rg_rrm_sap_init (void){
//-----------------------------------------------------------------------------
  int write_flag = O_WRONLY | O_NONBLOCK | O_NDELAY;
  int read_flag = O_RDONLY | O_NONBLOCK | O_NDELAY;

  // Create FIFOs
  rrc_create_fifo (RRM_INPUT_SAPI);
  rrc_create_fifo (RRM_OUTPUT_SAPI);

  // Open FIFOs
  while ((protocol_bs->rrc.rc_rrm.input_fifo = open (RRM_INPUT_SAPI, read_flag)) < 0) {
    msg ("%s returned value %d\n", RRM_INPUT_SAPI, protocol_bs->rrc.rc_rrm.input_fifo);
    perror("RRM_INPUT_SAPI - open failed: ");
    sleep (1);
  }
  msg("FIFO opened %s\n", RRM_INPUT_SAPI);

  while ((protocol_bs->rrc.rc_rrm.output_fifo = open (RRM_OUTPUT_SAPI, write_flag)) < 0) {
    msg ("%s returned value %d\n", RRM_OUTPUT_SAPI, protocol_bs->rrc.rc_rrm.output_fifo);
    perror("RRM_OUTPUT_SAPI - open failed: ");
    sleep (1);
  }
  msg("FIFO opened %s\n", RRM_OUTPUT_SAPI);

  // Print result
  msg ("%s returned value %d\n", "RRM_INPUT_SAPI", protocol_bs->rrc.rc_rrm.input_fifo);
  msg ("%s returned value %d\n", "RRM_OUTPUT_SAPI", protocol_bs->rrc.rc_rrm.output_fifo);
}
#endif

//-----------------------------------------------------------------------------
int rrc_rrm_main_proc (void){
//-----------------------------------------------------------------------------
  int bytes_read =0;
 // int tx_id;
  rpc_message  rpc_mess;
  connection_request cr;
  int count = 0;

   // prevoir ifdef
  //msg ("[RRC-RRM-INTF] rrc_rrm_main_proc state %d\n", protocol_bs->rrc.rc_rrm.connected_to_rrm);

  bytes_read = rtf_get (protocol_bs->rrc.rc_rrm.input_fifo, &protocol_bs->rrc.rc_rrm.rx_buffer, RRC_RRM_RX_BUFFER_SIZE);
//  bytes_read = rtf_get (protocol_bs->rrc.rc_rrm.input_fifo, &protocol_bs->rrc.rc_rrm.rx_buffer[protocol_bs->rrc.rc_rrm.rx_bytes], RRC_RRM_RX_BUFFER_SIZE);
  // send CONNECTION_REQUEST TO RRM if not CONNECTED
  if (protocol_bs->rrc.rc_rrm.connected_to_rrm != RRC_CONNECTED_TO_RRM) {
//    if ((((frame + 1) % (RPC_CONNECTION_REQUEST_TIME_OUT / 10)) == 0) || (frame == 6)) {
    if ((((protocol_bs->rrc.current_SFN + 1) % (RPC_CONNECTION_REQUEST_TIME_OUT / 10)) == 0) || (protocol_bs->rrc.current_SFN == 6)) {
      rpc_mess.type = RPC_CONNECTION_REQUEST;
      rpc_mess.length = sizeof (connection_request);
      #ifdef NODE_RG_SLAVE
      cr.radio_gateway_type = RADIO_GATEWAY_SLAVE;
      #else
      cr.radio_gateway_type = RADIO_GATEWAY_MASTER;
      #endif

      #ifdef USER_MODE
      cr.radio_gateway_execution_mode = RADIO_GATEWAY_SIMULATION;
      #else
      cr.radio_gateway_execution_mode = RADIO_GATEWAY_REAL_TIME_RF;
      #endif
      cr.nb_antennas = numANTENNAS;
//      msg ("[RRC-RRM-INTF] sending CONNECTION_REQUEST %d + %d bytes to RRM througth FIFO frame %d\n", sizeof (rpc_message), sizeof (connection_request), frame);
      msg ("[RRC-RRM-INTF] sending CONNECTION_REQUEST %d + %d bytes to RRM through FIFO frame %d\n", sizeof (rpc_message), sizeof (connection_request), protocol_bs->rrc.current_SFN);

      count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & rpc_mess, sizeof (rpc_message));
   if (count == sizeof (rpc_message)) {
   #ifdef DEBUG_RRC_STATE
     msg ("[RRC-RRM-INTF] RRM message sent successfully, length %d \n", count);
   #endif
   } else {
     msg ("[RRC-RRM-INTF] transmission on FIFO failed, %d bytes sent instaed of %d\n", count, sizeof (rpc_message));
   }

      count = rtf_put (protocol_bs->rrc.rc_rrm.output_fifo, (u8 *) & cr, sizeof (connection_request));
   if (count == sizeof (connection_request)) {
   #ifdef DEBUG_RRC_STATE
     msg ("[RRC-RRM-INTF] RRM message sent successfully, length %d \n", count);
   #endif
   } else {
     msg ("[RRC-RRM-INTF] transmission on FIFO failed, %d bytes sent instaed of %d\n", count, sizeof(connection_request));
   }

       #ifdef USER_MODE
      sleep (2);
       #endif
    }
  }
  if (bytes_read > 0) {
    // interface beetween RRM and RRC...
    msg("[RRC RRM][AS] RX %d bytes from RRM\n", bytes_read);
    protocol_bs->rrc.rc_rrm.rx_bytes += bytes_read;
    rrc_rrm_decode_message ();
  }

  return protocol_bs->rrc.rc_rrm.connected_to_rrm;
}

