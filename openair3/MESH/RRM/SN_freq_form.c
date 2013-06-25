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

