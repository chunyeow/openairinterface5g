/** Header file generated with fdesign on Fri Aug 12 18:02:36 2011.**/

#ifndef FD_lte_scope_h_
#define FD_lte_scope_h_

/** Callbacks, globals and object handlers **/


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *lte_scope;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *fer;
	FL_OBJECT *channel_t_re;
	FL_OBJECT *demod_out;
	FL_OBJECT *channel_drs_time;
	FL_OBJECT *channel_t_im;
	FL_OBJECT *scatter_plot2;
	FL_OBJECT *channel_srs_time;
	FL_OBJECT *channel_srs;
	FL_OBJECT *channel_drs;
	FL_OBJECT *rssi;
} FD_lte_scope;

extern FD_lte_scope * create_form_lte_scope(void);

#endif /* FD_lte_scope_h_ */
