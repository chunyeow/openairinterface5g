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

