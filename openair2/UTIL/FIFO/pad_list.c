/***************************************************************************
                             Job_list.c  -  description
                             -------------------
                             -------------------
  AUTHOR  : Lionel GAUTHIER, Mohamed Said MOSLI BOUKSIAA
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr, mosli@eurecom.fr
 ***************************************************************************/
//#define LIST_C
//#define NULL 0

#include "pad_list.h"
#include <stdio.h>
#include <stdlib.h>
//#include <mpi.h>


void job_list_init (Job_List * listP) {
  int  i = 0;

  listP->tail = NULL;
  listP->head = NULL;
  listP->nb_elements = 0;
}

void event_list_init (Event_List * listP) {
  int  i = 0;

  listP->tail = NULL;
  listP->head = NULL;
  listP->nb_elements = 0;
}

void pkt_list_init (Packet_OTG_List * listP) {
  int  i = 0;

  listP->tail = NULL;
  listP->head = NULL;
  listP->nb_elements = 0;
}

//-----------------------------------------------------------------------------

void job_list_free (Job_List * listP) {
  Job_elt *le;

  while ((le = job_list_remove_head (listP))) {
    free(le);
  }
}

void event_list_free (Event_List * listP) {
  Event_elt *le;

  while ((le = event_list_remove_head (listP))) {
    free(le);
  }
}

void pkt_list_free (Packet_OTG_List * listP) {
  Packet_otg_elt *le;

  while ((le = pkt_list_remove_head (listP))) {
    free(le);
  }
}

//-----------------------------------------------------------------------------

Job_elt * job_list_get_head (Job_List * listP) {
  return listP->head;
}

Event_elt * event_list_get_head (Event_List * listP) {
  return listP->head;
}

Packet_otg_elt * pkt_list_get_head (Packet_OTG_List * listP) {
  return listP->head;
}

//-----------------------------------------------------------------------------

/*
 *  remove an element from head of a list
 *  @param  pointer on targeted list
 *  @return pointer on removed Job_elt
 */
Job_elt * job_list_remove_head (Job_List * listP) {

  // access optimisation
  Job_elt *head;
  head = listP->head;
  // almost one element
  if (head != NULL) {
    listP->head = head->next;
    listP->nb_elements = listP->nb_elements - 1;
    // if only one element, update tail
    if (listP->head == NULL) {
      listP->tail = NULL;
    } else {
      head->next = NULL;
    }
  } else {
    //msg("[MEM_MGT][WARNING] remove_head_from_list(%s) no elements\n",listP->name);
  }
  return head;
}

Event_elt * event_list_remove_head (Event_List * listP) {

  // access optimisation
  Event_elt *head;
  head = listP->head;
  // almost one element
  if (head != NULL) {
    listP->head = head->next;
    listP->nb_elements = listP->nb_elements - 1;
    // if only one element, update tail
    if (listP->head == NULL) {
      listP->tail = NULL;
    } else {
      head->next = NULL;
    }
  } else {
    //msg("[MEM_MGT][WARNING] remove_head_from_list(%s) no elements\n",listP->name);
  }
  return head;
}

Packet_otg_elt * pkt_list_remove_head (Packet_OTG_List * listP) {

  // access optimisation
  Packet_otg_elt *head;
  head = listP->head;
  // almost one element
  if (head != NULL) {
    listP->head = head->next;
    listP->nb_elements = listP->nb_elements - 1;
    // if only one element, update tail
    if (listP->head == NULL) {
      listP->tail = NULL;
    } else {
      head->next = NULL;
    }
  } else {
    //msg("[MEM_MGT][WARNING] remove_head_from_list(%s) no elements\n",listP->name);
  }
  return head;
}

//-----------------------------------------------------------------------------

Job_elt * job_list_remove_element (Job_elt * elementP, Job_List * listP) {

  // access optimisation;
  Job_elt *head;

  if (elementP != NULL) {
    // test head
    head = listP->head;
    if (listP->head == elementP) {
      // almost one element
      if (head != NULL) {
        listP->head = head->next;
        listP->nb_elements = listP->nb_elements - 1;
        // if only one element, update tail
        if (listP->head == NULL) {
          listP->tail = NULL;
        } else {
          head->next = NULL;
        }
      }
    } else {
      while (head) {
        if (head->next == elementP) {
          head->next = elementP->next;
          listP->nb_elements = listP->nb_elements - 1;
          if (listP->tail == elementP) {
            listP->tail = head;
          }
          return elementP;
        } else {
          head = head->next;
        }
      }
    }
  }
  return elementP;
}

Event_elt * event_list_remove_element (Event_elt * elementP, Event_List * listP) {

  // access optimisation;
  Event_elt *head;

  if (elementP != NULL) {
    // test head
    head = listP->head;
    if (listP->head == elementP) {
      // almost one element
      if (head != NULL) {
        listP->head = head->next;
        listP->nb_elements = listP->nb_elements - 1;
        // if only one element, update tail
        if (listP->head == NULL) {
          listP->tail = NULL;
        } else {
          head->next = NULL;
        }
      }
    } else {
      while (head) {
        if (head->next == elementP) {
          head->next = elementP->next;
          listP->nb_elements = listP->nb_elements - 1;
          if (listP->tail == elementP) {
            listP->tail = head;
          }
          return elementP;
        } else {
          head = head->next;
        }
      }
    }
  }
  return elementP;
}

Packet_otg_elt * pkt_list_remove_element (Packet_otg_elt * elementP, Packet_OTG_List * listP) {

  // access optimisation;
  Packet_otg_elt *head;

  if (elementP != NULL) {
    // test head
    head = listP->head;
    if (listP->head == elementP) {
      // almost one element
      if (head != NULL) {
        listP->head = head->next;
        listP->nb_elements = listP->nb_elements - 1;
        // if only one element, update tail
        if (listP->head == NULL) {
          listP->tail = NULL;
        } else {
          head->next = NULL;
        }
      }
    } else {
      while (head) {
        if (head->next == elementP) {
          head->next = elementP->next;
          listP->nb_elements = listP->nb_elements - 1;
          if (listP->tail == elementP) {
            listP->tail = head;
          }
          return elementP;
        } else {
          head = head->next;
        }
      }
    }
  }
  return elementP;
}

//-----------------------------------------------------------------------------
/*
 *  add an element to the beginning of a list
 *  @param  pointer on targeted list
 *  @return pointer on removed Job_elt
 */

void job_list_add_head (Job_elt * elementP, Job_List * listP) {

  // access optimisation;
  Job_elt *head;

  if (elementP != NULL) {
    head = listP->head;
    listP->nb_elements = listP->nb_elements + 1;
    // almost one element
    if (head == NULL) {
      listP->head = elementP;
      listP->tail = elementP;
    } else {
      elementP->next = head;
      listP->head = elementP;
    }
  }
}

void event_list_add_head (Event_elt * elementP, Event_List * listP) {

  // access optimisation;
  Event_elt *head;

  if (elementP != NULL) {
    head = listP->head;
    listP->nb_elements = listP->nb_elements + 1;
    // almost one element
    if (head == NULL) {
      listP->head = elementP;
      listP->tail = elementP;
    } else {
      elementP->next = head;
      listP->head = elementP;
    }
  }
}

void pkt_list_add_head (Packet_otg_elt * elementP, Packet_OTG_List * listP) {

  // access optimisation;
  Packet_otg_elt *head;

  if (elementP != NULL) {
    head = listP->head;
    listP->nb_elements = listP->nb_elements + 1;
    // almost one element
    if (head == NULL) {
      listP->head = elementP;
      listP->tail = elementP;
    } else {
      elementP->next = head;
      listP->head = elementP;
    }
  }
}

void event_list_add_element (Event_elt * elementP, Event_elt * previous, Event_List * listP) {

  // access optimisation;
  Event_elt *next;
  elementP->next = NULL;

  if (elementP != NULL && previous != NULL) {
    next = previous->next;
    listP->nb_elements = listP->nb_elements + 1;
    // almost one element
    if (next == NULL) {
      previous->next = elementP;
      listP->tail = elementP;
    } else {
      elementP->next = previous->next;
      previous->next = elementP;
    }
  }
}


//-----------------------------------------------------------------------------
/*
 *  add an element to the end of a list
 *  @param  pointer on targeted list
 *  @return pointer on removed Job_elt
 */
void job_list_add_tail_eurecom (Job_elt * elementP, Job_List * listP) {
  Job_elt *tail;

  if (elementP != NULL) {
    // access optimisation
    listP->nb_elements = listP->nb_elements + 1;
    elementP->next = NULL;
    tail = listP->tail;
    // almost one element
    if (tail == NULL) {
      listP->head = elementP;
    } else {
      tail->next = elementP;
    }
    listP->tail = elementP;
  } else {
    //msg("[CNT_LIST][ERROR] add_cnt_tail() element NULL\n");
  }
}

void event_list_add_tail_eurecom (Event_elt * elementP, Event_List * listP) {
  Event_elt *tail;

  if (elementP != NULL) {
    // access optimisation
    listP->nb_elements = listP->nb_elements + 1;
    elementP->next = NULL;
    tail = listP->tail;
    // almost one element
    if (tail == NULL) {
      listP->head = elementP;
    } else {
      tail->next = elementP;
    }
    listP->tail = elementP;
  } else {
    //msg("[CNT_LIST][ERROR] add_cnt_tail() element NULL\n");
  }
}

void pkt_list_add_tail_eurecom (Packet_otg_elt * elementP, Packet_OTG_List * listP) {
  Packet_otg_elt *tail;

  if (elementP != NULL) {
    // access optimisation
    listP->nb_elements = listP->nb_elements + 1;
    elementP->next = NULL;
    tail = listP->tail;
    // almost one element
    if (tail == NULL) {
      listP->head = elementP;
    } else {
      tail->next = elementP;
    }
    listP->tail = elementP;
  } else {
    //msg("[CNT_LIST][ERROR] add_cnt_tail() element NULL\n");
  }
}

//-----------------------------------------------------------------------------
void job_list_add_list (Job_List * sublistP, Job_List * listP) {

  if (sublistP) {
    if (sublistP->head) {
      // access optimisation
      Job_elt *tail;

      tail = listP->tail;
      // almost one element
      if (tail == NULL) {
        listP->head = sublistP->head;
      } else {
        tail->next = sublistP->head;
      }
      listP->tail = sublistP->tail;
      // clear sublist
      sublistP->head = NULL;
      sublistP->tail = NULL;
      listP->nb_elements = listP->nb_elements + sublistP->nb_elements;
      sublistP->nb_elements = 0;
    }
  }
}

void event_list_add_list (Event_List * sublistP, Event_List * listP) {

  if (sublistP) {
    if (sublistP->head) {
      // access optimisation
      Event_elt *tail;

      tail = listP->tail;
      // almost one element
      if (tail == NULL) {
        listP->head = sublistP->head;
      } else {
        tail->next = sublistP->head;
      }
      listP->tail = sublistP->tail;
      // clear sublist
      sublistP->head = NULL;
      sublistP->tail = NULL;
      listP->nb_elements = listP->nb_elements + sublistP->nb_elements;
      sublistP->nb_elements = 0;
    }
  }
}

void pkt_list_add_list (Packet_OTG_List * sublistP, Packet_OTG_List * listP) {

  if (sublistP) {
    if (sublistP->head) {
      // access optimisation
      Packet_otg_elt *tail;

      tail = listP->tail;
      // almost one element
      if (tail == NULL) {
        listP->head = sublistP->head;
      } else {
        tail->next = sublistP->head;
      }
      listP->tail = sublistP->tail;
      // clear sublist
      sublistP->head = NULL;
      sublistP->tail = NULL;
      listP->nb_elements = listP->nb_elements + sublistP->nb_elements;
      sublistP->nb_elements = 0;
    }
  }
}

//-----------------------------------------------------------------------------
void job_list_display (Job_List * listP) {

    //Correct the output once the content of struct Job is fixed

    /*Job_elt *cursor;
    unsigned short nb_elements = 0;

    if (listP) {
      cursor = listP->head;
      if (cursor) {
        while (cursor != NULL) {
            if (cursor->job.type == eNB_DL)
                printf ("DL eNB From (%d,%d)\n", (cursor->job).nid, (cursor->job).mid);
            else if (cursor->job.type == UE_DL)
                printf ("DL UE From (%d,%d)\n", (cursor->job).nid, (cursor->job).mid);
            else if (cursor->job.type == UE_UL)
                printf ("UL UE From (%d,%d)\n", (cursor->job).nid, (cursor->job).mid);
            else if (cursor->job.type == eNB_UL)
                printf ("UL eNB From (%d,%d)\n", (cursor->job).nid, (cursor->job).mid);
          //msg ("From (%d,%d) To (%d,%d)\n", (cursor->job).node1, (cursor->job).master1, (cursor->job).node2, (cursor->job).master2);
          cursor = cursor->next;
          nb_elements++;
        }
        printf ("Found nb_elements %d nb_elements %d\n", nb_elements, listP->nb_elements);
        //msg ("Found nb_elements %d nb_elements %d\n", nb_elements, listP->nb_elements);
      }
    } else {
      printf ("[SDU_MNGT][WARNING] display_cnt_dbl_lk_list_up() : list is NULL\n");
      //msg ("[SDU_MNGT][WARNING] display_cnt_dbl_lk_list_up() : list is NULL\n");
    }*/
}

void event_list_display (Event_List * listP) {
    Event_elt *cursor;
    unsigned short nb_elements = 0;

    if (listP) {
      cursor = listP->head;
      if (cursor) {
        while (cursor != NULL) {
          printf ("Ev (%d,%d)\n", (cursor->event).type, (cursor->event).frame);
          //msg ("From (%d,%d) To (%d,%d)\n", (cursor->job).node1, (cursor->job).master1, (cursor->job).node2, (cursor->job).master2);
          cursor = cursor->next;
          nb_elements++;
        }
        printf ("Found nb_elements %d nb_elements %d\n", nb_elements, listP->nb_elements);
        //msg ("Found nb_elements %d nb_elements %d\n", nb_elements, listP->nb_elements);
      }
    } else {
      printf ("[SDU_MNGT][WARNING] display_cnt_dbl_lk_list_up() : list is NULL\n");
      //msg ("[SDU_MNGT][WARNING] display_cnt_dbl_lk_list_up() : list is NULL\n");
    }
}

void pkt_list_display (Packet_OTG_List * listP) {
    Packet_otg_elt *cursor;
    unsigned short nb_elements = 0;

    if (listP) {
      cursor = listP->head;
      if (cursor) {
        while (cursor != NULL) {
          printf ("Pkt (DST %d, RB %d)\n", (cursor->otg_pkt).dst_id, (cursor->otg_pkt).rb_id);
          //msg ("From (%d,%d) To (%d,%d)\n", (cursor->job).node1, (cursor->job).master1, (cursor->job).node2, (cursor->job).master2);
          cursor = cursor->next;
          nb_elements++;
        }
        printf ("Found nb_elements %d nb_elements %d\n", nb_elements, listP->nb_elements);
        //msg ("Found nb_elements %d nb_elements %d\n", nb_elements, listP->nb_elements);
      }
    } else {
      printf ("[SDU_MNGT][WARNING] display_cnt_dbl_lk_list_up() : list is NULL\n");
      //msg ("[SDU_MNGT][WARNING] display_cnt_dbl_lk_list_up() : list is NULL\n");
    }
}
