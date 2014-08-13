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

#include <forms.h>
#include <stdlib.h>
#include "phy_procedures_sim_form.h"

FD_phy_procedures_sim *create_form_phy_procedures_sim(void)
{
  FL_OBJECT *obj;
  FD_phy_procedures_sim *fdui = (FD_phy_procedures_sim *) fl_calloc(1, sizeof(*fdui));

  fdui->phy_procedures_sim = fl_bgn_form(FL_NO_BOX, 640, 320);
  obj = fl_add_box(FL_UP_BOX,0,0,640,320,"");
  fdui->pusch_constellation = obj = fl_add_xyplot(FL_POINTS_XYPLOT,50,30,220,190,"PUSCH constellation");
    fl_set_object_color(obj,FL_BLACK,FL_YELLOW);
  fdui->pdsch_constellation = obj = fl_add_xyplot(FL_POINTS_XYPLOT,370,30,220,190,"PDSCH constellation");
    fl_set_object_color(obj,FL_BLACK,FL_YELLOW);
  fdui->ch00 = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,370,240,210,60,"CH00");
    fl_set_object_color(obj,FL_BLACK,FL_YELLOW);
  fl_end_form();

  fdui->phy_procedures_sim->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

