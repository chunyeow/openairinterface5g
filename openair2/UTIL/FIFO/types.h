#include "platform_types.h"

/* Types of events triggering the end of the simulation */
typedef enum End_Of_Sim_Event_Type {FRAME, TRAFFIC} End_Of_Sim_Event_Type;

/* Types regrouping both user-defined and regular events */
typedef enum Event_Type {ET_DL, ET_UL, ET_S, ET_OMG, ET_OTG} Event_Type;

/* decomposition of node functions into jobs for a given event */
typedef enum Job_type { JT_OTG, JT_PDCP, JT_PHY_MAC, JT_INIT_SYNC, JT_DL, JT_UL, RN_DL, RN_UL, JT_END} Job_Type;

typedef struct Job {
    enum Job_type type;
    int exe_time; /* execution time at the worker*/
    int nid; /* node id*/
    int eNB_flag;
    int frame;
    int last_slot;
    int next_slot;
    int ctime;
} Job;

typedef struct Signal_buffers { // (s = transmit, r,r0 = receive)
  double **s_re;
  double **s_im;
  double **r_re;
  double **r_im;
  double **r_re0;
  double **r_im0;
} Signal_buffers;

/*!\brief  sybframe type : DL, UL, SF, */

typedef struct Packet_otg {
  unsigned int sdu_buffer_size;
  unsigned char *sdu_buffer;
  int module_id;
  int rb_id;
  int dst_id;
  u8 is_ue;
  u8 mode;
} Packet_otg;

typedef struct Event {
    enum Event_Type type;
    char * key;
    void * value;
    int frame;
} Event;

/*typedef struct Global_Time {
  u32 frame;
  s32 slot;
  s32 last_slot;
  s32 next_slot;
  double time_s;
  double time_ms;
};*/

typedef struct End_Of_Sim_Event {
    enum End_Of_Sim_Event_Type type;
    int value;
} End_Of_Sim_Event;

typedef struct Packet_otg_elt {
    struct Packet_otg_elt *next;
    struct Packet_otg_elt *previous;
    Packet_otg otg_pkt;
} Packet_otg_elt;

typedef struct Job_element {
  struct Job_elt *next;
  Job job;
} Job_elt;

typedef struct Event_element {
  struct Event_elt *next;
  struct Event_elt *previous;
  Event event;
} Event_elt;
