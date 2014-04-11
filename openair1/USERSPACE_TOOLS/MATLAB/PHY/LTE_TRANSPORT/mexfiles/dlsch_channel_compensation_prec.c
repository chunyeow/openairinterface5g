#include "../../../defs.h"

/*==============================================================================
* dlsch_channel_compensation_prec.c
*
* Returns the matched filter channel and the scaled channel magnitude.
*
* example: [ymf heff hmag hmagb] = dlsch_channel_compensation_prec(y,H,pmi,struct1,struct2,slot)
* 
* Author: Sebastian Wagner
* Date: 23-07-2012
*
===============================================================================*/

// #define DEBUG_CHANNEL_COMP

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[]
                )
{
	/* Declare */
	int *H,*y,*dl_ch_estimates_ext[NB_ANTENNAS_RX*NB_ANTENNAS_TX],*rxdataF_ext[NB_ANTENNAS_RX*NB_ANTENNAS_TX],*ymf, *hmag, *hmagb,*dl_ch_mag[NB_ANTENNAS_RX*NB_ANTENNAS_TX],*dl_ch_magb[NB_ANTENNAS_RX*NB_ANTENNAS_TX];
	int *rxdataF_comp[NB_ANTENNAS_RX*NB_ANTENNAS_TX], *ymf_out, *hmag_out, *hmagb_out, *heff_out, *heff;
	unsigned int nb_re_per_symbol, nb_re, nb_re_per_frame;
	unsigned char *pmi_ext, mod_order, output_shift,dl_power_off;
	unsigned char symbol;
	int eNB_id = 0,i; // dummy
	mxArray *tmp;
	
	LTE_DL_FRAME_PARMS *frame_parms;
	PHY_MEASUREMENTS *phy_measurements;
	
	/* Check proper input and output. */
	if(nrhs!=6)
		mexErrMsgTxt("5 inputs required.");
	else if(nlhs > 4)
		mexErrMsgTxt("Too many output arguments.");
	else if(!mxIsStruct(prhs[3]))
		mexErrMsgTxt("Fourth input must be a structure.");
	else if(!mxIsStruct(prhs[4]))
		mexErrMsgTxt("Fifth input must be a structure.");
	
	if(!mxIsInt16(prhs[0]))
		mexErrMsgTxt("First argument must belong to Int16 class.");
	
	if(!mxIsInt16(prhs[1]))
		mexErrMsgTxt("Second argument must belong to Int16 class.");
	
	if(!mxIsUint8(prhs[2]))
		mexErrMsgTxt("Third argument must belong to Uint8 class.");
	
	/* Allocate input */
	y = (int*) mxGetData(prhs[0]);
	H = (int*) mxGetData(prhs[1]);
	pmi_ext = (unsigned char*) mxGetData(prhs[2]);
	symbol = (unsigned char) mxGetScalar(prhs[5]);
		
	tmp = mxGetField(prhs[4],0,"mod_order");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'mod_order' in input argument 5.");
	} else {
		mod_order = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[3],0,"log2_maxh");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'log2_maxh' in input argument 4.");
	} else {
		output_shift = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[3],0,"dl_power_offset");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'dl_power_offset' in input argument 4.");
	} else {
		dl_power_off = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[3],0,"nb_re");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'nb_re' in input argument 4.");
	} else {
		nb_re = (unsigned int) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[3],0,"nb_re_per_frame");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'nb_re_per_frame' in input argument 4.");
	} else {
		nb_re_per_frame = (unsigned int) mxGetScalar(tmp);
	}
			
	// Create a LTE_DL_FRAME_PARMS structure and assign required params
	frame_parms = calloc(1,sizeof(LTE_DL_FRAME_PARMS));
	tmp = mxGetField(prhs[3],0,"nb_rb");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'nb_rb' in input argument 4.");
	} else {
		frame_parms->N_RB_DL = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[3],0,"nb_antennas_rx");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'nb_antennas_rx' in input argument 4.");
	} else {
		frame_parms->nb_antennas_rx = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[3],0,"nb_antennas_tx");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'nb_antennas_tx' in input argument 4.");
	} else {
		frame_parms->nb_antennas_tx = (unsigned char) mxGetScalar(tmp);		
	}
	tmp = mxGetField(prhs[3],0,"Ncp");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'Ncp' in input argument 4.");
	} else {
		frame_parms->Ncp = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[3],0,"ofdm_symbol_size");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'ofdm_symbol_size' in input argument 4.");
	} else {
		frame_parms->ofdm_symbol_size = (unsigned short) mxGetScalar(tmp);
	}
				
	// Create PHY_MEASUREMENTS structure and assign required params
	phy_measurements = calloc(1,sizeof(PHY_MEASUREMENTS));
	phy_measurements->n0_power[0] = 1; // dummy values
	phy_measurements->n0_power[1] = 1;
	phy_measurements->n0_power_tot = 1;
			
	// Adapt the channel estimates and receive signal	
	nb_re_per_symbol = frame_parms->N_RB_DL*12;
	
    // Assign pointers
	rxdataF_ext[0] = y;
	rxdataF_ext[1] = &y[nb_re_per_frame];
	rxdataF_ext[2] = &y[2*nb_re_per_frame];
	rxdataF_ext[3] = &y[3*nb_re_per_frame];
					
	/* Allocate Output */
	plhs[0] = mxCreateNumericMatrix(2*nb_re_per_symbol,NB_ANTENNAS_RX*NB_ANTENNAS_TX, mxINT16_CLASS, mxREAL);	
 	ymf_out = (int*) mxGetPr(plhs[0]);
	
	plhs[1] = mxCreateNumericMatrix(2*nb_re_per_symbol,NB_ANTENNAS_RX*NB_ANTENNAS_TX, mxINT16_CLASS, mxREAL);	
 	heff_out = (int*) mxGetPr(plhs[1]);
	
	plhs[2] = mxCreateNumericMatrix(2*nb_re_per_symbol,NB_ANTENNAS_RX*NB_ANTENNAS_TX, mxINT16_CLASS, mxREAL);	
 	hmag_out = (int*) mxGetPr(plhs[2]);
	
	plhs[3] = mxCreateNumericMatrix(2*nb_re_per_symbol,NB_ANTENNAS_RX*NB_ANTENNAS_TX, mxINT16_CLASS, mxREAL);	
 	hmagb_out = (int*) mxGetPr(plhs[3]);
	
	ymf = (int*) mxCalloc(nb_re_per_frame*NB_ANTENNAS_RX*NB_ANTENNAS_TX, sizeof(int));
	heff  = (int*) mxCalloc(nb_re_per_frame*NB_ANTENNAS_RX*NB_ANTENNAS_TX, sizeof(int));
	hmag = (int*) mxCalloc(nb_re_per_frame*NB_ANTENNAS_RX*NB_ANTENNAS_TX, sizeof(int));
	hmagb = (int*) mxCalloc(nb_re_per_frame*NB_ANTENNAS_RX*NB_ANTENNAS_TX, sizeof(int));
	
	// copy input since it gets modified in function "dlsch_channel_compensation_prec"
	memcpy(heff,H,(NB_ANTENNAS_RX*NB_ANTENNAS_TX*nb_re_per_frame)<<2);
		
	rxdataF_comp[0] = ymf;	
	rxdataF_comp[1] = &ymf[nb_re_per_frame];
	rxdataF_comp[2] = &ymf[2*nb_re_per_frame];
	rxdataF_comp[3] = &ymf[3*nb_re_per_frame];	
	dl_ch_estimates_ext[0] = heff;
	dl_ch_estimates_ext[1] = &heff[nb_re_per_frame];
	dl_ch_estimates_ext[2] = &heff[2*nb_re_per_frame];
	dl_ch_estimates_ext[3] = &heff[3*nb_re_per_frame];
	dl_ch_mag[0] = hmag;
	dl_ch_mag[1] = &hmag[nb_re_per_frame];
	dl_ch_mag[2] = &hmag[2*nb_re_per_frame];
	dl_ch_mag[3] = &hmag[3*nb_re_per_frame];
	dl_ch_magb[0] = hmagb;
	dl_ch_magb[1] = &hmagb[nb_re_per_frame];
	dl_ch_magb[2] = &hmagb[2*nb_re_per_frame];
	dl_ch_magb[3] = &hmagb[3*nb_re_per_frame];
	
		
	#ifdef DEBUG_CHANNEL_COMP
	mexPrintf("symbol = %d\n", symbol);	
	mexPrintf("nb_re_per_symbol = %d\n", nb_re_per_symbol);
	mexPrintf("nb_re_per_frame = %d\n", nb_re_per_frame);    
	for(i=0;i<25;i++)
		mexPrintf("pmi_ext = %d\n", pmi_ext[i]);
	#endif
							
    /* Algo */        
	dlsch_channel_compensation_prec(rxdataF_ext,
				                    dl_ch_estimates_ext,
			                        dl_ch_mag,
				                    dl_ch_magb,
				                    rxdataF_comp,
			                        pmi_ext,
			                        frame_parms,
				                    phy_measurements,
			                        eNB_id,
				                    symbol,				     
				                    mod_order,
			                        frame_parms->N_RB_DL,
			                        output_shift,
				                    dl_power_off);
				
	// memcpy copies bytes!
	memcpy(ymf_out,&rxdataF_comp[0][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
	memcpy(&ymf_out[nb_re_per_symbol],&rxdataF_comp[1][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
	memcpy(&ymf_out[2*nb_re_per_symbol],&rxdataF_comp[2][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
	memcpy(&ymf_out[3*nb_re_per_symbol],&rxdataF_comp[3][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
	
	memcpy(hmag_out,&dl_ch_mag[0][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
	memcpy(&hmag_out[nb_re_per_symbol],&dl_ch_mag[1][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
	memcpy(&hmag_out[2*nb_re_per_symbol],&dl_ch_mag[2][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
	memcpy(&hmag_out[3*nb_re_per_symbol],&dl_ch_mag[3][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
		
	memcpy(hmagb_out, &dl_ch_magb[0][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
	memcpy(&hmagb_out[nb_re_per_symbol],&dl_ch_magb[1][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
	memcpy(&hmagb_out[2*nb_re_per_symbol],&dl_ch_magb[2][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
	memcpy(&hmagb_out[3*nb_re_per_symbol],&dl_ch_magb[3][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
	
	memcpy(heff_out,&dl_ch_estimates_ext[0][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
	memcpy(&heff_out[nb_re_per_symbol],&dl_ch_estimates_ext[1][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
	memcpy(&heff_out[2*nb_re_per_symbol],&dl_ch_estimates_ext[2][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);
	memcpy(&heff_out[3*nb_re_per_symbol],&dl_ch_estimates_ext[3][symbol*frame_parms->N_RB_DL*12],nb_re_per_symbol<<2);	
	
	/* free */
	free(frame_parms);
	free(phy_measurements);
	mxFree(ymf);
	mxFree(heff);
	mxFree(hmag);
	mxFree(hmagb);	
}
