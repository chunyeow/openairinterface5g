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
#include "sens_scen_2_form.h"

FD_sens_scen_2 *create_form_sens_scen_2(void)
{
  FL_OBJECT *obj;
  FD_sens_scen_2 *fdui = (FD_sens_scen_2 *) fl_calloc(1, sizeof(*fdui));

  fdui->sens_scen_2 = fl_bgn_form(FL_NO_BOX, 460, 280);
  obj = fl_add_box(FL_UP_BOX,0,0,460,280,"");
  fdui->User_1 = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,20,10,420,100,"User_1");
    fl_set_object_color(obj,FL_BLACK,FL_PALEGREEN);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
 /* fdui->User_3 = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,80,340,600,120,"User_3");
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  fdui->User_4 = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,80,490,600,120,"User_4");
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);*/
  fdui->User_2 = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,20,140,420,100,"User_2");
    fl_set_object_color(obj,FL_BLACK,FL_PALEGREEN);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  fl_end_form();

  fdui->sens_scen_2->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

