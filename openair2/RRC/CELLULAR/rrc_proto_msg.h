/***************************************************************************
                          rrc_proto_msg.h  -  description
                             -------------------
    begin                : Jan 6, 2003
    copyright            : (C) 2003, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 ***************************************************************************/
#ifndef __RRC_PROTO_MSG_H__
#define __RRC_PROTO_MSG_H__
#ifdef NODE_MT
// UL
void  rrc_ue_msg_connreq (int *Message_Id);
void  rrc_ue_msg_connsucompl (int *Message_Id);
void  rrc_ue_msg_cellUpdate (int *Message_Id);
void  rrc_ue_msg_connrelUL (int *Message_Id);
void  rrc_ue_msg_initXfer (int *Message_Id);
void  rrc_ue_msg_ulXfer (int *Message_Id);
void  rrc_ue_msg_rbsetupcompl (int *Message_Id);
void  rrc_ue_msg_rbsetupfail (int *Message_Id);
void  rrc_ue_msg_rbreleasecompl (int *Message_Id);
void  rrc_ue_msg_rbreleasefail (int *Message_Id);
void  rrc_ue_msg_measrep (int *Message_Id);
void  rrc_ue_msg_ueCapInfo(int *Message_Id);
// DL
int  rrc_ue_msg_connsetup (DL_CCCH_Message * dl_ccch_msg);
int  rrc_ue_msg_connreject (DL_CCCH_Message * dl_ccch_msg);
int  rrc_ue_msg_cellupdatecnfccch (DL_CCCH_Message * dl_ccch_msg);
int  rrc_ue_msg_rbsetup (DL_DCCH_Message * dl_dcch_msg);
int  rrc_ue_msg_rbrelease (DL_DCCH_Message * dl_dcch_msg);
int  rrc_ue_msg_dlXfer (DL_DCCH_Message * dl_dcch_msg);
int  rrc_ue_msg_pagingt2 (DL_DCCH_Message * dl_dcch_msg);
int  rrc_ue_msg_measctl (DL_DCCH_Message * dl_dcch_msg);
int  rrc_ue_msg_ULPCHctl (DL_DCCH_Message * dl_dcch_msg);
int  rrc_ue_msg_ueCapInfoCnf(DL_DCCH_Message * dl_dcch_msg);
#endif

#ifdef NODE_RG
// UL
int  rrc_rg_msg_connreq (int *pUE, UL_CCCH_Message * ul_ccch_msg);
int  rrc_rg_msg_cellUpdate (int *pUE, UL_CCCH_Message * ul_ccch_msg);
int  rrc_rg_msg_connsucompl (int UE_Id, UL_DCCH_Message * ul_dcch_msg);
int  rrc_rg_msg_connrelUL (int UE_Id, UL_DCCH_Message * ul_dcch_msg);
int  rrc_rg_msg_initXfer (int UE_Id, UL_DCCH_Message * ul_dcch_msg);
int  rrc_rg_msg_ulXfer (int UE_Id, UL_DCCH_Message * ul_dcch_msg);
int  rrc_rg_msg_rbsetupcompl (int UE_Id, UL_DCCH_Message * ul_dcch_msg);
int  rrc_rg_msg_rbsetupfail (int UE_Id, UL_DCCH_Message * ul_dcch_msg);
int  rrc_rg_msg_rbreleasecompl (int UE_Id, UL_DCCH_Message * ul_dcch_msg);
int  rrc_rg_msg_rbreleasefail (int UE_Id, UL_DCCH_Message * ul_dcch_msg);
int  rrc_rg_msg_measrepr (int UE_Id, UL_DCCH_Message * ul_dcch_msg);
int  rrc_rg_msg_ueCapInfo(int UE_Id ,UL_DCCH_Message * ul_dcch_msg);
// DL
void  rrc_rg_msg_connsetup (int UE_Id, int *Message_Id);
void  rrc_rg_msg_connreject (int UE_Id, int *Message_Id);
void  rrc_rg_msg_cellupdatecnfccch (int UE_Id, int *Message_Id);
void  rrc_rg_msg_rbsetup (int UE_Id, int *Message_Id);
void  rrc_rg_msg_rbrelease (int UE_Id, int *Message_Id);
void  rrc_rg_msg_dlXfer (int UE_Id, int *Message_Id);
void  rrc_rg_msg_pagingt2 (int UE_Id, int *Message_Id);
void  rrc_rg_msg_measctl (int UE_Id, int *Message_Id);
void  rrc_rg_msg_ULPCHctl (int UE_Id, int *Message_Id);
void  rrc_rg_msg_ueCapInfoCnf(int UE_Id, int *Message_Id);
#endif

int  rrc_PEREnc_IntegrityCheckInfo (IntegrityCheckInfo value);
int  rrc_PERDec_IntegrityCheckInfo (IntegrityCheckInfo * pvalue);

/**************************************************************/
int  rrc_PEREnc_RRCConnectionRequest (RRCConnectionRequest * pvalue);
int  rrc_PERDec_RRCConnectionRequest (int UE_Id, RRCConnectionRequest * pvalue);

int  rrc_PEREnc_RRCConnectionSetup (int UE_Id, RRCConnectionSetup * pvalue);
int  rrc_PERDec_RRCConnectionSetup (RRCConnectionSetup * pvalue);

int  rrc_PEREnc_RRCConnectionSetupComplete (RRCConnectionSetupComplete * pvalue);
int  rrc_PERDec_RRCConnectionSetupComplete (int UE_Id, RRCConnectionSetupComplete * pvalue);

int  rrc_PEREnc_RRCConnectionReject (int UE_Id, RRCConnectionReject * pvalue);
int  rrc_PERDec_RRCConnectionReject (RRCConnectionReject * pvalue);

int  rrc_PEREnc_RRCConnectionRelease_UL (RRCConnectionRelease_UL * pvalue);
int  rrc_PERDec_RRCConnectionRelease_UL (int UE_Id, RRCConnectionRelease_UL * pvalue);

int  rrc_PEREnc_InitialDirectTransfer (InitialDirectTransfer * pvalue);
int  rrc_PERDec_InitialDirectTransfer (int UE_Id, InitialDirectTransfer * pvalue);

int  rrc_PEREnc_UplinkDirectTransfer (UplinkDirectTransfer * pvalue);
int  rrc_PERDec_UplinkDirectTransfer (int UE_Id, UplinkDirectTransfer * pvalue);

int  rrc_PEREnc_DownlinkDirectTransfer (int UE_Id, DownlinkDirectTransfer * pvalue);
int  rrc_PERDec_DownlinkDirectTransfer (DownlinkDirectTransfer * pvalue);

int  rrc_PEREnc_RadioBearerSetup (int UE_Id, RadioBearerSetup * pvalue);
int  rrc_PERDec_RadioBearerSetup (RadioBearerSetup * pvalue);

int  rrc_PEREnc_RadioBearerRelease (int UE_Id, RadioBearerRelease * pvalue);
int  rrc_PERDec_RadioBearerRelease (RadioBearerRelease * pvalue);

int  rrc_PEREnc_RadioBearerSetupComplete (RadioBearerSetupComplete * pvalue);
int  rrc_PERDec_RadioBearerSetupComplete (int UE_Id, RadioBearerSetupComplete * pvalue);

int  rrc_PEREnc_RadioBearerSetupFailure (RadioBearerSetupFailure * pvalue);
int  rrc_PERDec_RadioBearerSetupFailure (int UE_Id, RadioBearerSetupFailure * pvalue);

int  rrc_PEREnc_RadioBearerReleaseComplete (RadioBearerReleaseComplete * pvalue);
int  rrc_PERDec_RadioBearerReleaseComplete (int UE_Id, RadioBearerReleaseComplete * pvalue);

int  rrc_PEREnc_RadioBearerReleaseFailure (RadioBearerReleaseFailure * pvalue);
int  rrc_PERDec_RadioBearerReleaseFailure (int UE_Id, RadioBearerReleaseFailure * pvalue);

int  rrc_PEREnc_CellUpdate (CellUpdate * pvalue);
int  rrc_PERDec_CellUpdate (int *pUE_Id, CellUpdate * pvalue);

int  rrc_PEREnc_CellUpdateConfirm_CCCH (int UE_Id, CellUpdateConfirm_CCCH * pvalue);
int  rrc_PERDec_CellUpdateConfirm_CCCH (CellUpdateConfirm_CCCH * pvalue);

int  rrc_PEREnc_PagingType2 (int UE_Id, PagingType2 * pvalue);
int  rrc_PERDec_PagingType2 (PagingType2 * pvalue);

int  rrc_PEREnc_MeasurementControl (int UE_Id, MeasurementControl * pvalue);
int  rrc_PERDec_MeasurementControl (MeasurementControl * pvalue);

int  rrc_PEREnc_MeasurementReport (MeasurementReport * pvalue);
int  rrc_PERDec_MeasurementReport (int UE_Id, MeasurementReport * pvalue);

int  rrc_PEREnc_UECapabilityInformation (UECapabilityInformation* pvalue);
int  rrc_PERDec_UECapabilityInformation (int UE_Id, UECapabilityInformation* pvalue);

int  rrc_PEREnc_UECapabilityInformationConfirm(int UE_Id, UECapabilityInformationConfirm * pvalue);
int  rrc_PERDec_UECapabilityInformationConfirm(UECapabilityInformationConfirm* pvalue);


#endif
