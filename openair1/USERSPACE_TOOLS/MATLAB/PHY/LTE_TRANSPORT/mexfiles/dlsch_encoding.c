#include "../../../defs.h"

/*==============================================================================
* dlsch_encoding.c
*
* Returns the encoded bit sequence.
*
* example: e = dlsch_encoding(bits,struct1,struct2)
* 
* Author: Sebastian Wagner
* Date: 23-07-2012
*
===============================================================================*/

// #define DEBUG_DLSCH_ENCODING

void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[]
                )
{
    
    /* Declare */
	unsigned char *input_buffer;
	unsigned char *e; // encoded bits = output
	unsigned char mcs=0;	
	unsigned char mod_order=2;
	unsigned char num_pdcch_symbols=1;
	unsigned char harq_pid=0;
	unsigned char subframe=0;
	unsigned char Kmimo=1;
	unsigned char Mdlharq=8;
	unsigned char abstraction_flag;
	int ret, input_buffer_length;
	unsigned int G;
	mxArray *tmp;
	LTE_eNB_DLSCH_t* dlsch;
	LTE_DL_FRAME_PARMS *frame_parms;
	time_stats_t dlsch_rate_matching_stats, dlsch_turbo_encoding_stats, dlsch_interleaving_stats;
	
	
	/* Check proper input and output. */
	if(nrhs!=3)
		mexErrMsgTxt("Three inputs required.");
	else if(nlhs > 1)
		mexErrMsgTxt("Too many output arguments.");
	else if(!mxIsStruct(prhs[1]))
		mexErrMsgTxt("Second input must be a structure.");
	else if(!mxIsStruct(prhs[2]))
		mexErrMsgTxt("Third input must be a structure.");
	
	// Init CRC tables
	crcTableInit();	
	
	/* Create new dlsch */
	tmp = mxGetField(prhs[2],0,"Kmimo");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'Kmimo' in input argument 3.");
	} else {
		Kmimo = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[2],0,"Mdlharq");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'Mdlharq' in input argument 3.");
	} else {
		Mdlharq = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[1],0,"abstraction_flag");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'abstraction_flag' in input argument 2.");
	} else {
		abstraction_flag = (unsigned char) mxGetScalar(tmp);
	}
	
	dlsch = new_eNB_dlsch(Kmimo,Mdlharq,25,abstraction_flag);
			
	/* Allocate input */
	input_buffer = (unsigned char*) mxGetData(prhs[0]);				
	(mxGetM(prhs[0]) > 1) ? (input_buffer_length = mxGetM(prhs[0])) : (input_buffer_length = mxGetN(prhs[0]));
	
	tmp = mxGetField(prhs[2],0,"mcs");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'mcs' in input argument 3.");
	} else {
		mcs = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[2],0,"harq_pid");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'harq_pid' in input argument 3.");
	} else {
		harq_pid = (unsigned char) mxGetScalar(tmp);
	}
	
	dlsch->current_harq_pid = harq_pid;
	dlsch->harq_processes[harq_pid]->mcs = mcs;
	
	tmp = mxGetField(prhs[2],0,"rvidx");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'rvidx' in input argument 3.");
	} else {
		dlsch->harq_processes[harq_pid]->rvidx = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[2],0,"Nl");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'Nl' in input argument 3.");
	} else {
		dlsch->harq_processes[harq_pid]->Nl = (unsigned char) mxGetScalar(tmp);
	}
	/*
	tmp = mxGetField(prhs[2],0,"Ndi");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'Ndi' in input argument 3.");
	} else {
		dlsch->harq_processes[harq_pid]->Ndi = (unsigned char) mxGetScalar(tmp);
	}
	*/
	tmp = mxGetField(prhs[1],0,"rb_alloc");	
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'rb_alloc' in input argument 2.");
	} else {
		dlsch->rb_alloc[0] = (unsigned int) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[1],0,"nb_rb");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'nb_rb' in input argument 2.");
	} else {
		dlsch->nb_rb = (unsigned short) mxGetScalar(tmp);
	}
		
	dlsch->harq_processes[harq_pid]->TBS = dlsch_tbs25[get_I_TBS(mcs)][dlsch->nb_rb-1];
	
	// Check if input buffer length is correct
	if ((input_buffer_length) != (dlsch->harq_processes[harq_pid]->TBS/8+4)) {		
		free_eNB_dlsch(dlsch);
		mexErrMsgTxt("Length of the input buffer is incorrect.");
	}
	
	tmp = mxGetField(prhs[1],0,"num_pdcch_symbols");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'num_pdcch_symbols' in input argument 2.");
	} else {
		num_pdcch_symbols = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[1],0,"subframe");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'subframe' in input argument 2.");
	} else {
		subframe = (unsigned char) mxGetScalar(tmp);
	}	
	
	// Create a LTE_DL_FRAME_PARMS structure and assign required params
	frame_parms = calloc(1,sizeof(LTE_DL_FRAME_PARMS));	
	frame_parms->N_RB_DL = (unsigned char) dlsch->nb_rb;
	
	tmp = mxGetField(prhs[1],0,"frame_type");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'frame_type' in input argument 2.");
	} else {
		frame_parms->frame_type = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[1],0,"mode1_flag");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'frame_type' in input argument 2.");
	} else {
		frame_parms->mode1_flag = (unsigned char) mxGetScalar(tmp);
	}
	tmp = mxGetField(prhs[1],0,"Ncp");
	if (tmp == NULL) {
		mexErrMsgTxt("Non-existing field 'frame_type' in input argument 2.");
	} else {
		frame_parms->Ncp = (unsigned char) mxGetScalar(tmp);
	}
	
 	mod_order = get_Qm(dlsch->harq_processes[harq_pid]->mcs);
	G = get_G(frame_parms,dlsch->nb_rb,dlsch->rb_alloc,mod_order,num_pdcch_symbols,0,subframe);
	
	#ifdef DEBUG_DLSCH_ENCODING
	mexPrintf("dlsch->current_harq_pid = %d\n", dlsch->current_harq_pid);
	mexPrintf("dlsch->harq_processes[harq_pid]->rvidx = %d\n", dlsch->harq_processes[harq_pid]->rvidx);
	mexPrintf("dlsch->harq_processes[harq_pid]->Nl = %d\n", dlsch->harq_processes[harq_pid]->Nl);
	mexPrintf("dlsch->harq_processes[harq_pid]->Ndi = %d\n", dlsch->harq_processes[harq_pid]->Ndi);	
	mexPrintf("mod_order = %d\n",mod_order);
	mexPrintf("G = %d\n",G);	
	mexPrintf("frame_parms->N_RB_DL = %d\n", frame_parms->N_RB_DL);
	mexPrintf("frame_parms->frame_type = %d\n", frame_parms->frame_type);
	mexPrintf("frame_parms->mode1_flag = %d\n", frame_parms->mode1_flag);
	mexPrintf("frame_parms->Ncp = %d\n", frame_parms->Ncp);
	#endif
	
	/* Allocate Output */
	plhs[0] = mxCreateNumericMatrix(G, 1, mxUINT8_CLASS, mxREAL);
	e = (unsigned char*) mxGetPr(plhs[0]); 
        
    /* Algo */    	
	ret = dlsch_encoding(input_buffer, frame_parms, num_pdcch_symbols, dlsch, 0, subframe,&dlsch_rate_matching_stats, &dlsch_turbo_encoding_stats, &dlsch_interleaving_stats);		
	
	/* Assign output */
	memcpy((void*) e, (void*) dlsch->e,G);
	
	/* free */
	free_eNB_dlsch(dlsch);
	free(frame_parms);
}
