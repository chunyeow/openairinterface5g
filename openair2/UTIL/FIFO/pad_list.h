/*
                                 list.h
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#ifndef __list_H__
#define __list_H__

#include "types.h"
//-----------------------------------------------------------------------------
#    ifdef list_C
#        define private_list(x) x
#        define public_list(x) x
#    else
#        define private_list(x)
#        define public_list(x) extern x
#    endif
#    ifdef LIST2_C
#        define private_list2(x) x
#        define public_list2(x) x
#    else
#        define private_list2(x)
#        define public_list2(x) extern x
#    endif
//-----------------------------------------------------------------------------
//#include "platform_types.h"
//#include "platform_constants.h"
//#include "PHY/defs.h"


typedef struct {
  Job_elt *head;
  Job_elt *tail;
  int    nb_elements;
} Job_List;

typedef struct {
  Event_elt *head;
  Event_elt *tail;
  int    nb_elements;
} Event_List;

typedef struct {
  Packet_otg_elt *head;
  Packet_otg_elt *tail;
  int    nb_elements;
} Packet_OTG_List;

//-----------------------------------------------------------------------------
public_list( void         job_list_init (Job_List*);)
public_list( void         job_list_free (Job_List* listP);)
public_list( Job_elt* job_list_get_head (Job_List*);)
public_list( Job_elt* job_list_remove_head (Job_List* );)
public_list( Job_elt* job_list_remove_element (Job_elt*, Job_List*);)
public_list( void         job_list_add_head (Job_elt* , Job_List* );)
public_list( void         job_list_add_tail_eurecom (Job_elt* , Job_List* );)
public_list( void         job_list_add_list (Job_List* , Job_List* );)
public_list( void         job_list_display (Job_List* );)

public_list( void         event_list_init (Event_List*);)
public_list( void         event_list_free (Event_List* listP);)
public_list( Event_elt* event_list_get_head (Event_List*);)
public_list( Event_elt* event_list_remove_head (Event_List* );)
public_list( Event_elt* event_list_remove_element (Event_elt*, Event_List*);)
public_list( void         event_list_add_head (Event_elt* , Event_List* );)
public_list( void         event_list_add_element (Event_elt* , Event_elt* , Event_List* );)
public_list( void         event_list_add_tail_eurecom (Event_elt* , Event_List* );)
public_list( void         event_list_add_list (Event_List* , Event_List* );)
public_list( void         event_list_display (Event_List* );)

public_list( void         pkt_list_init (Packet_OTG_List*);)
public_list( void         pkt_list_free (Packet_OTG_List* listP);)
public_list( Packet_otg_elt* pkt_list_get_head (Packet_OTG_List*);)
public_list( Packet_otg_elt* pkt_list_remove_head (Packet_OTG_List* );)
public_list( Packet_otg_elt* pkt_list_remove_element (Packet_otg_elt*, Packet_OTG_List*);)
public_list( void         pkt_list_add_head (Packet_otg_elt* , Packet_OTG_List* );)
public_list( void         pkt_list_add_element (Packet_otg_elt* , Packet_otg_elt* , Packet_OTG_List* );)
public_list( void         pkt_list_add_tail_eurecom (Packet_otg_elt* , Packet_OTG_List* );)
public_list( void         pkt_list_add_list (Packet_OTG_List* , Packet_OTG_List* );)
public_list( void         pkt_list_display (Packet_OTG_List* );)

#endif
