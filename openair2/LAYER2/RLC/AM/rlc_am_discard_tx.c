/***************************************************************************
                          rlc_am_discard_tx.c  -
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr



 ***************************************************************************/
#include "rtos_header.h"
#include "platform_types.h"
//-----------------------------------------------------------------------------
#include "rlc_am_discard_notif_proto_extern.h"
#include "rlc_am_discard_rx_proto_extern.h"
#include "rlc_primitives.h"
#include "rlc_am_status_proto_extern.h"
#include "rlc_am_util_proto_extern.h"
#include "rlc_am_constants.h"
#include "rlc_am_structs.h"
#include "umts_timer_proto_extern.h"
//-----------------------------------------------------------------------------
void            rlc_am_schedule_procedure (struct rlc_am_entity *rlcP);
void            rlc_am_process_sdu_discarded (struct rlc_am_entity *rlcP);
void            rlc_am_sdu_discard_with_explicit_signalling_procedure_send_mrw_configured (struct rlc_am_entity *rlcP);
void            rlc_am_sdu_discard_with_explicit_signalling_procedure_send_mrw_not_configured (struct rlc_am_entity *rlcP);
//-----------------------------------------------------------------------------
void
rlc_am_schedule_procedure (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  mem_block_t      *pdu_status;
  struct rlc_am_discard_procedure *procedure;

  // if a procedure is running do nothing
  if (!(rlcP->timer_mrw) && (rlcP->discard_procedures.head)) {
    // launch remaining procedures (only 1 procedure running)
    procedure = (struct rlc_am_discard_procedure *) ((rlcP->discard_procedures.head)->data);

/*     sn_mrw_length = procedure->last_pdu_sn;

   if (rlc_am_comp_sn(rlcP, rlcP->vt_s, sn_mrw_length, rlcP->vt_s) > 0) {
#ifdef DEBUG_RLC_AM_DISCARD
      msg("[RLC_AM %p][DISCARD] SCHEDULE PROCEDURE %p UPDATE VT(S) %04X -> %04X\n", rlcP, rlcP->discard_procedures.head, rlcP->vt_s, sn_mrw_length);
#endif
      rlcP->vt_s = sn_mrw_length;
    }
*/
#ifdef DEBUG_RLC_AM_DISCARD
    msg ("[RLC_AM][RB %d] DISCARD  SCHEDULE PROCEDURE %p vt_s 0x%04X vt_a 0x%04X\n", rlcP->rb_id, rlcP->discard_procedures.head, rlcP->vt_s, rlcP->vt_a);
#endif
    procedure->running = 0xFF;
    pdu_status = get_free_mem_block (rlcP->pdu_size + sizeof (struct rlc_am_tx_control_pdu_allocation) + GUARD_CRC_LIH_SIZE);
    memcpy (pdu_status->data, procedure->control_pdu->data, rlcP->pdu_size + sizeof (struct rlc_am_tx_control_pdu_allocation));
    list_add_tail_eurecom (pdu_status, &rlcP->control);

  }
}

//-----------------------------------------------------------------------------
void
rlc_am_process_sdu_discarded (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  // We assume that the list sdu_discarded contains sdu(s)
  /*  From 3GPP TS 25.322 V4.3.0 (2001-12)
     The Sender shall initiate the SDU discard with explicit signalling procedure if one of the
     following triggers is detected:
     -  "Timer based SDU discard with explicit signalling" is configured, Timer_Discard expires
     for an SDU, and one or more segments of the SDU have been submitted to a lower layer;
     -  "Timer based SDU discard with explicit signalling" is configured, Timer_Discard expires
     for an SDU, and Send MRW is configured;
     -  "SDU discard after MaxDAT number of transmissions" is configured, and MaxDAT number of
     transmissions is reached (i.e. VT(DAT) ³ MaxDAT) for an AMD PDU.
   */

  // discard procedure
  if ((rlcP->send_mrw & RLC_AM_SEND_MRW_ON)) {
    rlc_am_sdu_discard_with_explicit_signalling_procedure_send_mrw_configured (rlcP);
  } else {                      // RLC_AM_SEND_MRW_OFF
    rlc_am_sdu_discard_with_explicit_signalling_procedure_send_mrw_not_configured (rlcP);
  }
  rlc_am_schedule_procedure (rlcP);
}

//-----------------------------------------------------------------------------
void
rlc_am_sdu_discard_with_explicit_signalling_procedure_send_mrw_configured (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  /* From 3GPP TS 25.322 V5.0.0 (2002-03)
     The Sender shall:
     - if "Send MRW" is configured:
     - if the last discarded SDU ended in an AMD PDU, and its "Length Indicator" is present in the same AMD
     PDU, and no new SDU is present inside this AMD PDU:
     - set the last SN_MRWi field in the MRW SUFI to 1 + "Sequence Number" of the AMD PDU which
     contains the "Length Indicator" of the last discarded SDU;
     - set the NLENGTH field in the MRW SUFI to "0000".
     - otherwise:
     - set the last SN_MRWi field in the MRW SUFI to the "Sequence Number" of the AMD PDU which
     contains the "Length Indicator" of the last discarded SDU;
     - set the NLENGTH field in the MRW SUFI so that the last data octet to be discarded in the Receiver shall be
     the octet indicated by the NLENGTH:th "Length Indicator" field of the AMD PDU which contains the
     "Length Indicator" of the last discarded SDU;
     - set each of the other SN_MRWi fields in the MRW SUFI to the "Sequence Number" of the AMD PDU which
     contains the "Length Indicator" of the i:th discarded SDU.
     - if the MRW SUFI contains only one SN_MRWi field and the value of SN_MRWi field
     VT(A)+Configured_Tx_Window_Size:
     - set the LENGTH field in the MRW SUFI to "0000".
     - otherwise:
     - set the LENGTH field in the MRW SUFI to the number of SN_MRWi fields in the same MRW SUFI. In this
     case, SN_MRW1 shall be in the interval VT(A)   SN_MRW1 < VT(A)+Configured_Tx_Window_Size.
   */
  mem_block_t      *mb_discard_procedure = NULL;
  mem_block_t      *sdu_discarded;
  struct rlc_am_tx_sdu_management *last_sdu_discarded_mngt;
  mem_block_t      *le;
  struct rlc_am_status_header *pdu;
  u8_t             *p8;
  int             last_sn_mrw_length;
  u8_t              count_sdu_discarded;
  u8_t              byte_aligned;
#ifdef DEBUG_RLC_AM_DISCARD
  u16_t             sn_mrw_length;
#endif
  while (rlcP->sdu_discarded.head) {

    // alloc a discard procedure
    mb_discard_procedure = get_free_mem_block (sizeof (struct rlc_am_discard_procedure));
    memset (mb_discard_procedure->data, 0, sizeof (struct rlc_am_discard_procedure));
    list_init (&((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->sdu_list, NULL);

    count_sdu_discarded = 0;

    // assign sdu discarded to discard procedure
    // sdu headers are registered in discard procedure
    last_sn_mrw_length = -1;
    while ((rlcP->sdu_discarded.head) && (count_sdu_discarded < 15)) {  // max 15 sdu discarded per procedure
      sdu_discarded = list2_remove_head (&rlcP->sdu_discarded);

      // this test is done to avoid signalling n times the same SN_MRW_length if a pdu contains n sdu
      // so it can save some discard procedures.
      if (last_sn_mrw_length != ((struct rlc_am_tx_sdu_management *) (sdu_discarded->data))->last_pdu_sn) {

#ifdef DEBUG_RLC_AM_DISCARD
        msg ("[RLC_AM][RB %d] DISCARD IN PROCEDURE %p ADD SDU SN_MRWlength 0x%03X\n", rlcP->rb_id, mb_discard_procedure, ((struct rlc_am_tx_sdu_management *) (sdu_discarded->data))->last_pdu_sn);
#endif
        list_add_tail_eurecom (sdu_discarded, &((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->sdu_list);
        last_sn_mrw_length = ((struct rlc_am_tx_sdu_management *) (sdu_discarded->data))->last_pdu_sn;
        count_sdu_discarded += 1;
      } else {
        free_mem_block (sdu_discarded);
      }
    }
    ((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->length = count_sdu_discarded;

    //((struct rlc_am_discard_procedure*)(mb_discard_procedure->data))->nlength =  ((struct rlc_am_sdu_discard_management*)((mem_block_t*)(sdu_header_copy_copy->data))->data)->li_index_for_discard;

    last_sdu_discarded_mngt = (struct rlc_am_tx_sdu_management *) (sdu_discarded->data);

    if ((last_sdu_discarded_mngt->no_new_sdu_segmented_in_last_pdu)) {
#ifdef DEBUG_RLC_AM_DISCARD
      msg ("[RLC_AM][RB %d] DISCARD  NO NEW SDU SEGMENTED IN LAST PDU DISCARDED\n", rlcP->rb_id);
#endif
      last_sdu_discarded_mngt->last_pdu_sn = (last_sdu_discarded_mngt->last_pdu_sn + 1) & SN_12BITS_MASK;
      ((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->nlength = 0;
    } else {
#ifdef DEBUG_RLC_AM_DISCARD
      msg ("[RLC_AM][RB %d] DISCARD  OTHER SDU(s) SEGMENTED IN LAST PDU DISCARDED\n", rlcP->rb_id);
#endif
      ((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->nlength = last_sdu_discarded_mngt->li_index_for_discard + 1;  // +1 since numerotation begins at 1
    }
#ifdef DEBUG_RLC_AM_DISCARD
    msg ("[RLC_AM][RB %d] DISCARD  SET  SN_MRW_LENGTH 0x%04X NLENGTH = %d \n", rlcP->rb_id, last_sdu_discarded_mngt->last_pdu_sn,
         ((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->nlength);
    sn_mrw_length = last_sdu_discarded_mngt->last_pdu_sn;
#endif


    // make status pdu
    le = get_free_mem_block (rlcP->pdu_size + sizeof (struct rlc_am_tx_control_pdu_allocation) + GUARD_CRC_LIH_SIZE);
    if (le == NULL) {
      // be carefull : lost resources in mb_discard : TO DO
      msg ("[RLC_AM][RB %d] FATAL ERROR : OUT OF MEMORY\n", rlcP->rb_id);
      return;
    } else {
      ((struct rlc_am_tx_control_pdu_management *) (le->data))->rlc_tb_type = RLC_AM_MRW_STATUS_PDU_TYPE;
      pdu = (struct rlc_am_status_header *) (&le->data[sizeof (struct rlc_am_tx_control_pdu_allocation)]);

      pdu->byte1 = RLC_PDU_TYPE_STATUS;
      p8 = &(pdu->byte1);
      *p8 = *p8 | RLC_AM_SUFI_MRW;
      p8 = p8 + 1;
      // fill field LENGTH
      *p8 = (count_sdu_discarded << 4);
      byte_aligned = 0;

      // fill fields SN_MRWi
      sdu_discarded = (((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->sdu_list.head);      // reuse of var sdu_discarded
      while ((count_sdu_discarded)) {
        count_sdu_discarded -= 1;

        if (byte_aligned) {
          *p8 = ((struct rlc_am_tx_sdu_management *) (sdu_discarded->data))->last_pdu_sn >> 4;
          p8 = p8 + 1;
          *p8 = ((struct rlc_am_tx_sdu_management *) (sdu_discarded->data))->last_pdu_sn << 4;
          byte_aligned = 0;
        } else {
          //*p8 =  0 << 4 | (temp_sn  >> 8);
          *p8 |= (((struct rlc_am_tx_sdu_management *) (sdu_discarded->data))->last_pdu_sn >> 8);
          p8 = p8 + 1;
          *p8 = ((struct rlc_am_tx_sdu_management *) (sdu_discarded->data))->last_pdu_sn;
          p8 = p8 + 1;
          byte_aligned = 1;
        }

        sdu_discarded = sdu_discarded->next;
      }

      // fill field Nlength
      if (byte_aligned) {
        *p8 = (((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->nlength << 4) | RLC_AM_SUFI_NO_MORE;
      } else {
        *p8 |= (((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->nlength);
        p8 = p8 + 1;
        *p8 = (RLC_AM_SUFI_NO_MORE << 4);
      }
      ((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->control_pdu = le;
      list2_add_tail (mb_discard_procedure, &rlcP->discard_procedures);
    }
  }
#ifdef DEBUG_RLC_AM_DISCARD
  msg ("[RLC_AM][RB %d] DISCARD  QUEUED NEW PROCEDURE SEND_MRW IS CONFIGURED SN_MRW_length = 0x%04X length %d nlength %d\n",
       rlcP->rb_id, sn_mrw_length, ((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->length, ((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->nlength);
#endif

}

//-----------------------------------------------------------------------------
void
rlc_am_sdu_discard_with_explicit_signalling_procedure_send_mrw_not_configured (struct rlc_am_entity *rlcP)
{
//-----------------------------------------------------------------------------
  /* From 3GPP TS 25.322 V5.0.0 (2002-03)
     The Sender shall:
     - if "Send MRW" is NOT configured:
     - if the last SDU to be discarded in the Receiver ended in an AMD PDU, and its "Length Indicator" is present
     in the same AMD PDU, and no new SDU is present inside this AMD PDU:
     - set the last SN_MRWi field in the MRW SUFI to 1 + "Sequence Number" of the AMD PDU which
     contains the "Length Indicator" of the last SDU to be discarded in the Receiver;
     - set the NLENGTH field in the MRW SUFI to "0000".
     - otherwise:
     - set the last SN_MRWi field in the MRW SUFI to the "Sequence Number" of the AMD PDU which
     contains the "Length Indicator" of the last SDU to be discarded in the Receiver;
     - set the NLENGTH field in the MRW SUFI so that the last data octet to be discarded in the Receiver shall be
     the octet indicated by the NLENGTH:th "Length Indicator" field of the AMD PDU which contains the
     "Length Indicator" of the last SDU to be discarded in the Receiver;
     - optionally set each of the other SN_MRWi fields in the MRW SUFI to the "Sequence Number" of the AMD
     PDU which contains the "Length Indicator" of the i:th SDU to be discarded in the Receiver;
     - if the MRW SUFI contains only one SN_MRWi field and the value of SN_MRWi field
     VT(A)+Configured_Tx_Window_Size:
     - set the LENGTH field in the MRW SUFI to "0000".
     - otherwise:
     - set the LENGTH field in the MRW SUFI to the number of SN_MRWi fields in the same MRW SUFI. In this
     case, SN_MRW1 shall be in the interval VT(A)   SN_MRW1 < VT(A)+Configured_Tx_Window_Size.
   */
  mem_block_t      *mb_discard_procedure = NULL;
  mem_block_t      *sdu_discarded;
  struct rlc_am_tx_sdu_management *last_sdu_discarded_mngt;
  mem_block_t      *le;
  struct rlc_am_status_header *pdu;
  int             last_sn_mrw_length;
  u8_t             *p8;
  u8_t              count_sdu_discarded;
  u8_t              byte_aligned;
#ifdef DEBUG_RLC_AM_DISCARD
  u16_t             sn_mrw_length;
#endif
  while (rlcP->sdu_discarded.head) {

    // alloc a discard procedure
    mb_discard_procedure = get_free_mem_block (sizeof (struct rlc_am_discard_procedure));
    memset (mb_discard_procedure->data, 0, sizeof (struct rlc_am_discard_procedure));
    list_init (&((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->sdu_list, NULL);

    count_sdu_discarded = 0;

    // assign sdu discarded to discard procedure
    // sdu headers are registered in discard procedure
    last_sn_mrw_length = -1;
    while ((rlcP->sdu_discarded.head) && (count_sdu_discarded < 15)) {  // max 15 sdu discarded per procedure

      sdu_discarded = list2_remove_head (&rlcP->sdu_discarded);
      // this test is done to avoid signalling n times the same SN_MRW_length if a pdu contains n sdu
      // so it can save some discard procedures.
      if (last_sn_mrw_length != ((struct rlc_am_tx_sdu_management *) (sdu_discarded->data))->last_pdu_sn) {
#ifdef DEBUG_RLC_AM_DISCARD
        msg ("[RLC_AM][RB %d] DISCARD IN PROCEDURE %p ADD SDU SN_MRWlength %03X hex\n", rlcP->rb_id, mb_discard_procedure, ((struct rlc_am_tx_sdu_management *) (sdu_discarded->data))->last_pdu_sn);
#endif
        list_add_tail_eurecom (sdu_discarded, &((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->sdu_list);
        last_sn_mrw_length = ((struct rlc_am_tx_sdu_management *) (sdu_discarded->data))->last_pdu_sn;
        count_sdu_discarded = 1;
      } else {
        free_mem_block (sdu_discarded);
      }
    }
    ((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->length = count_sdu_discarded;


    //((struct rlc_am_discard_procedure*)(mb_discard_procedure->data))->nlength =  ((struct rlc_am_sdu_discard_management*)((mem_block_t*)(sdu_header_copy_copy->data))->data)->li_index_for_discard;

    last_sdu_discarded_mngt = (struct rlc_am_tx_sdu_management *) (sdu_discarded->data);

    if ((last_sdu_discarded_mngt->no_new_sdu_segmented_in_last_pdu)) {
      last_sdu_discarded_mngt->last_pdu_sn = (last_sdu_discarded_mngt->last_pdu_sn + 1) & SN_12BITS_MASK;
      ((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->nlength = 0;
    } else {
      ((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->nlength = last_sdu_discarded_mngt->li_index_for_discard;
    }
#ifdef DEBUG_RLC_AM_DISCARD
    sn_mrw_length = last_sdu_discarded_mngt->last_pdu_sn;
#endif

    // make status pdu
    if ((le = get_free_mem_block (rlcP->pdu_size + sizeof (struct rlc_am_tx_control_pdu_allocation) + GUARD_CRC_LIH_SIZE))) {
      ((struct rlc_am_tx_control_pdu_management *) (le->data))->rlc_tb_type = RLC_AM_MRW_STATUS_PDU_TYPE;

      pdu = (struct rlc_am_status_header *) (&le->data[sizeof (struct rlc_am_tx_control_pdu_allocation)]);
      pdu->byte1 = RLC_PDU_TYPE_STATUS;
      p8 = &(pdu->byte1);
      *p8 = *p8 | RLC_AM_SUFI_MRW;
      p8 = p8 + 1;
      // fill field LENGTH
      *p8 = (count_sdu_discarded << 4);
      byte_aligned = 0;

      // fill fields SN_MRWi
      sdu_discarded = (((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->sdu_list.head);      // reuse of var sdu_discarded

      //*p8 =  0 << 4 | (temp_sn  >> 8);
      *p8 |= (((struct rlc_am_tx_sdu_management *) (sdu_discarded->data))->last_pdu_sn >> 8);
      p8 = p8 + 1;
      *p8 = ((struct rlc_am_tx_sdu_management *) (sdu_discarded->data))->last_pdu_sn;
      p8 = p8 + 1;
      byte_aligned = 1;

      // fill field Nlength
      *p8 = (((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->nlength << 4) | RLC_AM_SUFI_NO_MORE;
      ((struct rlc_am_discard_procedure *) (mb_discard_procedure->data))->control_pdu = le;
      list2_add_tail (mb_discard_procedure, &rlcP->discard_procedures);
    } else {
      // out of memory
      return;
    }
  }
}
