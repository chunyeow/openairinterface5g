#include "../../../defs.h"

/*==============================================================================
* dlsch_dual_stream_correlation.c
*
* Returns the correlation coefficient Heff1'Heff0.
*
* example: rho = dlsch_dual_stream_correlation(Heff0,Heff1,simparms,slot); 
* 
* Author: Sebastian Wagner
* Date: 24-07-2012
*
===============================================================================*/

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[]
                )
{
	/* Declare */
	int *dl_ch_estimates_ext[NB_ANTENNAS_RX*NB_ANTENNAS_TX],*dl_ch_estimates_ext_i[NB_ANTENNAS_RX*NB_ANTENNAS_TX],*Heff0,*Heff1,*rho10,*rho10_out,*dl_ch_rho_ext[NB_ANTENNAS_RX*NB_ANTENNAS_TX];	
	unsigned int nb_re_per_symbol, nb_re_per_frame;
	unsigned char output_shift, symbol;		
	LTE_DL_FRAME_PARMS *frame_parms;	
	mxArray *tmp;
	
	/* Check proper input and output. */
	if(nrhs!=4)
		mexErrMsgTxt("4 inputs required.");
	else if(nlhs > 1)
		mexErrMsgTxt("Too many output arguments.");
	else if(!mxIsStruct(prhs[2]))
		mexErrMsgTxt("Third input must be a structure.");
		
	if(!mxIsInt16(prhs[0]))
		mexErrMsgTxt("First argument must belong to Int16 class.");
	
	if(!mxIsInt16(prhs[1]))
		mexErrMsgTxt("Second argument must belong to Int16 class.");
			
	/* Allocate input */	
	Heff0 = (int*) mxGetData(prhs[0]);
	Heff1 = (int*) mxGetData(prhs[1]);	
	symbol = (unsigned char) mxGetScalar(prhs[3]);
				
	tmp = mxGetField(prhs[2],0,"log2_maxh");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'log2_maxh' in input argument 3.");
	} else {
		output_shift = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[2],0,"nb_re_per_frame");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'nb_re_per_frame' in input argument 3.");
	} else {
		nb_re_per_frame = (unsigned int) mxGetScalar(tmp);
	}    
			
	// Create a LTE_DL_FRAME_PARMS structure and assign required params
	frame_parms = calloc(1,sizeof(LTE_DL_FRAME_PARMS));
	tmp = mxGetField(prhs[2],0,"nb_rb");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'nb_rb' in input argument 3.");
	} else {
		frame_parms->N_RB_DL = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[2],0,"nb_antennas_rx");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'nb_antennas_rx' in input argument 3.");
	} else {
		frame_parms->nb_antennas_rx = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[2],0,"Ncp");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'Ncp' in input argument 3.");
	} else {
		frame_parms->Ncp = (unsigned char) mxGetScalar(tmp);
	}
			
	nb_re_per_symbol = frame_parms->N_RB_DL*12;		
				
	/* Allocate Output */
	plhs[0] = mxCreateNumericMatrix(2*nb_re_per_symbol,NB_ANTENNAS_RX*NB_ANTENNAS_TX, mxINT16_CLASS, mxREAL);	
 	rho10_out = (int*) mxGetPr(plhs[0]);
		
	rho10 = (int*) mxCalloc(nb_re_per_frame*NB_ANTENNAS_RX*NB_ANTENNAS_TX, sizeof(int));
	
	dl_ch_rho_ext[0] = rho10;
	dl_ch_rho_ext[1] = &rho10[nb_re_per_frame];
	dl_ch_rho_ext[2] = &rho10[2*nb_re_per_frame];
    dl_ch_rho_ext[3] = &rho10[3*nb_re_per_frame];
        
    dl_ch_estimates_ext[0] = Heff0;
    dl_ch_estimates_ext[1] = &Heff0[nb_re_per_frame];
    dl_ch_estimates_ext[2] = &Heff0[2*nb_re_per_frame];
    dl_ch_estimates_ext[3] = &Heff0[3*nb_re_per_frame];
    
    dl_ch_estimates_ext_i[0] = Heff1;
    dl_ch_estimates_ext_i[1] = &Heff1[nb_re_per_frame];
    dl_ch_estimates_ext_i[2] = &Heff1[2*nb_re_per_frame];
    dl_ch_estimates_ext_i[3] = &Heff1[3*nb_re_per_frame];

        
    /* Algo */  	
	dlsch_dual_stream_correlation(frame_parms,
			                      symbol,
			                      frame_parms->N_RB_DL,
				                  dl_ch_estimates_ext,
				                  dl_ch_estimates_ext_i,
				                  dl_ch_rho_ext,
				                  output_shift);
		
	
	memcpy(rho10_out,&dl_ch_rho_ext[0][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
	memcpy(&rho10_out[nb_re_per_symbol],&dl_ch_rho_ext[1][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
	memcpy(&rho10_out[2*nb_re_per_symbol],&dl_ch_rho_ext[2][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
	memcpy(&rho10_out[3*nb_re_per_symbol],&dl_ch_rho_ext[3][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
	
	/* free */
	free(frame_parms);
	mxFree(rho10);
}
