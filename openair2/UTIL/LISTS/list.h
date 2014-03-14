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
/*
                                 list.h
                             -------------------
  AUTHOR  : Lionel GAUTHIER
  COMPANY : EURECOM
  EMAIL   : Lionel.Gauthier@eurecom.fr

 ***************************************************************************/
#ifndef __LIST_H__
#    define __LIST_H__

#    include "UTIL/MEM/mem_block.h"
//-----------------------------------------------------------------------------
#    ifdef LIST_C
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
#include "platform_constants.h"

#define LIST_NAME_MAX_CHAR 32


typedef struct {
  struct mem_block_t *head;
  struct mem_block_t *tail;
  int                nb_elements;
  char               name[LIST_NAME_MAX_CHAR];
} list2_t;
//-----------------------------------------------------------------------------
typedef struct {
  struct mem_block_t *head;
  struct mem_block_t *tail;
  int                nb_elements;
  char               name[LIST_NAME_MAX_CHAR];
} list_t;
//-----------------------------------------------------------------------------
public_list( void         list_init (list_t* , char *);)
public_list( void         list_free (list_t* listP);)
public_list( mem_block_t* list_get_head (list_t*);)
public_list( mem_block_t* list_remove_head (list_t* );)
public_list( mem_block_t* list_remove_element (mem_block_t*, list_t*);)
public_list( void         list_add_head (mem_block_t* , list_t* );)
public_list( void         list_add_tail_eurecom (mem_block_t* , list_t* );)
public_list( void         list_add_list (list_t* , list_t* );)
public_list( void         list_display (list_t* );)
//-----------------------------------------------------------------------------
public_list2( void         list2_init           (list2_t*, char*);)
public_list2( void         list2_free           (list2_t* );)
public_list2( mem_block_t* list2_get_head       (list2_t*);)
public_list2( mem_block_t* list2_get_tail       (list2_t*);)
public_list2( mem_block_t* list2_remove_element (mem_block_t* , list2_t* );)
public_list2( mem_block_t* list2_remove_head    (list2_t* );)
public_list2( mem_block_t* list2_remove_tail    (list2_t* );)
public_list2( void         list2_add_head       (mem_block_t* , list2_t* );)
public_list2( void         list2_add_tail       (mem_block_t* , list2_t* );)
public_list2( void         list2_add_list       (list2_t* , list2_t* );)
public_list2( void         list2_display        (list2_t* );)

#endif
