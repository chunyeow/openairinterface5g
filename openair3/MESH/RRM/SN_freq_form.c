
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
#include "SN_freq_form.h"

FD_Secondary_Network_frequencies *create_form_Secondary_Network_frequencies(void)
{
  FL_OBJECT *obj;
  FD_Secondary_Network_frequencies *fdui = (FD_Secondary_Network_frequencies *) fl_calloc(1, sizeof(*fdui));

  fdui->Secondary_Network_frequencies = fl_bgn_form(FL_NO_BOX, 620, 200);
  obj = fl_add_box(FL_UP_BOX,0,0,620,200,"");
  fdui->Selected_frequencies = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,70,50,500,100,"Selected_frequencies");
    fl_set_object_color(obj,FL_BLACK,FL_GREEN);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  fl_end_form();

  fdui->Secondary_Network_frequencies->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

