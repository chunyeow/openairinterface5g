/***************************************************************************
                          rlc_am_discard_rx.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc_primitives.h"
#include "rlc_am_entity.h"
#include "rlc_am_structs.h"
#include "mem_block.h"
//-----------------------------------------------------------------------------
inline int      rlc_am_comp_sn (struct rlc_am_entity *rlcP, u16_t low_boundaryP, u16 sn1P, u16 sn2P);
void            adjust_vr_r_mr (struct rlc_am_entity *rlcP);
void            adjust_vt_a_ms (struct rlc_am_entity *rlcP);
void            display_protocol_vars_rlc_am (struct rlc_am_entity *rlcP);
void            display_retransmission_buffer (struct rlc_am_entity *rlcP);
void            display_receiver_buffer (struct rlc_am_entity *rlcP);
void            rlc_am_check_retransmission_buffer (struct rlc_am_entity *rlcP, u8_t * messageP);
void            rlc_am_check_receiver_buffer (struct rlc_am_entity *rlcP, u8_t * messageP);
void            rlc_am_display_data_pdu7 (mem_block_t * pduP);
//-----------------------------------------------------------------------------
#ifdef DEBUG_RLC_AM_CONFIRM
//-----------------------------------------------------------------------------
void
debug_rlc_am_confirm (struct rlc_am_entity *rlcP, mem_block_t * confP)
{
//-----------------------------------------------------------------------------
  msg ("[RLC_AM][RB %d] CONFIRM SDU MUI %d\n", rlcP->rb_id, ((struct rlc_output_primitive *) (confP->data))->primitive.conf.mui);
  free_mem_block_t (confP);
}
#endif /* DEBUG_RLC_AM_CONFIRM */

// return 1 if sn1 > sn2
// return 0 if sn1 = sn2
// return -1 if sn1 < sn2
//-----------------------------------------------------------------------------
inline int
rlc_am_comp_sn (struct rlc_am_entity *rlcP, u16_t low_boundaryP, u16 sn1P, u16 sn2P)
{
//-----------------------------------------------------------------------------
  // we require that sn are in tx window, if they are not we consider that they are always
  // before low_boundaryP (one loop over the max sn 0x0FFF), never above the boundary
  u16_t             bound = (low_boundaryP + rlcP->configured_tx_window_size - 1) & SN_12BITS_MASK;
  if (sn1P == sn2P) {
    return 0;
  }
  if (low_boundaryP < bound) {
    if ((sn1P >= low_boundaryP) && (sn1P <= bound)) {
      if ((sn2P >= low_boundaryP) && (sn2P <= bound)) {
        if (sn1P > sn2P) {
          return 1;
        } else {
          return -1;
        }
      } else {                  // not in tx window
        return 1;
      }
    } else {                    // not in tx window
      if ((sn2P >= low_boundaryP) && (sn2P <= bound)) {
        return -1;
      } else {
        if ((sn1P > sn2P) && (sn1P <= low_boundaryP)) {
          return 1;
        } else if ((sn2P > sn1P) && (sn2P <= low_boundaryP)) {
          return -1;
        } else if ((sn1P > sn2P) && (sn1P > bound)) {
          return -1;
        } else {
          return 1;
        }
      }
    }
  } else {
    if (sn1P >= low_boundaryP) {
      if (sn2P <= bound) {
        return -1;
      } else if ((sn2P >= low_boundaryP)) {
        if (sn1P > sn2P) {
          return 1;
        } else {
          return -1;
        }
      } else {                  // not in tx window
        return -1;
      }
    } else if (sn1P <= bound) {
      if (sn2P > bound) {
        return 1;
      } else {
        if (sn1P > sn2P) {
          return 1;
        } else {
          return -1;
        }
      }
    } else {                    // bound < sn1< vt_a
      if (sn2P >= low_boundaryP) {
        return -1;
      } else if (sn2P <= bound) {
        return -1;
      } else {
        if (sn1P > sn2P) {
          return 1;
        } else {
          return -1;
        }
      }
    }
  }
}

//-----------------------------------------------------------------------------
void
adjust_vt_a_ms (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------

  u16_t             vt_a_index, vt_s_index;

  vt_a_index = rlcP->vt_a % rlcP->recomputed_configured_tx_window_size;
  vt_s_index = rlcP->vt_s % rlcP->recomputed_configured_tx_window_size;

  while ((rlcP->retransmission_buffer[vt_a_index] == NULL) && (vt_a_index != vt_s_index)) {
    rlcP->vt_a = (rlcP->vt_a + 1) & SN_12BITS_MASK;
    vt_a_index = rlcP->vt_a % rlcP->recomputed_configured_tx_window_size;
  }
  rlcP->vt_ms = (rlcP->vt_a + rlcP->vt_ws - 1) & SN_12BITS_MASK;
}

//-----------------------------------------------------------------------------
void
adjust_vr_r_mr (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------

  u16_t             vr_r_index, vr_h_index;

  vr_r_index = rlcP->vr_r % rlcP->recomputed_configured_rx_window_size;
  vr_h_index = rlcP->vr_h % rlcP->recomputed_configured_rx_window_size;

  while ((rlcP->receiver_buffer[vr_r_index] != NULL) && (vr_r_index != vr_h_index)) {
    rlcP->vr_r = (rlcP->vr_r + 1) & SN_12BITS_MASK;
    vr_r_index = rlcP->vr_r % rlcP->recomputed_configured_rx_window_size;
  }

  rlcP->ack.vr_r_modified = 1;

#ifdef DEBUG_RECEIVER_BUFFER
  msg ("[RLC_AM][RB %d][RECEIVER_BUFFER] ADJUST VR(R)->0x%04X VR(H)=0x%04X\n", rlcP->rb_id, rlcP->vr_r, rlcP->vr_h);
#endif
  rlcP->vr_mr = (rlcP->vr_r + rlcP->configured_rx_window_size - 1) & SN_12BITS_MASK;
}

//-----------------------------------------------------------------------------
void
display_protocol_vars_rlc_am (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------

  msg ("[RLC_AM][RB %d] VT(A) 0x%04X  VT(S) 0x%04X  VT(MS) 0x%04X  VT(PDU) 0x%04X  VT(SDU) 0x%04X  VR(R) 0x%04X  VR(H) 0x%04X  VR(MR) 0x%04X\n",
       rlcP->rb_id, rlcP->vt_a, rlcP->vt_s, rlcP->vt_ms, rlcP->vt_pdu, rlcP->vt_sdu, rlcP->vr_r, rlcP->vr_h, rlcP->vr_mr);
}

//-----------------------------------------------------------------------------
void
display_retransmission_buffer (struct rlc_am_entity *rlcP)
{
  //-----------------------------------------------------------------------------

  u16_t             working_sn, working_sn_index;
  u16_t             end_sn, end_sn_index;

  working_sn = rlcP->vt_a;
  working_sn_index = working_sn % rlcP->recomputed_configured_tx_window_size;

  end_sn = (rlcP->vt_a + rlcP->recomputed_configured_tx_window_size - 1) & SN_12BITS_MASK;
  end_sn_index = end_sn % rlcP->recomputed_configured_tx_window_size;

  msg ("-------------------------------------------\nRetrans Buf rlc %p: VT(A) 0x%04X VT(S) 0x%04X  XXX = sn based on index, (XXX) = sn read from pdu\n", rlcP, rlcP->vt_a, rlcP->vt_s);

  while (working_sn_index != end_sn_index) {

    if (rlcP->retransmission_buffer[working_sn_index] != NULL) {
      msg ("%03X(%03X).", working_sn, ((struct rlc_am_tx_data_pdu_management *) (rlcP->retransmission_buffer[working_sn_index]->data))->sn);
    } else {
      msg ("_.");
    }
    if ((working_sn_index % 32) == 0) {
      msg ("\n");
    }
    working_sn = (working_sn + 1) & SN_12BITS_MASK;
    working_sn_index = working_sn % rlcP->recomputed_configured_tx_window_size;
  }
  msg ("\n------------------------------------------\n");
}

//-----------------------------------------------------------------------------
void
display_receiver_buffer (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------

  u16_t             working_sn, working_sn_index;
  u16_t             end_sn, end_sn_index;

  working_sn = rlcP->vr_r;
  working_sn_index = working_sn % rlcP->recomputed_configured_rx_window_size;

  end_sn = rlcP->vr_mr;
  end_sn_index = end_sn % rlcP->recomputed_configured_rx_window_size;

  msg ("-------------------------------\nRec Buf rlc %p VR(R) 0x%04X: ", rlcP, rlcP->vr_r);

  while (working_sn_index != end_sn_index) {

    if (rlcP->receiver_buffer[working_sn_index] != NULL) {
      msg ("%03X(%03X)", working_sn, ((struct rlc_am_rx_pdu_management *) (rlcP->receiver_buffer[working_sn_index]->data))->sn);
    } else {
      msg ("_.");
    }
    if ((working_sn_index % 32) == 0) {
      msg ("\n");
    }
    working_sn = (working_sn + 1) & SN_12BITS_MASK;
    working_sn_index = working_sn % rlcP->recomputed_configured_rx_window_size;
  }
  msg ("\n--------------------------------------\n");
}

//-----------------------------------------------------------------------------
void
rlc_am_check_retransmission_buffer (struct rlc_am_entity *rlcP, u8_t * messageP)
{
//-----------------------------------------------------------------------------

  int             error_found = 0;
  u16_t             working_sn, working_sn_index;
  u16_t             end_sn, end_sn_index;

  //---------------------------------------------
  // check if pdu remaining outside the window
  //---------------------------------------------
  working_sn = rlcP->vt_s;
  working_sn_index = working_sn % rlcP->recomputed_configured_tx_window_size;

  end_sn = rlcP->vt_a;
  end_sn_index = end_sn % rlcP->recomputed_configured_tx_window_size;

  while (working_sn_index != end_sn_index) {

    if (rlcP->retransmission_buffer[working_sn_index] != NULL) {
      msg ("[RLC_AM][RB %d] CHECK RETRANSMISSION BUFFER ERROR %s : REMAINING PDU INDEX %d=0x%04X\n", rlcP->rb_id, messageP, working_sn_index, working_sn_index);
      rlc_am_display_data_pdu7 (rlcP->retransmission_buffer[working_sn_index]);
      error_found = 1;
    }
    working_sn = (working_sn + 1) & SN_12BITS_MASK;
    working_sn_index = working_sn % rlcP->recomputed_configured_tx_window_size;
  }
  //---------------------------------------------
  // check if pdu remaining inside the window have the correct value for ack field
  //---------------------------------------------
  end_sn = rlcP->vt_s;
  end_sn_index = end_sn % rlcP->recomputed_configured_tx_window_size;

  working_sn = rlcP->vt_a;
  working_sn_index = working_sn % rlcP->recomputed_configured_tx_window_size;

  while (working_sn_index != end_sn_index) {

    if (rlcP->retransmission_buffer[working_sn_index] != NULL) {
      if (((struct rlc_am_tx_data_pdu_management *) (rlcP->retransmission_buffer[working_sn_index]->data))->ack != RLC_AM_PDU_ACK_NO_EVENT) {
        msg ("[RLC_AM][RB %d] CHECK RETRANSMISSION BUFFER ERROR %s EVENT (%d) IN PDU SN 0x%04X\n", rlcP->rb_id, messageP,
             ((struct rlc_am_tx_data_pdu_management *) (rlcP->retransmission_buffer[working_sn_index]->data))->ack, working_sn);
        rlc_am_display_data_pdu7 (rlcP->retransmission_buffer[working_sn_index]);
        error_found = 1;
      }
    }
    working_sn = (working_sn + 1) & SN_12BITS_MASK;
    working_sn_index = working_sn % rlcP->recomputed_configured_tx_window_size;
  }

  if ((error_found)) {
    display_protocol_vars_rlc_am (rlcP);
    while (1);
  } else {
    msg ("[RLC_AM][RB %d] CHECK RETRANSMISSION BUFFER OK\n", rlcP->rb_id);
  }
}

//-----------------------------------------------------------------------------
void
rlc_am_check_receiver_buffer (struct rlc_am_entity *rlcP, u8_t * messageP)
{
//-----------------------------------------------------------------------------

  int             error_found = 0;
  u16_t             working_sn, working_sn_index;
  u16_t             end_sn, end_sn_index;

  working_sn = rlcP->vr_h;
  working_sn_index = working_sn % rlcP->recomputed_configured_tx_window_size;

  end_sn = rlcP->vr_r;
  end_sn_index = end_sn % rlcP->recomputed_configured_tx_window_size;

  while (working_sn_index != end_sn_index) {

    if (rlcP->receiver_buffer[working_sn_index] != NULL) {
      msg ("[RLC_AM][RB %d] CHECK RECEIVER BUFFER ERROR %s : REMAINING PDU INDEX %d=0x%04X\n", rlcP->rb_id, messageP, working_sn_index, working_sn_index);
      error_found = 1;
    }
    working_sn = (working_sn + 1) & SN_12BITS_MASK;
    working_sn_index = working_sn % rlcP->recomputed_configured_rx_window_size;
  }
  if ((error_found)) {
    while (1);
  }
}

//-----------------------------------------------------------------------------
void
rlc_am_display_data_pdu7 (mem_block_t * pduP)
{
//-----------------------------------------------------------------------------
  struct rlc_am_tx_data_pdu_management *pdu_mngt;
  struct rlc_am_pdu_header *rlc_header;
  int             index;
  int             nb_li = 0;
  int             li_index = 0;
  u16_t             li[32];

  if ((pduP)) {
    pdu_mngt = (struct rlc_am_tx_data_pdu_management *) (pduP->data);

    msg ("[RLC_AM] DISPLAY DATA PDU %p SN 0x%04X LAST_PDU_OF_SDU %d\n", pduP, pdu_mngt->sn, pdu_mngt->last_pdu_of_sdu);
    msg ("[RLC_AM] DISPLAY DATA PDU HEADER ");
    for (index = 0; index < 10; index++) {
      msg ("%02X.", pdu_mngt->first_byte[index]);
    }
    msg ("\n");
    //--------------------------------------
    // LINKED SDUs
    //--------------------------------------
    msg ("[RLC_AM] DISPLAY DATA PDU CONTAINS SDU(s) INDEX ");
    for (index = 0; index < pdu_mngt->nb_sdu; index++) {
      msg ("%d", pdu_mngt->sdu[index]);
    }
    msg ("\n");
    //--------------------------------------
    // LENGTH INDICATORS
    //--------------------------------------
    rlc_header = (struct rlc_am_pdu_header *) (pdu_mngt->first_byte);

    if ((rlc_header->byte2 & RLC_HE_MASK) != RLC_HE_SUCC_BYTE_CONTAINS_DATA) {
      msg ("[RLC_AM] DISPLAY DATA PDU CONTAINS LI(s) ");
      nb_li = 0;
      while ((li[nb_li] = (rlc_header->li_data_7[nb_li])) & RLC_E_NEXT_FIELD_IS_LI_E) {
        li[nb_li] = li[nb_li] & (~(u8_t) RLC_E_NEXT_FIELD_IS_LI_E);
        nb_li++;
      }
      nb_li++;
      while (li_index < nb_li) {
        switch (li[li_index]) {
            case RLC_LI_LAST_PDU_EXACTLY_FILLED:
              msg ("LAST_PDU_EXACTLY_FILLED ");
              break;
            case RLC_LI_LAST_PDU_ONE_BYTE_SHORT:
              msg ("LAST_PDU_ONE_BYTE_SHORT ");
              break;
            case RLC_LI_PDU_PIGGY_BACKED_STATUS:       // ignore
              msg ("PDU_PIGGY_BACKED_STATUS ");
              break;
            case RLC_LI_PDU_PADDING:
              msg ("PDU_PADDING ");
              break;
            default:           // li is length
              msg ("LENGTH %d ", li[li_index] >> 1);
        }
        li_index++;
      }
    } else {
      msg ("[RLC_AM] DISPLAY DATA PDU CONTAINS NO LI(s) ");
    }
    msg ("\n");

    //--------------------------------------
    // DATA
    //--------------------------------------
    msg ("[RLC_AM] DISPLAY DATA PDU CONTENT <PDU>");
    for (index = 0; index < pdu_mngt->data_size; index++) {
      msg ("%02X.", pdu_mngt->payload[index]);
    }
    msg ("</PDU>\n");

  } else {
    msg ("[RLC_AM] DISPLAY DATA PDU : IS NULL\n");
  }
}
//-----------------------------------------------------------------------------
void
rlc_am_stat_req     (struct rlc_am_entity *rlcP, 
							  unsigned int* tx_pdcp_sdu,
							  unsigned int* tx_pdcp_sdu_discarded,
							  unsigned int* tx_retransmit_pdu_unblock,
							  unsigned int* tx_retransmit_pdu_by_status,
							  unsigned int* tx_retransmit_pdu,
							  unsigned int* tx_data_pdu,
							  unsigned int* tx_control_pdu,
							  unsigned int* rx_sdu,
							  unsigned int* rx_error_pdu,  
							  unsigned int* rx_data_pdu,
							  unsigned int* rx_data_pdu_out_of_window,
							  unsigned int* rx_control_pdu) {
//-----------------------------------------------------------------------------
			*tx_pdcp_sdu                        = rlcP->stat_tx_pdcp_sdu;
			*tx_pdcp_sdu_discarded         = rlcP->stat_tx_pdcp_sdu_discarded;
			*tx_retransmit_pdu_unblock = rlcP->stat_tx_retransmit_pdu_unblock;
			*tx_retransmit_pdu_by_status = rlcP->stat_tx_retransmit_pdu_by_status;
			*tx_retransmit_pdu = rlcP->stat_tx_retransmit_pdu;
			*tx_data_pdu = rlcP->stat_tx_data_pdu;
			*tx_control_pdu = rlcP->stat_tx_control_pdu;
			*rx_sdu = rlcP->stat_rx_sdu;
			*rx_error_pdu = rlcP->stat_rx_error_pdu;
			*rx_data_pdu = rlcP->stat_rx_data_pdu;
			*rx_data_pdu_out_of_window = rlcP->stat_rx_data_pdu_out_of_window;
			*rx_control_pdu = rlcP->stat_rx_control_pdu;
}
