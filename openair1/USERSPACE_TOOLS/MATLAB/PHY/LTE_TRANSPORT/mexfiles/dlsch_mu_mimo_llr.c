#include "../../../defs.h"

/*==============================================================================
* dlsch_mu_mimo_llr.c
*
* Returns the LLRs for interference aware receiver.
*
* example: llr = dlsch_mu_mimo_llr(ymf0,ymf1,Hmag0,Hmag1,rho10,simparms,slot);
* 
* Author: Sebastian Wagner
* Date: 24-07-2012
*
===============================================================================*/

// #define DEBUG_MU_MIMO_LLR
#define LLR_GUARD 48

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[]
                )
{
	/* Declare */
	short *ymf0, *ymf1,*hmag0, *hmag1, *rho10, *llr;
    short *llr_16 __attribute__ ((aligned(16)));
	unsigned char mod_order;
	unsigned char symbol, symbol_mod;	
	int nb_re_per_symbol,i;
	mxArray *tmp;
	LTE_DL_FRAME_PARMS *frame_parms;	
                
	/* Check proper input and output. */
	if(nrhs!=7)
		mexErrMsgTxt("7 inputs required.");
	else if(nlhs > 1)
		mexErrMsgTxt("Too many output arguments.");
	else if(!mxIsStruct(prhs[5]))
		mexErrMsgTxt("6. input must be a structure.");
		
	if(!mxIsInt16(prhs[0]))
		mexErrMsgTxt("First argument must belong to Int16 class.");
	
	if(!mxIsInt16(prhs[1]))
		mexErrMsgTxt("Second argument must belong to Int16 class.");
	
	if(!mxIsInt16(prhs[2]))
		mexErrMsgTxt("Third argument must belong to Int16 class.");
	
	if(!mxIsInt16(prhs[3]))
		mexErrMsgTxt("4. argument must belong to Int16 class.");
	
	if(!mxIsInt16(prhs[4]))
		mexErrMsgTxt("5. argument must belong to Int16 class.");
	
	/* Allocate input */
	ymf0 = (short*) mxGetData(prhs[0]);
	ymf1 = (short*) mxGetData(prhs[1]);
	hmag0 = (short*) mxGetData(prhs[2]);
	hmag1 = (short*) mxGetData(prhs[3]);
	rho10 = (short*) mxGetData(prhs[4]);
	symbol = (unsigned char) mxGetScalar(prhs[6]);
		
	tmp = mxGetField(prhs[5],0,"codeword");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'codeword' in input argument 6.");
	} else {
		tmp = mxGetField(mxGetField(prhs[5],0,"codeword"),0,"mod_order");
		if (tmp == NULL) {
			mexErrMsgTxt("Non-existing field 'mod_order' in input argument '6.codeword(1)'.");
		} else {
			mod_order = (unsigned char) mxGetScalar(tmp);
		}
	}	
        
	// Create a LTE_DL_FRAME_PARMS structure and assign required params
	frame_parms = (LTE_DL_FRAME_PARMS*) mxCalloc(1,sizeof(LTE_DL_FRAME_PARMS));	
	tmp = mxGetField(prhs[5],0,"nb_rb");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'nb_rb' in input argument 6.");
	} else {
		frame_parms->N_RB_DL = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[5],0,"nb_antennas_rx");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'nb_antennas_rx' in input argument 6.");
	} else {
		frame_parms->nb_antennas_rx = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[5],0,"nb_antennas_tx");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'nb_antennas_tx' in input argument 6.");
	} else {
		frame_parms->nb_antennas_tx = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[5],0,"Ncp");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'Ncp' in input argument 6.");
	} else {
		frame_parms->Ncp = (unsigned char) mxGetScalar(tmp);
	}
				
	// Adapt the channel estimates and receive signal
	symbol_mod = (symbol>=(7-frame_parms->Ncp)) ? symbol-(7-frame_parms->Ncp) : symbol;
	
	if ((symbol_mod == 0) || (symbol_mod == (4-frame_parms->Ncp)))
 		nb_re_per_symbol = frame_parms->N_RB_DL*8; // pilots
	else
		nb_re_per_symbol = frame_parms->N_RB_DL*12;
		
	/* Allocate Output */
    plhs[0] = mxCreateNumericMatrix(mod_order*nb_re_per_symbol,1, mxINT16_CLASS, mxREAL);
    llr = (short*) mxGetPr(plhs[0]);
    
    #ifdef DEBUG_MU_MIMO_LLR
    mexPrintf("mod_order: %d\n", mod_order);
    mexPrintf("frame_parms->N_RB_DL %d\n", frame_parms->N_RB_DL);
    mexPrintf("frame_parms->nb_antennas_rx %d\n", frame_parms->nb_antennas_rx);
    mexPrintf("frame_parms->nb_antennas_tx %d\n", frame_parms->nb_antennas_tx);
    mexPrintf("frame_parms->Ncp %d\n", frame_parms->Ncp);
    mexPrintf("nb_re_per_symbol %d\n", nb_re_per_symbol);
    mexPrintf("frame_parms %p\n", frame_parms);
//     mexPrintf("ymf0 %p\n",ymf0);
//     mexPrintf("ymf1 %p\n",ymf1);
//     mexPrintf("llr %p\n",llr);
//     mexPrintf("hmag0 %p\n",hmag0);
//     mexPrintf("hmag1 %p\n",hmag1);
//     mexPrintf("rho10 %p\n",rho10);

//     for(i=0;i<(nb_re_per_symbol*2);i++) {
    for(i=0;i<(8*2);i++) {
        mexPrintf("i=%d\n",i);
        mexPrintf("ymf0 = %d\n",ymf0[i]);
        mexPrintf("ymf1 = %d\n",ymf1[i]);
        mexPrintf("hmag0 = %d\n",hmag0[i]);
        mexPrintf("hmag1 = %d\n",hmag1[i]);
        mexPrintf("rho10 = %d\n",rho10[i]);
    }
    #endif
	
    // 16 bit aligned memory allocation with guard samples
    llr_16 = (short *) malloc16((mod_order*nb_re_per_symbol+LLR_GUARD)*sizeof(short));
    
    /* Algo */ 
	switch (mod_order) {
		case 2 :
			qpsk_qpsk(ymf0, ymf1, llr_16, rho10, nb_re_per_symbol);
			break;
		case 4 :
			qam16_qam16(ymf0, ymf1, hmag0, hmag1, llr_16, rho10, nb_re_per_symbol);
			break;			
		case 6 :
			qam64_qam64(ymf0, ymf1, hmag0, hmag1, llr_16, rho10, nb_re_per_symbol);
			break;
			
		default :
			mexErrMsgTxt("Unknown mod_order.");
			break;
	}
    
    // copy just valid LLRs to output
    memcpy((void*) llr, (void *) llr_16, mod_order*nb_re_per_symbol*sizeof(short));
	    
    
	/* free */
	mxFree(frame_parms);
    free(llr_16);
}
