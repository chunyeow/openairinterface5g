/***************************************************************************
                          rrc_proto_bch.h  -  description
                             -------------------
    begin                : Aug 30 2002
    copyright            : (C) 2002, 2010 by Eurecom
    email                : Michelle.Wetterwald@eurecom.fr
 ***************************************************************************/
#ifndef __RRC_PROTO_BCH_H__
#define __RRC_PROTO_BCH_H__

/**************************************************************/
void rrc_new_per_parms (PERParms * pParms, char *pBuffer);
void rrc_set_per_length (PERParms * pParms);
int  rrc_get_per_length (ENCODEDBLOCK * pBlock);
void rrc_print_per_parms (PERParms * pParms);

int  rrc_PEREnc_padding (PERParms * pParms, int length);
// Constrained integer
int  rrc_PEREnc_ConstrInteger (PERParms * per_p, int pvalue, int lower, int upper);
int  rrc_PERDec_ConstrInteger (PERParms * per_p, int *value, int lower, int upper);
//  Constrained Unsigned Integer
int  rrc_PEREnc_ConsUnsigned (PERParms * pParms, unsigned int value, unsigned int lower, unsigned int upper);
int  rrc_PERDec_ConsUnsigned (PERParms * pParms, unsigned int *pvalue, unsigned int lower, unsigned int upper);
//  Bit String
int  rrc_PEREnc_BitString (PERParms * pParms, unsigned int numbits, unsigned char *data);
int  rrc_PERDec_BitString (PERParms * pParms, unsigned int *numbits_p, char *buffer, unsigned int bufsiz);
//  Octet string
int  rrc_PEREnc_OctetString (PERParms * pParms, unsigned int numocts, char *data);
int  rrc_PERDec_OctetString (PERParms * pParms, unsigned int *numocts_p, char *buffer, unsigned int bufsiz);
//  Variable Octet string with length
int  rrc_PEREnc_VarOctetString (PERParms * pParms, unsigned int numocts, unsigned char *data);
int  rrc_PERDec_VarOctetString (PERParms * pParms, unsigned int *numocts_p, unsigned char *data);
//  Digit
int  rrc_PEREnc_Digit (PERParms * pParms, Digit value);
int  rrc_PERDec_Digit (PERParms * pParms, Digit * pvalue);
//  Length
int  rrc_PEREnc_Length (PERParms * pParms, unsigned int value);
int  rrc_PERDec_Length (PERParms * pParms, unsigned int *pvalue);
/**************************************************************/

#ifdef NODE_RG
void rrc_rg_prepare_next_segment (void);
void rrc_rg_init_bch (void);
void rrc_init_blocks (void);
void rrc_init_mib (void);
void rrc_init_sib1 (void);
void rrc_update_SIB1_period (int period);
void rrc_init_sib2 (void);
void rrc_init_sib5 (void);
void rrc_init_sib11 (void);
void rrc_fill_sib14 (void);
void rrc_init_sib14 (void);
void rrc_init_sib18 (void);
void rrc_update_SIB18_period (int period);
#endif
#ifdef NODE_MT
void rrc_ue_bch_init (void);
void rrc_ue_read_next_segment (void);
void rrc_ue_bch_process_MIB (PERParms * pParms);
void rrc_ue_bch_process_SIB1 (PERParms * pParms);
void rrc_ue_bch_process_SIB2 (PERParms * pParms);
void rrc_ue_bch_process_SIB5 (PERParms * pParms);
void rrc_ue_bch_process_SIB11 (PERParms * pParms);
void rrc_ue_bch_process_SIB14 (PERParms * pParms);
void rrc_ue_bch_process_SIB18 (PERParms * pParms);
#endif

int  rrc_PEREnc_SI_BCH (PERParms * pParms, SystemInformation_BCH * pvalue);
int  rrc_PERDec_SI_BCH (PERParms * pParms, SystemInformation_BCH * pvalue);

int  rrc_PEREnc_SI_BCH_payload (PERParms * pParms, SystemInformation_BCH_payload * pvalue);
int  rrc_PERDec_SI_BCH_payload (PERParms * pParms, SystemInformation_BCH_payload * pvalue);
/**************************************************************/

int  rrc_PEREnc_FirstSegment (PERParms * pParms, FirstSegment * pvalue);
int  rrc_PERDec_FirstSegment (PERParms * pParms, FirstSegment * pvalue);

int  rrc_PEREnc_SubsequentSegment (PERParms * pParms, SubsequentSegment * pvalue);
int  rrc_PERDec_SubsequentSegment (PERParms * pParms, SubsequentSegment * pvalue);

int  rrc_PEREnc_CompleteSIB (PERParms * pParms, CompleteSIB * pvalue);
int  rrc_PERDec_CompleteSIB (PERParms * pParms, CompleteSIB * pvalue);

int  rrc_PEREnc_LastSegment (PERParms * pParms, LastSegment * pvalue);
int  rrc_PERDec_LastSegment (PERParms * pParms, LastSegment * pvalue);

/**************************************************************/
int  rrc_PEREnc_SIB_Type (PERParms * pParms, SIB_Type value);
int  rrc_PERDec_SIB_Type (PERParms * pParms, SIB_Type * pvalue);

int  rrc_PEREnc_SegCount (PERParms * pParms, SegCount value);
int  rrc_PERDec_SegCount (PERParms * pParms, SegCount * pvalue);
int  rrc_PEREnc_SegmentIndex (PERParms * pParms, SegmentIndex value);
int  rrc_PERDec_SegmentIndex (PERParms * pParms, SegmentIndex * pvalue);

int  rrc_PEREnc_SIB_Data_fixed (PERParms * pParms, SIB_Data_fixed value);
int  rrc_PERDec_SIB_Data_fixed (PERParms * pParms, SIB_Data_fixed * pvalue);
int  rrc_PEREnc_CompleteSIB_sib_Data_fixed (PERParms * pParms, CompleteSIB_sib_Data_fixed value);
int  rrc_PERDec_CompleteSIB_sib_Data_fixed (PERParms * pParms, CompleteSIB_sib_Data_fixed * pvalue);

int  rrc_PEREnc_SFN_Prime (PERParms * pParms, SFN_Prime value);
int  rrc_PERDec_SFN_Prime (PERParms * pParms, SFN_Prime * pvalue);

/**************************************************************/
int  rrc_PEREnc_MasterInformationBlock (PERParms * pParms, MasterInformationBlock * pvalue);
int  rrc_PERDec_MasterInformationBlock (PERParms * pParms, MasterInformationBlock * pvalue);

int  rrc_PEREnc_MIB_ValueTag (PERParms * pParms, MIB_ValueTag value);
int  rrc_PERDec_MIB_ValueTag (PERParms * pParms, MIB_ValueTag * pvalue);

int  rrc_PEREnc_PLMN_Identity (PERParms * pParms, RCELL_PLMN_Identity * pvalue);
int  rrc_PERDec_PLMN_Identity (PERParms * pParms, RCELL_PLMN_Identity * pvalue);

int  rrc_PEREnc_SIBSb_ReferenceList (PERParms * pParms, SIBSb_ReferenceList * pvalue);
int  rrc_PERDec_SIBSb_ReferenceList (PERParms * pParms, SIBSb_ReferenceList * pvalue);

/**************************************************************/
int  rrc_PEREnc_SysInfoType1 (PERParms * pParms, SysInfoType1 * pvalue);
int  rrc_PERDec_SysInfoType1 (PERParms * pParms, SysInfoType1 * pvalue);

int  rrc_PEREnc_NAS_SystemInformation (PERParms * pParms, NAS_SystemInformation value);
int  rrc_PERDec_NAS_SystemInformation (PERParms * pParms, NAS_SystemInformation * pvalue);

int  rrc_PEREnc_UE_TimersAndConstants (PERParms * pParms, RCELL_UE_TimersAndConstants value);
int  rrc_PERDec_UE_TimersAndConstants (PERParms * pParms, RCELL_UE_TimersAndConstants * pvalue);

/**************************************************************/
int  rrc_PEREnc_SysInfoType2 (PERParms * pParms, SysInfoType2 * pvalue);
int  rrc_PERDec_SysInfoType2 (PERParms * pParms, SysInfoType2 * pvalue);

int  rrc_PEREnc_IP_Address (PERParms * pParms, IP_Address * pvalue);
int  rrc_PERDec_IP_Address (PERParms * pParms, IP_Address * pvalue);

/**************************************************************/
int  rrc_PEREnc_SysInfoType5 (PERParms * pParms, SysInfoType5 * pvalue);
int  rrc_PERDec_SysInfoType5 (PERParms * pParms, SysInfoType5 * pvalue);

int  rrc_PEREnc_PRACH_SCCPCH_SIList (PERParms * pParms, PRACH_SCCPCH_SIList value);
int  rrc_PERDec_PRACH_SCCPCH_SIList (PERParms * pParms, PRACH_SCCPCH_SIList * pvalue);

int  rrc_PEREnc_OpenLoopPowerControl_TDD (PERParms * pParms, OpenLoopPowerControl_TDD * pvalue);
int  rrc_PERDec_OpenLoopPowerControl_TDD (PERParms * pParms, OpenLoopPowerControl_TDD * pvalue);

/**************************************************************/
int  rrc_PEREnc_SysInfoType14 (PERParms * pParms, SysInfoType14 * pvalue);
int  rrc_PERDec_SysInfoType14 (PERParms * pParms, SysInfoType14 * pvalue);

int  rrc_PEREnc_IndividualTS_InterferenceList (PERParms * pParms, IndividualTS_InterferenceList * pvalue);
int  rrc_PERDec_IndividualTS_InterferenceList (PERParms * pParms, IndividualTS_InterferenceList * pvalue);

int  rrc_PEREnc_ExpirationTimeFactor (PERParms * pParms, ExpirationTimeFactor value);
int  rrc_PERDec_ExpirationTimeFactor (PERParms * pParms, ExpirationTimeFactor * pvalue);

/**************************************************************/
int  rrc_PEREnc_SysInfoType18 (PERParms * pParms, SysInfoType18 * pvalue);
int  rrc_PERDec_SysInfoType18 (PERParms * pParms, SysInfoType18 * pvalue);

int  rrc_PEREnc_IdentitiesOfNeighbourCells (PERParms * pParms, IdentitiesOfNeighbourCells value);
int  rrc_PERDec_IdentitiesOfNeighbourCells (PERParms * pParms, IdentitiesOfNeighbourCells * pvalue);

int  rrc_PEREnc_CodeGroupsOfNeighbourCells (PERParms * pParms, CodeGroupsOfNeighbourCells * pvalue);
int  rrc_PERDec_CodeGroupsOfNeighbourCells (PERParms * pParms, CodeGroupsOfNeighbourCells * pvalue);

/**************************************************************/
#ifdef NODE_RG
//extern void rrc_broadcast_tx (void);
extern int rrc_broadcast_tx (char *buffer);
extern void rrc_init_blocks (void);
#endif
#ifdef NODE_MT
// #include "mem_block.h"
//extern void rrc_broadcast_rx (void *protP, mem_block_t *sduP);
void rrc_broadcast_rx (char *Sdu);
//extern void rrc_init_tags (void);
# endif

#endif
