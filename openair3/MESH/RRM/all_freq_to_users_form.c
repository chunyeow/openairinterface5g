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
#include "all_freq_to_users_form.h"

FD_all_freq_to_users *create_form_all_freq_to_users(void)
{
  FL_OBJECT *obj;
  FD_all_freq_to_users *fdui = (FD_all_freq_to_users *) fl_calloc(1, sizeof(*fdui));

  //mod_lor_10_12_07++
  /*fdui->all_freq_to_users = fl_bgn_form(FL_NO_BOX, 460, 300);
  obj = fl_add_box(FL_UP_BOX,0,0,460,300,"");
  fdui->User_2_channels = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,20,160,420,100,"User_2_channels");
    fl_set_object_color(obj,FL_BLACK,FL_CHARTREUSE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
fdui->User_1_channels = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,20,20,420,100,"User_1_channels");
    fl_set_object_color(obj,FL_BLACK,FL_PALEGREEN);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);

  fl_end_form();*/
  //mod_lor_10_12_07--: remove upper part and put part below when on 2 PCs
  
  
  fdui->all_freq_to_users = fl_bgn_form(FL_NO_BOX, 460, 420);
  obj = fl_add_box(FL_UP_BOX,0,0,460,420,"");
  fdui->User_1_channels = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,20,20,420,100,"User_1_channels");
    fl_set_object_color(obj,FL_BLACK,FL_PALEGREEN);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  fdui->User_2_channels = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,20,155,420,100,"User_2_channels");
    fl_set_object_color(obj,FL_BLACK,FL_CHARTREUSE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  fdui->User_3_channels = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,20,290,420,100,"User_3_channels");
    fl_set_object_color(obj,FL_BLACK,FL_CYAN);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  /*fdui->User_4_channels = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,20,440,420,100,"User_4_channels");
    fl_set_object_color(obj,FL_BLACK,FL_SLATEBLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);*/
  fl_end_form();

  fdui->all_freq_to_users->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

