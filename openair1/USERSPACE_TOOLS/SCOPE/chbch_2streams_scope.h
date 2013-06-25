/** Header file generated with fdesign on Tue Nov  4 16:26:49 2008.**/

#ifndef FD_chbch_scope_h_
#define FD_chbch_scope_h_

/** Callbacks, globals and object handlers **/


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *chbch_scope;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *control_miniframe;
	FL_OBJECT *channel_t_re;
	FL_OBJECT *scatter_plot;
	FL_OBJECT *demod_out;
	FL_OBJECT *channel_f;
	FL_OBJECT *channel_t_im;
	FL_OBJECT *decoder_input;
	FL_OBJECT *determ;
	FL_OBJECT *ideterm;
} FD_chbch_scope;

extern FD_chbch_scope * create_form_chbch_scope(void);

#endif /* FD_chbch_scope_h_ */
