/** Header file generated with fdesign on Tue Jun  1 11:00:12 2010.**/

#ifndef FD_sensing_form_h_
#define FD_sensing_form_h_

/** Callbacks, globals and object handlers **/


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *sensing_form;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *spec_SN1;
	FL_OBJECT *spec_SN2;
	FL_OBJECT *spec_SN3;
	FL_OBJECT *Secondary_Network_frequencies;
} FD_sensing_form;

extern FD_sensing_form * create_form_sensing_form(void);

#endif /* FD_sensing_form_h_ */
