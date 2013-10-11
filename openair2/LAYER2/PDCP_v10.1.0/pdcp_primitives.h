/*
                             pdcp_primitives.h
                             -------------------
  AUTHOR  : Baris Demiray
  COMPANY : EURECOM
  EMAIL   : Baris.Demiray@eurecom.fr
 ***************************************************************************/

#ifndef PDCP_PRIMITIVES_H
#define PDCP_PRIMITIVES_H

#ifndef TRUE
  #define TRUE 0x01
  #define FALSE 0x00
  typedef unsigned char BOOL;
#endif

/*
 * 3GPP TS 36.323 V10.1.0 (2011-03)
 */

/*
 * Data or control (1-bit, see 6.3.7)
 */
#define PDCP_CONTROL_PDU 0x00
#define PDCP_DATA_PDU 0x01
#define PDCP_TM 0x02 // transparent mode 

/*
 * PDU-type (3-bit, see 6.3.8)
 */
#define PDCP_STATUS_REPORT 0x00
#define INTERSPERSED_ROHC_FEEDBACK_PACKET 0x01

/*
 * 6.1 Protocol Data Units
 * 6.2.2 Control Plane PDCP Data PDU
 */
#define PDCP_CONTROL_PLANE_DATA_PDU_SN_SIZE 1
#define PDCP_CONTROL_PLANE_DATA_PDU_MAC_I_SIZE 4
typedef struct {
  u8 sn;      // PDCP sequence number will wrap around 2^5-1 so
              // reserved field is unnecessary here
  u8 mac_i[PDCP_CONTROL_PLANE_DATA_PDU_MAC_I_SIZE];  // Integration protection is not implemented (pad with 0)
} pdcp_control_plane_data_pdu_header;

/*
 * 6.2.3 User Plane PDCP Data PDU with long PDCP SN (12-bit)
 */
typedef struct {
  u8 dc;      // Data or control (see 6.3.7)
  u16 sn;     // 12-bit sequence number
} pdcp_user_plane_data_pdu_header_with_long_sn;
#define PDCP_USER_PLANE_DATA_PDU_LONG_SN_HEADER_SIZE 2

/*
 * 6.2.4 User Plane PDCP Data PDU with short PDCP SN (7-bit)
 */
typedef struct {
  u8 dc;
  u8 sn;      // 7-bit sequence number
} pdcp_user_plane_data_pdu_header_with_short_sn;
#define PDCP_USER_PLANE_DATA_PDU_SHORT_SN_HEADER_SIZE 1

/*
 * 6.2.5 PDCP Control PDU for interspersed ROHC feedback packet
 */
typedef struct {
  u8 dc;
  u8 pdu_type; // PDU type (see 6.3.8)
} pdcp_control_pdu_for_interspersed_rohc_feedback_packet_header;
#define PDCP_CONTROL_PDU_INTERSPERSED_ROHC_FEEDBACK_HEADER_SIZE 1

/*
 * 6.2.6 PDCP Control PDU for PDCP status report
 */
typedef struct {
  u8 dc;
  u8 pdu_type; // PDU type (see 6.3.8)
  u16 first_missing_sn; // First missing PDCP SN
  unsigned char* window_bitmap; // Ack/Nack information coded as a bitmap
  u16 window_bitmap_size;
} pdcp_control_pdu_for_pdcp_status_report;
/*
 * Following symbolic constant is the size of FIXED part of this PDU
 * so bitmap size should be added to find total header size
 */
#define PDCP_CONTROL_PDU_STATUS_REPORT_HEADER_SIZE 2

/*
 * Parses data/control field out of buffer of User Plane PDCP Data PDU with
 * long PDCP SN (12-bit)
 *
 * @param pdu_buffer PDCP PDU buffer
 * @return 1 bit dc
 */

u8 pdcp_get_dc_filed(unsigned char* pdu_buffer);

/*
 * Parses sequence number out of buffer of User Plane PDCP Data PDU with
 * long PDCP SN (12-bit)
 *
 * @param pdu_buffer PDCP PDU buffer
 * @return 12-bit sequence number
 */
u16 pdcp_get_sequence_number_of_pdu_with_long_sn(unsigned char* pdu_buffer);

/*
 * Parses sequence number out of buffer of User Plane PDCP Data PDU with
 * short PDCP SN (7-bit)
 *
 * @param pdu_buffer PDCP PDU buffer
 * @return 7-bit sequence number
 */
u8 pdcp_get_sequence_number_of_pdu_with_short_sn(unsigned char* pdu_buffer);

/*
 * Parses sequence number out of buffer of Control Plane PDCP Data PDU with
 * short PDCP SN (5-bit)
 *
 * @param pdu_buffer PDCP PDU buffer
 * @return 5-bit sequence number
 */
u8 pdcp_get_sequence_number_of_pdu_with_SRB_sn(unsigned char* pdu_buffer);

/*
 * Fills the incoming buffer with the fields of the header for SRB1 
 *
 * @param pdu_buffer PDCP PDU buffer
 * @return TRUE on success, FALSE otherwise
 */
BOOL pdcp_serialize_control_plane_data_pdu_with_SRB_sn_buffer(unsigned char* pdu_buffer, \
							      pdcp_control_plane_data_pdu_header* pdu);
/*
 * Fills the incoming buffer with the fields of the header for long SN (RLC UM and AM)
 *
 * @param pdu_buffer PDCP PDU buffer
 * @return TRUE on success, FALSE otherwise
 */
BOOL pdcp_serialize_user_plane_data_pdu_with_long_sn_buffer(unsigned char* pdu_buffer, \
     pdcp_user_plane_data_pdu_header_with_long_sn* pdu);

/*
 * Fills the incoming status report header with given value of bitmap 
 * and 'first missing pdu' sequence number
 *
 * @param FMS First Missing PDCP SN
 * @param bitmap Received/Missing sequence number bitmap
 * @param pdu A status report header
 * @return TRUE on success, FALSE otherwise
 */
BOOL pdcp_serialize_control_pdu_for_pdcp_status_report(unsigned char* pdu_buffer, \
     u8 bitmap[512], pdcp_control_pdu_for_pdcp_status_report* pdu);

int pdcp_netlink_dequeue_element(uint8_t eNB_flag, uint8_t UE_index, uint8_t eNB_index,
                                 struct pdcp_netlink_element_s **data);

void pdcp_config_set_security(module_id_t module_id, u32 frame, u8 eNB_flag, rb_id_t rb_id,
                              u16 lc_id, u8 security_mode, u8 *kRRCenc, u8 *kRRCint, u8 *kUPenc);

#if defined(ENABLE_SECURITY)
int pdcp_apply_security(pdcp_t *pdcp_entity, rb_id_t rb_id,
                        u8 pdcp_header_len, u16 current_sn, u8 *pdcp_pdu_buffer,
                        u16 sdu_buffer_size);

int pdcp_validate_security(pdcp_t *pdcp_entity, rb_id_t rb_id,
                           u8 pdcp_header_len, u16 current_sn, u8 *pdcp_pdu_buffer,
                           u16 sdu_buffer_size);
#endif /* defined(ENABLE_SECURITY) */

#endif
