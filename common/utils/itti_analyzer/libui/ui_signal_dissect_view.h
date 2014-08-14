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

#ifndef UI_SIGNAL_DISSECT_VIEW_H_
#define UI_SIGNAL_DISSECT_VIEW_H_

typedef struct {
    GtkWidget            *text_view;
    PangoFontDescription *text_view_font;
} ui_text_view_t;

ui_text_view_t *ui_signal_dissect_new(GtkWidget *hbox);

int ui_signal_dissect_clear_view(ui_text_view_t *text_view);

gboolean ui_signal_set_text(gpointer user_data, gchar *text, gint length);

#endif /*UI_SIGNAL_DISSECT_VIEW_H_ */
