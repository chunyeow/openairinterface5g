/*******************************************************************************
    OpenAirInterface 
    Copyright(c) 1999 - 2014 Eurecom

    OpenAirInterface is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.


    OpenAirInterface is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with OpenAirInterface.The full GNU General Public License is 
   included in this distribution in the file called "COPYING". If not, 
   see <http://www.gnu.org/licenses/>.

  Contact Information
  OpenAirInterface Admin: openair_admin@eurecom.fr
  OpenAirInterface Tech : openair_tech@eurecom.fr
  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
  
  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE

 *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <execinfo.h>
#include <sys/stat.h>

#include "SIMULATION/TOOLS/defs.h"
#include "PHY/types.h"
#include "PHY/defs.h"
#include "PHY/vars.h"
#include "MAC_INTERFACE/vars.h"
#include "ARCH/CBMIMO1/DEVICE_DRIVER/vars.h"
#include "SCHED/defs.h"
#include "SCHED/vars.h"
#include "LAYER2/MAC/vars.h"
#include "OCG_vars.h"


#include "femtoUtils.h"


#define BW    7.68
#define N_RB  25		//50 for 10MHz and 25 for 5 MHz

#define UL_RB_ALLOC 0x1ff;
#define CCCH_RB_ALLOC computeRIV(PHY_vars_eNB->lte_frame_parms.N_RB_UL,0,2)

#define uint64_t DLSCH_RB_ALLOC = 0x1fff;  	 //TODO:  why this value?

PHY_VARS_eNB   *PHY_vars_eNB;
PHY_VARS_UE    *PHY_vars_UE;
PHY_VARS_eNB   **interf_PHY_vars_eNB;
channel_desc_t *eNB2UE;


DCI1E_5MHz_2A_M10PRB_TDD_t  DLSCH_alloc_pdu2_1E;  //TODO:  what it's the use of this variable?

LTE_DL_FRAME_PARMS *frame_parms; //WARNING if you don't put this variable, some macros dosn't work

int main(int argc,char **argv)
{

    options_t opts;
    data_t data;
    uint16_t NB_RB;

    DCI_ALLOC_t dci_alloc[8],dci_alloc_rx[8];

    //Init LOG
    logInit();
    set_comp_log(PHY,LOG_DEBUG,LOG_LOW,1);

    //Parse options
    _initDefaults(&opts);
    _parseOptions(&opts,argc,argv);
    _printOptions(&opts);

    _makeOutputDir(&opts);

    //Init Lte Params

    frame_parms=_lte_param_init(opts);

    NB_RB=conv_nprb(0,(uint32_t)DLSCH_RB_ALLOC);		//TODO: why a function , what to this function?
	
    _allocData(&data,opts.n_tx,opts.n_rx,FRAME_LENGTH_COMPLEX_SAMPLES);

    _fill_Ul_CCCH_DLSCH_Alloc(opts);

    _generatesRandomChannel(opts);

    _allocDLSChannel(opts); // ??

    _generateDCI(opts,dci_alloc,dci_alloc_rx);//,&input_buffer);


    fprintf(opts.outputFile,"s0=[");


    _makeSimulation(data,opts,dci_alloc,dci_alloc_rx,NB_RB,frame_parms);


    _freeMemory(data,opts);

    fprintf(opts.outputFile,"];\n");
    fclose(opts.outputFile);
    fclose(opts.outputBler);

    return 0;
}



void _initDefaults(options_t *opts) {

    opts->snr_init =0;
    opts->snr_max=5;
    opts->snr_step=1;
    opts->nframes=1;

    opts->nsymb=14;
    opts->frame_type=1;				//1 FDD
    opts->transmission_mode=1;		//
    opts->n_tx=1;
    opts->n_rx=1;
    opts->nInterf=0;
    opts->Nid_cell=0;
    opts->oversampling=1;			//TODO why?
    opts->channel_model=Rayleigh1;
    opts->dbInterf=NULL;
    opts->awgn_flag=0;

    opts->num_layers=1; 			//TODO why we need a num of layers?
    opts->n_rnti=0x1234;			//Ratio Network Temporary Identifiers
    opts->mcs=0;					//TODO  why this value?  esto es una variable que tania quiere cambiar ,... investigar implementaciones

    opts->extended_prefix_flag=0; 	//false
    opts->nsymb=14;					// Prefix normal
    opts->pilot1 = 4;
    opts->pilot2 = 7;
    opts->pilot3 = 11;

    opts->num_rounds=4;
    opts->subframe=0;  //TODO why??
    opts->eNB_id = 0;
    opts->amp=1024;
    opts->dci_flag=0;

    opts->testNumber=0;


}

LTE_DL_FRAME_PARMS* _lte_param_init(options_t opts) {
    int i;
    printf("Start lte_param_init\n");

    PHY_vars_eNB = malloc(sizeof(PHY_VARS_eNB));
    PHY_vars_UE = malloc(sizeof(PHY_VARS_UE));
    mac_xface = malloc(sizeof(MAC_xface));

    LTE_DL_FRAME_PARMS *lte_frame_parms = &(PHY_vars_eNB->lte_frame_parms);


    lte_frame_parms->N_RB_DL            = N_RB;
    lte_frame_parms->N_RB_UL            = N_RB;
    lte_frame_parms->Ncp                = opts.extended_prefix_flag;
    lte_frame_parms->Nid_cell           = opts.Nid_cell;
    lte_frame_parms->nushift            = 0;
    lte_frame_parms->nb_antennas_tx     = opts.n_tx;
    lte_frame_parms->nb_antennas_rx     = opts.n_rx;
    lte_frame_parms->phich_config_common.phich_resource         = oneSixth;  //TODO Why??
    lte_frame_parms->tdd_config = 3;
    lte_frame_parms->frame_type         = opts.frame_type;
    lte_frame_parms->mode1_flag = (opts.transmission_mode == 1)? 1 : 0;

    randominit(1);
    set_taus_seed(1);

    init_frame_parms(lte_frame_parms,opts.oversampling);
    phy_init_top(lte_frame_parms);

    //para que se usan estos ??
    lte_frame_parms->twiddle_fft      = twiddle_fft;		//TODO Why?? Pointer to twiddle factors for FFT.
    lte_frame_parms->twiddle_ifft     = twiddle_ifft;		//TODO Why?? pointer to twiddle factors for IFFT
    lte_frame_parms->rev              = rev; 				//TODO Why?? pointer to FFT permutation vector

    PHY_vars_UE->is_secondary_ue = 0;

    PHY_vars_UE->lte_frame_parms = *lte_frame_parms;
    PHY_vars_eNB->lte_frame_parms = *lte_frame_parms;

    phy_init_lte_top(lte_frame_parms);
    dump_frame_parms(lte_frame_parms); //print

    for (i=0; i<3; i++)
        lte_gold(lte_frame_parms,PHY_vars_UE->lte_gold_table[i],i);   //TODO why it's necessary make this 3 times?  ... lo hace por sector.

    phy_init_lte_ue(PHY_vars_UE,0);
    phy_init_lte_eNB(PHY_vars_eNB,0,0,0);

    //Init interference nodes

    if (opts.nInterf>0)
    {
        interf_PHY_vars_eNB = malloc(opts.nInterf*sizeof(PHY_VARS_eNB));

        for (i=0; i<opts.nInterf; i++)
        {
            interf_PHY_vars_eNB[i]=malloc(sizeof(PHY_VARS_eNB));
            memcpy((void*)&interf_PHY_vars_eNB[i]->lte_frame_parms,(void*)&lte_frame_parms,sizeof(LTE_DL_FRAME_PARMS));
            interf_PHY_vars_eNB[i]->lte_frame_parms.Nid_cell=opts.Nid_cell+i+1;
            interf_PHY_vars_eNB[i]->lte_frame_parms.nushift=(opts.Nid_cell+i+1)%6;
            interf_PHY_vars_eNB[i]->Mod_id=i+1;
            phy_init_lte_eNB(interf_PHY_vars_eNB[i],0,0,0);

        }
    }


    printf("Done lte_param_init\n");

    return &PHY_vars_eNB->lte_frame_parms;


}

void _fill_Ul_CCCH_DLSCH_Alloc(options_t opts) {

    PHY_vars_UE->lte_ue_pdcch_vars[0]->crnti = opts.n_rnti;

    UL_alloc_pdu.type    = 0;
    UL_alloc_pdu.hopping = 0;
    UL_alloc_pdu.rballoc = UL_RB_ALLOC;
    UL_alloc_pdu.mcs     = 1;
    UL_alloc_pdu.ndi     = 1;
    UL_alloc_pdu.TPC     = 0;
    UL_alloc_pdu.cqi_req = 1;

    CCCH_alloc_pdu.type               = 0;
    CCCH_alloc_pdu.vrb_type           = 0;
    CCCH_alloc_pdu.rballoc            = CCCH_RB_ALLOC;
    CCCH_alloc_pdu.ndi      = 1;
    CCCH_alloc_pdu.mcs      = 1;
    CCCH_alloc_pdu.harq_pid = 0;

    DLSCH_alloc_pdu2_1E.rah              = 0;
    DLSCH_alloc_pdu2_1E.rballoc          = DLSCH_RB_ALLOC;
    DLSCH_alloc_pdu2_1E.TPC              = 0;
    DLSCH_alloc_pdu2_1E.dai              = 0;
    DLSCH_alloc_pdu2_1E.harq_pid         = 0;
    //DLSCH_alloc_pdu2_1E.tb_swap          = 0;
    DLSCH_alloc_pdu2_1E.mcs             = opts.mcs;
    DLSCH_alloc_pdu2_1E.ndi             = 1;
    DLSCH_alloc_pdu2_1E.rv              = 0;
    // Forget second codeword
    DLSCH_alloc_pdu2_1E.tpmi             = (opts.transmission_mode>=5 ? 5 : 0);  // precoding
    DLSCH_alloc_pdu2_1E.dl_power_off     = (opts.transmission_mode==5 ? 0 : 1);


}

void _generatesRandomChannel(options_t opts) {
    eNB2UE = new_channel_desc_scm(PHY_vars_eNB->lte_frame_parms.nb_antennas_tx,
                                  PHY_vars_UE->lte_frame_parms.nb_antennas_rx,
                                  opts.channel_model,
                                  BW,
                                  0.0,	//forgetting_factor,
                                  0,		//rx_sample_offset,
                                  0);		//path_loss_dB
    if (eNB2UE==NULL)
    {
        msg("Problem generating channel model. Exiting.\n");
        exit(-1);
    }
    //TODO: generate  channel for interference
}

//TODO: I don't know what it's the objetive of all this code
void _allocDLSChannel(options_t opts) {
    int i;        
    //dlsch_eNB[user][TB]
    for (i=0; i<2; i++)
    {        
        PHY_vars_eNB->dlsch_eNB[0][i] = new_eNB_dlsch(1,8,0);   //Kmimo, 	Maximum number of HARQ rounds, 	 abstraction_flag

        if (!PHY_vars_eNB->dlsch_eNB[0][i]) {
            printf("Can't get eNB dlsch structures\n");
            exit(-1);
        }

        PHY_vars_eNB->dlsch_eNB[0][i]->rnti = opts.n_rnti;
    }

    for (i=0; i<2; i++)
    {
        PHY_vars_UE->dlsch_ue[0][i]  = new_ue_dlsch(1,8,0);//Kmimo,Mdlharq,abstraction_flag
        if (!PHY_vars_UE->dlsch_ue[0][i]) {
            printf("Can't get ue dlsch structures\n");
            exit(-1);
        }
        PHY_vars_UE->dlsch_ue[0][i]->rnti   = opts.n_rnti; 
    }

    if (DLSCH_alloc_pdu2_1E.tpmi == 5)
        PHY_vars_eNB->eNB_UE_stats[0].DL_pmi_single = (unsigned short)(taus()&0xffff);//DL PMI Single Stream.  (precoding matrix indicator)
    else
        PHY_vars_eNB->eNB_UE_stats[0].DL_pmi_single = 0;


}

void _generateDCI(options_t opts,DCI_ALLOC_t *dci_alloc,DCI_ALLOC_t *dci_alloc_rx)//,uint8_t **input_buffer)
{
    int  num_dci = 0;
    generate_eNB_dlsch_params_from_dci(0,						//subframe
                                       &DLSCH_alloc_pdu2_1E,	//
                                       opts.n_rnti,
                                       format1E_2A_M10PRB,		
                                       PHY_vars_eNB->dlsch_eNB[0],
                                       &PHY_vars_eNB->lte_frame_parms,   
                                       SI_RNTI,
                                       0,
                                       P_RNTI,
                                       PHY_vars_eNB->eNB_UE_stats[0].DL_pmi_single);



    // UE specific DCI

    memcpy(&dci_alloc[num_dci].dci_pdu[0],&DLSCH_alloc_pdu2_1E,sizeof(DCI1E_5MHz_2A_M10PRB_TDD_t));
    dci_alloc[num_dci].dci_length = sizeof_DCI1E_5MHz_2A_M10PRB_TDD_t;
    dci_alloc[num_dci].L          = 2;
    dci_alloc[num_dci].rnti       = opts.n_rnti;
    dci_alloc[num_dci].format     = format1E_2A_M10PRB;


}

void _freeMemory(data_t data,options_t opts)
{
    int i;
    printf("Freeing channel I/O\n");
    for (i=0; i<opts.n_tx; i++) {
        free(data.s_re[i]);
        free(data.s_im[i]);
        free(data.r_re[i]);
        free(data.r_im[i]);
    }
    free(data.s_re);
    free(data.s_im);
    free(data.r_re);
    free(data.r_im);

    printf("Freeing dlsch structures\n");
    for (i=0; i<2; i++) {
        printf("eNB %d\n",i);
        free_eNB_dlsch(PHY_vars_eNB->dlsch_eNB[0][i]);
        printf("UE %d\n",i);
        free_ue_dlsch(PHY_vars_UE->dlsch_ue[0][i]);
    }



}


void _printResults(uint32_t *errs,uint32_t *round_trials,uint32_t dci_errors,double rate)
{
    printf("Errors/trials (%d/%d, %d/%d ,%d/%d ,%d/%d) Pe = (%e,%e,%e,%e) \n\tdci_errors %d/%d, Pe = %e  \n\teffective rate \t%f (%f) \n\tnormalized delay\t %f (%f)\n",
           errs[0],
           round_trials[0],
           errs[1],
           round_trials[1],
           errs[2],
           round_trials[2],
           errs[3],
           round_trials[3],
           (double)errs[0]/(round_trials[0]),
           (double)errs[1]/(round_trials[1]),
           (double)errs[2]/(round_trials[2]),
           (double)errs[3]/(round_trials[3]),
           dci_errors,
           round_trials[0],
           (double)dci_errors/(round_trials[0]),
           rate*((double)(round_trials[0]-dci_errors)/((double)round_trials[0] + round_trials[1] + round_trials[2] + round_trials[3])),
           rate,
           (1.0*(round_trials[0]-errs[0])+2.0*(round_trials[1]-errs[1])+3.0*(round_trials[2]-errs[2])+4.0*(round_trials[3]-errs[3]))/((double)round_trials[0])/(double)PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->TBS,
           (1.0*(round_trials[0]-errs[0])+2.0*(round_trials[1]-errs[1])+3.0*(round_trials[2]-errs[2])+4.0*(round_trials[3]-errs[3]))/((double)round_trials[0]));

}

void _printFileResults(double SNR, double rate,uint32_t  *errs,uint32_t  *round_trials,uint32_t dci_errors,options_t opts)
{

    fprintf(opts.outputFile,"%f %f;\n", SNR, (float)errs[0]/round_trials[0]);

    fprintf(opts.outputBler,"%f;%d;%d;%f;%d;%d;%d;%d;%d;%d;%d;%d;%d\n",
            SNR,
            opts.mcs,
            PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->TBS,
            rate,
            errs[0],
            round_trials[0],
            errs[1],
            round_trials[1],
            errs[2],
            round_trials[2],
            errs[3],
            round_trials[3],
            dci_errors);

}

void _initErrsRoundsTrials(uint32_t **errs,uint32_t **trials,int allocFlag,options_t opts)
{

    int i=0;
    if (allocFlag==1)
    {
        *errs=(uint32_t*)malloc(opts.num_rounds*sizeof(uint32_t));
        *trials=(uint32_t*)malloc(opts.num_rounds*sizeof(uint32_t));
    }

    for (i=0; i<opts.num_rounds; i++)
    {

        (*errs)[i]=0;
        (*trials)[i]=0;
    }
}

void _fillData(options_t opts,data_t data)
{
    uint32_t aux=2*opts.subframe*PHY_vars_UE->lte_frame_parms.samples_per_tti;
    int i,aa,aarx;

    for (i=0; i<2*opts.nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES; i++)
    {
        for (aa=0; aa<PHY_vars_eNB->lte_frame_parms.nb_antennas_tx; aa++)
        {
            if (opts.awgn_flag == 0) {
                data.s_re[aa][i] = ((double)(((short *)PHY_vars_eNB->lte_eNB_common_vars.txdata[opts.eNB_id][aa]))[aux + (i<<1)]);
                data.s_im[aa][i] = ((double)(((short *)PHY_vars_eNB->lte_eNB_common_vars.txdata[opts.eNB_id][aa]))[aux +(i<<1)+1]);
            }
            else {
                for (aarx=0; aarx<PHY_vars_UE->lte_frame_parms.nb_antennas_rx; aarx++) {
                    if (aa==0) {
                        data.r_re[aarx][i] = ((double)(((short *)PHY_vars_eNB->lte_eNB_common_vars.txdata[opts.eNB_id][aa]))[aux +(i<<1)]);
                        data.r_im[aarx][i] = ((double)(((short *)PHY_vars_eNB->lte_eNB_common_vars.txdata[opts.eNB_id][aa]))[aux +(i<<1)+1]);
                    }
                    else {
                        data.r_re[aarx][i] += ((double)(((short *)PHY_vars_eNB->lte_eNB_common_vars.txdata[opts.eNB_id][aa]))[aux+(i<<1)]);
                        data.r_im[aarx][i] += ((double)(((short *)PHY_vars_eNB->lte_eNB_common_vars.txdata[opts.eNB_id][aa]))[aux +(i<<1)+1]);
                    }
                }
            }
        }
    }


}

void _applyNoise(options_t opts, data_t data,double sigma2,double iqim)
{
    uint32_t aux=2*opts.subframe*PHY_vars_UE->lte_frame_parms.samples_per_tti;
    int i,aa;
    for (i=0; i<2*opts.nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES; i++)
    {
        for (aa=0; aa<PHY_vars_eNB->lte_frame_parms.nb_antennas_rx; aa++) {


            ((short*) PHY_vars_UE->lte_ue_common_vars.rxdata[aa])[(aux)+2*i]   = (short) (data.r_re[aa][i] + sqrt(sigma2/2)*gaussdouble(0.0,1.0));
            ((short*) PHY_vars_UE->lte_ue_common_vars.rxdata[aa])[(aux)+2*i+1] = (short) (data.r_im[aa][i] + (iqim*data.r_re[aa][i]) + sqrt(sigma2/2)*gaussdouble(0.0,1.0));
        }
    }



}

uint8_t _generate_dci_top(int num_ue_spec_dci,int num_common_dci,DCI_ALLOC_t *dci_alloc,options_t opts,uint8_t num_pdcch_symbols)
{

    uint8_t num_pdcch_symbols_2=0;

    num_pdcch_symbols_2= generate_dci_top(num_ue_spec_dci,
                                          num_common_dci,
                                          dci_alloc,
                                          0,
                                          1024,
                                          &PHY_vars_eNB->lte_frame_parms,
                                          PHY_vars_eNB->lte_eNB_common_vars.txdataF[opts.eNB_id],
                                          opts.subframe);

    if (num_pdcch_symbols_2 > num_pdcch_symbols) {
        msg("Error: given num_pdcch_symbols not big enough\n");
        exit(-1);
    }
    return num_pdcch_symbols_2;
}


void _makeSimulation(data_t data,options_t opts,DCI_ALLOC_t *dci_alloc,DCI_ALLOC_t *dci_alloc_rx,uint16_t NB_RB,LTE_DL_FRAME_PARMS  *frame_parms)
{
    uint32_t  *errs,*round_trials;
    unsigned char *input_buffer[2];
    unsigned short input_buffer_length;

    //Index and counters
    int aa;			//Antennas index
    int i; 			//General index for arrays
    uint32_t round;
    double SNR;
    uint32_t dci_errors=0;
    uint32_t cont_frames=0;
    uint8_t Ns,l,m;


    //Variables
    uint32_t tbs,coded_bits_per_codeword;
    int num_common_dci=0,num_ue_spec_dci=0;
    double rate=0, sigma2, sigma2_dB=10,uncoded_ber;
    short *uncoded_ber_bit;
    unsigned int dci_cnt,dlsch_active=0;
    unsigned int tx_lev,tx_lev_dB=0; // Signal Power



    //Other defaults values
    double iqim=0.0;
    uint8_t i_mod = 2;
    uint8_t num_pdcch_symbols=3,num_pdcch_symbols_2=0;
    uint8_t dual_stream_UE = 0;
    int eNB_id_i = NUMBER_OF_eNB_MAX;
    int idUser=0;    //index of  number of user, this program use just one user allowed in position 0 of  PHY_vars_eNB->dlsch_eNB
    //Just allow transmision mode 1
    int numOFDMSymbSubcarrier;

    //Status flags
    int32_t status;
    uint32_t ret;
    int re_allocated;


    //Init Pointers to 8 HARQ processes for the DLSCH
    input_buffer_length = PHY_vars_eNB->dlsch_eNB[idUser][0]->harq_processes[0]->TBS/8;//Transport block size/8
    input_buffer[idUser] = (unsigned char *)malloc(input_buffer_length+4);
    memset(input_buffer[idUser],0,input_buffer_length+4);
    for (i=0; i<input_buffer_length; i++) {
        input_buffer[idUser][i]= (unsigned char)(taus()&0xff);//Tausworthe Uniform Random Generator. -Gaussian Noise Generator
    }

    /*********************************************************************************/

    numOFDMSymbSubcarrier=PHY_vars_UE->lte_frame_parms.ofdm_symbol_size/(NB_RB*12);

    _initErrsRoundsTrials(&errs,&round_trials,1, opts);

    for (SNR=opts.snr_init; SNR<opts.snr_max; SNR+=opts.snr_step)
    {
        _initErrsRoundsTrials(&errs,&round_trials,0,opts);

        dci_errors=0;

        for (cont_frames = 0; cont_frames<opts.nframes; cont_frames++)
        {
            round=0;
            eNB2UE->first_run = 1;

            while (round < opts.num_rounds)
            {
                round_trials[round]++;
                tx_lev = 0;
                //Clear the the transmit data in the frequency domain
                printf("FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX %d",FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX);
                for (aa=0; aa<PHY_vars_eNB->lte_frame_parms.nb_antennas_tx; aa++) {
                    memset(&PHY_vars_eNB->lte_eNB_common_vars.txdataF[opts.eNB_id][aa][0],0,FRAME_LENGTH_COMPLEX_SAMPLES_NO_PREFIX*sizeof(mod_sym_t));
                }


                // Simulate HARQ procedures!!!
                if (round == 0) // First round, set Ndi to 1 and rv to floor(round/2)
                {
                    PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->Ndi = 1;
                    PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->rvidx = round>>1;
                    DLSCH_alloc_pdu2_1E.ndi             = 1;			//New Data Indicator 1. 
                    DLSCH_alloc_pdu2_1E.rv              = 0;			//Redundancy version 1. 
                    memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu2_1E,sizeof(DCI1E_5MHz_2A_M10PRB_TDD_t));
                }
                else 	// set Ndi to 0
                {
                    PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->Ndi = 0;
                    PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->rvidx = round>>1;
                    DLSCH_alloc_pdu2_1E.ndi             = 0;				//New Data Indicator 1. 
                    DLSCH_alloc_pdu2_1E.rv              = round>>1;			//Redundancy version 1. 
                    memcpy(&dci_alloc[0].dci_pdu[0],&DLSCH_alloc_pdu2_1E,sizeof(DCI1E_5MHz_2A_M10PRB_TDD_t));
                }


                num_pdcch_symbols_2 =  _generate_dci_top(num_ue_spec_dci,num_common_dci,dci_alloc,opts,num_pdcch_symbols);


                /*****Sending******/

                i_mod=get_Qm(opts.mcs); //Compute Q (modulation order) based on I_MCS.

                coded_bits_per_codeword = get_G(&PHY_vars_eNB->lte_frame_parms,
                                                PHY_vars_eNB->dlsch_eNB[idUser][0]->nb_rb,
                                                PHY_vars_eNB->dlsch_eNB[idUser][0]->rb_alloc,l
                                                i_mod,
                                                num_pdcch_symbols,
                                                opts.subframe);

                //printf("coded_bits_per_codeword:%d",coded_bits_per_codeword);
                tbs = (double)dlsch_tbs25[get_I_TBS(PHY_vars_eNB->dlsch_eNB[idUser][0]->harq_processes[0]->mcs)][PHY_vars_eNB->dlsch_eNB[idUser][0]->nb_rb-1];


                rate = (double)tbs/(double)coded_bits_per_codeword;

                uncoded_ber_bit = (short*) malloc(2*coded_bits_per_codeword);

                if (cont_frames==0 && round==0)
                    printf("\tRate = %f (%f bits/dim) (G %d, TBS %d, mod %d, pdcch_sym %d)\n",
                           rate,rate*i_mod,coded_bits_per_codeword,tbs,i_mod,num_pdcch_symbols);


                // use the PMI from previous trial
                if (DLSCH_alloc_pdu2_1E.tpmi == 5)
                {
                    PHY_vars_eNB->dlsch_eNB[0][0]->pmi_alloc = quantize_subband_pmi(&PHY_vars_UE->PHY_measurements,0);
                    PHY_vars_UE->dlsch_ue[0][0]->pmi_alloc = quantize_subband_pmi(&PHY_vars_UE->PHY_measurements,0);
                }

                status= dlsch_encoding(input_buffer[idUser],
                                       &PHY_vars_eNB->lte_frame_parms,num_pdcch_symbols,
                                       PHY_vars_eNB->dlsch_eNB[idUser][0],opts.subframe);

                if (status<0)	exit(-1);


                PHY_vars_eNB->dlsch_eNB[idUser][0]->rnti = opts.n_rnti+idUser;

                //scrambling
                dlsch_scrambling(&PHY_vars_eNB->lte_frame_parms,
                                 num_pdcch_symbols,
                                 PHY_vars_eNB->dlsch_eNB[idUser][0],
                                 coded_bits_per_codeword, 0, opts.subframe<<1);

                if (opts.nframes==1)
                {
                    _dumpTransportBlockSegments(PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->C,
                                                PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->Cminus,
                                                PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->Kminus,
                                                PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->Kplus,
                                                null,
                                                PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->c);
                }


                re_allocated = dlsch_modulation(PHY_vars_eNB->lte_eNB_common_vars.txdataF[opts.eNB_id],
                                                opts.amp,
                                                opts.subframe,
                                                &PHY_vars_eNB->lte_frame_parms,
                                                num_pdcch_symbols,
                                                PHY_vars_eNB->dlsch_eNB[idUser][0]);


                if (cont_frames==0 && round==0) printf("re_allocated:  %d\n",re_allocated);


                if (opts.num_layers>1)
                    re_allocated = dlsch_modulation(PHY_vars_eNB->lte_eNB_common_vars.txdataF[opts.eNB_id],
                                                    opts.amp, opts.subframe,
                                                    &PHY_vars_eNB->lte_frame_parms,
                                                    num_pdcch_symbols,
                                                    PHY_vars_eNB->dlsch_eNB[idUser][1]);





                generate_pilots(PHY_vars_eNB,PHY_vars_eNB->lte_eNB_common_vars.txdataF[opts.eNB_id],opts.amp,LTE_NUMBER_OF_SUBFRAMES_PER_FRAME);

                do_OFDM_mod(PHY_vars_eNB->lte_eNB_common_vars.txdataF[opts.eNB_id],
                            PHY_vars_eNB->lte_eNB_common_vars.txdata[opts.eNB_id],
                            (opts.subframe*2),
                            &PHY_vars_eNB->lte_frame_parms);
                do_OFDM_mod(PHY_vars_eNB->lte_eNB_common_vars.txdataF[opts.eNB_id],
                            PHY_vars_eNB->lte_eNB_common_vars.txdata[opts.eNB_id],
                            (opts.subframe*2)+1,
                            &PHY_vars_eNB->lte_frame_parms);
                do_OFDM_mod(PHY_vars_eNB->lte_eNB_common_vars.txdataF[opts.eNB_id],
                            PHY_vars_eNB->lte_eNB_common_vars.txdata[opts.eNB_id],
                            (opts.subframe*2)+2,
                            &PHY_vars_eNB->lte_frame_parms);


                if (opts.nframes==1) {
                    write_output("dl_txsigF0.m","txsF0", &PHY_vars_eNB->lte_eNB_common_vars.txdataF[opts.eNB_id][0][opts.subframe*opts.nsymb*PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size],opts.nsymb*PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size*LTE_NUMBER_OF_SUBFRAMES_PER_FRAME ,1,1);
                    if (PHY_vars_eNB->lte_frame_parms.nb_antennas_tx>1)
                        write_output("dl_txsigF1.m","txsF1", &PHY_vars_eNB->lte_eNB_common_vars.txdataF[opts.eNB_id][1][opts.subframe*opts.nsymb*PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size],opts.nsymb*PHY_vars_eNB->lte_frame_parms.ofdm_symbol_size,1,1);
                }



                for (aa=0; aa<PHY_vars_eNB->lte_frame_parms.nb_antennas_tx; aa++) {
                    tx_lev += signal_energy(&PHY_vars_eNB->lte_eNB_common_vars.txdata[opts.eNB_id][aa]
                                            [opts.subframe*PHY_vars_eNB->lte_frame_parms.samples_per_tti],
                                            PHY_vars_eNB->lte_frame_parms.samples_per_tti);
                }
                tx_lev_dB = (unsigned int) dB_fixed(tx_lev);

                if (opts.nframes==1) {
                    printf("tx_lev = %d (%d dB)\n",tx_lev,tx_lev_dB);
                    write_output("dl_txsig0.m","txs0", &PHY_vars_eNB->lte_eNB_common_vars.txdata[opts.eNB_id][0][opts.subframe* PHY_vars_eNB->lte_frame_parms.samples_per_tti],

                                 PHY_vars_eNB->lte_frame_parms.samples_per_tti,1,1);
                }

                /*****End Sending***/


                _fillData(opts,data);



                sigma2_dB = 10*log10((double)tx_lev) +10*log10(numOFDMSymbSubcarrier) - SNR;
                sigma2 = pow(10,sigma2_dB/10);


                _apply_Multipath_Noise_Interference(opts,data,sigma2_dB,sigma2);



                if (opts.nframes==1) {
                    printf("Sigma2 %f (sigma2_dB %f)\n",sigma2,sigma2_dB);
                    printf("RX level in null symbol %d\n",dB_fixed(signal_energy(&PHY_vars_UE->lte_ue_common_vars.rxdata[0][160+OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES],OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2)));
                    printf("RX level in data symbol %d\n",dB_fixed(signal_energy(&PHY_vars_UE->lte_ue_common_vars.rxdata[0][160+(2*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES)],OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2)));
                    printf("rx_level Null symbol %f\n",10*log10(signal_energy_fp(data.r_re,data.r_im,1,OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2,256+(OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES))));
                    printf("rx_level data symbol %f\n",10*log10(signal_energy_fp(data.r_re,data.r_im,1,OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2,256+(2*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES))));
                }

                i_mod = get_Qm(opts.mcs);

if(1)exit(1);
                //TODO: Optimize and clean code
                // Inner receiver scheduling for 3 slots
                for (Ns=(2*opts.subframe); Ns<((2*opts.subframe)+3); Ns++)
                {
                    for (l=0; l<opts.pilot2; l++)
                    {
                        if (opts.nframes==1)
                            printf("Ns %d, l %d\n",Ns,l);
                        /*
                        This function implements the OFDM front end processor (FEP).
                        Parameters:
                        frame_parms 	LTE DL Frame Parameters
                        ue_common_vars 	LTE UE Common Vars
                        l 	symbol within slot (0..6/7)
                        Ns 	Slot number (0..19)
                        sample_offset 	offset within rxdata (points to beginning of subframe)
                        no_prefix 	if 1 prefix is removed by HW

                        */
                        slot_fep(PHY_vars_UE,l,Ns%20,0,0);

#ifdef PERFECT_CE
                        if (opts.awgn_flag==0) {
                            // fill in perfect channel estimates
                            freq_channel(eNB2UE,PHY_vars_UE->lte_frame_parms.N_RB_DL,12*PHY_vars_UE->lte_frame_parms.N_RB_DL + 1);

                            for (k=0; k<NUMBER_OF_eNB_MAX; k++) {
                                for (aa=0; aa<frame_parms->nb_antennas_tx; aa++)
                                {
                                    for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++)
                                    {
                                        for (i=0; i<frame_parms->N_RB_DL*12; i++)
                                        {
                                            ((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[idUser][(aa<<1)+aarx])[2*i+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=(int16_t)(eNB2UE->chF[aarx+(aa*frame_parms->nb_antennas_rx)][i].x*AMP/2);
                                            ((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[idUser][(aa<<1)+aarx])[2*i+1+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=(int16_t)(eNB2UE->chF[aarx+(aa*frame_parms->nb_antennas_rx)][i].y*AMP/2) ;
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            for (aa=0; aa<frame_parms->nb_antennas_tx; aa++)
                            {
                                for (aarx=0; aarx<frame_parms->nb_antennas_rx; aarx++)
                                {
                                    for (i=0; i<frame_parms->N_RB_DL*12; i++)
                                    {
                                        ((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[0][(aa<<1)+aarx])[2*i+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=AMP/2;
                                        ((int16_t *) PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[0][(aa<<1)+aarx])[2*i+1+(l*frame_parms->ofdm_symbol_size+LTE_CE_FILTER_LENGTH)*2]=0/2;
                                    }
                                }
                            }
                        }
#endif


                        if ((Ns==(2+(2*opts.subframe))) && (l==0)) {
                            lte_ue_measurements(PHY_vars_UE,opts.subframe*PHY_vars_UE->lte_frame_parms.samples_per_tti,1,0);
                        }


                        if ((Ns==(2*opts.subframe)) && (l==opts.pilot1))
                        {// process symbols 0,1,2

                            if (opts.dci_flag == 1)
                            {
                                rx_pdcch(&PHY_vars_UE->lte_ue_common_vars,
                                         PHY_vars_UE->lte_ue_pdcch_vars,
                                         &PHY_vars_UE->lte_frame_parms,
                                         opts.subframe,
                                         0,
                                         (PHY_vars_UE->lte_frame_parms.mode1_flag == 1) ? SISO : ALAMOUTI,
                                         0);

                                // overwrite number of pdcch symbols
                                PHY_vars_UE->lte_ue_pdcch_vars[0]->num_pdcch_symbols = num_pdcch_symbols;

                                dci_cnt = dci_decoding_procedure(PHY_vars_UE,
                                                                 dci_alloc_rx,
                                                                 opts.eNB_id,
                                                                 opts.subframe);

                                if (dci_cnt==0)
                                {
                                    dlsch_active = 0;
                                    if (round==0) {
                                        dci_errors++;
                                        round=5;
                                        errs[0]++;
                                        round_trials[0]++;
                                    }
                                }

                                for (i=0; i<dci_cnt; i++)
                                {
                                    //printf("Generating dlsch parameters for RNTI %x\n",dci_alloc_rx[i].rnti);
                                    if ((dci_alloc_rx[i].rnti == opts.n_rnti) &&
                                            (generate_ue_dlsch_params_from_dci(0,
                                                                               dci_alloc_rx[i].dci_pdu,
                                                                               dci_alloc_rx[i].rnti,
                                                                               dci_alloc_rx[i].format,
                                                                               PHY_vars_UE->dlsch_ue[0],
                                                                               &PHY_vars_UE->lte_frame_parms,
                                                                               SI_RNTI,
                                                                               0,
                                                                               P_RNTI)==0)) {

                                        coded_bits_per_codeword = get_G(&PHY_vars_eNB->lte_frame_parms,
                                                                        PHY_vars_UE->dlsch_ue[0][0]->nb_rb,
                                                                        PHY_vars_UE->dlsch_ue[0][0]->rb_alloc,
                                                                        get_Qm(PHY_vars_UE->dlsch_ue[0][0]->harq_processes[PHY_vars_UE->dlsch_ue[0][0]->current_harq_pid]->mcs),
                                                                        PHY_vars_UE->lte_ue_pdcch_vars[0]->num_pdcch_symbols,
                                                                        opts.subframe);
                                        dlsch_active = 1;
                                    }
                                    else {
                                        dlsch_active = 0;
                                        if (round==0) {
                                            dci_errors++;
                                            errs[0]++;
                                            round_trials[0]++;

                                            if (opts.nframes==1) {
                                                printf("DCI misdetection trial %d\n",cont_frames);
                                                round=5;
                                            }
                                        }
                                    }
                                }
                            }  // if dci_flag==1
                            else { //dci_flag == 0

                                PHY_vars_UE->lte_ue_pdcch_vars[0]->crnti = opts.n_rnti;
                                PHY_vars_UE->lte_ue_pdcch_vars[0]->num_pdcch_symbols = num_pdcch_symbols;

                                generate_ue_dlsch_params_from_dci(0,
                                                                  &DLSCH_alloc_pdu2_1E,
                                                                  C_RNTI,
                                                                  format1E_2A_M10PRB,
                                                                  PHY_vars_UE->dlsch_ue[0],
                                                                  &PHY_vars_UE->lte_frame_parms,
                                                                  SI_RNTI,
                                                                  0,
                                                                  P_RNTI);
                                dlsch_active = 1;
                            } // if dci_flag == 1
                        }

                        if (dlsch_active == 1)
                        {
                            if ((Ns==(1+(2*opts.subframe))) && (l==0))
                            {// process PDSCH symbols 1,2,3,4,5,(6 Normal Prefix)

                                for (m=PHY_vars_UE->lte_ue_pdcch_vars[0]->num_pdcch_symbols; m<opts.pilot2; m++)
                                {
#if defined ENABLE_FXP || ENABLE_FLP
                                    //		      printf("fxp or flp release used\n");
                                    if (rx_pdsch(PHY_vars_UE,
                                                 PDSCH,
                                                 opts.eNB_id,
                                                 eNB_id_i,
                                                 opts.subframe,
                                                 m,
                                                 (m==PHY_vars_UE->lte_ue_pdcch_vars[0]->num_pdcch_symbols)?1:0,
                                                 dual_stream_UE,
                                                 i_mod)==-1)
                                    {
                                        dlsch_active = 0;
                                        break;
                                    }
#endif
#ifdef ENABLE_FULL_FLP
                                    // printf("Full flp release used\n");
                                    if (rx_pdsch_full_flp(PHY_vars_UE,
                                                          PDSCH,
                                                          opts.eNB_id,
                                                          eNB_id_i,
                                                          opts.subframe,
                                                          m,
                                                          (m==PHY_vars_UE->lte_ue_pdcch_vars[0]->num_pdcch_symbols)?1:0,
                                                          dual_stream_UE,
                                                          i_mod)==-1)
                                    {
                                        dlsch_active = 0;
                                        break;
                                    }
#endif
                                }
                            }

                            if ((Ns==(1+(2*opts.subframe))) && (l==opts.pilot1))
                            {// process symbols (6 Extended Prefix),7,8,9
                                for (m=opts.pilot2; m<opts.pilot3; m++)
                                {
#if defined ENABLE_FXP || ENABLE_FLP
                                    //			printf("fxp or flp release used\n");
                                    if (rx_pdsch(PHY_vars_UE,
                                                 PDSCH,
                                                 opts.eNB_id,
                                                 eNB_id_i,
                                                 opts.subframe,
                                                 m,
                                                 0,
                                                 dual_stream_UE,
                                                 i_mod)==-1)
                                    {
                                        dlsch_active=0;
                                        break;
                                    }
#endif
#ifdef ENABLE_FULL_FLP
                                    // printf("Full flp release used\n");
                                    if (rx_pdsch_full_flp(PHY_vars_UE,
                                                          PDSCH,
                                                          opts.eNB_id,
                                                          eNB_id_i,
                                                          opts.subframe,
                                                          m,
                                                          0,
                                                          dual_stream_UE,
                                                          i_mod)==-1)
                                    {
                                        dlsch_active=0;
                                        break;
                                    }
#endif
                                }
                            }

                            if ((Ns==(2+(2*opts.subframe))) && (l==0))  // process symbols 10,11,(12,13 Normal Prefix) do deinterleaving for TTI
                            {
                                for (m=opts.pilot3; m<PHY_vars_UE->lte_frame_parms.symbols_per_tti; m++)
                                {
#if defined ENABLE_FXP || ENABLE_FLP
                                    //			printf("fxp or flp release used\n");
                                    if (rx_pdsch(PHY_vars_UE,
                                                 PDSCH,
                                                 opts.eNB_id,
                                                 eNB_id_i,
                                                 opts.subframe,
                                                 m,
                                                 0,
                                                 dual_stream_UE,
                                                 i_mod)==-1)
                                    {
                                        dlsch_active=0;
                                        break;
                                    }
#endif
#ifdef ENABLE_FULL_FLP
                                    // printf("Full flp release used\n");
                                    if (rx_pdsch_full_flp(PHY_vars_UE,
                                                          PDSCH,
                                                          opts.eNB_id,
                                                          eNB_id_i,
                                                          opts.subframe,
                                                          m,
                                                          0,
                                                          dual_stream_UE,
                                                          i_mod)==-1)
                                    {
                                        dlsch_active=0;
                                        break;
                                    }
#endif
                                }
                            }

                            if ((opts.nframes==1) && (Ns==(2+(2*opts.subframe))) && (l==0))
                            {
                                write_output("dl_ch0.m","ch0",eNB2UE->ch[0],eNB2UE->channel_length,1,8);
                                if (PHY_vars_eNB->lte_frame_parms.nb_antennas_tx>1)
                                    write_output("dl_ch1.m","ch1",eNB2UE->ch[PHY_vars_eNB->lte_frame_parms.nb_antennas_rx],eNB2UE->channel_length,1,8);

                                //common vars
                                write_output("dl_rxsig0.m","rxs0", &PHY_vars_UE->lte_ue_common_vars.rxdata[0][0],10*PHY_vars_UE->lte_frame_parms.samples_per_tti,1,1);
                                write_output("dl_rxsigF0.m","rxsF0", &PHY_vars_UE->lte_ue_common_vars.rxdataF[0][0],2*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb,2,1);
                                if (PHY_vars_UE->lte_frame_parms.nb_antennas_rx>1) {
                                    write_output("dl_rxsig1.m","rxs1", PHY_vars_UE->lte_ue_common_vars.rxdata[1],PHY_vars_UE->lte_frame_parms.samples_per_tti,1,1);
                                    write_output("dl_rxsigF1.m","rxsF1", PHY_vars_UE->lte_ue_common_vars.rxdataF[1],2*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb,2,1);
                                }

                                write_output("dlsch00_ch0.m","dl00_ch0",
                                             &(PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[opts.eNB_id][0][0]),
                                             PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb/2,1,1);
                                if (PHY_vars_UE->lte_frame_parms.nb_antennas_rx>1)
                                    write_output("dlsch01_ch0.m","dl01_ch0",
                                                 &(PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[opts.eNB_id][1][0]),
                                                 PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb/2,1,1);
                                if (PHY_vars_eNB->lte_frame_parms.nb_antennas_tx>1)
                                    write_output("dlsch10_ch0.m","dl10_ch0",
                                                 &(PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[opts.eNB_id][2][0]),
                                                 PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb/2,1,1);
                                if ((PHY_vars_UE->lte_frame_parms.nb_antennas_rx>1) && (PHY_vars_eNB->lte_frame_parms.nb_antennas_tx>1))
                                    write_output("dlsch11_ch0.m","dl11_ch0",
                                                 &(PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[opts.eNB_id][3][0]),
                                                 PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb/2,1,1);

                                //pdsch_vars
                                dump_dlsch2(PHY_vars_UE,opts.eNB_id,coded_bits_per_codeword);
                                dump_dlsch2(PHY_vars_UE,eNB_id_i,coded_bits_per_codeword);
                                write_output("dlsch_e.m","e",PHY_vars_eNB->dlsch_eNB[0][0]->e,coded_bits_per_codeword,1,4);

                                //pdcch_vars
                                write_output("pdcchF0_ext.m","pdcchF_ext", PHY_vars_UE->lte_ue_pdcch_vars[opts.eNB_id]->rxdataF_ext[0],2*3*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size,1,1);
                                write_output("pdcch00_ch0_ext.m","pdcch00_ch0_ext",PHY_vars_UE->lte_ue_pdcch_vars[opts.eNB_id]->dl_ch_estimates_ext[0],300*3,1,1);

                                write_output("pdcch_rxF_comp0.m","pdcch0_rxF_comp0",PHY_vars_UE->lte_ue_pdcch_vars[opts.eNB_id]->rxdataF_comp[0],4*300,1,1);
                                write_output("pdcch_rxF_llr.m","pdcch_llr",PHY_vars_UE->lte_ue_pdcch_vars[opts.eNB_id]->llr,2400,1,4);

                            }

                        }
                    }
                }

                //saving PMI incase of Transmission Mode > 5





                PHY_vars_UE->dlsch_ue[0][0]->rnti = opts.n_rnti;
                dlsch_unscrambling(&PHY_vars_UE->lte_frame_parms,
                                   PHY_vars_UE->lte_ue_pdcch_vars[0]->num_pdcch_symbols,
                                   PHY_vars_UE->dlsch_ue[0][0],
                                   coded_bits_per_codeword,
                                   PHY_vars_UE->lte_ue_pdsch_vars[opts.eNB_id]->llr[0],
                                   0,
                                   opts.subframe<<1);



                ret = dlsch_decoding(PHY_vars_UE->lte_ue_pdsch_vars[opts.eNB_id]->llr[0],
                                     &PHY_vars_UE->lte_frame_parms,
                                     PHY_vars_UE->dlsch_ue[0][0],
                                     opts.subframe,
                                     PHY_vars_UE->lte_ue_pdcch_vars[0]->num_pdcch_symbols);


                if (ret <= MAX_TURBO_ITERATIONS)  //No hay errores
                {
                    round=5;
                    if (opts.nframes==1)
                        printf("No DLSCH errors found\n");
                }
                else
                {
                    errs[round]++;
                    round++;

                    if (opts.nframes==1)
                    {
                        printf("DLSCH in error in round %d\n",round);
                        printf("DLSCH errors found, uncoded ber %f\n",uncoded_ber);
                        _dumpTransportBlockSegments(PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->C,
                                                    PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->Cminus,
                                                    PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->Kminus,
                                                    PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->Kplus,
                                                    PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->c,
                                                    PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->c);

                        _writeOuputOneFrame(opts,coded_bits_per_codeword,uncoded_ber_bit,tbs);

                        exit(1);
                    }
                    round++;

                    if (opts.nframes==1) printf("DLSCH in error in round %d\n",round);

                }
                free(uncoded_ber_bit);
                uncoded_ber_bit = NULL;

            }  //round

            if ((errs[0]>=opts.nframes/10) && (cont_frames>(opts.nframes/2)))
                break;



        }   //cont_frames


        printf("---------------------------------------------------------------------\n");
        printf("SNR = %f dB (tx_lev %f, sigma2_dB %f)\n",SNR,(double)tx_lev_dB+10*log10(numOFDMSymbSubcarrier),sigma2_dB);


        _printResults(errs,round_trials,dci_errors,rate);
        _printFileResults( SNR,  rate,errs,round_trials, dci_errors, opts);


        if (((double)errs[0]/(round_trials[0]))<1e-2) break;

    }// SNR


}



void do_OFDM_mod(mod_sym_t **txdataF, int32_t **txdata, uint16_t next_slot, LTE_DL_FRAME_PARMS *frame_parms) {

    int aa, slot_offset, slot_offset_F;


    slot_offset_F = (next_slot)*(frame_parms->ofdm_symbol_size)*((frame_parms->Ncp==1) ? 6 : 7);
    slot_offset = (next_slot)*(frame_parms->samples_per_tti>>1);

    for (aa=0; aa<frame_parms->nb_antennas_tx; aa++) {
        if (frame_parms->Ncp == 1)
            PHY_ofdm_mod(&txdataF[aa][slot_offset_F],        // input
                         &txdata[aa][slot_offset],         // output
                         frame_parms->log2_symbol_size,                // log2_fft_size
                         6,                 // number of symbols
                         frame_parms->nb_prefix_samples,               // number of prefix samples
                         frame_parms->twiddle_ifft,  // IFFT twiddle factors
                         frame_parms->rev,           // bit-reversal permutation
                         CYCLIC_PREFIX);
        else {
            normal_prefix_mod(&txdataF[aa][slot_offset_F],
                              &txdata[aa][slot_offset],
                              7,
                              frame_parms);
        }
    }


}
void _apply_Multipath_Noise_Interference(options_t opts,data_t data,double sigma2_dB,double sigma2)
{
    double iqim=0.0;

    //Multipath channel
    //Generates and applys a random frequency selective random channel model.
    
    printf("\n\nCambios:\n2*opts.nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES: %d\n2*frame_parms->samples_per_tti:%d\n\n",(2*opts.nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES),(2*2*frame_parms->samples_per_tti));
    if (opts.awgn_flag == 0) {

        multipath_channel(eNB2UE,data.s_re,data.s_im,data.r_re,data.r_im,2*opts.nsymb*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES,0);

    }

    //Noise
    _applyNoise(opts,data,sigma2,iqim);

    if (opts.nframes==1) {
        printf("Sigma2 %f (sigma2_dB %f)\n",sigma2,sigma2_dB);
        printf("RX level in null symbol %d\n",dB_fixed(signal_energy(&PHY_vars_UE->lte_ue_common_vars.rxdata[0][160+OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES],OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2)));
        printf("RX level in data symbol %d\n",dB_fixed(signal_energy(&PHY_vars_UE->lte_ue_common_vars.rxdata[0][160+(2*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES)],OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2)));
        printf("rx_level Null symbol %f\n",10*log10(signal_energy_fp(data.r_re,data.r_im,1,OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2,256+(OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES))));
        printf("rx_level data symbol %f\n",10*log10(signal_energy_fp(data.r_re,data.r_im,1,OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES/2,256+(2*OFDM_SYMBOL_SIZE_COMPLEX_SAMPLES))));
    }

    //Interference

    //TODO: implements.....

}

void _writeOuputOneFrame(options_t opts,uint32_t coded_bits_per_codeword,short *uncoded_ber_bit,uint32_t tbs)
{



    write_output("dl_rxsig0.m","rxs0", &PHY_vars_UE->lte_ue_common_vars.rxdata[0][0],10*PHY_vars_UE->lte_frame_parms.samples_per_tti,1,1);
    write_output("dl_rxsigF0.m","rxsF0", &PHY_vars_UE->lte_ue_common_vars.rxdataF[0][0],2*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb,2,1);

    if (PHY_vars_UE->lte_frame_parms.nb_antennas_rx>1) {
        write_output("dl_rxsig1.m","rxs1", PHY_vars_UE->lte_ue_common_vars.rxdata[1],PHY_vars_UE->lte_frame_parms.samples_per_tti,1,1);
        write_output("dl_rxsigF1.m","rxsF1", PHY_vars_UE->lte_ue_common_vars.rxdataF[1],2*PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb,2,1);
        write_output("dlsch01_ch0.m","dl01_ch0",&(PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[opts.eNB_id][1][0]),PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb/2,1,1);
        write_output("dlsch10_ch0.m","dl10_ch0",&(PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[opts.eNB_id][2][0]),PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb/2,1,1);
        if (PHY_vars_eNB->lte_frame_parms.nb_antennas_tx>1)
            write_output("dlsch11_ch0.m","dl11_ch0",&(PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[opts.eNB_id][3][0]),PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb/2,1,1);
    }

    write_output("dlsch00_ch0.m","dl00_ch0",&(PHY_vars_UE->lte_ue_common_vars.dl_ch_estimates[opts.eNB_id][0][0]),PHY_vars_UE->lte_frame_parms.ofdm_symbol_size*opts.nsymb/2,1,1);

    //pdsch_vars
    dump_dlsch2(PHY_vars_UE,opts.eNB_id,coded_bits_per_codeword);

    write_output("dlsch_e.m","e",PHY_vars_eNB->dlsch_eNB[0][0]->e,coded_bits_per_codeword,1,4);
    write_output("dlsch_ber_bit.m","ber_bit",uncoded_ber_bit,coded_bits_per_codeword,1,0);
    write_output("dlsch_eNB_w.m","w",PHY_vars_eNB->dlsch_eNB[0][0]->harq_processes[0]->w[0],3*(tbs+64),1,4);
    write_output("dlsch_UE_w.m","w",PHY_vars_UE->dlsch_ue[0][0]->harq_processes[0]->w[0],3*(tbs+64),1,0);
}

void _dumpTransportBlockSegments(uint32_t C,uint32_t Cminus,uint32_t Kminus,uint32_t Kplus,  uint8_t ** c_UE, uint8_t ** c_eNB)
{
    int i,s;
    int Kr,Kr_bytes;
    for (s=0; s<C; s++)
    {
        if (s<Cminus)
            Kr = Kminus;
        else
            Kr = Kplus;
        Kr_bytes = Kr>>3;

        printf("Decoded_output (Segment %d):\n",s);
        for (i=0; i<Kr_bytes; i++)
        {
            if ( c_UE !=NULL)
                printf("%d : %x (%x)\n",i,c_UE[s][i],c_UE[s][i]^c_eNB[s][i]);
            else
                printf("%d : (%x)\n",i,c_eNB[s][i]);
        }
    }
}
