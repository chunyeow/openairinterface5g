/*
                                rlc_mac.c
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
*/

//-----------------------------------------------------------------------------
#define RLC_MAC_C
#include "rlc.h"
#include "LAYER2/MAC/extern.h"
#include "UTIL/LOG/log.h"

#define DEBUG_MAC_INTERFACE

// tb_size_t in bytes
//-----------------------------------------------------------------------------
struct mac_data_ind mac_rlc_deserialize_tb (char* bufferP, tb_size_t tb_sizeP, num_tb_t num_tbP, crc_t *crcsP) {
//-----------------------------------------------------------------------------

  struct mac_data_ind  data_ind;
  mem_block_t*         tb;
  num_tb_t             nb_tb_read;
  tbs_size_t   tbs_size;

  nb_tb_read = 0;
  tbs_size   = 0;
  list_init(&data_ind.data, NULL);

  while (num_tbP > 0) {
        tb = get_free_mem_block(sizeof (mac_rlc_max_rx_header_size_t) + tb_sizeP);
        if (tb != NULL) {
            ((struct mac_tb_ind *) (tb->data))->first_bit = 0;
            ((struct mac_tb_ind *) (tb->data))->data_ptr = (u8_t*)&tb->data[sizeof (mac_rlc_max_rx_header_size_t)];
            ((struct mac_tb_ind *) (tb->data))->size = tb_sizeP;
            if (crcsP)
                ((struct mac_tb_ind *) (tb->data))->error_indication = crcsP[nb_tb_read];
            else
                ((struct mac_tb_ind *) (tb->data))->error_indication = 0;

            memcpy(((struct mac_tb_ind *) (tb->data))->data_ptr, &bufferP[tbs_size], tb_sizeP);

#ifdef DEBUG_MAC_INTERFACE
            LOG_T(RLC, "[MAC-RLC] DUMP RX PDU(%d bytes):\n", tb_sizeP);
            rlc_util_print_hex_octets(RLC, ((struct mac_tb_ind *) (tb->data))->data_ptr, tb_sizeP);
#endif
            nb_tb_read = nb_tb_read + 1;
            tbs_size   = tbs_size   + tb_sizeP;
            list_add_tail_eurecom(tb, &data_ind.data);
        }
        num_tbP = num_tbP - 1;
  }
  data_ind.no_tb            = nb_tb_read;
  data_ind.tb_size          = tb_sizeP << 3;

  return data_ind;
}
//-----------------------------------------------------------------------------
tbs_size_t mac_rlc_serialize_tb (char* bufferP, list_t transport_blocksP) {
//-----------------------------------------------------------------------------
  mem_block_t* tb;
  tbs_size_t   tbs_size;
  tbs_size_t   tb_size;

  tbs_size = 0;
  while (transport_blocksP.nb_elements > 0) {
    tb = list_remove_head (&transport_blocksP);
    if (tb != NULL) {
       tb_size = ((struct mac_tb_req *) (tb->data))->tb_size_in_bits>>3;
#ifdef DEBUG_MAC_INTERFACE
        LOG_T(RLC, "[MAC-RLC] DUMP TX PDU(%d bytes):\n", tb_size);
        rlc_util_print_hex_octets(RLC, ((struct mac_tb_req *) (tb->data))->data_ptr, tb_size);
#endif
       memcpy(&bufferP[tbs_size], &((struct mac_tb_req *) (tb->data))->data_ptr[0], tb_size);
       tbs_size = tbs_size + tb_size;
       free_mem_block(tb);
    }
  }
  return tbs_size;
}
//-----------------------------------------------------------------------------
tbs_size_t mac_rlc_data_req     (module_id_t module_idP, u32_t frame, u8_t MBMS_flagP, chan_id_t channel_idP, char* bufferP) {
//-----------------------------------------------------------------------------
    struct mac_data_req    data_request;
    rb_id_t                rb_id;
#ifdef DEBUG_MAC_INTERFACE
    LOG_D(RLC, "\n[RLC] Inst %d(%d): MAC_RLC_DATA_REQ channel %d (%d) MAX RB %d, Num_tb %d\n",
	    module_idP,MAX_MODULES,  channel_idP, RLC_MAX_LC, RLC_MAX_RB);
    
#endif // DEBUG_MAC_INTERFACE

    if ((module_idP >= 0) && (module_idP < MAX_MODULES)) {
        if ((channel_idP >= 0) && (channel_idP < RLC_MAX_LC)) {
            rb_id = rlc[module_idP].m_lcid2rbid[channel_idP];
            if ((rb_id > 0) && (rb_id < RLC_MAX_RB)) {
                switch (rlc[module_idP].m_rlc_pointer[rb_id].rlc_type) {
                    case RLC_NONE:
                        //handle_event(WARNING,"FILE %s FONCTION mac_rlc_data_ind() LINE %s : no radio bearer configured :%d\n", __FILE__, __LINE__, channel_idP);
                        break;

                    case RLC_AM:
                        data_request = rlc_am_mac_data_request(&rlc[module_idP].m_rlc_am_array[rlc[module_idP].m_rlc_pointer[rb_id].rlc_index], frame);
                        return mac_rlc_serialize_tb(bufferP, data_request.data);
                        break;

                    case RLC_UM:
                        data_request = rlc_um_mac_data_request(&rlc[module_idP].m_rlc_um_array[rlc[module_idP].m_rlc_pointer[rb_id].rlc_index], frame);
                        return mac_rlc_serialize_tb(bufferP, data_request.data);
                        break;

                    case RLC_TM:
                        data_request = rlc_tm_mac_data_request(&rlc[module_idP].m_rlc_tm_array[rlc[module_idP].m_rlc_pointer[rb_id].rlc_index], frame);
                        return mac_rlc_serialize_tb(bufferP, data_request.data);
                        break;

                    default:;
                        //handle_event(ERROR,"FILE %s FONCTION mac_rlc_data_ind() LINE %s : no RLC found for this radio bearer %d\n", __FILE__, __LINE__, channel_idP);
                }
            } else {
                LOG_E(RLC, "%s() : radio bearer id out of bounds :%d\n", __FUNCTION__, rb_id);
            }
      } else {
          //handle_event(ERROR,"FILE %s FONCTION mac_rlc_data_ind() LINE %s : parameter rb_id out of bounds :%d\n", __FILE__, __LINE__, channel_idP);
          LOG_E(RLC, "%s() : parameter channel_id out of bounds :%d\n", __FUNCTION__, channel_idP);
          return(-1);
      }
  } else {
      LOG_E(RLC, "%s() : parameter module_id out of bounds :%d\n", __FUNCTION__, module_idP);
  }
  return (tbs_size_t)0;
}
//-----------------------------------------------------------------------------
void mac_rlc_data_ind     (module_id_t module_idP,  u32_t frame, u8_t eNB_flag, u8_t MBMS_flagP, chan_id_t channel_idP, char* bufferP, tb_size_t tb_sizeP, num_tb_t num_tbP, crc_t *crcs) {
//-----------------------------------------------------------------------------
    rb_id_t                rb_id;
#ifdef DEBUG_MAC_INTERFACE
    if (num_tbP) {
      LOG_D(RLC, "\n[RLC] Inst %d(%d): MAC_RLC_DATA_IND on channel %d (%d), rb max %d, Num_tb %d\n",
	    module_idP,MAX_MODULES,  channel_idP, RLC_MAX_LC, RLC_MAX_RB, num_tbP);
    }
#endif // DEBUG_MAC_INTERFACE

    if ((module_idP >= 0) && (module_idP < MAX_MODULES)) {
        if ((channel_idP >= 0) && (channel_idP < RLC_MAX_LC)) {
            rb_id = rlc[module_idP].m_lcid2rbid[channel_idP];
            if ((rb_id > 0) && (rb_id < RLC_MAX_RB)) {
                //if (num_tbP > 0) {
                struct mac_data_ind data_ind = mac_rlc_deserialize_tb(bufferP, tb_sizeP, num_tbP, crcs);

                switch (rlc[module_idP].m_rlc_pointer[rb_id].rlc_type) {
                    case RLC_NONE:
                        //handle_event(WARNING,"FILE %s FONCTION mac_rlc_data_ind() LINE %s : no radio bearer configured :%d\n", __FILE__, __LINE__, rb_idP);
                        LOG_W(RLC, " FONCTION mac_rlc_data_ind()  : no radio bearer configured :%d\n",  rb_id);
                        break;

                    case RLC_AM:
#ifdef DEBUG_MAC_INTERFACE
		      LOG_D(RLC, "MAC DATA IND TO RLC_AM MOD_ID %d RB_INDEX %d (%d) MOD_ID_RLC %d\n", module_idP, rlc[module_idP].m_rlc_pointer[rb_id].rlc_index, rb_id, rlc[module_idP].m_rlc_am_array[rlc[module_idP].m_rlc_pointer[rb_id].rlc_index].module_id);
#endif

                        rlc_am_mac_data_indication(&rlc[module_idP].m_rlc_am_array[rlc[module_idP].m_rlc_pointer[rb_id].rlc_index], frame, eNB_flag, data_ind);
                        break;

                    case RLC_UM:
#ifdef DEBUG_MAC_INTERFACE
                        LOG_D(RLC, "MAC DATA IND TO RLC_UM MOD_ID %d RB_INDEX %d MOD_ID_RLC %d\n", module_idP, rlc[module_idP].m_rlc_pointer[rb_id].rlc_index, rlc[module_idP].m_rlc_um_array[rlc[module_idP].m_rlc_pointer[rb_id].rlc_index].module_id);
#endif
                        rlc_um_mac_data_indication(&rlc[module_idP].m_rlc_um_array[rlc[module_idP].m_rlc_pointer[rb_id].rlc_index], frame, eNB_flag, data_ind);
                        break;

                    case RLC_TM:
#ifdef DEBUG_MAC_INTERFACE
                        LOG_D(RLC, "MAC DATA IND TO RLC_TM MOD_ID %d RB_INDEX %d MOD_ID_RLC %d\n", module_idP, rlc[module_idP].m_rlc_pointer[rb_id].rlc_index, rlc[module_idP].m_rlc_tm_array[rlc[module_idP].m_rlc_pointer[rb_id].rlc_index].module_id);
#endif
                        rlc_tm_mac_data_indication(&rlc[module_idP].m_rlc_tm_array[rlc[module_idP].m_rlc_pointer[rb_id].rlc_index], frame, eNB_flag, data_ind);
                        break;

                    default:
                        //handle_event(ERROR,"FILE %s FONCTION mac_rlc_data_ind() LINE %s : no RLC found for this radio bearer %d\n", __FILE__, __LINE__, rb_idP);
                        LOG_W(RLC, "FILE  FONCTION mac_rlc_data_ind() LINE  : no RLC found for this radio bearer %d\n",  rb_id);
                        ;

                }
            } else {
                LOG_E(RLC, "%s() : radio bearer id out of bounds : rb is %d\n", __FUNCTION__, rb_id);
            }
        } else {
            LOG_E(RLC, "%s() : parameter channel_id out of bounds : channel is %d\n", __FUNCTION__, channel_idP);
        }
    } else {
        LOG_E(RLC, "%s() : parameter module_id out of bounds : module id is %d\n", __FUNCTION__, module_idP);
    }
}
//-----------------------------------------------------------------------------
mac_rlc_status_resp_t mac_rlc_status_ind     (module_id_t module_idP, u32_t frame, u8_t eNB_flag, u8_t MBMS_flagP, chan_id_t channel_idP, tb_size_t tb_sizeP) {
//-----------------------------------------------------------------------------
  rb_id_t                rb_id = 0;
  mac_rlc_status_resp_t  mac_rlc_status_resp;

  mac_rlc_status_resp.bytes_in_buffer = 0;
  mac_rlc_status_resp.pdus_in_buffer  = 0;

  if ((module_idP >= 0) && (module_idP < MAX_MODULES)) {
      if ((channel_idP >= 0) && (channel_idP < RLC_MAX_LC)) {
          rb_id = rlc[module_idP].m_lcid2rbid[channel_idP];
          if ((rb_id > 0) && (rb_id < RLC_MAX_RB)) {
              struct mac_status_resp status_resp;
              struct mac_status_ind tx_status;
                  switch (rlc[module_idP].m_rlc_pointer[channel_idP].rlc_type) {
                    case RLC_NONE:
                        //handle_event(WARNING,"FILE %s FONCTION mac_rlc_data_ind() LINE %s : no radio bearer configured :%d\n", __FILE__, __LINE__, channel_idP);
                        break;

                    case RLC_AM:
                        status_resp = rlc_am_mac_status_indication(&rlc[module_idP].m_rlc_am_array[rlc[module_idP].m_rlc_pointer[channel_idP].rlc_index], frame, tb_sizeP, tx_status);
                        mac_rlc_status_resp.bytes_in_buffer = status_resp.buffer_occupancy_in_bytes;
                        mac_rlc_status_resp.pdus_in_buffer = status_resp.buffer_occupancy_in_pdus;
                        return mac_rlc_status_resp;
                        break;

                    case RLC_UM:
                        //msg("[RLC_UM][MOD %d] mac_rlc_status_ind  tb_size %d\n", module_idP,  tb_sizeP);
                        status_resp = rlc_um_mac_status_indication(&rlc[module_idP].m_rlc_um_array[rlc[module_idP].m_rlc_pointer[channel_idP].rlc_index], frame, eNB_flag, tb_sizeP, tx_status);
                        mac_rlc_status_resp.bytes_in_buffer = status_resp.buffer_occupancy_in_bytes;
                        //mac_rlc_status_resp.pdus_in_buffer = status_resp.buffer_occupancy_in_pdus;
                        return mac_rlc_status_resp;
                        break;

                    case RLC_TM:
                        status_resp = rlc_tm_mac_status_indication(&rlc[module_idP].m_rlc_tm_array[rlc[module_idP].m_rlc_pointer[channel_idP].rlc_index], frame, tb_sizeP, tx_status);
                        mac_rlc_status_resp.bytes_in_buffer = status_resp.buffer_occupancy_in_bytes;
                        mac_rlc_status_resp.pdus_in_buffer = status_resp.buffer_occupancy_in_pdus;
                        return mac_rlc_status_resp;
                        break;

                    default:;
                        //handle_event(ERROR,"FILE %s FONCTION mac_rlc_data_ind() LINE %s : no RLC found for this radio bearer %d\n", __FILE__, __LINE__, channel_idP);

                  }
          } else {
              // two many traces because MAC probe inactive channels. LOG_E(RLC, "%s(Mod=%d, eNB_flag=%d, mbms flag=%d,channel_id=%d) : radio bearer id out of bounds :%d \n", __FUNCTION__, module_idP, eNB_flag, MBMS_flagP, channel_idP, rb_id);
          }
      } else {
          LOG_E(RLC, "%s() : parameter channel_id out of bounds :%d\n", __FUNCTION__, channel_idP);
      }
  } else {
      LOG_E(RLC, "%s() : parameter module_id out of bounds :%d\n", __FUNCTION__, module_idP);
  }
  return mac_rlc_status_resp;
}

