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
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06410 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/

/* Form definition file generated with fdesign. */

#include "forms.h"
#include <stdlib.h>
#include "sensing_form.h"

FD_sensing_form *create_form_sensing_form(void)
{
  FL_OBJECT *obj;
  FD_sensing_form *fdui = (FD_sensing_form *) fl_calloc(1, sizeof(*fdui));

  fdui->sensing_form = fl_bgn_form(FL_NO_BOX, 620, 600);
  obj = fl_add_box(FL_UP_BOX,0,0,620,600,"");
  fdui->spec_SN1 = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,70,50,500,100,"spectrum_plot_SN1");
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  fdui->spec_SN2 = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,70,180,500,100,"spectrum_plot_SN2");
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  fdui->spec_SN3 = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,70,310,500,100,"spectrum_plot_SN3");
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  fdui->Secondary_Network_frequencies = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,70,460,500,100,"Secondary_Network_frequencies");
    fl_set_object_color(obj,FL_BLACK,FL_GREEN);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  fl_end_form();

  fdui->sensing_form->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

