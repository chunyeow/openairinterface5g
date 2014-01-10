/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 2014 Eurecom

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
  Address      : Eurecom, 2229, route des crêtes, 06560 Valbonne Sophia Antipolis, France

*******************************************************************************/
#ifndef __L2_H__
#    define __L2_H__

#ifdef L2_C
#   define TYPEDEF_LIST2(DATA_TYPE_NAME)    typedef  struct DATA_TYPE_NAME ## _lst2_s {DATA_TYPE_NAME *head;DATA_TYPE_NAME *tail; int nb_elements;} DATA_TYPE_NAME ## _lst2_t;\
    void DATA_TYPE_NAME ## _lst2_init ( DATA_TYPE_NAME ## _lst2_t * list_pP);\
    void DATA_TYPE_NAME ## _lst2_free (DATA_TYPE_NAME ## _lst2_t * list_pP);\
    DATA_TYPE_NAME* DATA_TYPE_NAME ## _lst2_remove_element (DATA_TYPE_NAME * element_pP, DATA_TYPE_NAME ## _lst2_t * list_pP);\
    DATA_TYPE_NAME* DATA_TYPE_NAME ## _lst2_get_head (DATA_TYPE_NAME ## _lst2_t* list_pP);\
    DATA_TYPE_NAME* DATA_TYPE_NAME ## _lst2_get_tail (DATA_TYPE_NAME ## _lst2_t* list_pP);\
    DATA_TYPE_NAME* DATA_TYPE_NAME ## _lst2_remove_head (DATA_TYPE_NAME ## _lst2_t* list_pP);\
    DATA_TYPE_NAME* DATA_TYPE_NAME ## _lst2_remove_tail (DATA_TYPE_NAME ## _lst2_t* list_pP);\
    void DATA_TYPE_NAME ## _lst2_add_head (DATA_TYPE_NAME* element_pP, DATA_TYPE_NAME ## _lst2_t* list_pP);\
    void DATA_TYPE_NAME ## _lst2_add_tail (DATA_TYPE_NAME* element_pP, DATA_TYPE_NAME ## _lst2_t* list_pP);\
    void  DATA_TYPE_NAME ## _lst2_add_lst2 (DATA_TYPE_NAME ## _lst2_t* sublist_pP, DATA_TYPE_NAME ## _lst2_t* list_pP);\
    inline void DATA_TYPE_NAME ## _lst2_init ( DATA_TYPE_NAME ## _lst2_t * list_pP) {\
        list_pP->tail = NULL;\
        list_pP->head = NULL;\
        list_pP->nb_elements = 0;\
    }\
    inline void DATA_TYPE_NAME ## _lst2_free (DATA_TYPE_NAME ## _lst2_t * list_pP) {\
        DATA_TYPE_NAME      *le;\
        while ((le = DATA_TYPE_NAME ## _lst2_remove_head (list_pP))) {\
            free_mem_block (le);\
        }\
    }\
    inline DATA_TYPE_NAME* DATA_TYPE_NAME ## _lst2_remove_element (DATA_TYPE_NAME * element_pP, DATA_TYPE_NAME ## _lst2_t * list_pP) {\
        if (element_pP != NULL) {\
            /* head of list*/\
            if (element_pP == list_pP->head) {\
                list_pP->head = element_pP->next;\
                if (list_pP->head == NULL) {\
                    list_pP->tail = NULL;\
                } else {\
                    element_pP->next->previous = NULL;\
                    element_pP->next = NULL;\
                }\
                /* tail of the list*/\
                /* note : case of 1 remaining element in the list has been treated above*/\
            } else if (element_pP == list_pP->tail) {\
                /* so several elements in the list*/\
                list_pP->tail = element_pP->previous;\
                list_pP->tail->next = NULL;\
                element_pP->previous = NULL;\
                /* in the middle of the list, after a head element and before the tail element*/\
            } else {\
                /* link element n-1 with element n+1*/\
                element_pP->previous->next = element_pP->next;\
                element_pP->next->previous = element_pP->previous;\
                element_pP->next = NULL;\
                element_pP->previous = NULL;\
            }\
            list_pP->nb_elements = list_pP->nb_elements - 1;\
        }\
        return element_pP;\
    }\
    inline DATA_TYPE_NAME* DATA_TYPE_NAME ## _lst2_get_head (DATA_TYPE_NAME ## _lst2_t* list_pP) {return list_pP->head;}\
    inline DATA_TYPE_NAME* DATA_TYPE_NAME ## _lst2_get_tail (DATA_TYPE_NAME ## _lst2_t* list_pP) {return list_pP->tail;}\
    inline DATA_TYPE_NAME* DATA_TYPE_NAME ## _lst2_remove_head (DATA_TYPE_NAME ## _lst2_t* list_pP) {\
        DATA_TYPE_NAME      *head;\
        head = list_pP->head;\
        if (head != NULL) {\
            list_pP->head = head->next;\
            list_pP->nb_elements = list_pP->nb_elements - 1;\
            if (list_pP->head == NULL) {\
                list_pP->tail = NULL;\
            } else {\
                list_pP->head->previous = NULL;\
                head->next = NULL;\
            }\
        }\
        return head;\
    }\
    inline DATA_TYPE_NAME* DATA_TYPE_NAME ## _lst2_remove_tail (DATA_TYPE_NAME ## _lst2_t* list_pP){\
        DATA_TYPE_NAME      *tail;\
        tail = list_pP->tail;\
        if (tail != NULL) {\
            list_pP->nb_elements = list_pP->nb_elements - 1;\
            if (list_pP->head == tail) {\
                list_pP->head = NULL;\
                list_pP->tail = NULL;\
            } else {\
                list_pP->tail = tail->previous;\
                tail->previous->next = NULL;\
            }\
            tail->previous = NULL;\
        }\
        return tail;\
    }\
    inline void DATA_TYPE_NAME ## _lst2_add_head (DATA_TYPE_NAME* element_pP, DATA_TYPE_NAME ## _lst2_t* list_pP) {\
        DATA_TYPE_NAME      *head;\
        if (element_pP != NULL) {\
            head = list_pP->head;\
            list_pP->nb_elements = list_pP->nb_elements + 1;\
            if (head == NULL) {\
                list_pP->head = element_pP;\
                list_pP->tail = element_pP;\
            } else {\
                element_pP->next = head;\
                head->previous = element_pP;\
                element_pP->previous = NULL;\
                list_pP->head = element_pP;\
            }\
        }\
    }\
    inline void DATA_TYPE_NAME ## _lst2_add_tail (DATA_TYPE_NAME* element_pP, DATA_TYPE_NAME ## _lst2_t* list_pP) {\
        DATA_TYPE_NAME      *tail;\
        if (element_pP != NULL) {\
            list_pP->nb_elements = list_pP->nb_elements + 1;\
            element_pP->next = NULL;\
            tail = list_pP->tail;\
            if (tail == NULL) {\
                element_pP->previous = NULL;\
                list_pP->head = element_pP;\
            } else {\
                tail->next = element_pP;\
                element_pP->previous = tail;\
            }\
            list_pP->tail = element_pP;\
        }\
    }\
    inline void  DATA_TYPE_NAME ## _lst2_add_lst2 (DATA_TYPE_NAME ## _lst2_t* sublist_pP, DATA_TYPE_NAME ## _lst2_t* list_pP){ \
        if (sublist_pP) {\
            if (sublist_pP->head) {\
                DATA_TYPE_NAME      *tail;\
                tail = list_pP->tail;\
                if (tail == NULL) {\
                    list_pP->head = sublist_pP->head;\
                } else {\
                    tail->next = sublist_pP->head;\
                    sublist_pP->head->previous = tail;\
                }\
                list_pP->tail = sublist_pP->tail;\
                sublist_pP->head = NULL;\
                sublist_pP->tail = NULL;\
            }\
            list_pP->nb_elements = list_pP->nb_elements + sublist_pP->nb_elements;\
        }\
    }
#else
#define TYPEDEF_LIST2(DATA_TYPE_NAME)    typedef  struct DATA_TYPE_NAME ## _lst2_s {DATA_TYPE_NAME *head;DATA_TYPE_NAME *tail; int nb_elements;} DATA_TYPE_NAME ## _lst2_t;\
    extern void DATA_TYPE_NAME ## _lst2_init ( DATA_TYPE_NAME ## _lst2_t * list_pP);\
    extern void DATA_TYPE_NAME ## _lst2_free (DATA_TYPE_NAME ## _lst2_t * list_pP);\
    extern DATA_TYPE_NAME* DATA_TYPE_NAME ## _lst2_remove_element (DATA_TYPE_NAME * element_pP, DATA_TYPE_NAME ## _lst2_t * list_pP);\
    extern DATA_TYPE_NAME* DATA_TYPE_NAME ## _lst2_get_head (DATA_TYPE_NAME ## _lst2_t* list_pP);\
    extern DATA_TYPE_NAME* DATA_TYPE_NAME ## _lst2_get_tail (DATA_TYPE_NAME ## _lst2_t* list_pP);\
    extern DATA_TYPE_NAME* DATA_TYPE_NAME ## _lst2_remove_head (DATA_TYPE_NAME ## _lst2_t* list_pP);\
    extern DATA_TYPE_NAME* DATA_TYPE_NAME ## _lst2_remove_tail (DATA_TYPE_NAME ## _lst2_t* list_pP);\
    extern void DATA_TYPE_NAME ## _lst2_add_head (DATA_TYPE_NAME* element_pP, DATA_TYPE_NAME ## _lst2_t* list_pP);\
    extern void DATA_TYPE_NAME ## _lst2_add_tail (DATA_TYPE_NAME* element_pP, DATA_TYPE_NAME ## _lst2_t* list_pP);\
    extern void DATA_TYPE_NAME ## _lst2_add_lst2 (DATA_TYPE_NAME ## _lst2_t* sublist_pP, DATA_TYPE_NAME ## _lst2_t* list_pP);
#endif
#define LIST2(DATA_TYPE_NAME, VAR_NAME)              DATA_TYPE_NAME ## _lst2_t VAR_NAME ## _list;






#endif
