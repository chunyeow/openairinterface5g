/*******************************************************************************

  Eurecom OpenAirInterface
  Copyright(c) 1999 - 2012 Eurecom

  This program is free software; you can redistribute it and/or modify it
  under the terms and conditions of the GNU General Public License,
  version 2, as published by the Free Software Foundation.

  This program is distributed in the hope it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

  The full GNU General Public License is included in this distribution in
  the file called "COPYING".

  Contact Information
  Openair Admin: openair_admin@eurecom.fr
  Openair Tech : openair_tech@eurecom.fr
  Forums       : http://forums.eurecom.fr/openairinterface
  Address      : EURECOM, Campus SophiaTech, 450 Route des Chappes
                 06410 Biot FRANCE

*******************************************************************************/

#if HAVE_CONFIG_H_
# include "config.h"
#endif

#include "Criticality.h"
#include "Presence.h"
#include "PrivateIE-ID.h"
#include "ProcedureCode.h"
#include "ProtocolExtensionID.h"
#include "ProtocolIE-ID.h"
#include "TriggeringMessage.h"
#include "AllocationAndRetentionPriority.h"
#include "Bearers-SubjectToStatusTransfer-List.h"
#include "Bearers-SubjectToStatusTransfer-Item.h"
#include "BitRate.h"
#include "BPLMNs.h"
#include "BroadcastCompletedAreaList.h"
#include "Cause.h"
#include "CauseMisc.h"
#include "CauseProtocol.h"
#include "CauseRadioNetwork.h"
#include "CauseTransport.h"
#include "CauseNas.h"
#include "CellIdentity.h"
#include "CellID-Broadcast.h"
#include "CellID-Broadcast-Item.h"
#include "Cdma2000PDU.h"
#include "Cdma2000RATType.h"
#include "Cdma2000SectorID.h"
#include "Cdma2000HOStatus.h"
#include "Cdma2000HORequiredIndication.h"
#include "Cdma2000OneXSRVCCInfo.h"
#include "Cdma2000OneXMEID.h"
#include "Cdma2000OneXMSI.h"
#include "Cdma2000OneXPilot.h"
#include "Cdma2000OneXRAND.h"
#include "Cell-Size.h"
#include "CellType.h"
#include "CGI.h"
#include "CI.h"
#include "CNDomain.h"
#include "CSFallbackIndicator.h"
#include "CSG-Id.h"
#include "CSG-IdList.h"
#include "CSG-IdList-Item.h"
#include "COUNTvalue.h"
#include "CriticalityDiagnostics.h"
#include "CriticalityDiagnostics-IE-List.h"
#include "CriticalityDiagnostics-IE-Item.h"
#include "DataCodingScheme.h"
#include "DL-Forwarding.h"
#include "Direct-Forwarding-Path-Availability.h"
#include "ECGIList.h"
#include "EmergencyAreaIDList.h"
#include "EmergencyAreaID.h"
#include "EmergencyAreaID-Broadcast.h"
#include "EmergencyAreaID-Broadcast-Item.h"
#include "CompletedCellinEAI.h"
#include "CompletedCellinEAI-Item.h"
#include "ENB-ID.h"
#include "GERAN-Cell-ID.h"
#include "Global-ENB-ID.h"
#include "ENB-StatusTransfer-TransparentContainer.h"
#include "ENB-UE-S1AP-ID.h"
#include "ENBname.h"
#include "ENBX2TLAs.h"
#include "EncryptionAlgorithms.h"
#include "EPLMNs.h"
#include "EventType.h"
#include "E-RAB-ID.h"
#include "E-RABInformationList.h"
#include "E-RABInformationListItem.h"
#include "E-RABList.h"
#include "E-RABItem.h"
#include "E-RABLevelQoSParameters.h"
#include "EUTRAN-CGI.h"
#include "ExtendedRNC-ID.h"
#include "ForbiddenInterRATs.h"
#include "ForbiddenTAs.h"
#include "ForbiddenTAs-Item.h"
#include "ForbiddenTACs.h"
#include "ForbiddenLAs.h"
#include "ForbiddenLAs-Item.h"
#include "ForbiddenLACs.h"
#include "GBR-QosInformation.h"
#include "GTP-TEID.h"
#include "GUMMEI.h"
#include "HandoverRestrictionList.h"
#include "HandoverType.h"
#include "HFN.h"
#include "IMSI.h"
#include "IntegrityProtectionAlgorithms.h"
#include "InterfacesToTrace.h"
#include "LAC.h"
#include "LAI.h"
#include "LastVisitedCell-Item.h"
#include "LastVisitedEUTRANCellInformation.h"
#include "LastVisitedUTRANCellInformation.h"
#include "LastVisitedGERANCellInformation.h"
#include "L3-Information.h"
#include "MessageIdentifier.h"
#include "MMEname.h"
#include "MME-Group-ID.h"
#include "MME-Code.h"
#include "MME-UE-S1AP-ID.h"
#include "M-TMSI.h"
#include "MSClassmark2.h"
#include "MSClassmark3.h"
#include "NAS-PDU.h"
#include "NASSecurityParametersfromE-UTRAN.h"
#include "NASSecurityParameterstoE-UTRAN.h"
#include "NumberofBroadcastRequest.h"
#include "NumberOfBroadcasts.h"
#include "OldBSS-ToNewBSS-Information.h"
#include "OverloadAction.h"
#include "OverloadResponse.h"
#include "PagingDRX.h"
#include "PDCP-SN.h"
#include "PLMNidentity.h"
#include "Pre-emptionCapability.h"
#include "Pre-emptionVulnerability.h"
#include "PriorityLevel.h"
#include "QCI.h"
#include "ReceiveStatusofULPDCPSDUs.h"
#include "RelativeMMECapacity.h"
#include "RAC.h"
#include "RequestType.h"
#include "RIMTransfer.h"
#include "RIMInformation.h"
#include "RIMRoutingAddress.h"
#include "ReportArea.h"
#include "RepetitionPeriod.h"
#include "RNC-ID.h"
#include "RRC-Container.h"
#include "RRC-Establishment-Cause.h"
#include "SecurityKey.h"
#include "SecurityContext.h"
#include "SerialNumber.h"
#include "SONInformation.h"
#include "SONInformationRequest.h"
#include "SONInformationReply.h"
#include "SONConfigurationTransfer.h"
#include "Source-ToTarget-TransparentContainer.h"
#include "SourceBSS-ToTargetBSS-TransparentContainer.h"
#include "SourceeNB-ID.h"
#include "SRVCCOperationPossible.h"
#include "SRVCCHOIndication.h"
#include "SourceeNB-ToTargeteNB-TransparentContainer.h"
#include "SourceRNC-ToTargetRNC-TransparentContainer.h"
#include "ServedGUMMEIs.h"
#include "ServedGUMMEIsItem.h"
#include "ServedGroupIDs.h"
#include "ServedMMECs.h"
#include "ServedPLMNs.h"
#include "SubscriberProfileIDforRFP.h"
#include "SupportedTAs.h"
#include "SupportedTAs-Item.h"
#include "S-TMSI.h"
#include "TAC.h"
#include "TAIItem.h"
#include "TAIList.h"
#include "TAIListforWarning.h"
#include "TAI.h"
#include "TAI-Broadcast.h"
#include "TAI-Broadcast-Item.h"
#include "CompletedCellinTAI.h"
#include "CompletedCellinTAI-Item.h"
#include "TBCD-STRING.h"
#include "TargetID.h"
#include "TargeteNB-ID.h"
#include "TargetRNC-ID.h"
#include "TargeteNB-ToSourceeNB-TransparentContainer.h"
#include "Target-ToSource-TransparentContainer.h"
#include "TargetRNC-ToSourceRNC-TransparentContainer.h"
#include "TargetBSS-ToSourceBSS-TransparentContainer.h"
#include "TimeToWait.h"
#include "Time-UE-StayedInCell.h"
#include "TransportLayerAddress.h"
#include "TraceActivation.h"
#include "TraceDepth.h"
#include "E-UTRAN-Trace-ID.h"
#include "TypeOfError.h"
#include "UEAggregateMaximumBitrate.h"
#include "UE-S1AP-IDs.h"
#include "UE-S1AP-ID-pair.h"
#include "UE-associatedLogicalS1-ConnectionItem.h"
#include "UEIdentityIndexValue.h"
#include "UE-HistoryInformation.h"
#include "UEPagingID.h"
#include "UERadioCapability.h"
#include "UESecurityCapabilities.h"
#include "WarningAreaList.h"
#include "WarningType.h"
#include "WarningSecurityInfo.h"
#include "WarningMessageContents.h"
#include "X2TNLConfigurationInfo.h"
#include "S1AP-PDU.h"
#include "InitiatingMessage.h"
#include "SuccessfulOutcome.h"
#include "UnsuccessfulOutcome.h"
#include "HandoverRequired.h"
#include "HandoverCommand.h"
#include "HandoverNotify.h"
#include "HandoverPreparationFailure.h"
#include "HandoverRequest.h"
#include "HandoverRequestAcknowledge.h"
#include "HandoverFailure.h"
#include "PathSwitchRequest.h"
#include "PathSwitchRequestAcknowledge.h"
#include "PathSwitchRequestFailure.h"
#include "E-RABSetupRequest.h"
#include "E-RABSetupResponse.h"
#include "E-RABModifyRequest.h"
#include "E-RABModifyResponse.h"
#include "E-RABReleaseIndication.h"
#include "E-RABReleaseCommand.h"
#include "E-RABReleaseResponse.h"
#include "InitialContextSetupRequest.h"
#include "InitialContextSetupResponse.h"
#include "InitialContextSetupFailure.h"
#include "UEContextReleaseRequest.h"
#include "Paging.h"
#include "DownlinkNASTransport.h"
#include "InitialUEMessage.h"
#include "UplinkNASTransport.h"
#include "NASNonDeliveryIndication.h"
#include "HandoverCancel.h"
#include "HandoverCancelAcknowledge.h"
#include "Reset.h"
#include "ResetType.h"
#include "ResetAcknowledge.h"
#include "S1SetupResponse.h"
#include "S1SetupRequest.h"
#include "S1SetupFailure.h"
#include "ErrorIndication.h"
#include "ENBConfigurationUpdate.h"
#include "ENBConfigurationUpdateAcknowledge.h"
#include "ENBConfigurationUpdateFailure.h"
#include "MMEConfigurationUpdate.h"
#include "MMEConfigurationUpdateAcknowledge.h"
#include "MMEConfigurationUpdateFailure.h"
#include "DownlinkS1cdma2000tunneling.h"
#include "UplinkS1cdma2000tunneling.h"
#include "UEContextModificationRequest.h"
#include "UEContextModificationResponse.h"
#include "UEContextModificationFailure.h"
#include "UECapabilityInfoIndication.h"
#include "UEContextReleaseCommand.h"
#include "UEContextReleaseComplete.h"
#include "ENBStatusTransfer.h"
#include "MMEStatusTransfer.h"
#include "DeactivateTrace.h"
#include "TraceStart.h"
#include "TraceFailureIndication.h"
#include "CellTrafficTrace.h"
#include "LocationReportingControl.h"
#include "LocationReportingFailureIndication.h"
#include "LocationReport.h"
#include "OverloadStart.h"
#include "OverloadStop.h"
#include "WriteReplaceWarningRequest.h"
#include "WriteReplaceWarningResponse.h"
#include "ENBDirectInformationTransfer.h"
#include "MMEDirectInformationTransfer.h"
#include "ENBConfigurationTransfer.h"
#include "MMEConfigurationTransfer.h"
#include "PrivateMessage.h"
#include "Inter-SystemInformationTransferType.h"
#include "E-RABReleaseItemBearerRelComp.h"
#include "E-RABToBeSwitchedDLList.h"
#include "E-RABToBeSwitchedDLItem.h"
#include "E-RABToBeSwitchedULList.h"
#include "E-RABToBeSwitchedULItem.h"
#include "E-RABToBeSetupListBearerSUReq.h"
#include "E-RABToBeSetupItemBearerSUReq.h"
#include "E-RABDataForwardingList.h"
#include "E-RABDataForwardingItem.h"
#include "E-RABToBeSetupListHOReq.h"
#include "E-RABToBeSetupItemHOReq.h"
#include "E-RABAdmittedList.h"
#include "E-RABAdmittedItem.h"
#include "E-RABFailedtoSetupListHOReqAck.h"
#include "E-RABToBeSetupItemCtxtSUReq.h"
#include "E-RABToBeSetupListCtxtSUReq.h"
#include "E-RABSetupItemBearerSURes.h"
#include "E-RABSetupListBearerSURes.h"
#include "E-RABSetupItemCtxtSURes.h"
#include "E-RABSetupListCtxtSURes.h"
#include "E-RABReleaseListBearerRelComp.h"
#include "E-RABModifyItemBearerModRes.h"
#include "E-RABModifyListBearerModRes.h"
#include "E-RABFailedToSetupItemHOReqAck.h"
#include "E-RABFailedtoSetupListHOReqAck.h"
#include "E-RABToBeModifiedItemBearerModReq.h"
#include "E-RABToBeModifiedListBearerModReq.h"
#include "UE-associatedLogicalS1-ConnectionListResAck.h"
#include "IE.h"

#if defined (UPDATE_RELEASE_9)
# include "BroadcastCancelledAreaList.h"
# include "CancelledCellinEAI.h"
# include "CancelledCellinEAI-Item.h"
# include "CancelledCellinTAI.h"
# include "CancelledCellinTAI-Item.h"
# include "CellAccessMode.h"
# include "CellID-Cancelled.h"
# include "CellID-Cancelled-Item.h"
# include "ConcurrentWarningMessageIndicator.h"
# include "CSGMembershipStatus.h"
# include "Data-Forwarding-Not-Possible.h"
# include "EmergencyAreaID-Cancelled.h"
# include "EmergencyAreaID-Cancelled-Item.h"
# include "E-RABList.h"
# include "EUTRANRoundTripDelayEstimationInfo.h"
# include "ExtendedRepetitionPeriod.h"
# include "LPPa-PDU.h"
# include "PS-ServiceNotAvailable.h"
# include "Routing-ID.h"
# include "StratumLevel.h"
# include "SynchronizationStatus.h"
# include "TimeSynchronizationInfo.h"
# include "TAI-Cancelled.h"
# include "TAI-Cancelled-Item.h"
# include "KillRequest.h"
# include "KillResponse.h"
# include "DownlinkUEAssociatedLPPaTransport.h"
# include "UplinkUEAssociatedLPPaTransport.h"
# include "DownlinkNonUEAssociatedLPPaTransport.h"
# include "UplinkNonUEAssociatedLPPaTransport.h"
#endif /* (UPDATE_RELEASE_9) */

#ifndef S1AP_COMMON_H_
#define S1AP_COMMON_H_

/** @defgroup _s1ap_impl_ S1AP Layer Reference Implementation
 * @ingroup _ref_implementation_
 * @{
 */

/* Checking version of ASN1C compiler */
#if (ASN1C_ENVIRONMENT_VERSION < 923)
# error "You are compiling s1ap with the wrong version of ASN1C"
#endif

#define S1AP_PORT 36412

#define TRUE 1
#define FALSE 0

extern int asn_debug;
extern int asn1_xer_print;

#if defined(ENB_MODE)
# include "log.h"
# define S1AP_ERROR(x, args...) LOG_E(S1AP, x, ##args)
# define S1AP_WARN(x, args...)  LOG_W(S1AP, x, ##args)
# define S1AP_DEBUG(x, args...) LOG_D(S1AP, x, ##args)
#else
# define S1AP_ERROR(x, args...) do { fprintf(stdout, "[S1AP][E]"x, ##args); } while(0)
# define S1AP_WARN(x, args...)  do { fprintf(stdout, "[S1AP][W]"x, ##args); } while(0)
# define S1AP_DEBUG(x, args...) do { fprintf(stdout, "[S1AP][D]"x, ##args); } while(0)
#endif

//Forward declaration
struct s1ap_message_s;

/** \brief Function callback prototype.
 **/
typedef int (*s1ap_message_decoded_callback)(
    uint32_t assocId,
    uint32_t stream,
    struct s1ap_message_s *message);

/** \brief Encode a successfull outcome message
 \param buffer pointer to buffer in which data will be encoded
 \param length pointer to the length of buffer
 \param procedureCode Procedure code for the message
 \param criticality Criticality of the message
 \param td ASN1C type descriptor of the sptr
 \param sptr Deferenced pointer to the structure to encode
 @returns size in bytes encded on success or 0 on failure
 **/
ssize_t s1ap_generate_successfull_outcome(
    uint8_t               **buffer,
    uint32_t               *length,
    e_ProcedureCode         procedureCode,
    Criticality_t           criticality,
    asn_TYPE_descriptor_t  *td,
    void                   *sptr);

/** \brief Encode an initiating message
 \param buffer pointer to buffer in which data will be encoded
 \param length pointer to the length of buffer
 \param procedureCode Procedure code for the message
 \param criticality Criticality of the message
 \param td ASN1C type descriptor of the sptr
 \param sptr Deferenced pointer to the structure to encode
 @returns size in bytes encded on success or 0 on failure
 **/
ssize_t s1ap_generate_initiating_message(
    uint8_t               **buffer,
    uint32_t               *length,
    e_ProcedureCode         procedureCode,
    Criticality_t           criticality,
    asn_TYPE_descriptor_t  *td,
    void                   *sptr);

/** \brief Encode an unsuccessfull outcome message
 \param buffer pointer to buffer in which data will be encoded
 \param length pointer to the length of buffer
 \param procedureCode Procedure code for the message
 \param criticality Criticality of the message
 \param td ASN1C type descriptor of the sptr
 \param sptr Deferenced pointer to the structure to encode
 @returns size in bytes encded on success or 0 on failure
 **/
ssize_t s1ap_generate_unsuccessfull_outcome(
    uint8_t               **buffer,
    uint32_t               *length,
    e_ProcedureCode         procedureCode,
    Criticality_t           criticality,
    asn_TYPE_descriptor_t  *td,
    void                   *sptr);

/** \brief Generate a new IE
 \param id Protocol ie id of the IE
 \param criticality Criticality of the IE
 \param type ASN1 type descriptor of the IE value
 \param sptr Structure to be encoded in the value field
 @returns a pointer to the newly created IE structure or NULL in case of failure
 **/
IE_t *s1ap_new_ie(ProtocolIE_ID_t        id,
                  Criticality_t          criticality,
                  asn_TYPE_descriptor_t *type,
                  void                  *sptr);

/** \brief Handle criticality
 \param criticality Criticality of the IE
 @returns void
 **/
void s1ap_handle_criticality(e_Criticality criticality);

#endif /* S1AP_COMMON_H_ */
