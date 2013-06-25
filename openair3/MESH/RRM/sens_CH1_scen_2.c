/* Form definition file generated with fdesign. */

#include "forms.h"
#include <stdlib.h>
#include "sens_CH1_scen_2.h"

FD_sens_CH1_scen_2 *create_form_sens_CH1_scen_2(void)
{
  FL_OBJECT *obj;
  FD_sens_CH1_scen_2 *fdui = (FD_sens_CH1_scen_2 *) fl_calloc(1, sizeof(*fdui));

//mod_lor_10_12_07++
  fdui->sens_CH1_scen_2 = fl_bgn_form(FL_NO_BOX, 460, 560);
  obj = fl_add_box(FL_UP_BOX,0,0,460,560,"");
  fdui->User_1_sens = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,20,10,420,100,"User_1_sens");
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  fdui->User_2_sens = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,20,140,420,100,"User_2_sens");
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  fdui->User_3_sens = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,20,280,420,100,"User_3_sens");
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  fdui->Cluster_2_sensing = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,20,420,420,100,"Cluster_2_received_information");
    fl_set_object_color(obj,FL_BLACK,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
 //mod_lor_10_12_07--: messo in nota   
 
/*  fdui->sens_CH1_scen_2 = fl_bgn_form(FL_NO_BOX, 650, 740);
  obj = fl_add_box(FL_UP_BOX,0,0,650,740,"");
  fdui->User_1_sens = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,40,10,560,110,"User_1_sens");
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  fdui->User_2_sens = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,40,150,560,110,"User_2_sens");
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  fdui->User_3_sens = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,40,290,560,110,"User_3_sens");
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  fdui->User_4_sens = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,40,430,560,110,"User_4_sens"); 
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  fdui->Cluster_2_sensing = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,40,570,560,130,"Cluster_2_global_sensing");
    fl_set_object_color(obj,FL_BLACK,FL_YELLOW);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    */
  fl_end_form();

  fdui->sens_CH1_scen_2->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

