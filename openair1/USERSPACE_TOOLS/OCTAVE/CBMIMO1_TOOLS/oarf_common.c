// Maxime Guillaud - Created Fri May 12 16:06:53 CEST 2006
// this files gathers the various functions used to control the openair RF interface
// it is equivalent to the openair_readconfigfile.c and openair_configure.c of the config/ directory




// Structures shared by various functions in this file
PHY_CONFIG PHY_config_mem;
PHY_CONFIG *PHY_config;
FILE *config, *scenario;

PHY_FRAMING phyFraming[MAX_CFG_SECTIONS];
PHY_CHBCH phyCHBCH[MAX_CFG_SECTIONS];
PHY_CHSCH phyCHSCH[MAX_CFG_SECTIONS];
PHY_SCH phySCH[MAX_CFG_SECTIONS];
PHY_SACH phySACH[MAX_CFG_SECTIONS];

const cfg_Action Action[] =
  {
    {"PHY_FRAMING", phyFraming_ProcessInitReq}, // to do some manipulation of the raw data
    {"PHY_CHSCH",   phyCHSCH_ProcessInitReq},
    {"PHY_SCH",   phySCH_ProcessInitReq},
    {"PHY_CHBCH", phyCHBCH_ProcessInitReq},
    {"PHY_SACH", phySACH_ProcessInitReq}
  };

const cfg_Section Section[]=
  {
    {"PHY_FRAMING", cfg_readPhyFraming},
    {"PHY_CHBCH", cfg_readPhyCHBCH},
    {"PHY_CHSCH", cfg_readPhyCHSCH},
    {"PHY_SCH", cfg_readPhySCH},
    {"PHY_SACH", cfg_readPhySACH}
  };



void 
dump_config() {

  int i;
  FILE *fid;

  msg("[openair][CONFIG] Dumping Config\n\n");
  msg("[openair][CONFIG] PHY_FRAMING\n");
  msg("[openair][CONFIG] -----------\n");
  msg("[openair][CONFIG] fc = %d kHz\n",PHY_config->PHY_framing.fc_khz);
  msg("[openair][CONFIG] fs = %d kHz\n",PHY_config->PHY_framing.fs_khz);
  msg("[openair][CONFIG] Nsymb= %d\n",PHY_config->PHY_framing.Nsymb);
  msg("[openair][CONFIG] Nd= %d\n",PHY_config->PHY_framing.Nd);
  msg("[openair][CONFIG] log2Nd= %d\n",PHY_config->PHY_framing.log2Nd);
  msg("[openair][CONFIG] Nc= %d\n",PHY_config->PHY_framing.Nc);
  msg("[openair][CONFIG] Nz= %d\n",PHY_config->PHY_framing.Nz);
  msg("[openair][CONFIG] Nf= %d\n",PHY_config->PHY_framing.Nf);
  switch (PHY_config->PHY_framing.Extension_type) {
    
  case CYCLIC_SUFFIX:
    msg("[openair][CONFIG] Extension= CYCLIC_SUFFIX \n");
    break;
  case CYCLIC_PREFIX:
    msg("[op`enair][CONFIG] Extension= CYCLIC_PREFIX \n");
    break;
  case ZEROS:
    msg("[openair][CONFIG] Extension= ZEROS\n");
    break;
  default:
    msg("[openair][CONFIG] Extension= CYCLIC_SUFFIX\n");
    break;
  }	

  msg("[openair][CONFIG]\n");
  msg("[openair][CONFIG] PHY_CHSCH\n");
  msg("[openair][CONFIG] -----------\n");
  msg("[openair][CONFIG] symbol = %d\n",PHY_config->PHY_chsch[0].symbol);
  msg("[openair][CONFIG] Nsymb= %d\n",PHY_config->PHY_chsch[0].Nsymb);
  msg("[openair][CONFIG] dd_offset= %d\n",PHY_config->PHY_chsch[0].dd_offset);
  for (i=0;i<32;i++) {
    msg("[openair][CONFIG] chsch_seq_re[%d]= %x\n",i,PHY_config->PHY_chsch[0].chsch_seq_re[i]);
    msg("[openair][CONFIG] chsch_seq_im[%d]= %x\n",i,PHY_config->PHY_chsch[0].chsch_seq_im[i]);
  }
  msg("[openair][CONFIG] CHSCH_POWER= %d dBm\n",PHY_config->PHY_chsch[0].CHSCH_POWER_dBm);

  msg("[openair][CONFIG]\n");
  msg("[openair][CONFIG] PHY_CHBCH\n");
  msg("[openair][CONFIG] -----------\n");
  msg("[openair][CONFIG] symbol = %d\n",PHY_config->PHY_chbch[0].symbol);
  msg("[openair][CONFIG] Nsymb= %d\n",PHY_config->PHY_chbch[0].Nsymb);
  msg("[openair][CONFIG] IntDepth= %d\n",PHY_config->PHY_chbch[0].IntDepth);
  msg("[openair][CONFIG] dd_offset= %d\n",PHY_config->PHY_chbch[0].dd_offset);
  msg("[openair][CONFIG] Npilot= %d\n",PHY_config->PHY_chbch[0].Npilot);
  for (i=0;i<8;i++) {
    msg("[openair][CONFIG] pilot_re[%d]= %x\n",i,PHY_config->PHY_chbch[0].pilot_re[i]);
    msg("[openair][CONFIG] pilot_im[%d]= %x\n",i,PHY_config->PHY_chbch[0].pilot_im[i]);
  }
  msg("[openair][CONFIG] FreqReuse= %d\n",PHY_config->PHY_chbch[0].FreqReuse);
  msg("[openair][CONFIG] FreqReuse_ind= %d\n",PHY_config->PHY_chbch[0].FreqReuse_ind);
  msg("[openair][CONFIG] CHBCH_POWER= %d dBm\n",PHY_config->PHY_chbch[0].CHBCH_POWER_dBm);



}


int
reconfigure_MACPHY(FILE* scenario)
{
 
  FILE *inscenario=scenario;
  int cfgNumber, ActionIndex;
  char LineBuffer[MAX_LINE_SIZE];
  char ActionName[MAX_ACTION_NAME_SIZE];
 
  /*!< \brief Reading or opening the scenario*/
 

  loadConfig();
 
  while ( fgets(LineBuffer, MAX_LINE_SIZE, inscenario) != NULL )
    {
      printf("[CONFIG] : %s\n",LineBuffer);
      if ( sscanf(LineBuffer, "%s %d", ActionName, &cfgNumber) != 2)
	{
	  msg("[CONFIG] Parse Error: \"%s\" \n", LineBuffer);	
	  continue;
	}
      for (ActionIndex=0; ActionIndex <6;ActionIndex++) //sizeof(Action)/sizeof(cfg_Action); ActionIndex++)
	{

	  if (strcmp(ActionName, Action[ActionIndex].ActionName) == 0)
	    {
	      Action[ActionIndex].Func(cfgNumber);
	      break;
	    }
	}
      if (ActionIndex == 6)//(sizeof(Action)/sizeof(cfg_Action) == ActionIndex)
	msg("[CONFIG] Unknown <%s> Action! \n", ActionName);
    }
 
  fclose(inscenario);
 
  return 1;
}


int 
phyFraming_ProcessInitReq (int cfgNumber)
{
  PHY_FRAMING *phyframing_processinitreq;
  phyframing_processinitreq = cfg_getPhyFraming(cfgNumber);

  if(phyframing_processinitreq != NULL)
    {
      memcpy((void *)&PHY_config->PHY_framing,phyframing_processinitreq,sizeof(PHY_FRAMING));
#ifdef TRACE
      msg("[CONFIG] PHY_FRAMING configuration completed!\n");
#endif
    }
  else
    msg("[CONFIG FRAMING] The config number <%d> is not loaded.\n", cfgNumber);
  return cfgNumber;
}
 
 




void 
loadConfig (void)
{
  int SectionIndex;
  int cfgNumber;
  char SectionName[MAX_SECTION_NAME_SIZE];
  char LineBuffer[MAX_LINE_SIZE];
	
	
  while ( (fgets (LineBuffer, MAX_LINE_SIZE, config)) != NULL)
    {
      if (LineBuffer[0] != '[') 
	continue;
		
      if (sscanf(LineBuffer, "[%s %d]", SectionName, &cfgNumber) != 2)	
	{
	  msg("[CONFIG] Parse Error: \"%s\"\n", LineBuffer);
	  continue;
	}
      for (SectionIndex=0; SectionIndex < 5; SectionIndex++) //sizeof(Section)/sizeof(cfg_Section) ; SectionIndex++)
	{
	  if ( strcmp(SectionName,Section[SectionIndex].SectionName) == 0)
	    {
	      Section[SectionIndex].Func(config, cfgNumber);
	      break;
	    }
	}
      if (SectionIndex == 5)//(sizeof(Section)/sizeof(cfg_Section) == SectionIndex)
	msg("[CONFIG] Unknown <%s> section!\n", SectionName);
    }
  msg("[CONFIG] loadConfig done.\n");
}


void 
cfg_readPhyFraming (FILE* config, int cfgNumber)
{
  char LineBuffer[MAX_LINE_SIZE];
	
  if( cfgNumber >= MAX_CFG_SECTIONS )
    {
      msg("[CONFIG] %d outrang number of sections\n", cfgNumber);	
    }
	
  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf(LineBuffer, "fc_khz: %lu", &phyFraming[cfgNumber].fc_khz ) != 1)
    {
      msg("[CONFIG] Parse Error: \"%s\"\n", LineBuffer);
    }
  //msg("[CONFIG] %s (%d)\n",LineBuffer,phyFraming[cfgNumber].fc_khz);

  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf( LineBuffer, "fs_khz: %lu", &phyFraming[cfgNumber].fs_khz ) != 1)
    {
      msg("[CONFIG] Parse Error: \"%s\"\n", LineBuffer);
    }
  //msg("[CONFIG] %s (%d)\n",LineBuffer,phyFraming[cfgNumber].fs_khz);

  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf( LineBuffer, "Nsymb: %hu", &phyFraming[cfgNumber].Nsymb ) != 1)
    {
      msg("[CONFIG] Parse Error: \"%s\"\n", LineBuffer);
    }
  //msg("[CONFIG] %s (%d)\n",LineBuffer,phyFraming[cfgNumber].Nsymb);
		
  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf( LineBuffer, "Nd: %hu", &phyFraming[cfgNumber].Nd ) != 1)
    {
      msg("[CONFIG] Parse Error: \"%s\"\n", LineBuffer);
    }
  //msg("[CONFIG] %s (%d)\n",LineBuffer,phyFraming[cfgNumber].Nd);

  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf( LineBuffer, "log2Nd: %hu", &phyFraming[cfgNumber].log2Nd ) != 1)
    {
      msg("[CONFIG] Parse Error: \"%s\"\n", LineBuffer);
    } 
  //msg("[CONFIG] %s (%d)\n",LineBuffer,phyFraming[cfgNumber].log2Nd);

  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf( LineBuffer, "Nc: %hu", &phyFraming[cfgNumber].Nc ) != 1)
    {
      msg("[CONFIG] Parse Error: \"%s\"\n", LineBuffer);
    }
  //msg("[CONFIG] %s (%d)\n",LineBuffer,phyFraming[cfgNumber].Nc);

  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf( LineBuffer, "Nz: %hu", &phyFraming[cfgNumber].Nz ) != 1)
    {
      msg("[CONFIG] Parse Error: \"%s\"\n", LineBuffer);
    }
  //msg("[CONFIG] %s (%d)\n",LineBuffer,phyFraming[cfgNumber].Nz);

  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf( LineBuffer, "Nf: %hu", &phyFraming[cfgNumber].Nf ) !=1)
    {
      msg("[CONFIG] Parse Error: \"%s\"\n", LineBuffer);
    }

  //msg("[CONFIG] %s (%d)\n",LineBuffer,phyFraming[cfgNumber].Nf);

  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf( LineBuffer, "Extension_type: %hu", &phyFraming[cfgNumber].Extension_type ) !=1)
    {
      msg("[CONFIG] Parse Error: \"%s\"\n", LineBuffer);
    }

  msg("[CONFIG FRAMING] : Done\n");
}


PHY_FRAMING *cfg_getPhyFraming(int cfgNumber)
{
  if (cfgNumber < MAX_CFG_SECTIONS)
    return &phyFraming[cfgNumber];
  else
    {
      msg("[CONFIG] Outrange config number <%d>.\n", cfgNumber);
      return NULL; //exit(1)??
    }
}


void 
cfg_readPhyCHBCH(FILE* config, int cfgNumber)
{
  char LineBuffer[MAX_LINE_SIZE];
  char temp_string[32];
  char i;
	
  if( cfgNumber >= MAX_CFG_SECTIONS )
    {
      msg("[CONFIG CHBCH] %d outrang number of sections\n", cfgNumber);	
    }
	
  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf(LineBuffer, "symbol: %hu", &phyCHBCH[cfgNumber].symbol ) != 1)
    {
      msg("[CONFIG CHBCH] Parse Error: \"%s\"\n", LineBuffer);
    }
  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf( LineBuffer, "Nsymb: %hu", &phyCHBCH[cfgNumber].Nsymb ) != 1)
    {
      msg("[CONFIG CHBCH] Parse Error: \"%s\"\n", LineBuffer);
    }
  fgets( LineBuffer, MAX_LINE_SIZE, config );
  if( sscanf( LineBuffer, "IntDepth: %hu", &phyCHBCH[cfgNumber].IntDepth ) != 1)
    {
      msg("[CONFIG CHBCH] Parse Error: \"%s\"\n", LineBuffer);	  
    }
  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf( LineBuffer, "dd_offset: %hu", &phyCHBCH[cfgNumber].dd_offset ) != 1)
    {
      msg("[CONFIG CHBCH] Parse Error: \"%s\"\n", LineBuffer);
    }		
  fgets( LineBuffer, MAX_LINE_SIZE, config );
  if( sscanf( LineBuffer, "Npilot: %hu", &phyCHBCH[cfgNumber].Npilot ) != 1)
    {
      msg("[CONFIG CHBCH] Parse Error: \"%s\"\n", LineBuffer);	  
    }
  for (i=0;i<8;i++) {
    fgets(LineBuffer, MAX_LINE_SIZE, config);
    sprintf(temp_string,"pilot_re[%d]: %%x",i);

    if( sscanf( LineBuffer,temp_string, &phyCHBCH[cfgNumber].pilot_re[i] ) != 1)
      {
	msg("[CONFIG CHBCH] Parse Error: \"%s\"\n", LineBuffer);
      }		

    fgets(LineBuffer, MAX_LINE_SIZE, config);
    sprintf(temp_string,"pilot_im[%d]: %%x",i);

    if( sscanf( LineBuffer,temp_string, &phyCHBCH[cfgNumber].pilot_im[i] ) != 1)
      {
	msg("[CONFIG CHBCH] Parse Error: \"%s\"\n", LineBuffer);
      }		

  }

  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf( LineBuffer, "FreqReuse: %hu", &phyCHBCH[cfgNumber].FreqReuse ) !=1)
    {
      msg("[CONFIG CHBCH] Parse Error: \"%s\"\n", LineBuffer);
    }

  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf( LineBuffer, "FreqReuse_Ind: %hu", &phyCHBCH[cfgNumber].FreqReuse_ind ) !=1)
    {
      msg("[CONFIG CHBCH] Parse Error: \"%s\"\n", LineBuffer);
    }

  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf( LineBuffer, "CHBCH_POWER_dBm: %d", &phyCHBCH[cfgNumber].CHBCH_POWER_dBm ) != 1)
    {
      msg("[CONFIG CHBCH] Parse Error: \"%s\"\n", LineBuffer);
    }
  msg("[CONFIG CHBCH]: Done\n");
}

 
PHY_CHBCH *cfg_getPhyCHBCH(int cfgNumber)
{
  if ( cfgNumber < MAX_CFG_SECTIONS )
    return &phyCHBCH[cfgNumber];
  else
    {
      msg("[CONFIG CHBCH] Outrange config number <%d>.\n", cfgNumber);
      return NULL; //exit(1)??
    }
}

void 
cfg_readPhySCH(FILE* config, int cfgNumber)
{
  char LineBuffer[MAX_LINE_SIZE];
  char temp_string[32];
  int i;

  if( cfgNumber >= MAX_CFG_SECTIONS )
    {
      msg("[SCH CONFIG] %d outrang number of sections\n", cfgNumber);	
    }
	
  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf( LineBuffer, "Nsymb: %hu", &phySCH[cfgNumber].Nsymb ) != 1)
    {
      msg("[SCH CONFIG] %d: Parse Error: \"%s\"\n", cfgNumber,LineBuffer);
    }
  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf( LineBuffer, "dd_offset: %lu", &phySCH[cfgNumber].dd_offset ) != 1)
    {
      msg("[SCH CONFIG] %d: Parse Error: \"%s\"\n", cfgNumber,LineBuffer);
    }

  for (i=0;i<32;i++) {
    fgets(LineBuffer, MAX_LINE_SIZE, config);
    sprintf(temp_string,"sch_seq_re[%d]: %%x",i);

    if( sscanf( LineBuffer,temp_string, &phySCH[cfgNumber].sch_seq_re[i] ) != 1)
      {
	msg("[SCH CONFIG] %d: Parse Error: \"%s\"\n (%s)", cfgNumber,LineBuffer,temp_string);
      }		

    fgets(LineBuffer, MAX_LINE_SIZE, config);
    sprintf(temp_string,"sch_seq_im[%d]: %%x",i);

    if( sscanf( LineBuffer,temp_string, &phySCH[cfgNumber].sch_seq_im[i] ) != 1)
      {
	msg("[SCH CONFIG] %d: Parse Error: \"%s\"\n", cfgNumber,LineBuffer);
      }		

  }

  fgets( LineBuffer, MAX_LINE_SIZE, config );
  if( sscanf( LineBuffer, "SCH_POWER_dBm: %d", &phySCH[cfgNumber].SCH_POWER_dBm ) != 1)
    {
      msg("[SCH CONFIG] %d:Parse Error: \"%s\"\n", cfgNumber,LineBuffer);	  
    }

  msg("[CONFIG SCH]: Done\n");			
}

PHY_SCH* cfg_getPhySCH(int cfgNumber)
{
  if ( cfgNumber < MAX_CFG_SECTIONS )
    return &phySCH[cfgNumber];
  else
    {
      msg("[CONFIG] Outrange config number <%d>.\n", cfgNumber);
      return NULL; //exit(1)??
    }
}

void 
cfg_readPhyCHSCH(FILE* config, int cfgNumber)
{
  char LineBuffer[MAX_LINE_SIZE];
  char temp_string[32];
  int i;

  if( cfgNumber >= MAX_CFG_SECTIONS )
    {
      msg("[CHSCH CONFIG] %d outrang number of sections\n", cfgNumber);	
    }
	
  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf(LineBuffer, "symbol: %hu", &phyCHSCH[cfgNumber].symbol ) != 1)
    {
      msg("[CHSCH CONFIG] %d: Parse Error: \"%s\"\n", cfgNumber,LineBuffer);
    }
  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf( LineBuffer, "Nsymb: %hu", &phyCHSCH[cfgNumber].Nsymb ) != 1)
    {
      msg("[CHSCH CONFIG] %d: Parse Error: \"%s\"\n", cfgNumber,LineBuffer);
    }
  fgets(LineBuffer, MAX_LINE_SIZE, config);
  if( sscanf( LineBuffer, "dd_offset: %lu", &phyCHSCH[cfgNumber].dd_offset ) != 1)
    {
      msg("[CHSCH CONFIG] %d: Parse Error: \"%s\"\n", cfgNumber,LineBuffer);
    }

  for (i=0;i<32;i++) {
    fgets(LineBuffer, MAX_LINE_SIZE, config);
    sprintf(temp_string,"chsch_seq_re[%d]: %%x",i);

    if( sscanf( LineBuffer,temp_string, &phyCHSCH[cfgNumber].chsch_seq_re[i] ) != 1)
      {
	msg("[CHSCH CONFIG] %d: Parse Error: \"%s\"\n (%s)", cfgNumber,LineBuffer,temp_string);
      }		

    fgets(LineBuffer, MAX_LINE_SIZE, config);
    sprintf(temp_string,"chsch_seq_im[%d]: %%x",i);

    if( sscanf( LineBuffer,temp_string, &phyCHSCH[cfgNumber].chsch_seq_im[i] ) != 1)
      {
	msg("[CHSCH CONFIG] %d: Parse Error: \"%s\"\n", cfgNumber,LineBuffer);
      }		

  }

  fgets( LineBuffer, MAX_LINE_SIZE, config );
  if( sscanf( LineBuffer, "CHSCH_POWER_dBm: %d", &phyCHSCH[cfgNumber].CHSCH_POWER_dBm ) != 1)
    {
      msg("[CHSCH CONFIG] %d:Parse Error: \"%s\"\n", cfgNumber,LineBuffer);	  
    }

  msg("[CONFIG CHSCH]: Done\n");			
}

 
PHY_CHSCH* cfg_getPhyCHSCH(int cfgNumber)
{
  if ( cfgNumber < MAX_CFG_SECTIONS )
    return &phyCHSCH[cfgNumber];
  else
    {
      msg("[CONFIG] Outrange config number <%d>.\n", cfgNumber);
      return NULL; //exit(1)??
    }
}


void 
cfg_readPhySACH(FILE* config, int cfgNumber)
{
  char LineBuffer[MAX_LINE_SIZE];
  char temp_string[32];
  int i;

  if( cfgNumber >= MAX_CFG_SECTIONS )
    {
      msg("[CONFIG] %d outrang number of sections\n", cfgNumber);	
    }
	
  fgets(LineBuffer, MAX_LINE_SIZE, config);
  //msg("[CONFIG SACH] : %s\n",LineBuffer);
  if( sscanf(LineBuffer, "Signal_format: %hu", &phySACH[cfgNumber].Signal_format ) != 1)
    {
      msg("[CONFIG] Parse Error: \"%s\"\n", LineBuffer);
    }


  fgets( LineBuffer, MAX_LINE_SIZE, config );
  //msg("[CONFIG SACH] : %s\n",LineBuffer);
  if( sscanf( LineBuffer, "Npilot: %hu", &phySACH[cfgNumber].Npilot ) != 1)
    {
      msg("[CONFIG] Parse Error: \"%s\"\n", LineBuffer);	  
    }

  for (i=0;i<8;i++) {
    fgets(LineBuffer, MAX_LINE_SIZE, config);
    sprintf(temp_string,"pilot_re[%d]: %%x",i);

    if( sscanf( LineBuffer,temp_string, &phySACH[cfgNumber].pilot_re[i] ) != 1)
      {
	msg("[CONFIG] Parse Error: \"%s\"\n", LineBuffer);
      }		

    fgets(LineBuffer, MAX_LINE_SIZE, config);
    sprintf(temp_string,"pilot_im[%d]: %%x",i);

    if( sscanf( LineBuffer,temp_string, &phySACH[cfgNumber].pilot_im[i] ) != 1)
      {
	msg("[CONFIG] Parse Error: \"%s\"\n", LineBuffer);
      }		

  }

  fgets(LineBuffer, MAX_LINE_SIZE, config);
  //msg("[CONFIG SACH] : %s\n",LineBuffer);
  if( sscanf( LineBuffer, "SACH_POWER_dBm: %d", &phySACH[cfgNumber].SACH_POWER_dBm ) !=1)
    {
      msg("[CONFIG] Parse Error: \"%s\"\n", LineBuffer);
    }	

}

 
PHY_SACH *cfg_getPhySACH(int cfgNumber)
{
  if ( cfgNumber < MAX_CFG_SECTIONS ) 
    return &phySACH[cfgNumber];
  else
    {
      msg("[CONFIG ] Outrange config number <%d>.\n", cfgNumber);
      return NULL; //exit(1)??
    }
}


int 
phyCHBCH_ProcessInitReq (int cfgNumber) {

  PHY_CHBCH *phychbch_processinitreq;
  
  phychbch_processinitreq = cfg_getPhyCHBCH(cfgNumber);
  if (phychbch_processinitreq != NULL)
    { 
      memcpy((void *)&PHY_config->PHY_chbch,phychbch_processinitreq,sizeof(PHY_CHBCH));
#ifdef TRACE			 
      msg("[CONFIG] PHY_CHBCH configuration completed!\n");
#endif
    }
  else
    msg("[CONFIG CHBCH ] The config number <%d> is not loaded.\n", cfgNumber);
  return cfgNumber;
}

int 
phySCH_ProcessInitReq (int cfgNumber)
{
  PHY_SCH *physch_processinitreq;
  physch_processinitreq = cfg_getPhySCH(cfgNumber);
  if (physch_processinitreq != NULL)
    { 
      memcpy(&PHY_config->PHY_sch[0],physch_processinitreq,sizeof(PHY_SCH));

#ifdef TRACE			 
      printf("[CONFIG] PHY_SCH configuration completed!\n");
#endif TRACE   
    }
  else
    msg("[CONFIG SCH] The config number <%d> is not loaded.\n", cfgNumber);
  return cfgNumber;
}

int 
phySACH_ProcessInitReq (int cfgNumber)
{
  PHY_SACH *physach_processinitreq;
  physach_processinitreq = cfg_getPhySACH(cfgNumber);
  if (physach_processinitreq != NULL)
    { 
      memcpy((void *)&PHY_config->PHY_sach,physach_processinitreq,sizeof(PHY_SACH));

#ifdef TRACE			 
      msg("[CONFIG] PHY_SACH configuration completed!\n");
#endif TRACE   
    }
  else
    msg("[CONFIG SACH] The config number <%d> is not loaded.\n", cfgNumber);
  return cfgNumber;
} 
 
int 
phyCHSCH_ProcessInitReq (int cfgNumber)
{
  PHY_CHSCH *phychsch_processinitreq;
  phychsch_processinitreq = cfg_getPhyCHSCH(cfgNumber);
  if (phychsch_processinitreq != NULL)
    { 
      memcpy(&PHY_config->PHY_chsch[0],phychsch_processinitreq,sizeof(PHY_CHSCH));

#ifdef TRACE			 
      printf("[CONFIG] PHY_CHSCH configuration completed!\n");
#endif
    }
  else
    msg("[CONFIG CHSCH] The config number <%d> is not loaded.\n", cfgNumber);
  return cfgNumber;
}



