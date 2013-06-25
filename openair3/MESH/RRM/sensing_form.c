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

