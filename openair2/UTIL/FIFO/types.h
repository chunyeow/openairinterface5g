#ifndef __FIFO_TYPES_H__
#define __FIFO_TYPES_H__
#include "platform_types.h"

/* Types of events triggering the end of the simulation */
typedef enum End_Of_Sim_Event_Type_e {FRAME, TRAFFIC} End_Of_Sim_Event_Type_t;

/* Types regrouping both user-defined and regular events */
typedef enum Event_Type_e {ET_DL, ET_UL, ET_S, ET_OMG, ET_OTG} Event_Type_t;

/* decomposition of node functions into jobs for a given event */
typedef enum Job_type_e { JT_OTG, JT_PDCP, JT_PHY_MAC, JT_INIT_SYNC, JT_DL, JT_UL, RN_DL, RN_UL, JT_END} Job_Type_t;

typedef struct Job_s {
    enum Job_type_e type;
    int             exe_time; /* execution time at the worker*/
    int             nid; /* node id*/
    eNB_flag_t      eNB_flag;
    frame_t         frame;
    int             last_slot;
    int             next_slot;
    int             ctime;
} Job_t;

typedef struct Signal_buffers_s { // (s = transmit, r,r0 = receive)
  double **s_re;
  double **s_im;
  double **r_re;
  double **r_im;
  double **r_re0;
  double **r_im0;
} Signal_buffers_t;

/*!\brief  sybframe type : DL, UL, SF, */

typedef struct Packet_otg_s {
  unsigned int              sdu_buffer_size;
  unsigned char            *sdu_buffer;
  module_id_t               module_id;
  rb_id_t                   rb_id;
  module_id_t               dst_id;
  boolean_t                 is_ue;
  pdcp_transmission_mode_t  mode;
} Packet_otg_t;

typedef struct Event_s {
    enum Event_Type_e type;
    char             *key;
    void             *value;
    frame_t           frame;
} Event_t;

/*typedef struct Global_Time {
  uint32_t frame;
  int32_t slot;
  int32_t last_slot;
  int32_t next_slot;
  double time_s;
  double time_ms;
};*/

typedef struct End_Of_Sim_Event_s {
    enum End_Of_Sim_Event_Type_e type;
    int value;
} End_Of_Sim_Event_t;

typedef struct Packet_otg_elt_s {
    struct Packet_otg_elt_s *next;
    struct Packet_otg_elt_s *previous;
    Packet_otg_t             otg_pkt;
} Packet_otg_elt_t;

typedef struct Job_element_s {
  struct Job_element_s *next;
  Job_t                 job;
} Job_elt_t;

typedef struct Event_element_s {
  struct Event_element *next;
  struct Event_element *previous;
  Event_t               event;
} Event_elt_t;
#endif
