#include "../../../defs.h"

/*==============================================================================
* get_tbs.c
*
* Returns the transport block size (TBS) given the MCS and the number of RBs.
*
* example: tbs = get_tbs(16,25)
* 
* Author: Sebastian Wagner
* Date: 23-07-2012
*
===============================================================================*/

unsigned char get_I_TBS(unsigned char I_MCS);

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[]
                )
{
	
    /* Declare */
	unsigned char mcs, nb_rb;	
	unsigned int *tbs;

	/* Check correct I/O number */
	if(nrhs!=2) {
		mexErrMsgTxt("Two inputs required.");
	} else if(nlhs>1) {
		mexErrMsgTxt("Too many output arguments.");
	}
	
	/* Allocate input */
	mcs = (unsigned char) mxGetScalar(prhs[0]);
	nb_rb = (unsigned char) mxGetScalar(prhs[1]);
		
	/* Check input */
	if((mcs < 0) || (mcs > 28)) {
		mexErrMsgTxt("Wrong MCS index.");
	} else if((nb_rb<=0) || (nb_rb>25)) {
		mexErrMsgTxt("Unsupported number of RBs.");
	};	
	
	/* Allocate Output */
	plhs[0] = mxCreateNumericMatrix(1, 1, mxUINT32_CLASS, mxREAL);
	tbs = (unsigned int*) mxGetPr(plhs[0]); 
        
    /* Algo */    	
	*tbs = dlsch_tbs25[get_I_TBS(mcs)][nb_rb-1];
				
}
