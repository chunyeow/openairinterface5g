#include "../../../defs.h"

/*==============================================================================
* dlsch_encoding_init.c
*
* Returns the decoded bit sequence.
*
* example: dlsch_decoding_init(struct1)
* 
* Author: Sebastian Wagner
* Date: 23-07-2012
*
===============================================================================*/

//#define DEBUG_DLSCH_DECODING_INIT

void mexFunction( int mlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[]
                )
{
    extern int *pi2tab16[188],*pi5tab16[188],*pi4tab16[188],*pi6tab16[188];
    int i;        
    unsigned int *ptr;
    int **tmp[1];
    
    // Init CRC tables
    // 	crcTableInit();	
	init_td16();
    
    // assign output
    plhs[0] = mxCreateNumericMatrix(4,1, mxUINT32_CLASS, mxREAL);
    ptr = (unsigned int*) mxGetData(plhs[0]);
        
    tmp[0] = &pi2tab16[0];
    memcpy(&ptr[0], tmp, sizeof(tmp[0]));  
    tmp[0] = &pi4tab16[0];   
    memcpy(&ptr[1], tmp, sizeof(tmp[0]));
    tmp[0] = &pi5tab16[0];
    memcpy(&ptr[2], tmp, sizeof(tmp[0]));
    tmp[0] = &pi6tab16[0];
    memcpy(&ptr[3], tmp, sizeof(tmp[0]));
        
    #ifdef DEBUG_DLSCH_DECODING_INIT
    mexPrintf("pi2tab %p\n",&pi2tab16[0]);
    mexPrintf("pi4tab %p\n",&pi4tab16[0]);
    mexPrintf("pi5tab %p\n",&pi5tab16[0]);
    mexPrintf("pi6tab %p\n",&pi6tab16[0]);
    
    mexPrintf("ptr0 %p\n",ptr[0]);
    mexPrintf("ptr1 %p\n",ptr[1]);
    mexPrintf("ptr2 %p\n",ptr[2]);
    mexPrintf("ptr3 %p\n",ptr[3]);
    #endif
    
    // The memory here is never explictly freed via mxFree. It is implicitly taken
    // care of by clear functions at the beginning of the simulation.
    for (i=0;i<188;i++) {
        mexMakeMemoryPersistent((void *) pi2tab16[i]);
        mexMakeMemoryPersistent((void *) pi5tab16[i]);
        mexMakeMemoryPersistent((void *) pi4tab16[i]);
        mexMakeMemoryPersistent((void *) pi6tab16[i]);
    }    
}
