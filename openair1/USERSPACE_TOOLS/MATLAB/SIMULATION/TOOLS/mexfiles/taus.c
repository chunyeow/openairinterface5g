#include "../../../defs.h"
	
/*==============================================================================
* taus.c
*
* Returns random unsigned chars as well as the new seeds.
*
* example: [rchar seeds] = taus(len, seeds)
* 
* Author: Sebastian Wagner
* Date: 23-07-2012
*
===============================================================================*/

unsigned int s0, s1, s2, b;

inline unsigned int mtaus() {

  b = (((s0 << 13) ^ s0) >> 19);
  s0 = (((s0 & 0xFFFFFFFE) << 12)^  b);
  b = (((s1 << 2) ^ s1) >> 25);
  s1 = (((s1 & 0xFFFFFFF8) << 4)^  b);
  b = (((s2 << 3) ^ s2) >> 11);
  s2 = (((s2 & 0xFFFFFFF0) << 17)^  b);
  return s0 ^ s1 ^ s2;
}

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[]
                )
{
    
    /* Declare */	
	unsigned int len, *tseeds, *tseeds_out;
	unsigned char *out;	
	int i;
	
	if(nrhs!=2)
		mexErrMsgTxt("Two inputs required.");
	else if(nlhs > 2)
		mexErrMsgTxt("Too many output arguments.");
	
	/* Allocate input */		
	len = (unsigned int) mxGetScalar(prhs[0]);
	tseeds = (unsigned int*) mxGetData(prhs[1]);
	
	if (mxGetM(prhs[1])!=3 && mxGetN(prhs[1])!=3)
		mexErrMsgTxt("Three seeds are required.");
	
	s0 = tseeds[0];
	s1 = tseeds[1];
	s2 = tseeds[2];
	
	/* Allocate Output */	
	plhs[0] = mxCreateNumericMatrix(len, 1, mxUINT8_CLASS, mxREAL);
	out = (unsigned char*) mxGetPr(plhs[0]);    
	
	plhs[1] = mxCreateNumericMatrix(3, 1, mxUINT32_CLASS, mxREAL);
	tseeds_out = (unsigned int*) mxGetPr(plhs[1]);	
        
    /* Algo */	
	for(i=0;i<len;i++)
		out[i] = (unsigned char)(mtaus()&0xff);
	
	tseeds_out[0] = s0;
	tseeds_out[1] = s1;
	tseeds_out[2] = s2;
}