/** Header file generated with fdesign on Fri Feb 18 14:31:56 2011.**/

#ifndef FD_sens_sensor_h_
#define FD_sens_sensor_h_

/** Callbacks, globals and object handlers **/


/**** Forms and Objects ****/
typedef struct {
	FL_FORM *sens_sensor;
	void *vdata;
	char *cdata;
	long  ldata;
	FL_OBJECT *local_sensing_results;
} FD_sens_sensor;

extern FD_sens_sensor * create_form_sens_sensor(void);

#endif /* FD_sens_sensor_h_ */
