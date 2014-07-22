
/*! \file pad_list.c
* \brief list management primimtives
* \author Mohamed Said MOSLI BOUKSIAA, Lionel GAUTHIER, Navid Nikaein
* \date 2012 - 2014
* \version 0.5
* @ingroup util
*/

/***************************************************************************
                             list_t.c  -  description
                             -------------------
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr
 ***************************************************************************/
#define LIST_C
#define NULL 0

#include "list.h"

//-----------------------------------------------------------------------------
/*
 * initialize list
 */
//-----------------------------------------------------------------------------
void
list_init (list_t * listP, char *nameP)
{
//-----------------------------------------------------------------------------
  int  i = 0;

  if (nameP) {
    while ((listP->name[i] = nameP[i]) && (i++ < LIST_NAME_MAX_CHAR));
  }
  listP->tail = NULL;
  listP->head = NULL;
  listP->nb_elements = 0;
}
//-----------------------------------------------------------------------------
void
list_free (list_t * listP)
{
//-----------------------------------------------------------------------------
  mem_block_t      *le;

  while ((le = list_remove_head (listP))) {
    free_mem_block (le);
  }
}
//-----------------------------------------------------------------------------
mem_block_t *
list_get_head (list_t * listP)
{
//-----------------------------------------------------------------------------
  return listP->head;
}
//-----------------------------------------------------------------------------
/*
 *  remove an element from head of a list
 *  @param  pointer on targeted list
 *  @return pointer on removed mem_block_t
 */
mem_block_t *
list_remove_head (list_t * listP)
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
      head->next = NULL;
    }
  } else {
    //msg("[MEM_MGT][WARNING] remove_head_from_list(%s) no elements\n",listP->name);
  }
  return head;
}

//-----------------------------------------------------------------------------
mem_block_t *
list_remove_element (mem_block_t * elementP, list_t * listP)
{
//-----------------------------------------------------------------------------

  // access optimisation;
  mem_block_t      *head;

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
 *  @return pointer on removed mem_block_t
 */
void
list_add_head (mem_block_t * elementP, list_t * listP)
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
list_add_tail_eurecom (mem_block_t * elementP, list_t * listP)
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
void
list_add_list (list_t * sublistP, list_t * listP)
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
void
list_display (list_t * listP)
{
//-----------------------------------------------------------------------------

  mem_block_t      *cursor;
  //  unsigned short             nb_elements = 0;

  // test lists
  if (listP) {
    cursor = listP->head;
    if (cursor) {
      // almost one element
      //msg ("Display list %s %p", listP->name, listP);
      // while (cursor != NULL) {
      // msg ("%d:", cursor->pool_id);
      // msg ("\n");
      // cursor = cursor->next;
      // nb_elements++;
      //}
      //      msg (" found nb_elements %d nb_elements %d\n", nb_elements, listP->nb_elements);
    }
  } else {
    //msg ("[SDU_MNGT][WARNING] display_cnt_list() : list is NULL\n");
  }
}
