################################################################################
#   OpenAirInterface
#   Copyright(c) 1999 - 2014 Eurecom
#
#    OpenAirInterface is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#
#    OpenAirInterface is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with OpenAirInterface.The full GNU General Public License is
#    included in this distribution in the file called "COPYING". If not,
#    see <http://www.gnu.org/licenses/>.
#
#  Contact Information
#  OpenAirInterface Admin: openair_admin@eurecom.fr
#  OpenAirInterface Tech : openair_tech@eurecom.fr
#  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
#
#  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.
#
################################################################################
S1AP_ASN_MODULE_SOURCES= \
    S1ap-Criticality.o   \
    S1ap-Presence.o  \
    S1ap-PrivateIE-ID.o  \
    S1ap-ProcedureCode.o \
    S1ap-ProtocolExtensionID.o   \
    S1ap-ProtocolIE-ID.o \
    S1ap-TriggeringMessage.o \
    S1ap-IE-Extensions.o \
    S1ap-AllocationAndRetentionPriority.o    \
    S1ap-Bearers-SubjectToStatusTransfer-List.o   \
    S1ap-Bearers-SubjectToStatusTransfer-Item.o  \
    S1ap-BitRate.o   \
    S1ap-BPLMNs.o    \
    S1ap-BroadcastCompletedAreaList.o    \
    S1ap-Cause.o \
    S1ap-CauseMisc.o \
    S1ap-CauseProtocol.o \
    S1ap-CauseRadioNetwork.o \
    S1ap-CauseTransport.o    \
    S1ap-CauseNas.o  \
    S1ap-CellIdentity.o  \
    S1ap-CellID-Broadcast.o  \
    S1ap-CellID-Broadcast-Item.o \
    S1ap-Cdma2000PDU.o   \
    S1ap-Cdma2000RATType.o   \
    S1ap-Cdma2000SectorID.o  \
    S1ap-Cdma2000HOStatus.o  \
    S1ap-Cdma2000HORequiredIndication.o  \
    S1ap-Cdma2000OneXSRVCCInfo.o \
    S1ap-Cdma2000OneXMEID.o  \
    S1ap-Cdma2000OneXMSI.o   \
    S1ap-Cdma2000OneXPilot.o \
    S1ap-Cdma2000OneXRAND.o  \
    S1ap-Cell-Size.o \
    S1ap-CellType.o  \
    S1ap-CGI.o   \
    S1ap-CI.o    \
    S1ap-CNDomain.o  \
    S1ap-CSFallbackIndicator.o   \
    S1ap-CSG-Id.o    \
    S1ap-CSG-IdList.o    \
    S1ap-CSG-IdList-Item.o   \
    S1ap-COUNTvalue.o    \
    S1ap-CriticalityDiagnostics.o    \
    S1ap-CriticalityDiagnostics-IE-List.o    \
    S1ap-CriticalityDiagnostics-IE-Item.o    \
    S1ap-DataCodingScheme.o  \
    S1ap-DL-Forwarding.o \
    S1ap-Direct-Forwarding-Path-Availability.o   \
    S1ap-E-RABToBeModifiedListBearerModReq.o \
    S1ap-E-RABToBeModifiedItemBearerModReq.o \
    S1ap-E-RABReleaseListBearerRelComp.o \
    S1ap-E-RABReleaseItemBearerRelComp.o \
    S1ap-E-RABDataForwardingList.o   \
    S1ap-E-RABDataForwardingItem.o   \
    S1ap-E-RABToBeSetupListCtxtSUReq.o   \
    S1ap-E-RABToBeSetupItemCtxtSUReq.o   \
    S1ap-E-RABSetupListBearerSURes.o \
    S1ap-E-RABSetupItemBearerSURes.o \
    S1ap-E-RABFailedToSetupListHOReqAck.o    \
    S1ap-E-RABFailedToSetupItemHOReqAck.o    \
    S1ap-E-RABToBeSetupListBearerSUReq.o \
    S1ap-E-RABToBeSetupItemBearerSUReq.o \
    S1ap-E-RABAdmittedList.o \
    S1ap-E-RABAdmittedItem.o \
    S1ap-E-RABToBeSwitchedDLList.o   \
    S1ap-E-RABToBeSwitchedDLItem.o   \
    S1ap-E-RABToBeSwitchedULList.o   \
    S1ap-E-RABToBeSwitchedULItem.o   \
    S1ap-E-RABModifyListBearerModRes.o   \
    S1ap-E-RABModifyItemBearerModRes.o   \
    S1ap-E-RABSetupListCtxtSURes.o   \
    S1ap-E-RABSetupItemCtxtSURes.o   \
    S1ap-E-RABToBeSetupListHOReq.o   \
    S1ap-E-RABToBeSetupItemHOReq.o   \
    S1ap-ECGIList.o  \
    S1ap-EmergencyAreaIDList.o   \
    S1ap-EmergencyAreaID.o   \
    S1ap-EmergencyAreaID-Broadcast.o \
    S1ap-EmergencyAreaID-Broadcast-Item.o    \
    S1ap-CompletedCellinEAI.o    \
    S1ap-CompletedCellinEAI-Item.o   \
    S1ap-ENB-ID.o    \
    S1ap-GERAN-Cell-ID.o \
    S1ap-Global-ENB-ID.o \
    S1ap-ENB-StatusTransfer-TransparentContainer.o   \
    S1ap-ENB-UE-S1AP-ID.o    \
    S1ap-ENBname.o   \
    S1ap-ENBX2TLAs.o \
    S1ap-EncryptionAlgorithms.o  \
    S1ap-EPLMNs.o    \
    S1ap-EventType.o \
    S1ap-E-RAB-ID.o  \
    S1ap-E-RABInformationList.o  \
    S1ap-E-RABInformationListItem.o  \
    S1ap-E-RABList.o \
    S1ap-E-RABItem.o \
    S1ap-E-RABLevelQoSParameters.o   \
    S1ap-EUTRAN-CGI.o    \
    S1ap-ExtendedRNC-ID.o    \
    S1ap-ForbiddenInterRATs.o    \
    S1ap-ForbiddenTAs.o  \
    S1ap-ForbiddenTAs-Item.o \
    S1ap-ForbiddenTACs.o \
    S1ap-ForbiddenLAs.o  \
    S1ap-ForbiddenLAs-Item.o \
    S1ap-ForbiddenLACs.o \
    S1ap-GBR-QosInformation.o    \
    S1ap-GTP-TEID.o  \
    S1ap-GUMMEI.o    \
    S1ap-HandoverRestrictionList.o   \
    S1ap-HandoverType.o  \
    S1ap-HFN.o   \
    S1ap-Inter-SystemInformationTransferType.o   \
    S1ap-IMSI.o  \
    S1ap-IntegrityProtectionAlgorithms.o \
    S1ap-InterfacesToTrace.o \
    S1ap-LAC.o   \
    S1ap-LAI.o   \
    S1ap-LastVisitedCell-Item.o  \
    S1ap-LastVisitedEUTRANCellInformation.o  \
    S1ap-LastVisitedUTRANCellInformation.o   \
    S1ap-LastVisitedGERANCellInformation.o   \
    S1ap-L3-Information.o    \
    S1ap-MessageIdentifier.o \
    S1ap-MMEname.o   \
    S1ap-MME-Group-ID.o  \
    S1ap-MME-Code.o  \
    S1ap-MME-UE-S1AP-ID.o    \
    S1ap-M-TMSI.o    \
    S1ap-MSClassmark2.o  \
    S1ap-MSClassmark3.o  \
    S1ap-NAS-PDU.o   \
    S1ap-NASSecurityParametersfromE-UTRAN.o  \
    S1ap-NASSecurityParameterstoE-UTRAN.o    \
    S1ap-NumberofBroadcastRequest.o  \
    S1ap-NumberOfBroadcasts.o \
    S1ap-OldBSS-ToNewBSS-Information.o   \
    S1ap-OverloadAction.o    \
    S1ap-OverloadResponse.o  \
    S1ap-PagingDRX.o \
    S1ap-PDCP-SN.o   \
    S1ap-PLMNidentity.o  \
    S1ap-Pre-emptionCapability.o \
    S1ap-Pre-emptionVulnerability.o  \
    S1ap-PriorityLevel.o \
    S1ap-QCI.o   \
    S1ap-ReceiveStatusofULPDCPSDUs.o \
    S1ap-RelativeMMECapacity.o   \
    S1ap-RAC.o   \
    S1ap-RequestType.o   \
    S1ap-ResetType.o \
    S1ap-ResetAll.o  \
    S1ap-RIMTransfer.o   \
    S1ap-RIMInformation.o    \
    S1ap-RIMRoutingAddress.o \
    S1ap-ReportArea.o    \
    S1ap-RepetitionPeriod.o  \
    S1ap-RNC-ID.o    \
    S1ap-RRC-Container.o \
    S1ap-RRC-Establishment-Cause.o   \
    S1ap-SecurityKey.o   \
    S1ap-SecurityContext.o   \
    S1ap-SerialNumber.o  \
    S1ap-SONInformation.o    \
    S1ap-SONInformationRequest.o \
    S1ap-SONInformationReply.o   \
    S1ap-SONConfigurationTransfer.o  \
    S1ap-Source-ToTarget-TransparentContainer.o  \
    S1ap-SourceBSS-ToTargetBSS-TransparentContainer.o    \
    S1ap-SourceeNB-ID.o  \
    S1ap-SRVCCOperationPossible.o    \
    S1ap-SRVCCHOIndication.o \
    S1ap-SourceeNB-ToTargeteNB-TransparentContainer.o    \
    S1ap-SourceRNC-ToTargetRNC-TransparentContainer.o    \
    S1ap-ServedGUMMEIs.o \
    S1ap-ServedGUMMEIsItem.o \
    S1ap-ServedGroupIDs.o    \
    S1ap-ServedMMECs.o   \
    S1ap-ServedPLMNs.o   \
    S1ap-SubscriberProfileIDforRFP.o \
    S1ap-SupportedTAs.o  \
    S1ap-SupportedTAs-Item.o \
    S1ap-S-TMSI.o    \
    S1ap-TAC.o   \
    S1ap-TAIListforWarning.o \
    S1ap-TAIList.o   \
    S1ap-TAIItem.o   \
    S1ap-TAI.o   \
    S1ap-TAI-Broadcast.o \
    S1ap-TAI-Broadcast-Item.o    \
    S1ap-CompletedCellinTAI.o    \
    S1ap-CompletedCellinTAI-Item.o   \
    S1ap-TBCD-STRING.o   \
    S1ap-TargetID.o  \
    S1ap-TargeteNB-ID.o  \
    S1ap-TargetRNC-ID.o  \
    S1ap-TargeteNB-ToSourceeNB-TransparentContainer.o    \
    S1ap-Target-ToSource-TransparentContainer.o  \
    S1ap-TargetRNC-ToSourceRNC-TransparentContainer.o    \
    S1ap-TargetBSS-ToSourceBSS-TransparentContainer.o    \
    S1ap-TimeToWait.o    \
    S1ap-Time-UE-StayedInCell.o  \
    S1ap-TransportLayerAddress.o \
    S1ap-TraceActivation.o   \
    S1ap-TraceDepth.o    \
    S1ap-E-UTRAN-Trace-ID.o  \
    S1ap-TypeOfError.o   \
    S1ap-UEAggregateMaximumBitrate.o \
    S1ap-UE-S1AP-IDs.o   \
    S1ap-UE-S1AP-ID-pair.o   \
    S1ap-UE-associatedLogicalS1-ConnectionListRes.o  \
    S1ap-UE-associatedLogicalS1-ConnectionListResAck.o   \
    S1ap-UE-associatedLogicalS1-ConnectionItem.o \
    S1ap-UEIdentityIndexValue.o  \
    S1ap-UE-HistoryInformation.o \
    S1ap-UEPagingID.o    \
    S1ap-UERadioCapability.o \
    S1ap-UESecurityCapabilities.o    \
    S1ap-WarningAreaList.o   \
    S1ap-WarningType.o   \
    S1ap-WarningSecurityInfo.o   \
    S1ap-WarningMessageContents.o    \
    S1ap-X2TNLConfigurationInfo.o    \
    S1AP-PDU.o  \
    S1ap-InitiatingMessage.o \
    S1ap-SuccessfulOutcome.o \
    S1ap-UnsuccessfulOutcome.o   \
    S1ap-HandoverRequired.o  \
    S1ap-HandoverCommand.o   \
    S1ap-HandoverPreparationFailure.o    \
    S1ap-HandoverRequest.o   \
    S1ap-HandoverRequestAcknowledge.o    \
    S1ap-HandoverFailure.o   \
    S1ap-HandoverNotify.o \
    S1ap-PathSwitchRequest.o \
    S1ap-PathSwitchRequestAcknowledge.o  \
    S1ap-PathSwitchRequestFailure.o  \
    S1ap-E-RABSetupRequest.o \
    S1ap-E-RABSetupResponse.o    \
    S1ap-E-RABModifyRequest.o    \
    S1ap-E-RABModifyResponse.o   \
    S1ap-E-RABReleaseIndication.o    \
    S1ap-E-RABReleaseCommand.o   \
    S1ap-E-RABReleaseResponse.o  \
    S1ap-InitialContextSetupRequest.o    \
    S1ap-InitialContextSetupResponse.o   \
    S1ap-InitialContextSetupFailure.o    \
    S1ap-UEContextReleaseRequest.o   \
    S1ap-Paging.o    \
    S1ap-DownlinkNASTransport.o  \
    S1ap-InitialUEMessage.o  \
    S1ap-UplinkNASTransport.o    \
    S1ap-NASNonDeliveryIndication.o  \
    S1ap-HandoverCancel.o    \
    S1ap-HandoverCancelAcknowledge.o \
    S1ap-Reset.o \
    S1ap-ResetAcknowledge.o  \
    S1ap-S1SetupResponse.o   \
    S1ap-S1SetupRequest.o    \
    S1ap-S1SetupFailure.o    \
    S1ap-ErrorIndication.o   \
    S1ap-ENBConfigurationUpdate.o    \
    S1ap-ENBConfigurationUpdateAcknowledge.o \
    S1ap-ENBConfigurationUpdateFailure.o \
    S1ap-MMEConfigurationUpdate.o    \
    S1ap-MMEConfigurationUpdateAcknowledge.o \
    S1ap-MMEConfigurationUpdateFailure.o \
    S1ap-DownlinkS1cdma2000tunneling.o   \
    S1ap-UplinkS1cdma2000tunneling.o \
    S1ap-UEContextModificationRequest.o  \
    S1ap-UEContextModificationResponse.o \
    S1ap-UEContextModificationFailure.o  \
    S1ap-UECapabilityInfoIndication.o    \
    S1ap-UEContextReleaseCommand.o   \
    S1ap-UEContextReleaseComplete.o  \
    S1ap-ENBStatusTransfer.o \
    S1ap-MMEStatusTransfer.o \
    S1ap-DeactivateTrace.o   \
    S1ap-TraceStart.o    \
    S1ap-TraceFailureIndication.o    \
    S1ap-CellTrafficTrace.o  \
    S1ap-LocationReportingControl.o  \
    S1ap-LocationReportingFailureIndication.o    \
    S1ap-LocationReport.o    \
    S1ap-OverloadStart.o \
    S1ap-OverloadStop.o  \
    S1ap-WriteReplaceWarningRequest.o    \
    S1ap-WriteReplaceWarningResponse.o   \
    S1ap-ENBDirectInformationTransfer.o  \
    S1ap-MMEDirectInformationTransfer.o  \
    S1ap-ENBConfigurationTransfer.o  \
    S1ap-MMEConfigurationTransfer.o  \
    S1ap-PrivateMessage.o    \
    S1ap-IE.o

S1AP_ASN_MODULE_HEADERS= \
    S1ap-Criticality.h   \
    S1ap-Presence.h  \
    S1ap-PrivateIE-ID.h  \
    S1ap-ProcedureCode.h \
    S1ap-ProtocolExtensionID.h   \
    S1ap-ProtocolIE-ID.h \
    S1ap-TriggeringMessage.h \
    S1ap-IE-Extensions.h \
    S1ap-AllocationAndRetentionPriority.h    \
    S1ap-Bearers-SubjectToStatusTransfer-List.h   \
    S1ap-Bearers-SubjectToStatusTransfer-Item.h  \
    S1ap-BitRate.h   \
    S1ap-BPLMNs.h    \
    S1ap-BroadcastCompletedAreaList.h    \
    S1ap-Cause.h \
    S1ap-CauseMisc.h \
    S1ap-CauseProtocol.h \
    S1ap-CauseRadioNetwork.h \
    S1ap-CauseTransport.h    \
    S1ap-CauseNas.h  \
    S1ap-CellIdentity.h  \
    S1ap-CellID-Broadcast.h  \
    S1ap-CellID-Broadcast-Item.h \
    S1ap-Cdma2000PDU.h   \
    S1ap-Cdma2000RATType.h   \
    S1ap-Cdma2000SectorID.h  \
    S1ap-Cdma2000HOStatus.h  \
    S1ap-Cdma2000HORequiredIndication.h  \
    S1ap-Cdma2000OneXSRVCCInfo.h \
    S1ap-Cdma2000OneXMEID.h  \
    S1ap-Cdma2000OneXMSI.h   \
    S1ap-Cdma2000OneXPilot.h \
    S1ap-Cdma2000OneXRAND.h  \
    S1ap-Cell-Size.h \
    S1ap-CellType.h  \
    S1ap-CGI.h   \
    S1ap-CI.h    \
    S1ap-CNDomain.h  \
    S1ap-CSFallbackIndicator.h   \
    S1ap-CSG-Id.h    \
    S1ap-CSG-IdList.h    \
    S1ap-CSG-IdList-Item.h   \
    S1ap-COUNTvalue.h    \
    S1ap-CriticalityDiagnostics.h    \
    S1ap-CriticalityDiagnostics-IE-List.h    \
    S1ap-CriticalityDiagnostics-IE-Item.h    \
    S1ap-DataCodingScheme.h  \
    S1ap-DL-Forwarding.h \
    S1ap-Direct-Forwarding-Path-Availability.h   \
    S1ap-E-RABToBeModifiedListBearerModReq.h \
    S1ap-E-RABToBeModifiedItemBearerModReqIEs.h  \
    S1ap-E-RABToBeModifiedItemBearerModReq.h \
    S1ap-E-RABReleaseListBearerRelComp.h \
    S1ap-E-RABReleaseItemBearerRelCompIEs.h  \
    S1ap-E-RABReleaseItemBearerRelComp.h \
    S1ap-E-RABSubjecttoDataForwardingList.h  \
    S1ap-E-RABDataForwardingList.h   \
    S1ap-E-RABDataForwardingItemIEs.h    \
    S1ap-E-RABDataForwardingItem.h   \
    S1ap-E-RABToBeSetupListCtxtSUReq.h   \
    S1ap-E-RABToBeSetupItemCtxtSUReqIEs.h    \
    S1ap-E-RABToBeSetupItemCtxtSUReq.h   \
    S1ap-E-RABSetupListBearerSURes.h \
    S1ap-E-RABSetupItemBearerSUResIEs.h  \
    S1ap-E-RABSetupItemBearerSURes.h \
    S1ap-E-RABFailedToSetupListHOReqAck.h    \
    S1ap-E-RABFailedToSetupItemHOReqAckIEs.h \
    S1ap-E-RABFailedToSetupItemHOReqAck.h    \
    S1ap-E-RABToBeSetupListBearerSUReq.h \
    S1ap-E-RABToBeSetupItemBearerSUReqIEs.h  \
    S1ap-E-RABToBeSetupItemBearerSUReq.h \
    S1ap-E-RABAdmittedList.h \
    S1ap-E-RABAdmittedItemIEs.h  \
    S1ap-E-RABAdmittedItem.h \
    S1ap-E-RABToBeSwitchedDLList.h   \
    S1ap-E-RABToBeSwitchedDLListIEs.h    \
    S1ap-E-RABToBeSwitchedDLItem.h   \
    S1ap-E-RABToBeSwitchedULList.h   \
    S1ap-E-RABToBeSwitchedULItemIEs.h    \
    S1ap-E-RABToBeSwitchedULItem.h   \
    S1ap-E-RABModifyListBearerModRes.h   \
    S1ap-E-RABModifyItemBearerModResIEs.h    \
    S1ap-E-RABModifyItemBearerModRes.h   \
    S1ap-E-RABSetupListCtxtSURes.h   \
    S1ap-E-RABSetupListCtxtSUResIEs.h    \
    S1ap-E-RABSetupItemCtxtSURes.h   \
    S1ap-E-RABToBeSetupListHOReq.h   \
    S1ap-E-RABToBeSetupListHOReqIEs.h    \
    S1ap-E-RABToBeSetupItemHOReq.h   \
    S1ap-ECGIList.h  \
    S1ap-EmergencyAreaIDList.h   \
    S1ap-EmergencyAreaID.h   \
    S1ap-EmergencyAreaID-Broadcast.h \
    S1ap-EmergencyAreaID-Broadcast-Item.h    \
    S1ap-CompletedCellinEAI.h    \
    S1ap-CompletedCellinEAI-Item.h   \
    S1ap-ENB-ID.h    \
    S1ap-GERAN-Cell-ID.h \
    S1ap-Global-ENB-ID.h \
    S1ap-ENB-StatusTransfer-TransparentContainer.h   \
    S1ap-ENB-UE-S1AP-ID.h    \
    S1ap-ENBname.h   \
    S1ap-ENBX2TLAs.h \
    S1ap-EncryptionAlgorithms.h  \
    S1ap-EPLMNs.h    \
    S1ap-EventType.h \
    S1ap-E-RAB-ID.h  \
    S1ap-E-RABInformationList.h  \
    S1ap-E-RABInformationListItem.h  \
    S1ap-E-RABList.h \
    S1ap-E-RABItem.h \
    S1ap-E-RABLevelQoSParameters.h   \
    S1ap-EUTRAN-CGI.h    \
    S1ap-ExtendedRNC-ID.h    \
    S1ap-ForbiddenInterRATs.h    \
    S1ap-ForbiddenTAs.h  \
    S1ap-ForbiddenTAs-Item.h \
    S1ap-ForbiddenTACs.h \
    S1ap-ForbiddenLAs.h  \
    S1ap-ForbiddenLAs-Item.h \
    S1ap-ForbiddenLACs.h \
    S1ap-GBR-QosInformation.h    \
    S1ap-GTP-TEID.h  \
    S1ap-GUMMEI.h    \
    S1ap-HandoverRestrictionList.h   \
    S1ap-HandoverType.h  \
    S1ap-HFN.h   \
    S1ap-HandoverNotify.h    \
    S1ap-Inter-SystemInformationTransferType.h   \
    S1ap-IMSI.h  \
    S1ap-IntegrityProtectionAlgorithms.h \
    S1ap-InterfacesToTrace.h \
    S1ap-LAC.h   \
    S1ap-LAI.h   \
    S1ap-LastVisitedCell-Item.h  \
    S1ap-LastVisitedEUTRANCellInformation.h  \
    S1ap-LastVisitedUTRANCellInformation.h   \
    S1ap-LastVisitedGERANCellInformation.h   \
    S1ap-L3-Information.h    \
    S1ap-MessageIdentifier.h \
    S1ap-MMEname.h   \
    S1ap-MME-Group-ID.h  \
    S1ap-MME-Code.h  \
    S1ap-MME-UE-S1AP-ID.h    \
    S1ap-M-TMSI.h    \
    S1ap-MSClassmark2.h  \
    S1ap-MSClassmark3.h  \
    S1ap-NAS-PDU.h   \
    S1ap-NASSecurityParametersfromE-UTRAN.h  \
    S1ap-NASSecurityParameterstoE-UTRAN.h    \
    S1ap-NumberofBroadcastRequest.h  \
    S1ap-NumberOfBroadcasts.h \
    S1ap-OldBSS-ToNewBSS-Information.h   \
    S1ap-OverloadAction.h    \
    S1ap-OverloadResponse.h  \
    S1ap-PagingDRX.h \
    S1ap-PDCP-SN.h   \
    S1ap-PLMNidentity.h  \
    S1ap-Pre-emptionCapability.h \
    S1ap-Pre-emptionVulnerability.h  \
    S1ap-PriorityLevel.h \
    S1ap-QCI.h   \
    S1ap-ReceiveStatusofULPDCPSDUs.h \
    S1ap-RelativeMMECapacity.h   \
    S1ap-RAC.h   \
    S1ap-RequestType.h   \
    S1ap-ResetType.h \
    S1ap-ResetAll.h  \
    S1ap-RIMTransfer.h   \
    S1ap-RIMInformation.h    \
    S1ap-RIMRoutingAddress.h \
    S1ap-ReportArea.h    \
    S1ap-RepetitionPeriod.h  \
    S1ap-RNC-ID.h    \
    S1ap-RRC-Container.h \
    S1ap-RRC-Establishment-Cause.h   \
    S1ap-SecurityKey.h   \
    S1ap-SecurityContext.h   \
    S1ap-SerialNumber.h  \
    S1ap-SONInformation.h    \
    S1ap-SONInformationRequest.h \
    S1ap-SONInformationReply.h   \
    S1ap-SONConfigurationTransfer.h  \
    S1ap-Source-ToTarget-TransparentContainer.h  \
    S1ap-SourceBSS-ToTargetBSS-TransparentContainer.h    \
    S1ap-SourceeNB-ID.h  \
    S1ap-SRVCCOperationPossible.h    \
    S1ap-SRVCCHOIndication.h \
    S1ap-SourceeNB-ToTargeteNB-TransparentContainer.h    \
    S1ap-SourceRNC-ToTargetRNC-TransparentContainer.h    \
    S1ap-ServedGUMMEIs.h \
    S1ap-ServedGUMMEIsItem.h \
    S1ap-ServedGroupIDs.h    \
    S1ap-ServedMMECs.h   \
    S1ap-ServedPLMNs.h   \
    S1ap-SubscriberProfileIDforRFP.h \
    S1ap-SupportedTAs.h  \
    S1ap-SupportedTAs-Item.h \
    S1ap-S-TMSI.h    \
    S1ap-TAC.h   \
    S1ap-TAIListforWarning.h \
    S1ap-TAIList.h   \
    S1ap-TAIItem.h   \
    S1ap-TAI.h   \
    S1ap-TAI-Broadcast.h \
    S1ap-TAI-Broadcast-Item.h    \
    S1ap-CompletedCellinTAI.h    \
    S1ap-CompletedCellinTAI-Item.h   \
    S1ap-TBCD-STRING.h   \
    S1ap-TargetID.h  \
    S1ap-TargeteNB-ID.h  \
    S1ap-TargetRNC-ID.h  \
    S1ap-TargeteNB-ToSourceeNB-TransparentContainer.h    \
    S1ap-Target-ToSource-TransparentContainer.h  \
    S1ap-TargetRNC-ToSourceRNC-TransparentContainer.h    \
    S1ap-TargetBSS-ToSourceBSS-TransparentContainer.h    \
    S1ap-TimeToWait.h    \
    S1ap-Time-UE-StayedInCell.h  \
    S1ap-TransportLayerAddress.h \
    S1ap-TraceActivation.h   \
    S1ap-TraceDepth.h    \
    S1ap-E-UTRAN-Trace-ID.h  \
    S1ap-TypeOfError.h   \
    S1ap-UEAggregateMaximumBitrate.h \
    S1ap-UE-S1AP-IDs.h   \
    S1ap-UE-S1AP-ID-pair.h   \
    S1ap-UE-associatedLogicalS1-ConnectionListRes.h  \
    S1ap-UE-associatedLogicalS1-ConnectionListResAck.h   \
    S1ap-UE-associatedLogicalS1-ConnectionItem.h \
    S1ap-UEIdentityIndexValue.h  \
    S1ap-UE-HistoryInformation.h \
    S1ap-UEPagingID.h    \
    S1ap-UERadioCapability.h \
    S1ap-UESecurityCapabilities.h    \
    S1ap-WarningAreaList.h   \
    S1ap-WarningType.h   \
    S1ap-WarningSecurityInfo.h   \
    S1ap-WarningMessageContents.h    \
    S1ap-X2TNLConfigurationInfo.h    \
    S1AP-PDU.h  \
    S1ap-InitiatingMessage.h \
    S1ap-SuccessfulOutcome.h \
    S1ap-UnsuccessfulOutcome.h   \
    S1ap-HandoverRequired.h  \
    S1ap-HandoverCommand.h   \
    S1ap-HandoverPreparationFailure.h    \
    S1ap-HandoverRequest.h   \
    S1ap-HandoverRequestAcknowledge.h    \
    S1ap-HandoverFailure.h   \
    S1ap-HandoverNotify.h \
    S1ap-PathSwitchRequest.h \
    S1ap-PathSwitchRequestAcknowledge.h  \
    S1ap-PathSwitchRequestFailure.h  \
    S1ap-E-RABSetupRequest.h \
    S1ap-E-RABSetupResponse.h    \
    S1ap-E-RABModifyRequest.h    \
    S1ap-E-RABModifyResponse.h   \
    S1ap-E-RABReleaseIndication.h    \
    S1ap-E-RABReleaseCommand.h   \
    S1ap-E-RABReleaseResponse.h  \
    S1ap-InitialContextSetupRequest.h    \
    S1ap-InitialContextSetupResponse.h   \
    S1ap-InitialContextSetupFailure.h    \
    S1ap-UEContextReleaseRequest.h   \
    S1ap-Paging.h    \
    S1ap-DownlinkNASTransport.h  \
    S1ap-InitialUEMessage.h  \
    S1ap-UplinkNASTransport.h    \
    S1ap-NASNonDeliveryIndication.h  \
    S1ap-HandoverCancel.h    \
    S1ap-HandoverCancelAcknowledge.h \
    S1ap-Reset.h \
    S1ap-ResetAcknowledge.h  \
    S1ap-S1SetupResponse.h   \
    S1ap-S1SetupRequest.h    \
    S1ap-S1SetupFailure.h    \
    S1ap-ErrorIndication.h   \
    S1ap-ENBConfigurationUpdate.h    \
    S1ap-ENBConfigurationUpdateAcknowledge.h \
    S1ap-ENBConfigurationUpdateFailure.h \
    S1ap-MMEConfigurationUpdate.h    \
    S1ap-MMEConfigurationUpdateAcknowledge.h \
    S1ap-MMEConfigurationUpdateFailure.h \
    S1ap-DownlinkS1cdma2000tunneling.h   \
    S1ap-UplinkS1cdma2000tunneling.h \
    S1ap-UEContextModificationRequest.h  \
    S1ap-UEContextModificationResponse.h \
    S1ap-UEContextModificationFailure.h  \
    S1ap-UECapabilityInfoIndication.h    \
    S1ap-UEContextReleaseCommand.h   \
    S1ap-UEContextReleaseComplete.h  \
    S1ap-ENBStatusTransfer.h \
    S1ap-MMEStatusTransfer.h \
    S1ap-DeactivateTrace.h   \
    S1ap-TraceStart.h    \
    S1ap-TraceFailureIndication.h    \
    S1ap-CellTrafficTrace.h  \
    S1ap-LocationReportingControl.h  \
    S1ap-LocationReportingFailureIndication.h    \
    S1ap-LocationReport.h    \
    S1ap-OverloadStart.h \
    S1ap-OverloadStop.h  \
    S1ap-WriteReplaceWarningRequest.h    \
    S1ap-WriteReplaceWarningResponse.h   \
    S1ap-ENBDirectInformationTransfer.h  \
    S1ap-MMEDirectInformationTransfer.h  \
    S1ap-ENBConfigurationTransfer.h  \
    S1ap-MMEConfigurationTransfer.h  \
    S1ap-PrivateMessage.h    \
    S1ap-IE.h

S1AP_ASN_MODULE_SOURCES_R9 = \
    S1ap-KillResponse.o  \
    S1ap-KillRequest.o   \
    S1ap-Data-Forwarding-Not-Possible.o  \
    S1ap-DownlinkUEAssociatedLPPaTransport.o \
    S1ap-UplinkUEAssociatedLPPaTransport.o   \
    S1ap-DownlinkNonUEAssociatedLPPaTransport.o  \
    S1ap-UplinkNonUEAssociatedLPPaTransport.o    \
    S1ap-TAI-Cancelled.o \
    S1ap-TAI-Cancelled-Item.o    \
    S1ap-StratumLevel.o  \
    S1ap-SynchronizationStatus.o \
    S1ap-TimeSynchronizationInfo.o   \
    S1ap-Routing-ID.o    \
    S1ap-PS-ServiceNotAvailable.o    \
    S1ap-LPPa-PDU.o  \
    S1ap-ExtendedRepetitionPeriod.o  \
    S1ap-EUTRANRoundTripDelayEstimationInfo.o    \
    S1ap-EmergencyAreaID-Cancelled.o \
    S1ap-EmergencyAreaID-Cancelled-Item.o    \
    S1ap-CSGMembershipStatus.o   \
    S1ap-ConcurrentWarningMessageIndicator.o \
    S1ap-CellID-Cancelled.o  \
    S1ap-CellID-Cancelled-Item.o \
    S1ap-CellAccessMode.o    \
    S1ap-CancelledCellinEAI.o    \
    S1ap-CancelledCellinEAI-Item.o   \
    S1ap-CancelledCellinTAI.o    \
    S1ap-CancelledCellinTAI-Item.o   \
    S1ap-BroadcastCancelledAreaList.o

S1AP_ASN_MODULE_HEADERS_R9 = \
    S1ap-BroadcastCancelledAreaList.h    \
    S1ap-CancelledCellinEAI.h    \
    S1ap-CancelledCellinEAI-Item.h   \
    S1ap-CancelledCellinTAI.h    \
    S1ap-CancelledCellinTAI-Item.h   \
    S1ap-CellAccessMode.h    \
    S1ap-CellID-Cancelled.h  \
    S1ap-CellID-Cancelled-Item.h \
    S1ap-ConcurrentWarningMessageIndicator.h \
    S1ap-CSGMembershipStatus.h   \
    S1ap-Data-Forwarding-Not-Possible.h  \
    S1ap-EmergencyAreaID-Cancelled.h \
    S1ap-EmergencyAreaID-Cancelled-Item.h    \
    S1ap-E-RABList.h \
    S1ap-EUTRANRoundTripDelayEstimationInfo.h    \
    S1ap-ExtendedRepetitionPeriod.h  \
    S1ap-LPPa-PDU.h  \
    S1ap-PS-ServiceNotAvailable.h    \
    S1ap-Routing-ID.h    \
    S1ap-StratumLevel.h  \
    S1ap-SynchronizationStatus.h \
    S1ap-TimeSynchronizationInfo.h   \
    S1ap-TAI-Cancelled.h \
    S1ap-TAI-Cancelled-Item.h    \
    S1ap-KillResponse.h  \
    S1ap-KillRequest.h   \
    S1ap-DownlinkUEAssociatedLPPaTransport.h \
    S1ap-UplinkUEAssociatedLPPaTransport.h   \
    S1ap-DownlinkNonUEAssociatedLPPaTransport.h  \
    S1ap-UplinkNonUEAssociatedLPPaTransport.h

S1AP_ASN_MODULE_SOURCES_R10 = \
    S1ap-PagingPriority.o    \
    S1ap-RelayNode-Indicator.o   \
    S1ap-GWContextReleaseIndication.o    \
    S1ap-MMERelaySupportIndicator.o  \
    S1ap-ManagementBasedMDTAllowed.o \
    S1ap-PrivacyIndicator.o  \
    S1ap-TrafficLoadReductionIndication.o    \
    S1ap-GUMMEIList.o

S1AP_ASN_MODULE_HEADERS_R10 = \
    S1ap-PagingPriority.h    \
    S1ap-RelayNode-Indicator.h   \
    S1ap-GWContextReleaseIndication.h    \
    S1ap-MMERelaySupportIndicator.h  \
    S1ap-ManagementBasedMDTAllowed.h \
    S1ap-PrivacyIndicator.h  \
    S1ap-TrafficLoadReductionIndication.h    \
    S1ap-GUMMEIList.h

S1AP_ASN_MODULE_HEADERS+=ANY.h
S1AP_ASN_MODULE_SOURCES+=ANY.o
S1AP_ASN_MODULE_HEADERS+=BOOLEAN.h
S1AP_ASN_MODULE_SOURCES+=BOOLEAN.o
S1AP_ASN_MODULE_HEADERS+=INTEGER.h
S1AP_ASN_MODULE_HEADERS+=NativeEnumerated.h
S1AP_ASN_MODULE_SOURCES+=INTEGER.o
S1AP_ASN_MODULE_HEADERS+=NULL.h
S1AP_ASN_MODULE_SOURCES+=NULL.o
S1AP_ASN_MODULE_SOURCES+=NativeEnumerated.o
S1AP_ASN_MODULE_HEADERS+=NativeInteger.h
S1AP_ASN_MODULE_SOURCES+=NativeInteger.o
S1AP_ASN_MODULE_HEADERS+=OBJECT_IDENTIFIER.h
S1AP_ASN_MODULE_SOURCES+=OBJECT_IDENTIFIER.o
S1AP_ASN_MODULE_HEADERS+=PrintableString.h
S1AP_ASN_MODULE_SOURCES+=PrintableString.o
S1AP_ASN_MODULE_HEADERS+=asn_SEQUENCE_OF.h
S1AP_ASN_MODULE_SOURCES+=asn_SEQUENCE_OF.o
S1AP_ASN_MODULE_HEADERS+=asn_SET_OF.h
S1AP_ASN_MODULE_SOURCES+=asn_SET_OF.o
S1AP_ASN_MODULE_HEADERS+=constr_CHOICE.h
S1AP_ASN_MODULE_SOURCES+=constr_CHOICE.o
S1AP_ASN_MODULE_HEADERS+=constr_SEQUENCE.h
S1AP_ASN_MODULE_SOURCES+=constr_SEQUENCE.o
S1AP_ASN_MODULE_HEADERS+=constr_SEQUENCE_OF.h
S1AP_ASN_MODULE_SOURCES+=constr_SEQUENCE_OF.o
S1AP_ASN_MODULE_HEADERS+=constr_SET_OF.h
S1AP_ASN_MODULE_SOURCES+=constr_SET_OF.o
S1AP_ASN_MODULE_HEADERS+=asn_application.h
S1AP_ASN_MODULE_HEADERS+=asn_system.h
S1AP_ASN_MODULE_HEADERS+=asn_codecs.h
S1AP_ASN_MODULE_HEADERS+=asn_internal.h
S1AP_ASN_MODULE_HEADERS+=OCTET_STRING.h
S1AP_ASN_MODULE_SOURCES+=OCTET_STRING.o
S1AP_ASN_MODULE_HEADERS+=BIT_STRING.h
S1AP_ASN_MODULE_SOURCES+=BIT_STRING.o
S1AP_ASN_MODULE_SOURCES+=asn_codecs_prim.o
S1AP_ASN_MODULE_HEADERS+=asn_codecs_prim.h
S1AP_ASN_MODULE_HEADERS+=ber_tlv_length.h
S1AP_ASN_MODULE_SOURCES+=ber_tlv_length.o
S1AP_ASN_MODULE_HEADERS+=ber_tlv_tag.h
S1AP_ASN_MODULE_SOURCES+=ber_tlv_tag.o
S1AP_ASN_MODULE_HEADERS+=ber_decoder.h
S1AP_ASN_MODULE_SOURCES+=ber_decoder.o
S1AP_ASN_MODULE_HEADERS+=der_encoder.h
S1AP_ASN_MODULE_SOURCES+=der_encoder.o
S1AP_ASN_MODULE_HEADERS+=constr_TYPE.h
S1AP_ASN_MODULE_SOURCES+=constr_TYPE.o
S1AP_ASN_MODULE_HEADERS+=constraints.h
S1AP_ASN_MODULE_SOURCES+=constraints.o
S1AP_ASN_MODULE_HEADERS+=xer_support.h
S1AP_ASN_MODULE_SOURCES+=xer_support.o
S1AP_ASN_MODULE_HEADERS+=xer_decoder.h
S1AP_ASN_MODULE_SOURCES+=xer_decoder.o
S1AP_ASN_MODULE_HEADERS+=xer_encoder.h
S1AP_ASN_MODULE_SOURCES+=xer_encoder.o
S1AP_ASN_MODULE_HEADERS+=per_support.h
S1AP_ASN_MODULE_SOURCES+=per_support.o
S1AP_ASN_MODULE_HEADERS+=per_decoder.h
S1AP_ASN_MODULE_SOURCES+=per_decoder.o
S1AP_ASN_MODULE_HEADERS+=per_encoder.h
S1AP_ASN_MODULE_SOURCES+=per_encoder.o
S1AP_ASN_MODULE_HEADERS+=per_opentype.h
S1AP_ASN_MODULE_SOURCES+=per_opentype.o