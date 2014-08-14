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

#ifndef UI_NOTIFICATIONS_H_
#define UI_NOTIFICATIONS_H_

typedef void (*message_write_callback_t)  (const gpointer buffer, const gchar *signal_name);

int ui_disable_connect_button(void);

int ui_enable_connect_button(void);

int ui_messages_read(char *filename);

int ui_messages_open_file_chooser(void);

int ui_messages_save_file_chooser(gboolean filtered);

int ui_filters_open_file_chooser(void);

int ui_filters_save_file_chooser(void);

void ui_progressbar_window_destroy(void);

int ui_progress_bar_set_fraction(double fraction);

int ui_progress_bar_terminate(void);

#endif /* UI_NOTIFICATIONS_H_ */
