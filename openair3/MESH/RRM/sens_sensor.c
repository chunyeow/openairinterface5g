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
#include "sens_sensor.h"

FD_sens_sensor *create_form_sens_sensor(void)
{
  FL_OBJECT *obj;
  FD_sens_sensor *fdui = (FD_sens_sensor *) fl_calloc(1, sizeof(*fdui));

  fdui->sens_sensor = fl_bgn_form(FL_NO_BOX, 940, 280);
  obj = fl_add_box(FL_UP_BOX,0,0,940,280,"");
  fdui->local_sensing_results = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,30,30,880,210,"Local Sensing Results");
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  fl_end_form();

  fdui->sens_sensor->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

