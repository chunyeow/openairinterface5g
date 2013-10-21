#include "../../../defs.h"

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[]
                )
{
    
    /* Declare */	
	unsigned int x;
	unsigned char *l2;
	
	/* Allocate input */
	x = (unsigned int) mxGetScalar(prhs[0]);	
		
	/* Allocate Output */
	plhs[0] = mxCreateNumericMatrix(1, 1, mxUINT8_CLASS, mxREAL);
	l2 = (unsigned char*) mxGetPr(plhs[0]); 
        
    /* Algo */    	
	*l2 = log2_approx(x);				
}