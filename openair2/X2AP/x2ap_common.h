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
#include "ProtocolIE-ID.h"
#include "TriggeringMessage.h"
#include "IE-Extensions.h"
#include "ActivatedCellList.h"
#include "AS-SecurityInformation.h"
#include "AllocationAndRetentionPriority.h"
#include "BitRate.h"
#include "BroadcastPLMNs-Item.h"
#include "CapacityValue.h"
#include "CellCapacityClassValue.h"
#include "Cause.h"
#include "CauseMisc.h"
#include "CauseProtocol.h"
#include "CauseRadioNetwork.h"
#include "CauseTransport.h"
#include "Cell-Size.h"
#include "CellToReport-List.h"
#include "CellToReport-Item.h"
#include "CellMeasurementResult-List.h"
#include "CellMeasurementResult-Item.h"
//#include "CellInformation.h"
#include "CellInformation-List.h"
#include "CellInformation-Item.h"
#include "CellType.h"
#include "CompositeAvailableCapacityGroup.h"
#include "CompositeAvailableCapacity.h"
#include "COUNTvalue.h"
#include "CriticalityDiagnostics.h"
#include "CriticalityDiagnostics-IE-List.h"
#include "CRNTI.h"
#include "CyclicPrefixDL.h"
#include "CyclicPrefixUL.h"
#include "DeactivationIndication.h"
#include "DL-Forwarding.h"
#include "E-RAB-Item.h"
#include "E-RABs-Admitted-List.h"
#include "E-RABs-Admitted-Item.h"
//#include "E-RABNotAdmittedItem.h"
#include "E-RABs-ToBeSetup-List.h"
#include "E-RABs-ToBeSetup-Item.h"
#include "E-RABs-SubjectToStatusTransfer-List.h"
#include "E-RABs-SubjectToStatusTransfer-Item.h"
#include "EARFCN.h"
#include "FDD-Info.h"
#include "TDD-Info.h"
#include "EUTRA-Mode-Info.h"
#include "ECGI.h"
#include "ENB-ID.h"
#include "EncryptionAlgorithms.h"
#include "EPLMNs.h"
#include "E-RAB-ID.h"
#include "E-RAB-Level-QoS-Parameters.h"
#include "E-RAB-List.h"
//#include "E-RAB-ItemIEs.h"
#include "EUTRANCellIdentifier.h"
#include "EUTRANTraceID.h"
#include "EventType.h"
#include "ForbiddenInterRATs.h"
#include "ForbiddenTAs.h"
#include "ForbiddenTAs-Item.h"
#include "ForbiddenTACs.h"
#include "ForbiddenLAs.h"
#include "ForbiddenLAs-Item.h"
#include "ForbiddenLACs.h"
#include "Fourframes.h"
#include "GBR-QosInformation.h"
#include "GlobalENB-ID.h"
#include "GTPtunnelEndpoint.h"
#include "GTP-TEI.h"
#include "GUGroupIDList.h"
#include "GU-Group-ID.h"
#include "GUMMEI.h"
#include "HandoverReportType.h"
#include "HandoverRestrictionList.h"
#include "HFN.h"
#include "HWLoadIndicator.h"
#include "IntegrityProtectionAlgorithms.h"
#include "InterfacesToTrace.h"
#include "Key-eNodeB-Star.h"
#include "LAC.h"
#include "LastVisitedCell-Item.h"
#include "LastVisitedEUTRANCellInformation.h"
#include "LastVisitedUTRANCellInformation.h"
#include "LastVisitedGERANCellInformation.h"
#include "LoadIndicator.h"
#include "LocationReportingInformation.h"
#include "MME-Group-ID.h"
#include "MME-Code.h"
#include "Measurement-ID.h"
#include "MBSFN-Subframe-Infolist.h"
#include "MBSFN-Subframe-Info.h"
#include "MobilityParametersModificationRange.h"
#include "MobilityParametersInformation.h"
#include "Neighbour-Information.h"
#include "Neighbour-Information.h"
#include "NextHopChainingCount.h"
#include "Number-of-Antennaports.h"
#include "Oneframe.h"
#include "PDCP-SN.h"
#include "PCI.h"
#include "PLMN-Identity.h"
#include "PRACH-Configuration.h"
#include "Pre-emptionCapability.h"
#include "Pre-emptionVulnerability.h"
#include "PriorityLevel.h"
#include "QCI.h"
#include "ReceiveStatusofULPDCPSDUs.h"
#include "Registration-Request.h"
#include "RelativeNarrowbandTxPower.h"
#include "ReportArea.h"
#include "ReportingPeriodicity.h"
#include "ReportCharacteristics.h"
#include "RNTP-Threshold.h"
#include "RRC-Context.h"
#include "RadioResourceStatus.h"
#include "DL-GBR-PRB-usage.h"
#include "UL-GBR-PRB-usage.h"
#include "DL-non-GBR-PRB-usage.h"
#include "UL-non-GBR-PRB-usage.h"
#include "DL-Total-PRB-usage.h"
#include "UL-Total-PRB-usage.h"
#include "RadioframeAllocationPeriod.h"
#include "RadioframeAllocationOffset.h"
#include "S1TNLLoadIndicator.h"
#include "ServedCellsToActivate.h"
#include "ServedCellsToActivate-Item.h"
#include "ServedCellsToModify.h"
#include "ServedCellsToModify-Item.h"
#include "Old-ECGIs.h"
#include "ServedCells.h"
#include "ServedCell-Information.h"
#include "ShortMAC-I.h"
#include "SRVCCOperationPossible.h"
#include "SubframeAssignment.h"
#include "SpecialSubframe-Info.h"
#include "SpecialSubframePatterns.h"
#include "SubscriberProfileIDforRFP.h"
#include "SubframeAllocation.h"
#include "TAC.h"
#include "TargeteNBtoSource-eNBTransparentContainer.h"
#include "TimeToWait.h"
#include "Time-UE-StayedInCell.h"
#include "TraceActivation.h"
#include "TraceCollectionEntityIPAddress.h"
#include "TraceDepth.h"
#include "Transmission-Bandwidth.h"
#include "TransportLayerAddress.h"
#include "TypeOfError.h"
#include "UE-HistoryInformation.h"
#include "UE-S1AP-ID.h"
#include "UE-X2AP-ID.h"
#include "UEAggregateMaximumBitRate.h"
#include "UESecurityCapabilities.h"
#include "UE-ContextInformation.h"
#include "UL-InterferenceOverloadIndication.h"
#include "UL-InterferenceOverloadIndication-Item.h"
#include "UL-HighInterferenceIndicationInfo.h"
#include "UL-HighInterferenceIndicationInfo-Item.h"
#include "UL-HighInterferenceIndication.h"
#include "UE-RLF-Report-Container.h"
#include "IE.h"
#include "X2AP-PDU.h"
#include "InitiatingMessage.h"
#include "SuccessfulOutcome.h"
#include "UnsuccessfulOutcome.h"
#include "HandoverRequest.h"
#include "HandoverRequestAcknowledge.h"
//#include "HandoverPreparation.h"
#include "HandoverPreparationFailure.h"
#include "SNStatusTransfer.h"
#include "UEContextRelease.h"
#include "HandoverCancel.h"
#include "HandoverReport.h"
#include "ErrorIndication.h"
#include "ResetRequest.h"
#include "ResetResponse.h"
#include "X2SetupRequest.h"
#include "X2SetupResponse.h"
#include "X2SetupFailure.h"
#include "LoadInformation.h"
#include "ENBConfigurationUpdate.h"
#include "ENBConfigurationUpdateAcknowledge.h"
#include "ENBConfigurationUpdateFailure.h"
#include "ResourceStatusRequest.h"
#include "ResourceStatusResponse.h"
#include "ResourceStatusFailure.h"
#include "ResourceStatusUpdate.h"
#include "RLFIndication.h"
#include "PrivateMessage.h"
#include "MobilityChangeRequest.h"
#include "MobilityChangeAcknowledge.h"
#include "MobilityChangeFailure.h"
#include "CellActivationRequest.h"
#include "CellActivationResponse.h"
#include "CellActivationFailure.h"

#ifndef X2AP_COMMON_H_
#define X2AP_COMMON_H_

/** @defgroup _x2ap_impl_ X2AP Layer Reference Implementation
 * @ingroup _ref_implementation_
 * @{
 */

/* Checking version of ASN1C compiler */
#if (ASN1C_ENVIRONMENT_VERSION < 923)
# error "You are compiling x2ap with the wrong version of ASN1C"
#endif

#ifndef X2AP_PORT
# define X2AP_PORT 36422
#endif

#define TRUE 1
#define FALSE 0

extern int asn_debug;
extern int asn1_xer_print;

#if defined(ENB_MODE)
# include "log.h"
# define X2AP_ERROR(x, args...) LOG_E(X2AP, x, ##args)
# define X2AP_WARN(x, args...)  LOG_W(X2AP, x, ##args)
# define X2AP_DEBUG(x, args...) LOG_D(X2AP, x, ##args)
#else
# define X2AP_ERROR(x, args...) do { fprintf(stdout, "[X2AP][E]"x, ##args); } while(0)
# define X2AP_WARN(x, args...)  do { fprintf(stdout, "[X2AP][W]"x, ##args); } while(0)
# define X2AP_DEBUG(x, args...) do { fprintf(stdout, "[X2AP][D]"x, ##args); } while(0)
#endif

//Forward declaration
struct x2ap_message_s;

/** \brief Function callback prototype.
 **/
typedef int (*x2ap_message_decoded_callback)(
    uint32_t assocId,
    uint32_t stream,
    struct x2ap_message_s *message);

/** \brief Encode a successfull outcome message
 \param buffer pointer to buffer in which data will be encoded
 \param length pointer to the length of buffer
 \param procedureCode Procedure code for the message
 \param criticality Criticality of the message
 \param td ASN1C type descriptor of the sptr
 \param sptr Deferenced pointer to the structure to encode
 @returns size in bytes encded on success or 0 on failure
 **/
ssize_t x2ap_generate_successfull_outcome(
    uint8_t               **buffer,
    uint32_t               *length,
    e_X2ap_ProcedureCode         procedureCode,
    X2ap_Criticality_t           criticality,
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
ssize_t x2ap_generate_initiating_message(
    uint8_t               **buffer,
    uint32_t               *length,
    e_X2ap_ProcedureCode    procedureCode,
    X2ap_Criticality_t      criticality,
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
ssize_t x2ap_generate_unsuccessfull_outcome(
    uint8_t               **buffer,
    uint32_t               *length,
    e_X2ap_ProcedureCode         procedureCode,
    X2ap_Criticality_t           criticality,
    asn_TYPE_descriptor_t  *td,
    void                   *sptr);

/** \brief Generate a new IE
 \param id Protocol ie id of the IE
 \param criticality Criticality of the IE
 \param type ASN1 type descriptor of the IE value
 \param sptr Structure to be encoded in the value field
 @returns a pointer to the newly created IE structure or NULL in case of failure
 **/
X2ap_IE_t *x2ap_new_ie(
    X2ap_ProtocolIE_ID_t   id,
    X2ap_Criticality_t     criticality,
    asn_TYPE_descriptor_t *type,
    void                  *sptr);

/** \brief Handle criticality
 \param criticality Criticality of the IE
 @returns void
 **/
void x2ap_handle_criticality(X2ap_Criticality_t criticality);

#endif /* X2AP_COMMON_H_ */
