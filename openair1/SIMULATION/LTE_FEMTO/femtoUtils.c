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
#include <getopt.h>
#include <string.h>
#include <math.h>
#include <sys/stat.h>
#include "femtoUtils.h"

#ifndef _FEMTO_UTILS

#include "PHY/types.h"
#include "SIMULATION/TOOLS/defs.h"
#include "UTIL/LOG/vcd_signal_dumper.h" //TVT:Navid

#endif


void _parseOptions(options_t *opts, int argc, char ** argv) {
  char c;
  char aux[100];
  //int prob_flag=0;
    
  static struct option long_options[] =
    {                             
      {"h",     no_argument,       0, 'h'},
      {"s",  required_argument, 0, 's'},
      {"S",  required_argument, 0, 'S'},
      {"T",  required_argument, 0, 'T'},
      {"n",    required_argument, 0, 'n'},
      {"x",    no_argument, 	  0, 'x'},
      {"d",    no_argument, 	  0, 'd'},
      {"t",    required_argument, 0, 't'},
      {"y",    required_argument, 0, 'y'},
      {"z",    required_argument, 0, 'z'},
      {"I",    required_argument, 0, 'I'},               
      {"j",    required_argument, 0, 'j'},
      {"N",    required_argument, 0, 'N'},
      {"o",    required_argument, 0, 'o'},
      {"g",    required_argument, 0, 'g'},
      {"f",    no_argument, 0, 'f'},
      {"a",    no_argument, 	  0, 'a'},
      {"i",    no_argument, 	  0, 'i'},
      {"b",    required_argument, 0, 'b'},
      {"w",    required_argument, 0, 'w'},
      {"k",    required_argument, 0, 'k'},
      {"c",    required_argument, 0, 'c'},
      {"e",    no_argument, 0, 'e'},
      {"m",    required_argument, 0, 'm'},
      {"A",    required_argument, 0, 'A'},
      {"D",    no_argument, 0, 'D'},
      {"p",    no_argument, 0, 'p'},
      {"r",    required_argument, 0, 'r'},
      {"p",    required_argument, 0, 'p'},
      {"Q",    required_argument, 0, 'Q'},
      {"O",    required_argument, 0, 'O'},
      {0, 0, 0, 0}
    };


  int option_index = 0;   
		
  while ((c = getopt_long (argc, argv, "hs:S:T:n:xdt:y:z:I:j:N:o:g:faib:r:R:w:c:em:A:Dp:B:k:Q:O:",long_options, &option_index)) != -1)
    {
      //printf("%c %s\n",c,optarg);
      switch (c)
        {
        case 'a':
	  opts->awgn_flag=1;
	  opts->channel_model=AWGN;
	  sprintf(opts->parameters,"%s -a",opts->parameters);
	  break;
	  case 'i':
	  opts->awgn_flagi=1;
	  opts->channel_modeli=AWGN;
	  sprintf(opts->parameters,"%s -i",opts->parameters);
	  break;
	  case 'B':
		opts->N_RB_DL=atoi(optarg);
		break;
	case 'f':
	  opts->fixed_channel_flag=1;               
	  sprintf(opts->parameters,"%s -f",opts->parameters);
	  break;
	case 'D':
	  if(opts->n_adj_cells==0 )
            {
	      msg("First  specify the number of  adjuncts cells to estimate channel using -A #!\n");                
	      exit(-1);
	    }
	  opts->dual_stream_UE=1;             
	  sprintf(opts->parameters,"%s -D",opts->parameters);
	  break;
	  case 'e':
	  opts->dci_flag=1;               
	  sprintf(opts->parameters,"%s -d",opts->parameters);
	  break; 
	  case 'r':  
	  opts->DLSCH_RB_ALLOC = atoi(optarg);
	  opts->rballocset = 1;
	  break;
      case 's':
	  opts->snr_init=atof(optarg);
	  //opts->snr_max= opts->snr_init+5;
	  sprintf(opts->parameters,"%s  -s%f",opts->parameters,opts->snr_init);
	  break;
        case 'S':
	  opts->snr_max=atof(optarg);
	  sprintf(opts->parameters,"%s  -S%f",opts->parameters,opts->snr_max);
	  break;
	   case 'Q':
	  opts->ratio=atoi(optarg);
	  break;
	   case 'O':	  
	   opts->nprb1=atoi(optarg);
	   opts->search_prb2=1;
	   break;
        case 'T':
	  opts->snr_step=atof(optarg);
	  sprintf(opts->parameters,"%s  -T%f",opts->parameters,opts->snr_step);
	  break;
        case 'n':
	  opts->nframes=atoi(optarg);
	  sprintf(opts->parameters,"%s  -n%d",opts->parameters,opts->nframes);
	  break;
        case 'x':
	  opts->extended_prefix_flag=1;
	  sprintf(opts->parameters,"%s  -x",opts->parameters);
	  if (opts->extended_prefix_flag == 0)
            {
	      opts->nsymb = 14 ;
	      opts->pilot1 = 4;
	      opts->pilot2 = 7;
	      opts->pilot3 = 11;
            } else
            {
	      opts->nsymb = 12;
	      opts->pilot1 = 3;
	      opts->pilot2 = 6;
	      opts->pilot3 = 9;
            }
	  break;
        case 'd':
	  opts->frame_type= 1;
	  sprintf(opts->parameters,"%s  -d",opts->parameters);
	  break;
        case 't':
	  opts->transmission_mode=atoi(optarg);
	  sprintf(opts->parameters,"%s  -t%d",opts->parameters,opts->transmission_mode);
	  if ((opts->transmission_mode!=1) &&  (opts->transmission_mode!=2) && (opts->transmission_mode!=6))
            {
	      printf("Unsupported transmission mode %d\n",opts->transmission_mode);
	      exit(-1);
            }
	  break;
        case 'y':
	  opts->n_tx=atoi(optarg);
	  sprintf(opts->parameters,"%s  -y%d",opts->parameters,opts->n_tx);
	  break;
        case 'z':
	  opts->n_rx=atoi(optarg);
	  sprintf(opts->parameters,"%s  -z%d",opts->parameters, opts->n_rx);
	  break;
        case 'I':
	  opts->nInterf=atoi(optarg);
	  sprintf(opts->parameters,"%s  -I%d",opts->parameters, opts->nInterf);
	  if(opts->nInterf>5 )
            {
	      msg("Max num interferer = 5 \n");                
	      exit(-1);
	    }
	  break;
        case 'w':			
	  if(opts->nInterf==0 )
            {
	      msg("First  specify the number of interferer with -I#  \n");                
	      exit(-1);
	    }
	  sprintf(aux,"%s",optarg);
	  strcpy(opts->interfLevels,aux);
	  sprintf(opts->parameters,"%s  -w%s", opts->parameters,opts->interfLevels);
           
	  break;
        case 'k':			
	  if(opts->nInterf==0 )
            {
	      msg("First  specify the number of interferer with -I#  \n");                
	      exit(-1);
	    }
	  sprintf(aux,"%s",optarg);
	  strcpy(opts->interfProbability,aux);
	  sprintf(opts->parameters,"%s  -k%s", opts->parameters,opts->interfProbability);
	  opts->prob_flag=1;
	  break;
        case 'N':
	  opts->Nid_cell = atoi(optarg);
	  sprintf(opts->parameters,"%s  -N%d",opts->parameters, opts->Nid_cell);
	  break;
	case 'c':
	  opts->interCellId = atoi(optarg);
	  sprintf(opts->parameters,"%s  -c%d",opts->parameters, opts->interCellId);
	  break;
        case 'o':
	  opts->oversampling=atoi(optarg);
	  sprintf(opts->parameters,"%s  -o%d",opts->parameters, opts->oversampling);
	  break;
        case 'b':
	  opts->testNumber=atoi(optarg);
	  sprintf(opts->parameters,"%s  -b%d",opts->parameters, opts->testNumber);
	  break;
        case 'm':
	  opts->mcs=atoi(optarg);
	  sprintf(opts->parameters,"%s  -b%d",opts->parameters, opts->mcs);
	  break;
        case 'g':
	  sprintf(opts->parameters,"%s  -g%s",opts->parameters, optarg);
	  switch ((char)*optarg) {
	  case 'A':
	    opts->channel_model=SCM_A;
	    break;
	  case 'B':
	    opts->channel_model=SCM_B;
	    break;
	  case 'C':
	    opts->channel_model=SCM_C;
	    break;
	  case 'D':
	    opts->channel_model=SCM_D;
	    break;
	  case 'E':
	    opts->channel_model=EPA;
	    break;
	  case 'F':
	    opts->channel_model=EVA;
	    break;
	  case 'G':
	    opts->channel_model=ETU;
	    break;                
	  case 'H':
	    opts->channel_model=Rayleigh8;
	    break;
	  case 'I':
	    opts->channel_model=Rayleigh1;
	    break;
	  case 'J':
	    opts->channel_model=Rayleigh1_corr;
	    break;
	  case 'K':
	    opts->channel_model=Rayleigh1_anticorr;
	    break;
	  case 'L':
	    opts->channel_model=Rice8;
	    break;
	  case 'M':
	    opts->channel_model=Rice1;
	    break;
	  default:
	    msg("Unsupported channel model! [A,B,C,D,E,F,G,H,I,J,K,L,M]\n");                
	    exit(-1);
	  }
	  break;
	case 'A':
	  opts->n_adj_cells=atoi(optarg);
	  sprintf(opts->parameters,"%s  -b%d",opts->parameters, opts->n_adj_cells);
	  break;
	case 'R':
	  opts->num_rounds=atoi(optarg);
	  sprintf(opts->parameters,"%s  -R%d",opts->parameters, opts->num_rounds);
	  if(opts->num_rounds>4 )
            {
	      msg("Max num round = 4 \n");                
	      exit(-1);
	    }
	   opts->fix_rounds=1;
	  break;
	case 'p':		 
	  sprintf(aux,"%s",optarg);
	  strcpy(opts->power,aux);
	  sprintf(opts->parameters,"%s  -w%s", opts->parameters,opts->power);
	  _parsePower(opts);
	  break;
        default:
        case 'h':
	  printf("-h    This message\n");
	  printf("-s    Starting SNR default value is %f\n",opts->snr_init);
	  printf("-S    Ending SNR default value is %f\n",opts->snr_max);
	  printf("-T    Step size of SNR, default value is %f\n",opts->snr_step);
	  printf("-n    Number of frames, default value is %d\n",opts->nframes);
	  printf("-x    Use extended prefix mode  flag, default value is Normal\n");
	  printf("-d    Use TDD flag\n");
	  printf("-t    Transmission mode (1,2,6 for the moment),default value is %d\n",opts->transmission_mode);
	  printf("-y    Number of TX antennas used in eNB, default value is %d\n",opts->n_tx);
	  printf("-z    Number of RX antennas used in UE, default value is %d\n",opts->n_rx);
	  printf("-I    Number of interference to apply, default value is %d \n",opts->nInterf);
	  printf("-w    Relative strength of  inteference list (in dB)  separeted by ',' \n");
	  printf("-N    Nid_cell, default value is %d \n",opts->Nid_cell);
	  printf("-o    Oversampling factor (1,2,4,8,16), default value is %d \n",opts->oversampling);
	  printf("-g    [A,B,C,D,E,F,G] Use 3GPP SCM (A,B,C,D) or 36-101 (E-EPA,F-EVA,G-ETU) models (ignores delay spread and Ricean factor), default value is AWGN\n");
	  //    printf("-f    Output filename (.txt format) for Pe/SNR results\n");
	  printf("-a    Use AWGN channel and not multipath\n");
	  printf("-i    Use AWGN channel for the interference\n");
	  printf("-b    Test Number\n");
	  printf("-c    CellId Number for interferer\n");
	  printf("-r    ressource block allocation (see  section 7.1.6.3 in 36.213\n");
	  printf("-m    MCS\n");            
	  printf("-D    Enable interference cancellation\n"); 
	  printf("-e    Enable verification of DCI\n"); 
	  printf("-A    Indicates  number of interfering  to estimate, by default does not estimate the channel from the interfering\n"); 
	  printf("-R    Number of rounds\n"); 
	  printf("-k    Probability of each interferer list (0-1)  separeted by ',' \n");
	  printf("-f    Use fixed data and channel\n");
	  printf("-B    Number of PRBs depending on the bandwidth\n");
	  exit (-1);
	  break;


        }
    }

  sprintf(opts->folderName,"%d_resp",opts->testNumber);
  if (opts->nInterf>0)
    {
      _parseInterferenceLevels(opts,opts->interfLevels,opts->nInterf);        
      _parseInterferenceProbability(opts,opts->interfProbability,opts->nInterf);
    }

}

void _printOptions(options_t *opts)
{
  int i;
  printf("\n----------Options----------");
  printf("\nsnr_init:\t\t%f",opts->snr_init);
  printf("\nsnr_max:\t\t%f",opts->snr_max);
  printf("\nsnr_step:\t\t%f",opts->snr_step);
  printf("\nnframes:\t\t%d",opts->nframes);
  printf("\nextended_prefix_flag:\t%d",opts->extended_prefix_flag);
  printf("\nframe_type:\t\t%d",opts->frame_type);
  printf("\ntransmission_mode:\t%d",opts->transmission_mode);
  printf("\nn_tx:\t\t\t%d",opts->n_tx);
  printf("\nn_rx:\t\t\t%d",opts->n_rx);
  printf("\nNid_cell:\t\t%d",opts->Nid_cell);
  printf("\noversampling:\t\t%d",opts->oversampling);
  printf("\nchannel_model:\t\t%d",opts->channel_model);
  printf("\nchannel_modeli:\t\t%d",opts->channel_modeli);
  printf("\nawgn_flag:\t\t%d",opts->awgn_flag);
  printf("\nawgn_flagi:\t\t%d",opts->awgn_flagi);
  printf("\nnInterf:\t\t%d",opts->nInterf);
  printf("\nxx:%p",(void *)opts->outputFile);

  for (i=0; i<opts->nInterf; i++)
    {
      printf("\n\tInterference n%d:%f (%f)",i+1,opts->dbInterf[i],opts->probabilityInterf[i]);
    }
 


  printf("\n");


}



void _parseInterferenceProbability(options_t *opts, char *interfProbability,int nInterf)
{
  int i;
  char * pch;

  opts->probabilityInterf=(double*)malloc(sizeof(double)*nInterf);
  for (i=0; i<nInterf; i++)
    {        
      opts->probabilityInterf[i]=1.0;
    }
  if(opts->prob_flag)
    {
      pch = strtok (interfProbability,",");
      i=0;
      while (pch != NULL)
	{
	  opts->probabilityInterf[i]=atof(pch);
	  i++;
	  pch = strtok (NULL,",");
	}
		
    }

}


void _parseInterferenceLevels(options_t *opts, char *interfLevels,int nInterf)
{
  int i;
  char * pch;
  opts->dbInterf=(double*)malloc(sizeof(double)*nInterf);
  for (i=0; i<nInterf; i++)
    {        
      opts->dbInterf[i]=0.0;
    }

  pch = strtok (interfLevels,",");
  i=0;
  while (pch != NULL)
    {
      opts->dbInterf[i]=atof(pch);
      i++;
      pch = strtok (NULL,",");
    }

}


void _allocData(options_t opts, data_t *data ,uint8_t n_tx,uint8_t n_rx, int Frame_length_complex_samples)
{
  int i,j;
  data->s_re = (double**)malloc(n_tx*sizeof(double*));
  data->s_im = (double**)malloc(n_tx*sizeof(double*));
  data->r_re = (double**)malloc(n_rx*sizeof(double*));
  data->r_im = (double**)malloc(n_rx*sizeof(double*));
    
  if(opts.nInterf>0)
    {
      data->is_re=(double***)malloc(opts.nInterf*sizeof(double**));
      data->is_im=(double***)malloc(opts.nInterf*sizeof(double**));
      data->ir_re=(double***)malloc(opts.nInterf*sizeof(double**));
      data->ir_im=(double***)malloc(opts.nInterf*sizeof(double**));
      for(i=0;i<opts.nInterf;i++)
	{
	  data->is_re[i]=(double**)malloc(n_tx*sizeof(double*));
	  data->is_im[i]=(double**)malloc(n_tx*sizeof(double*));
	  data->ir_re[i]=(double**)malloc(n_rx*sizeof(double*));
	  data->ir_im[i]=(double**)malloc(n_rx*sizeof(double*));
	}
    }

  for (i=0; i<n_tx; i++)
    {

      data->s_re[i] =(double*)malloc(Frame_length_complex_samples*sizeof(double));
      data->s_im[i] = (double*)malloc(Frame_length_complex_samples*sizeof(double));        
        
      bzero(data->s_re[i],Frame_length_complex_samples*sizeof(double));        
      bzero(data->s_im[i],Frame_length_complex_samples*sizeof(double));        
        
      for(j=0;j<opts.nInterf;j++)
        {
	  data->is_re[j][i] =(double*)malloc(Frame_length_complex_samples*sizeof(double));
	  data->is_im[j][i] = (double*)malloc(Frame_length_complex_samples*sizeof(double));			
			
	  bzero(data->is_re[j][i],Frame_length_complex_samples*sizeof(double));        
	  bzero(data->is_im[j][i],Frame_length_complex_samples*sizeof(double));			
	}
        
    }

  for (i=0; i<n_rx; i++)
    {
        
      data->r_re[i] =(double*)malloc(Frame_length_complex_samples*sizeof(double));
      data->r_im[i] = (double*)malloc(Frame_length_complex_samples*sizeof(double));
        
      bzero(data->r_re[i],Frame_length_complex_samples*sizeof(double));       
      bzero(data->r_im[i],Frame_length_complex_samples*sizeof(double));
        
      for(j=0;j<opts.nInterf;j++)
        {		
	  data->ir_re[j][i] =(double*)malloc(Frame_length_complex_samples*sizeof(double));
	  data->ir_im[j][i] = (double*)malloc(Frame_length_complex_samples*sizeof(double));
					
	  bzero(data->ir_re[j][i],Frame_length_complex_samples*sizeof(double));       
	  bzero(data->ir_im[j][i],Frame_length_complex_samples*sizeof(double));
	}
        
    }

}

void copyDataFixed(data_t * origin,data_t * destination,options_t  *opts, int Frame_length_complex_samples)
{
	
  int i,j;
                                                    
  int sizeData=4*opts->nInterf*sizeof(double**)+    
    (opts->nInterf+1)*2*(opts->n_tx+opts->n_rx)*(sizeof(double*)+Frame_length_complex_samples*sizeof(double));
    
  printf("Copying sizeData: %d\n",sizeData);
    
  opts->fixed_data_set=1;

}


void _makeOutputDir(options_t *opts)
{
  int status;
  char auxDir[100]; 
  char auxFile[100];   
  FILE *controlFile;

  status=mkdir ("testResults",S_IRWXU | S_IRWXG | S_IRWXO);
  // status=chdir("testResults");
  sprintf(auxDir,"%s",opts->folderName);
  //status=mkdir(auxDir,S_IRWXU | S_IRWXG | S_IRWXO);	
  //status=chdir(auxDir);
    
  sprintf(auxFile,"OutpuSimulation_%df_%dI_%sdB_%dch_%d.m",opts->nframes,opts->nInterf,opts->interfLevels,opts->channel_model,opts->testNumber);
  sprintf(auxFile,"Bler_%d.m",opts->testNumber);

  opts->outputFile =fopen(auxFile,"w");
    
  sprintf(auxFile,"OutputBlerRound_%d.m",opts->testNumber);
    
  opts->outputBler =fopen(auxFile,"w");
  fprintf( opts->outputBler,"SNR; rate1;rate2; MCS; TBS; rate; err0; trials0; err1; trials1; err2; trials2; err3; trials3; dci_err; nprb1; nprb2\n");
    
  sprintf(auxFile,"OutputBER_%d.m",opts->testNumber);
  opts->outputBer =fopen(auxFile,"w");
    
  sprintf(auxFile,"Throughput_%d.m",opts->testNumber);
  opts->outputTrougput =fopen(auxFile,"w");

  controlFile=fopen("ControlTest.txt","w");

  fprintf(controlFile,"Parameters\n");

  fprintf(controlFile,"./femtosim %s\n\n",opts->parameters);


  fprintf(controlFile,"testNumber:\t\t\n",opts->testNumber);

  fprintf(controlFile,"awgn_flag:\t\t%d\n",opts->awgn_flag);
  fprintf(controlFile,"snr_init:\t\t%f\n",opts->snr_init);
  fprintf(controlFile,"snr_max;\t\t%f\n",opts->snr_max);
  fprintf(controlFile,"snr_step:\t\t%f\n",opts->snr_step);
  fprintf(controlFile,"nframes:\t\t%d\n",opts->nframes);
  fprintf(controlFile,"extended_prefix_flag:\t\t%d\n",opts->extended_prefix_flag);
  fprintf(controlFile,"frame_type:\t\t%d\n",opts->frame_type);
  fprintf(controlFile,"transmission_mode:\t\t%d\n",opts->transmission_mode);
  fprintf(controlFile,"n_tx:\t\t%d\n",opts->n_tx);
  fprintf(controlFile,"n_rx:\t\t%d\n",opts->n_rx);
  fprintf(controlFile,"nInterf:\t\t%d\n",opts->nInterf);
  fprintf(controlFile,"interfLevels:\t\t%s\n",opts->interfLevels);
  fprintf(controlFile,"Nid_cell:\t\t%d\n",opts->Nid_cell);
  fprintf(controlFile,"oversampling:\t\t%d\n",opts->oversampling);

  fclose(controlFile);

}


void _parsePower(options_t *opts)
{
  //printf("opts->power:%s\n",opts->power);
  int i;
  char * pch;    
  pch=strtok (opts->power,",");
  if (pch != NULL)
    {
      opts->p_a=atoi(pch);
      pch=strtok (NULL,",");
      opts->p_b=atoi(pch);
      opts->d_offset=0;
      /*   pch=strtok (NULL,",");
	   opts->d_offset=atoi(pch);     */
    }
    
  if(opts->p_a< 0 || opts->p_a>7) 
    {
      msg("Error -> PA  (0...7) (dBm6, 	dBm477 	,dBm3 	,dBm177 	,dB0 	,dB1 	,dB2 	,dB3 )\n");
      exit(1);
    }
    
  if(opts->p_b< 0 || opts->p_b>3) 
    {
      msg("Error -> PB  (0...3)\n");
      exit(1);
    }
    
  /*  if(opts->d_offset< -6 || opts->d_offset>12) 
      {
      msg("Error -> Offset  (-6...12)\n");
      exit(1);
      }*/
    

}
