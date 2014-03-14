/*******************************************************************************
Eurecom OpenAirInterface 2
Copyright(c) 1999 - 2014 Eurecom

This program is free software; you can redistribute it and/or modify it
under the terms and conditions of the GNU General Public License,
version 2, as published by the Free Software Foundation.

This program is distributed in the hope it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

The full GNU General Public License is included in this distribution in
the file called "COPYING".

Contact Information
Openair Admin: openair_admin@eurecom.fr
Openair Tech : openair_tech@eurecom.fr
Forums       : http://forums.eurecom.fsr/openairinterface
Address      : EURECOM,
               Campus SophiaTech,
               450 Route des Chappes,
               CS 50193
               06904 Biot Sophia Antipolis cedex,
               FRANCE
*******************************************************************************/
/***************************************************************************
                          list2.c  -  description
                             -------------------
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
 ***************************************************************************/
#define LIST2_C
#include "list.h"
#ifdef USER_MODE
#include "assertions.h"
#else
#define NULL 0
#endif


//-----------------------------------------------------------------------------
/*
 * initialize list
 */
//-----------------------------------------------------------------------------
void
list2_init (list2_t * listP, char *nameP)
{
//-----------------------------------------------------------------------------
  unsigned char              i = 0;

  if (nameP) {
    while ((listP->name[i] = nameP[i]) && (i++ < LIST_NAME_MAX_CHAR));
  }
  listP->tail = NULL;
  listP->head = NULL;
  listP->nb_elements = 0;
}

//-----------------------------------------------------------------------------
void
list2_free (list2_t * listP)
{
//-----------------------------------------------------------------------------

  mem_block_t      *le;

  while ((le = list2_remove_head (listP))) {
    free_mem_block (le);
  }
}

//-----------------------------------------------------------------------------
/*
 *  remove an element from list
 *  @param  pointer on targeted list
 *  @param  mem_block_t to remove
 *  @return pointer on removed mem_block_t
 */
mem_block_t *
list2_remove_element (mem_block_t * elementP, list2_t * listP)
{
//-----------------------------------------------------------------------------

  if (elementP != NULL) {
    // head of list
    if (elementP == listP->head) {
      listP->head = elementP->next;
      if (listP->head == NULL) {
        listP->tail = NULL;
      } else {
        elementP->next->previous = NULL;
        elementP->next = NULL;
      }
      // tail of the list
      // note : case of 1 remaining element in the list has been treated above
    } else if (elementP == listP->tail) {
      // so several elements in the list
      listP->tail = elementP->previous;
      listP->tail->next = NULL;
      elementP->previous = NULL;
      // in the middle of the list, after a head element and before the tail element
    } else {
      // link element n-1 with element n+1
      elementP->previous->next = elementP->next;
      elementP->next->previous = elementP->previous;
      elementP->next = NULL;
      elementP->previous = NULL;
    }
    listP->nb_elements = listP->nb_elements - 1;
  }
  return elementP;
}
//-----------------------------------------------------------------------------
mem_block_t *
list2_get_head (list2_t * listP)
{
//-----------------------------------------------------------------------------
  return listP->head;
}

//-----------------------------------------------------------------------------
mem_block_t *
list2_get_tail (list2_t * listP)
{
//-----------------------------------------------------------------------------
  return listP->tail;
}

//-----------------------------------------------------------------------------
/*
 *  remove an element from head of a list
 *  @param  pointer on targeted list
 *  @return pointer on removed mem_block_t
 */
mem_block_t *
list2_remove_head (list2_t * listP)
{
//-----------------------------------------------------------------------------

  // access optimisation
  mem_block_t      *head;

  head = listP->head;
  // almost one element
  if (head != NULL) {
    listP->head = head->next;
    listP->nb_elements = listP->nb_elements - 1;
    // if only one element, update tail
    if (listP->head == NULL) {
      listP->tail = NULL;
    } else {
      listP->head->previous = NULL;
      head->next = NULL;
    }
  } else {
    //msg("[MEM_MGT][WARNING] remove_head_from_list(%s) no elements\n",listP->name);
  }
  return head;
}

//-----------------------------------------------------------------------------
/*
 *  remove an element from tail of a list
 *  @param  pointer on targeted list
 *  @return pointer on removed mem_block_t
 */
mem_block_t *
list2_remove_tail (list2_t * listP)
{
//-----------------------------------------------------------------------------

  // access optimisation;
  mem_block_t      *tail;


  tail = listP->tail;
  // almost one element;
  if (tail != NULL) {
    listP->nb_elements = listP->nb_elements - 1;
    // if only one element, update head, tail;
    if (listP->head == tail) {
      listP->head = NULL;
      listP->tail = NULL;
    } else {
      listP->tail = tail->previous;
      tail->previous->next = NULL;
    }
    tail->previous = NULL;
  } else {
    //msg("[MEM_MGT][WARNING] remove_head_from_list(%s) no elements\n",listP->name);
  }
  return tail;
}

//-----------------------------------------------------------------------------
/*
 *  add an element to the beginning of a list
 *  @param  pointer on targeted list
 *  @return pointer on removed mem_block_t
 */
void
list2_add_head (mem_block_t * elementP, list2_t * listP)
{
//-----------------------------------------------------------------------------

  // access optimisation;
  mem_block_t      *head;

  if (elementP != NULL) {
    head = listP->head;
    listP->nb_elements = listP->nb_elements + 1;
    // almost one element
    if (head == NULL) {
      listP->head = elementP;
      listP->tail = elementP;
    } else {
      elementP->next = head;
      head->previous = elementP;
      elementP->previous = NULL;
      listP->head = elementP;
    }
  }
}

//-----------------------------------------------------------------------------
/*
 *  add an element to the end of a list
 *  @param  pointer on targeted list
 *  @return pointer on removed mem_block_t
 */
void
list2_add_tail (mem_block_t * elementP, list2_t * listP)
{
  mem_block_t      *tail;
//-----------------------------------------------------------------------------

  if (elementP != NULL) {
    // access optimisation
    listP->nb_elements = listP->nb_elements + 1;
    elementP->next = NULL;
    tail = listP->tail;
    // almost one element
    if (tail == NULL) {
      elementP->previous = NULL;
      listP->head = elementP;
    } else {
      tail->next = elementP;
      elementP->previous = tail;
    }
    listP->tail = elementP;
  }
}

//-----------------------------------------------------------------------------
void
list2_add_list (list2_t * sublistP, list2_t * listP)
{
//-----------------------------------------------------------------------------

  if (sublistP) {
    if (sublistP->head) {
      // access optimisation
      mem_block_t      *tail;

      tail = listP->tail;
      // almost one element
      if (tail == NULL) {
        listP->head = sublistP->head;
      } else {
        tail->next = sublistP->head;
        sublistP->head->previous = tail;
      }
      listP->tail = sublistP->tail;
      // clear sublist
      sublistP->head = NULL;
      sublistP->tail = NULL;
    }

    listP->nb_elements = listP->nb_elements + sublistP->nb_elements;
  }
}

//-----------------------------------------------------------------------------
void
list2_display (list2_t * listP)
{
//-----------------------------------------------------------------------------
/*
  mem_block_t      *cursor;
  unsigned short             nb_elements = 0;
  //uint32_t nb_bytes;
  // uint32_t index;

  // test lists
  if (listP) {
    cursor = listP->head;
    if (cursor) {
      // almost one element
      msg ("Display list %s %p", listP->name, listP);
      while (cursor != NULL) {
        msg ("%d:", cursor->pool_id);
        //nb_bytes = (( sdu_management*)(cursor->misc))->size;
        //   for (index=0; index < nb_bytes; index++) {
        //   msg("%02X.",cursor->data[index]);
        //   }
        msg ("\n");
        cursor = cursor->next;
        nb_elements++;
      }
      msg (" found nb_elements %d nb_elements %d\n", nb_elements, listP->nb_elements);
#ifdef USER_MODE
      AssertFatal(nb_elements == listP->nb_elements, "Bad count of elements %d != %d", nb_elements, listP->nb_elements);
#endif
    }
  }*/
}
