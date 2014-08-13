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
/* Form definition file generated with fdesign. */

#include "forms.h"
#include <stdlib.h>
#include "chbch_2streams_scope.h"

FD_chbch_scope *create_form_chbch_scope(void)
{
  FL_OBJECT *obj;
  FD_chbch_scope *fdui = (FD_chbch_scope *) fl_calloc(1, sizeof(*fdui));

  fdui->chbch_scope = fl_bgn_form(FL_NO_BOX, 830, 650);
  obj = fl_add_box(FL_ROUNDED_BOX,0,0,830,650,"");
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
  fdui->control_miniframe = obj = fl_add_xyplot(FL_NORMAL_XYPLOT,600,220,190,180,"Control Miniframe");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_DARKORANGE);
  fdui->channel_t_re = obj = fl_add_xyplot(FL_NORMAL_XYPLOT,20,20,280,100,"Time-Domain Channel (Real Component)");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
  fdui->scatter_plot = obj = fl_add_xyplot(FL_POINTS_XYPLOT,600,20,190,190,"Scatter Plot");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_GREEN);
  fdui->demod_out = obj = fl_add_xyplot(FL_POINTS_XYPLOT,20,420,570,180,"Demodulator Output");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_YELLOW);
  fdui->channel_f = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,20,140,570,90,"Frequency Bin Response");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->channel_t_im = obj = fl_add_xyplot(FL_NORMAL_XYPLOT,310,20,280,100,"Time-Domain Channel (Imaginaryl Component)");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
  fdui->decoder_input = obj = fl_add_xyplot(FL_POINTS_XYPLOT,20,240,570,170,"Decoder Input");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_CYAN);
  fdui->determ = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,600,410,190,90,"determ");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_MAGENTA);
  fdui->ideterm = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,600,510,190,90,"ideterm");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_MAGENTA);
  fl_end_form();

  fdui->chbch_scope->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

