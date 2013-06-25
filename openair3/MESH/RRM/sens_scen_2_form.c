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

