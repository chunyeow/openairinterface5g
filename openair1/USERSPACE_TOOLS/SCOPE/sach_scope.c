/* Form definition file generated with fdesign. */

#include "forms.h"
#include <stdlib.h>
#include "sach_scope.h"

FD_sach_scope *create_form_sach_scope(void)
{
  FL_OBJECT *obj;
  FD_sach_scope *fdui = (FD_sach_scope *) fl_calloc(1, sizeof(*fdui));

  fdui->sach_scope = fl_bgn_form(FL_NO_BOX, 1380, 590);
  obj = fl_add_box(FL_UP_BOX,0,0,1380,590,"");
  fdui->sacch00 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,110,20,90,90,"U0 : DCCH");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach00 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,190,20,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch01 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,280,20,90,90,"U0: DTCH0");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach01 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,360,20,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch02 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,450,20,90,90,"U0: DTCH1");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach02 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,530,20,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch03 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,620,20,90,90,"U0: DTCH2");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach03 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,700,20,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch04 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,790,20,90,90,"U0: DTCH3");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach04 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,870,20,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch10 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,110,150,90,90,"U1 : DCCH");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach10 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,190,150,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch11 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,280,150,90,90,"U1: DTCH0");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach11 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,360,150,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch12 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,450,150,90,90,"U1: DTCH1");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach12 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,530,150,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch13 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,620,150,90,90,"U1: DTCH2");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach13 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,700,150,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch14 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,790,150,90,90,"U1: DTCH3");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach14 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,870,150,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch20 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,110,290,90,90,"U2 : DCCH");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach20 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,190,290,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch21 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,280,290,90,90,"U2: DTCH0");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach21 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,360,290,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch22 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,450,290,90,90,"U2: DTCH1");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach22 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,530,290,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch23 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,620,290,90,90,"U2: DTCH2");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach23 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,700,290,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch24 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,790,290,90,90,"U2: DTCH3");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach24 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,870,290,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch30 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,110,430,90,90,"U3 : DCCH");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach30 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,190,430,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch31 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,280,430,90,90,"U3: DTCH0");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach31 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,360,430,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch32 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,450,430,90,90,"U3: DTCH1");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach32 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,530,430,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch33 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,620,430,90,90,"U3: DTCH2");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach33 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,700,430,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fdui->sacch34 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,790,430,90,90,"U3: DTCH3");
    fl_set_object_color(obj,FL_BLACK,FL_DODGERBLUE);
  fdui->sach34 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,870,430,90,90,"");
    fl_set_object_color(obj,FL_BLACK,FL_RED);
  fl_end_form();

  fdui->sach_scope->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

