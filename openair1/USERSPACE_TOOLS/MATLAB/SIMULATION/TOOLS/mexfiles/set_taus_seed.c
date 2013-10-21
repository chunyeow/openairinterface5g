#include "../../../defs.h"
	
/*==============================================================================
* set_taus_seed.c
*
* Returns seeds which are the input to taus.c.
*
* example: seeds = set_taus_seed(0)
* 
* Author: Sebastian Wagner
* Date: 23-07-2012
*
===============================================================================*/


void mexFunction( int nlhs, mxArray *plhs[],
		int nrhs, const mxArray *prhs[]
		) {
	
	/* Declare */
	unsigned int seed_init;
	unsigned int s0, s1, s2, *ret;
	
	if(nrhs!=1)
		mexErrMsgTxt("Single input required.");
	else if(nlhs > 1)
		mexErrMsgTxt("Too many output arguments.");
	
	/* Allocate input */
	seed_init = (unsigned int) mxGetScalar(prhs[0]);
	
	/* Algo */
#ifdef USER_MODE
struct drand48_data buffer;
unsigned int result = 0;
#endif
	
if (seed_init == 0) {
	s0 = (unsigned int)time(NULL);
	s1 = (unsigned int)time(NULL);
	s2 = (unsigned int)time(NULL);
}
else {
#ifndef USER_MODE
s0 = (unsigned int)0x1e23d852;
s1 = (unsigned int)0x81f38a1c;
s2 = (unsigned int)0xfe1a133e;
#else
/* Use reentrant version of rand48 to ensure that no conflicts with other generators occur */
srand48_r((long int)seed_init, &buffer);
mrand48_r(&buffer, (long int *)&result);
s0 = result;
mrand48_r(&buffer, (long int *)&result);
s1 = result;
mrand48_r(&buffer, (long int *)&result);
s2 = result;
#endif
}
	
	/* Allocate Output */	
	plhs[0] = mxCreateNumericMatrix(3, 1, mxUINT32_CLASS, mxREAL);
	ret = (unsigned int*) mxGetPr(plhs[0]);
	
	ret[0] = s0;
	ret[1] = s1;
	ret[2] = s2;
	
}