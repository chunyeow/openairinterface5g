/* Form definition file generated with fdesign. */

#include "forms.h"
#include <stdlib.h>
#include "lte_scope_ul.h"

FD_lte_scope *create_form_lte_scope(void)
{
  FL_OBJECT *obj;
  FD_lte_scope *fdui = (FD_lte_scope *) fl_calloc(1, sizeof(*fdui));

  fdui->lte_scope = fl_bgn_form(FL_NO_BOX, 830, 650);
  obj = fl_add_box(FL_ROUNDED_BOX,0,0,830,650,"");
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
  fdui->fer = obj = fl_add_text(FL_NORMAL_TEXT,180,570,190,50,"FER =");
    fl_set_object_color(obj,FL_BLACK,FL_LEFT_BCOL);
    fl_set_object_lcolor(obj,FL_WHITE);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fdui->channel_t_re = obj = fl_add_xyplot(FL_NORMAL_XYPLOT,30,20,370,100,"RX Signal Antenna 0");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_RED);
    fl_set_object_lcolor(obj,FL_RED);
  fdui->demod_out = obj = fl_add_xyplot(FL_POINTS_XYPLOT,30,370,600,170,"Demodulator Output");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_YELLOW);
    fl_set_object_lcolor(obj,FL_YELLOW);
  fdui->channel_drs_time = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,30,260,370,90,"DRS Time Response");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
    fl_set_object_lcolor(obj,FL_BLUE);
  fdui->channel_t_im = obj = fl_add_xyplot(FL_NORMAL_XYPLOT,430,20,370,100,"RX Signal Antenna 0");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_RED);
    fl_set_object_lcolor(obj,FL_RED);
  fdui->scatter_plot2 = obj = fl_add_xyplot(FL_POINTS_XYPLOT,640,370,160,170,"Scatter Plot");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_YELLOW);
    fl_set_object_lcolor(obj,FL_YELLOW);
  fdui->channel_srs_time = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,30,140,370,100,"SRS Time Response");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_GREEN);
    fl_set_object_lcolor(obj,FL_GREEN);
  fdui->channel_srs = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,430,140,370,100,"SRS Frequency Response");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_GREEN);
    fl_set_object_lcolor(obj,FL_GREEN);
  fdui->channel_drs = obj = fl_add_xyplot(FL_IMPULSE_XYPLOT,430,260,370,90,"DRS Frequency Response");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_BLUE);
    fl_set_object_lcolor(obj,FL_BLUE);
  fdui->rssi = obj = fl_add_text(FL_NORMAL_TEXT,460,570,200,50,"RSSI =");
    fl_set_object_color(obj,FL_BLACK,FL_MCOL);
    fl_set_object_lcolor(obj,FL_WHITE);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fl_end_form();

  fdui->lte_scope->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

