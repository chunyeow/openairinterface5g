/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
   included in this distribution in the file called "COPYING". If not, 
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

#include "rc.h"
#include "types.h"

#ifndef XML_PARSE_H_
#define XML_PARSE_H_

extern void        *xml_raw_data;
extern uint32_t     xml_raw_data_size;

extern types_t     *root;

int xml_parse_file(const char *filename);

int xml_parse_buffer(char *xml_buffer, const int size);

int dissect_signal(buffer_t *buffer, ui_set_signal_text_cb_t ui_set_signal_text_cb,
                   gpointer user_data);

int dissect_signal_header(buffer_t *buffer, ui_set_signal_text_cb_t ui_set_signal_text_cb,
                          gpointer cb_user_data);

#endif  /* XML_PARSE_H_ */
