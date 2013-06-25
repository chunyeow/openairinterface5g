/***************************************************************************
                          rrc_mbms_pdus.c - description
                          -------------------
    begin               : May 9, 2005
    copyright           : (C) 2005, 2010 by Eurecom
    created by          : Michelle.Wetterwald@eurecom.fr, Huu-Nghia.Nguyen@eurecom.fr	
    modified by         :
 **************************************************************************/
/********************
//OpenAir definitions
 ********************/
#include "LAYER2/MAC/extern.h"
#include "UTIL/MEM/mem_block.h"

/********************
// RRC definitions
 ********************/
#ifdef NODE_MT
#include "rrc_ue_vars.h"
#endif
#ifdef NODE_RG
#include "rrc_rg_vars.h"
#endif
#include "rrc_mbms_pdus.h"
// #include "rrc_msg_class.h"
// #include "rrc_rg_mbms_variables.h"
// #include "rrc_ue_mbms_variables.h"
//-----------------------------------------------------------------------------
//#include "rrc_proto_mbms.h"


/**
 * @brief Test if x is in range of (low .. high)
 * @param x: value to be tested
 * @param low: the lower bound
 * @param high: the upper bound
 * @return P_SUCCESS if ok, else P_INVVALUE if not ok.
 */
int checkRange(int x, int low, int high){
	return (x < low || x > high) ? P_INVVALUE: P_SUCCESS;
}

//////////////////////////////////////////////////////////////////////////////////////////////////
//MBMSAccessInformation
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_MBMSAccessInformation (MBMSAccessInformation * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int i;

	status =  checkRange(protocol_bs->rrc.mbms.acc_numAccessInfo, 0, maxMBMSservCount);
	if (status == P_SUCCESS){
		pvalue->mbms_ServiceAccessInfoList.numAccessInfo = protocol_bs->rrc.mbms.acc_numAccessInfo;
		for (i = 0; i<protocol_bs->rrc.mbms.acc_numAccessInfo; i++)
		{
			pvalue->mbms_ServiceAccessInfoList.accessInfo[i].shortTransmissionID = protocol_bs->rrc.mbms.acc_shortTransmissionID[i];
			pvalue->mbms_ServiceAccessInfoList.accessInfo[i].accessprobabilityFactor_Idle = protocol_bs->rrc.mbms.acc_accessprobabilityFactor_Idle[i];
		}
	}
	
	#ifdef DEBUG_RRC_MBMS
		msg("[RRC-RG][MBMS] Encode MBMSAccessInformation, status %d\n", status);
	#endif
	return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_MBMSAccessInformation (MBMSAccessInformation * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int i;

	status =  checkRange(pvalue->mbms_ServiceAccessInfoList.numAccessInfo, 0, maxMBMSservCount);
	if (status == P_SUCCESS){
		protocol_ms->rrc.mbms.acc_numAccessInfo = pvalue->mbms_ServiceAccessInfoList.numAccessInfo;
		for (i = 0; i<protocol_ms->rrc.mbms.acc_numAccessInfo; i++){
			protocol_ms->rrc.mbms.acc_shortTransmissionID[i] = pvalue->mbms_ServiceAccessInfoList.accessInfo[i].shortTransmissionID;
			protocol_ms->rrc.mbms.acc_accessprobabilityFactor_Idle[i] = pvalue->mbms_ServiceAccessInfoList.accessInfo[i].accessprobabilityFactor_Idle;
		}		
	}
	#ifdef DEBUG_RRC_MBMS
		msg("[RRC-UE][MBMS] Decode MBMSAccessInformation, status %d\n", status);
	#endif
	return status;	
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////
//MBMSGeneralInformation
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_MBMSGeneralInformation (MBMSGeneralInformation * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
	
  status = checkRange(protocol_bs->rrc.mbms.gen_t_318, ms250, ms16000)
                || checkRange(protocol_bs->rrc.mbms.gen_cellGroupIdentity, 0, 0xFFF);
	if (status == P_SUCCESS){
    //Timers and Counters
		pvalue->mbms_TimersAndCounters.t_318 = protocol_bs->rrc.mbms.gen_t_318;
	
		//Cell group identity		
		pvalue->cellGroupIdentity = protocol_bs->rrc.mbms.gen_cellGroupIdentity;
	}
	
	#ifdef DEBUG_RRC_MBMS
		msg("[RRC-RG][MBMS] Encode MBMS General Information, status %d\n", status);
	#endif	
	return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_MBMSGeneralInformation (MBMSGeneralInformation * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
	
	status = checkRange(pvalue->mbms_TimersAndCounters.t_318, ms250, ms16000)
                || checkRange(pvalue->cellGroupIdentity, 0, 0xFFF);
	if (status == P_SUCCESS){
                //Timers and Counters
		protocol_ms->rrc.mbms.gen_t_318 = pvalue->mbms_TimersAndCounters.t_318;
	
		//Cell group identity		
		protocol_ms->rrc.mbms.gen_cellGroupIdentity = pvalue->cellGroupIdentity;
	}
	#ifdef DEBUG_RRC_MBMS
		msg("[RRC-UE][MBMS] Decode MBMS General Information, status %d\n", status);
	#endif
	
	return status;
}
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////
//MBMSCommonPTMRBInformation
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_MBMSCommonPTMRBInformation (MBMSCommonPTMRBInformation * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int buff_length;
  char * pwrite;
	
	//L1-L2 Configuration
// HGN memcpy(pvalue->l12Configuration, protocol_bs->rrc.mbms.comm_l12Configuration, MBMS_L12_CONFIGURATION_SIZE);
//
   /* encode mbms_common_configuration */
   //rrc_compress_buffer(new_conf,90, new_conf, &buff_length);
   buff_length = protocol_bs->rrc.mbms.comm_l12Config_lgth; //Temp

   pwrite = (char *)  &(pvalue->l12Configuration.numoctets);
   protocol_bs->rrc.mbms.curr_Message.msg_length = pwrite - (char *)pvalue + MSG_HEAD_LGTH;

   *(u16*)pwrite = buff_length ;
   pwrite +=2;
   memcpy(pwrite,&(protocol_bs->rrc.mbms.comm_l12Config_data), buff_length);
#ifdef DEBUG_RRC_MBMS
//   msg("\n[RRC_MSG] Buffer length - 1 : %d\n",
//                protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
#endif
   protocol_bs->rrc.mbms.curr_Message.msg_length += buff_length + 2;
#ifdef DEBUG_RRC_MBMS
//   msg("\n[RRC_MSG] Buffer length - 2 : %d\n",
//                protocol_bs->rrc.Mobile_List[UE_Id].rg_msg_infos.msg_length);
#endif

//	
	#ifdef DEBUG_RRC_MBMS
		msg("[RRC-RG][MBMS] Encode MBMS Common PTM RB Information, status %d\n", status);
	#endif
	
	return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_MBMSCommonPTMRBInformation (MBMSCommonPTMRBInformation * pvalue){
//-----------------------------------------------------------------------------
  int buff_length;
  char * buff_ptr;
  int status = P_SUCCESS;

	//L1-L2 Configuration
// HGN memcpy(protocol_ms->rrc.mbms.comm_l12Configuration, pvalue->l12Configuration, MBMS_L12_CONFIGURATION_SIZE);

   /* decode mbms_common_configuration */
   buff_ptr = (char *)(&(pvalue->l12Configuration.numoctets));
   buff_ptr += 2;
   //TEMP
   buff_length = pvalue->l12Configuration.numoctets;
   //rrc_uncompress_buffer(buff_ptr,(u16)pvalue->ue_Configuration.numoctets, buff_ptr, &buff_length);
   memcpy(&(protocol_ms->rrc.mbms.comm_l12Config_data), buff_ptr , buff_length);
   protocol_ms->rrc.mbms.comm_l12Config_lgth = buff_length;

   #ifdef DEBUG_RRC_MBMS_DETAIL
   msg("\n[RRC_MSG] Received MBMS Configuration : \n");
   rrc_print_buffer(buff_ptr, buff_length);
   #endif

	#ifdef DEBUG_RRC_MBMS
		msg("[RRC-UE][MBMS] Decode MBMS Common PTM RB Information, status %d\n", status);
	#endif
	return status;
}
#endif


//////////////////////////////////////////////////////////////////////////////////////////////////
//MBMSCurrentCellPTMRBInformation
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_MBMSCurrentCellPTMRBInformation (MBMSCurrentCellPTMRBInformation * pvalue){
//-----------------------------------------------------------------------------
  char * pwrite;
  int buff_length;
  int status = P_SUCCESS;

  /* encode mbms_current_configuration */
  pwrite = (char *)  &(pvalue->l12Configuration.numoctets);
  protocol_bs->rrc.mbms.curr_Message.msg_length = pwrite - (char *)pvalue + MSG_HEAD_LGTH;

  buff_length = protocol_bs->rrc.mbms.curr_l12Config_lgth;
  if (buff_length <= MTCH_CONFIGURATION_SIZE){
  *(u16*)pwrite = buff_length ;
  pwrite +=2;
  memcpy(pwrite,&(protocol_bs->rrc.mbms.curr_l12Config_data), buff_length);
  protocol_bs->rrc.mbms.curr_Message.msg_length += buff_length + 2;
  }else{
    msg("\n\n[RRC_RG][MBMS] ERROR Encode MBMS Current Cell PTM RB Information , too large: %d\n\n\n", buff_length);
  }

#ifdef DEBUG_RRC_MBMS_DETAIL
  msg("[RRC_RG][MBMS] Sent Current Cell PTM RB Information\n");
  rrc_print_buffer(pwrite, buff_length);
#endif	
#ifdef DEBUG_RRC_MBMS
//   msg("\n[RRC_RG][MBMS] Buffer length  : %d\n",
//                protocol_bs->rrc.mbms.curr_Message.msg_length);
  msg("[RRC_RG][MBMS] Encode MBMS Current Cell PTM RB Information , status %d\n", status);
#endif	
  return status;
}
#endif

#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_MBMSCurrentCellPTMRBInformation (MBMSCurrentCellPTMRBInformation * pvalue){
//-----------------------------------------------------------------------------

  int buff_length;
  char * buff_ptr;
  int status = P_SUCCESS;

   /* decode mbms_current_configuration */
   buff_ptr = (char *)(&(pvalue->l12Configuration.numoctets));
   buff_ptr += 2;
   buff_length = pvalue->l12Configuration.numoctets;
   memcpy(&(protocol_ms->rrc.mbms.curr_l12Config_data), buff_ptr , buff_length);
   protocol_ms->rrc.mbms.curr_l12Config_lgth = buff_length;
   #ifdef DEBUG_RRC_MBMS_DETAIL
   msg("\n[RRC_MSG] Received MBMS Configuration : \n");
   rrc_print_buffer(buff_ptr, pvalue->l12Configuration.numoctets);
   #endif

	#ifdef DEBUG_RRC_MBMS
		msg("[RRC-UE][MBMS] Decode MBMS Current Cell PTM RB Information , status %d\n", status);
	#endif
	return status;
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////
//MBMSNeighbouringCellPTMRBInformation
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_MBMSNeighbouringCellPTMRBInformation (int cellIndex, MBMSNeighbouringCellPTMRBInformation * pvalue) {
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
	if (cellIndex >= protocol_bs->rrc.mbms.neighb_cellCount || cellIndex < 0)
     status = P_OUTOFBOUNDS;
	else{
		//Validate the cellID: must be >= 1
		if (protocol_bs->rrc.mbms.neighb_cellIDList[cellIndex] < 1)
       status = P_INVVALUE;
		else{
			//neighbouring Cell identity
			pvalue->neighbouringCellIdentity = protocol_bs->rrc.mbms.neighb_cellIDList[cellIndex]; //neighbCellID;
			#ifdef DEBUG_RRC_MBMS
				msg("[RRC][MBMS] encode neighbouringCellIdentity = %d\n", (u32) pvalue->neighbouringCellIdentity);
			#endif		
			//Neighbouring Cell Config
			pvalue->neighbouringCellConfig.numoctets = protocol_bs->rrc.mbms.neighb_cellConf_lgth[cellIndex];
			memcpy(pvalue->neighbouringCellConfig.data, protocol_bs->rrc.mbms.neighb_cellConf_data[cellIndex], NEIGHBOUR_CONFIGURATION_SIZE);
		}	
	}

	#ifdef DEBUG_RRC_MBMS
		msg("[RRC-RG][MBMS] Encode MBMS Neigbouring Cell PTM Information, status %d\n", status);
	#endif	
	return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_MBMSNeighbouringCellPTMRBInformation (int *pCellIndex, MBMSNeighbouringCellPTMRBInformation * pvalue){
//-----------------------------------------------------------------------------

  int status = P_SUCCESS;
  int index ;
	
	if (pvalue->neighbouringCellIdentity < 1)
      status = P_INVVALUE;
	else{
		for (index = 0; index < protocol_ms->rrc.mbms.neighb_cellCount; index ++)
			if (protocol_ms->rrc.mbms.neighb_cellIDList[index] == pvalue->neighbouringCellIdentity)
          break;		
		//index will be the position to be written:
		//overwrite if found / insert into the array if not found.
		
		if (index >= MBMS_MAX_NEIGBOURING_COUNT)
       status = P_OUTOFBOUNDS;
		else{
			//neighbouring Cell identity
			protocol_ms->rrc.mbms.neighb_cellIDList[index] = pvalue->neighbouringCellIdentity;
			
			//Neighbouring Cell Config
			protocol_ms->rrc.mbms.neighb_cellConf_lgth[index]= pvalue->neighbouringCellConfig.numoctets;
			memcpy(protocol_ms->rrc.mbms.neighb_cellConf_data[index], pvalue->neighbouringCellConfig.data, NEIGHBOUR_CONFIGURATION_SIZE);
			
			if (index >= protocol_ms->rrc.mbms.neighb_cellCount)
				protocol_ms->rrc.mbms.neighb_cellCount++;
			else msg("[RRC-UE][MBMS] Neigbouring Cell exist, overwritten!\n");
		}
	}
	
	#ifdef DEBUG_RRC_MBMS
		msg("[RRC-UE][MBMS] Decode MBMS Neigbouring Cell PTM Information %dth, status %d\n", *pCellIndex, status);
	#endif		
	return status;
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////
//MBMSModifiedServicesInformation
// To DO (MW) - Separate encoding and decoding according to MCCH and DCCH
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_MBMSModifiedServicesInformation (MBMSModifiedServicesInformation * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int i;

	//Invalidate
  status = checkRange(protocol_bs->rrc.mbms.mod_numService, 0, maxMBMSservModif)
        || checkRange(protocol_bs->rrc.mbms.mod_numberOfNeighbourCells, 0, 32)
        || checkRange(protocol_bs->rrc.mbms.mod_ptm_activationTime, 0, 2047);
//      || checkRange(protocol_bs->rrc.mbms.mod_endOfModifiedMCCHInformation, 1, 15);
	if (status == P_SUCCESS){
    //Modified service list
		pvalue->modifiedServiceList.numService = protocol_bs->rrc.mbms.mod_numService;
		for (i = 0; i<protocol_bs->rrc.mbms.mod_numService; i++){
			//Transmission Identity
        memcpy(pvalue->modifiedServiceList.mbms_modifiedServices[i].mbms_TransmissionIdentity.mbms_ServiceIdentity.serviceIdentity,
               &protocol_bs->rrc.mbms.mod_serviceIdentity[i], 3);
        pvalue->modifiedServiceList.mbms_modifiedServices[i].mbms_TransmissionIdentity.mbms_SessionIdentity =
               protocol_bs->rrc.mbms.mod_sessionIdentity[i];
			//Required UE action
        pvalue->modifiedServiceList.mbms_modifiedServices[i].mbms_RequiredUEAction = protocol_bs->rrc.mbms.mod_requiredUEAction[i];
		}
	
		//MBMS reaquire MCCH
		pvalue->mbms_ReacquireMCCH = protocol_bs->rrc.mbms.mod_reacquireMCCH;
	
		//End of modified MCCH information
		//pvalue->endOfModifiedMCCHInformation = protocol_bs->rrc.mbms.mod_endOfModifiedMCCHInformation;
		
    //MBMS_NumberOfNeighbourCells
    pvalue->mbmsNumberOfNeighbourCells = protocol_bs->rrc.mbms.mod_numberOfNeighbourCells;
    //MBMS_PTMActivationTime
    pvalue->mbms_PTMActivationTime = protocol_bs->rrc.mbms.mod_ptm_activationTime;
	}
	#ifdef DEBUG_RRC_MBMS
		msg("[RRC-RG][MBMS] Encode MBMS Modified Service Information, status %d\n", status);
	#endif	
	return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_MBMSModifiedServicesInformation (MBMSModifiedServicesInformation * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int i;

	status = checkRange(pvalue->modifiedServiceList.numService, 0, maxMBMSservModif)
        || checkRange(pvalue->mbmsNumberOfNeighbourCells, 0, 32)
        || checkRange(pvalue->mbms_PTMActivationTime, 0, 2047);
//      || checkRange(pvalue->endOfModifiedMCCHInformation, 1, 15);

	if (status == P_SUCCESS){

     //Modified service list
     protocol_ms->rrc.mbms.mod_numService = pvalue->modifiedServiceList.numService;
     for (i = 0; i<protocol_ms->rrc.mbms.mod_numService; i++){
     	  //Transmission Identity
     	  protocol_ms->rrc.mbms.mod_serviceIdentity[i] = 0;
     	  memcpy(&protocol_ms->rrc.mbms.mod_serviceIdentity[i],
          pvalue->modifiedServiceList.mbms_modifiedServices[i].mbms_TransmissionIdentity.mbms_ServiceIdentity.serviceIdentity,
          3);
        protocol_ms->rrc.mbms.mod_sessionIdentity[i]=
          pvalue->modifiedServiceList.mbms_modifiedServices[i].mbms_TransmissionIdentity.mbms_SessionIdentity;
			  //Required UE action
			  protocol_ms->rrc.mbms.mod_requiredUEAction[i] =
          pvalue->modifiedServiceList.mbms_modifiedServices[i].mbms_RequiredUEAction;                	
     }
	
		//MBMS reaquire MCCH
		protocol_ms->rrc.mbms.mod_reacquireMCCH = pvalue->mbms_ReacquireMCCH;
		//End of modified MCCH information		
		//protocol_ms->rrc.mbms.mod_endOfModifiedMCCHInformation = pvalue->endOfModifiedMCCHInformation;
    //MBMS_NumberOfNeighbourCells
    protocol_ms->rrc.mbms.mod_numberOfNeighbourCells = pvalue->mbmsNumberOfNeighbourCells;
    //MBMS_PTMActivationTime
    protocol_ms->rrc.mbms.mod_ptm_activationTime = pvalue->mbms_PTMActivationTime;
	}
	
	#ifdef DEBUG_RRC_MBMS
		msg("[RRC-UE][MBMS] Decode MBMS Modified Services Information, status %d\n", status);
	#endif	
	return status;		
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////
//MBMSUnmodifiedServicesInformation
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_MBMSUnmodifiedServicesInformation (MBMSUnmodifiedServicesInformation * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;	
  int i;
	
	status = checkRange(protocol_bs->rrc.mbms.umod_numService, 0, maxMBMSservUnmodif);
	if (status == P_SUCCESS){
                //UnModified service list
		pvalue->unmodifiedServiceList.numService = protocol_bs->rrc.mbms.umod_numService;
		for (i = 0; i<protocol_bs->rrc.mbms.umod_numService; i++){
			//Transmission Identity
			memcpy(
				pvalue->unmodifiedServiceList.unmodifiedServices[i].mbms_TransmissionIdentity.mbms_ServiceIdentity.serviceIdentity,
				&protocol_bs->rrc.mbms.umod_serviceIdentity[i],
				3); //Octetstring (3)
		
			//Required UE action
			pvalue->unmodifiedServiceList.unmodifiedServices[i].mbms_RequiredUEAction = protocol_bs->rrc.mbms.umod_requiredUEAction[i];
		}	
	}	
	
	#ifdef DEBUG_RRC_MBMS
		msg("[RRC-RG][MBMS] Encode MBMS Unmodified Services Information, status %d\n", status);
	#endif			
	return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_MBMSUnmodifiedServicesInformation (MBMSUnmodifiedServicesInformation * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int i;
	status = checkRange(pvalue->unmodifiedServiceList.numService, 0, maxMBMSservUnmodif);
	if (status == P_SUCCESS){
		//UnModified service list
		protocol_ms->rrc.mbms.umod_numService = pvalue->unmodifiedServiceList.numService;
		for (i = 0; i<protocol_ms->rrc.mbms.umod_numService; i++){
			//Transmission Identity
			protocol_ms->rrc.mbms.umod_serviceIdentity[i] = 0;
			memcpy(&protocol_ms->rrc.mbms.umod_serviceIdentity[i],
			       pvalue->unmodifiedServiceList.unmodifiedServices[i].mbms_TransmissionIdentity.mbms_ServiceIdentity.serviceIdentity, 				
			       3); //Octetstring (3)	

//			memcpy(&
//				&pvalue->unmodifiedServiceList.unmodifiedServices[i].mbms_TransmissionIdentity.mbms_ServiceIdentity.plmn_Identity,		
//				sizeof(PLMN_Identity));
		
			//Required UE action
			protocol_ms->rrc.mbms.umod_requiredUEAction[i] = pvalue->unmodifiedServiceList.unmodifiedServices[i].mbms_RequiredUEAction;
//			msg("[RRC][UE] ue->umod_requiredUEAction[%d] = %d\n", i, protocol_ms->rrc.mbms.umod_requiredUEAction[i]);
		}
	}

	#ifdef DEBUG_RRC_MBMS
		msg("[RRC-UE][MBMS] Decode MBMS Unmodified Services Information, status %d\n", status);
	#endif				
	return status;
}
#endif

//////////////////////////////////////////////////////////////////////////////////////////////////
//MBMSSchedulingInformation
#ifdef NODE_RG
//-----------------------------------------------------------------------------
int rrc_PEREnc_MBMSSchedulingInformation (MBMSSchedulingInformation * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;	

  int i, j;
//pointer to the ith MBMS Service Scheduling Info in the list
  MBMS_ServiceSchedulingInfo * pschedInfo;
//pointer to the Transmission Info in a MBMS Service Scheduling Info
  MBMS_ServiceTransmInfo * ptransInfo;
	
	//Invalidate
	status = checkRange(protocol_bs->rrc.mbms.sched_numInfo, 0, maxMBMSservSched);	
	if (status == P_SUCCESS){
		//List of scheduling info
		pvalue->serviceSchedulingInfoList.numInfo =  protocol_bs->rrc.mbms.sched_numInfo;
		for (i = 0; i<protocol_bs->rrc.mbms.sched_numInfo && status == P_SUCCESS; i++){
			//invalidation for the number of transmission of each service
			if ((status = checkRange(protocol_bs->rrc.mbms.sched_trans_numTransmis[i], 0, maxMBMSTransmis)) != P_SUCCESS) break;
			pschedInfo = &pvalue->serviceSchedulingInfoList.schedulingInfo[i];

			//MBMS Transmission identity
			memcpy(
				pschedInfo->mbms_TransmissionIdentity.mbms_ServiceIdentity.serviceIdentity,
				&protocol_bs->rrc.mbms.sched_serviceIdentity[i],
				3);
			//MBMS Service transmission info list
			pschedInfo->mbms_TransmissionInfoList.numInfo = protocol_bs->rrc.mbms.sched_trans_numTransmis[i];
			for (j = 0; j<protocol_bs->rrc.mbms.sched_trans_numTransmis[i]; j++)
			{
				//invalidation for start and duration
				if ((status = checkRange(protocol_bs->rrc.mbms.sched_trans_start[i][j], 0, 1024)) != P_SUCCESS) break;
				if (protocol_bs->rrc.mbms.sched_trans_start[i][j] % 4 != 0) {status = P_INVVALUE; break;}
				if ((status = checkRange(protocol_bs->rrc.mbms.sched_trans_duration[i][j], 4, 1024)) != P_SUCCESS) break;
				//copy to the pdu
				ptransInfo = &pschedInfo->mbms_TransmissionInfoList.serviceTransmissionInfo[j];
				ptransInfo->start = protocol_bs->rrc.mbms.sched_trans_start[i][j];
				ptransInfo->duration = protocol_bs->rrc.mbms.sched_trans_duration[i][j];			
			}		
			//Next scheduling period
			pschedInfo->nextSchedulingPeriod = protocol_bs->rrc.mbms.sched_nextSchedPeriod[i];
		}
	}
	
	#ifdef DEBUG_RRC_MBMS
		msg("[RRC-RG][MBMS] Encode MBMS Scheduling Information, status %d\n", status);
	#endif		
	return status;
}
#endif
#ifdef NODE_MT
//-----------------------------------------------------------------------------
int rrc_PERDec_MBMSSchedulingInformation (MBMSSchedulingInformation * pvalue){
//-----------------------------------------------------------------------------
  int status = P_SUCCESS;
  int i, j;
  MBMS_ServiceSchedulingInfo * pschedInfo; //point to the MBMS Service Scheduling Info ith in the list
  MBMS_ServiceTransmInfo * ptransInfo; //point to the Transmission Info in a MBMS Service Scheduling Info
  
  //Invalidate
  status = checkRange(pvalue->serviceSchedulingInfoList.numInfo, 0, maxMBMSservSched);	
  if (status == P_SUCCESS){
    //List of scheduling info
    protocol_ms->rrc.mbms.sched_numInfo = pvalue->serviceSchedulingInfoList.numInfo;
    for (i = 0; i<protocol_ms->rrc.mbms.sched_numInfo && status == P_SUCCESS; i++){
      //invalidation for the number of transmission of each service
      pschedInfo = &pvalue->serviceSchedulingInfoList.schedulingInfo[i];
      if ((status = checkRange(pschedInfo->mbms_TransmissionInfoList.numInfo, 0, maxMBMSTransmis)) != P_SUCCESS) break;

      //MBMS Transmission identity
      protocol_ms->rrc.mbms.sched_serviceIdentity[i] = 0;
      memcpy( &protocol_ms->rrc.mbms.sched_serviceIdentity[i],
              pschedInfo->mbms_TransmissionIdentity.mbms_ServiceIdentity.serviceIdentity, 3);
      //MBMS Service transmission info list
      protocol_ms->rrc.mbms.sched_trans_numTransmis[i] = pschedInfo->mbms_TransmissionInfoList.numInfo;
      for (j = 0; j<protocol_ms->rrc.mbms.sched_trans_numTransmis[i] && status == P_SUCCESS; j++){
        ptransInfo = &pschedInfo->mbms_TransmissionInfoList.serviceTransmissionInfo[j];
        //invalidation for start and duration
        if ((status = checkRange(ptransInfo->start, 0, 1024)) != P_SUCCESS) 
          break;
        if (ptransInfo->start % 4 != 0){
          status = P_INVVALUE;
          break;
        }
        if ((status = checkRange(ptransInfo->duration, 4, 1024)) != P_SUCCESS) 
          break;

        //copy to the pdu				
        protocol_ms->rrc.mbms.sched_trans_start[i][j] = ptransInfo->start;
        protocol_ms->rrc.mbms.sched_trans_duration[i][j] = ptransInfo->duration;	
      }		
      //Next scheduling period
      protocol_ms->rrc.mbms.sched_nextSchedPeriod[i] = pschedInfo->nextSchedulingPeriod;
    }
  }

  #ifdef DEBUG_RRC_MBMS
          msg("[RRC-UE][MBMS] Decode MBMS Scheduling Information, status %d\n", status);
  #endif			
  return status;
}
#endif
