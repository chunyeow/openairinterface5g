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
libnas_api_SRCS =								\
    EURECOM-NAS/src/api/mme/mme_api.c			\
    EURECOM-NAS/src/api/mme/mme_api.h			\
    EURECOM-NAS/src/api/network/nas_message.c   \
    EURECOM-NAS/src/api/network/nas_message.h

libnas_emm_SRCS =								\
    EURECOM-NAS/src/emm/Attach.c				\
    EURECOM-NAS/src/emm/Authentication.c		\
    EURECOM-NAS/src/emm/Detach.c				\
    EURECOM-NAS/src/emm/EmmCommon.c				\
    EURECOM-NAS/src/emm/EmmCommon.h				\
    EURECOM-NAS/src/emm/emmData.h				\
    EURECOM-NAS/src/emm/emm_data_ctx.c			\
    EURECOM-NAS/src/emm/emm_main.c				\
    EURECOM-NAS/src/emm/emm_main.h				\
    EURECOM-NAS/src/emm/emm_proc.h				\
    EURECOM-NAS/src/emm/EmmStatusHdl.c			\
    EURECOM-NAS/src/emm/Identification.c		\
    EURECOM-NAS/src/emm/IdleMode.c				\
    EURECOM-NAS/src/emm/IdleMode.h				\
    EURECOM-NAS/src/emm/LowerLayer.c			\
    EURECOM-NAS/src/emm/LowerLayer.h			\
    EURECOM-NAS/src/emm/SecurityModeControl.c	\
    EURECOM-NAS/src/emm/ServiceRequestHdl.c		\
    EURECOM-NAS/src/emm/TrackingAreaUpdate.c

libnas_emm_msg_SRCS =										\
    EURECOM-NAS/src/emm/msg/AttachAccept.c					\
    EURECOM-NAS/src/emm/msg/AuthenticationFailure.c			\
    EURECOM-NAS/src/emm/msg/CsServiceNotification.c			\
    EURECOM-NAS/src/emm/msg/emm_cause.h						\
    EURECOM-NAS/src/emm/msg/ExtendedServiceRequest.c		\
    EURECOM-NAS/src/emm/msg/IdentityResponse.c				\
    EURECOM-NAS/src/emm/msg/SecurityModeReject.h			\
    EURECOM-NAS/src/emm/msg/TrackingAreaUpdateComplete.h	\
    EURECOM-NAS/src/emm/msg/AttachAccept.h					\
    EURECOM-NAS/src/emm/msg/AuthenticationFailure.h			\
    EURECOM-NAS/src/emm/msg/CsServiceNotification.h			\
    EURECOM-NAS/src/emm/msg/EmmInformation.c				\
    EURECOM-NAS/src/emm/msg/ExtendedServiceRequest.h		\
    EURECOM-NAS/src/emm/msg/IdentityResponse.h				\
    EURECOM-NAS/src/emm/msg/ServiceReject.c					\
    EURECOM-NAS/src/emm/msg/TrackingAreaUpdateReject.c		\
    EURECOM-NAS/src/emm/msg/AttachComplete.c				\
    EURECOM-NAS/src/emm/msg/AuthenticationReject.c			\
    EURECOM-NAS/src/emm/msg/DetachAccept.c					\
    EURECOM-NAS/src/emm/msg/EmmInformation.h				\
    EURECOM-NAS/src/emm/msg/GutiReallocationCommand.c		\
    EURECOM-NAS/src/emm/msg/ServiceReject.h					\
    EURECOM-NAS/src/emm/msg/TrackingAreaUpdateReject.h		\
    EURECOM-NAS/src/emm/msg/AttachComplete.h				\
    EURECOM-NAS/src/emm/msg/AuthenticationReject.h			\
    EURECOM-NAS/src/emm/msg/DetachAccept.h					\
    EURECOM-NAS/src/emm/msg/emm_msg.c						\
    EURECOM-NAS/src/emm/msg/GutiReallocationCommand.h		\
    EURECOM-NAS/src/emm/msg/SecurityModeCommand.c			\
    EURECOM-NAS/src/emm/msg/ServiceRequest.c				\
    EURECOM-NAS/src/emm/msg/TrackingAreaUpdateRequest.c		\
    EURECOM-NAS/src/emm/msg/AttachReject.c					\
    EURECOM-NAS/src/emm/msg/AuthenticationRequest.c			\
    EURECOM-NAS/src/emm/msg/DetachRequest.c					\
    EURECOM-NAS/src/emm/msg/emm_msgDef.h					\
    EURECOM-NAS/src/emm/msg/GutiReallocationComplete.c		\
    EURECOM-NAS/src/emm/msg/NASSecurityModeCommand.h		\
    EURECOM-NAS/src/emm/msg/ServiceRequest.h				\
    EURECOM-NAS/src/emm/msg/TrackingAreaUpdateRequest.h		\
    EURECOM-NAS/src/emm/msg/AttachReject.h					\
    EURECOM-NAS/src/emm/msg/AuthenticationRequest.h			\
    EURECOM-NAS/src/emm/msg/DetachRequest.h					\
    EURECOM-NAS/src/emm/msg/emm_msg.h						\
    EURECOM-NAS/src/emm/msg/GutiReallocationComplete.h		\
    EURECOM-NAS/src/emm/msg/SecurityModeComplete.c			\
    EURECOM-NAS/src/emm/msg/TrackingAreaUpdateAccept.c		\
    EURECOM-NAS/src/emm/msg/UplinkNasTransport.c			\
    EURECOM-NAS/src/emm/msg/AttachRequest.c					\
    EURECOM-NAS/src/emm/msg/AuthenticationResponse.c		\
    EURECOM-NAS/src/emm/msg/DownlinkNasTransport.c			\
    EURECOM-NAS/src/emm/msg/EmmStatus.c						\
    EURECOM-NAS/src/emm/msg/IdentityRequest.c				\
    EURECOM-NAS/src/emm/msg/NASSecurityModeComplete.h			\
    EURECOM-NAS/src/emm/msg/TrackingAreaUpdateAccept.h		\
    EURECOM-NAS/src/emm/msg/UplinkNasTransport.h			\
    EURECOM-NAS/src/emm/msg/AttachRequest.h					\
    EURECOM-NAS/src/emm/msg/AuthenticationResponse.h		\
    EURECOM-NAS/src/emm/msg/DownlinkNasTransport.h			\
    EURECOM-NAS/src/emm/msg/EmmStatus.h						\
    EURECOM-NAS/src/emm/msg/IdentityRequest.h				\
    EURECOM-NAS/src/emm/msg/SecurityModeReject.c			\
    EURECOM-NAS/src/emm/msg/TrackingAreaUpdateComplete.c

libnas_emm_sap_SRCS =											\
    EURECOM-NAS/src/emm/sap/emm_as.c							\
    EURECOM-NAS/src/emm/sap/emm_cn.c							\
    EURECOM-NAS/src/emm/sap/emm_cn.h							\
    EURECOM-NAS/src/emm/sap/EmmDeregisteredNoImsi.c				\
    EURECOM-NAS/src/emm/sap/emm_reg.c							\
    EURECOM-NAS/src/emm/sap/EmmRegisteredUpdateNeeded.c			\
    EURECOM-NAS/src/emm/sap/emm_asDef.h							\
    EURECOM-NAS/src/emm/sap/EmmDeregisteredNormalService.c		\
    EURECOM-NAS/src/emm/sap/emm_regDef.h						\
    EURECOM-NAS/src/emm/sap/emm_sap.c							\
    EURECOM-NAS/src/emm/sap/EmmDeregisteredPlmnSearch.c			\
    EURECOM-NAS/src/emm/sap/emm_reg.h							\
    EURECOM-NAS/src/emm/sap/emm_sap.h							\
    EURECOM-NAS/src/emm/sap/emm_as.h							\
    EURECOM-NAS/src/emm/sap/emm_esm.c							\
    EURECOM-NAS/src/emm/sap/EmmRegisteredAttemptingToUpdate.c	\
    EURECOM-NAS/src/emm/sap/emm_send.c							\
    EURECOM-NAS/src/emm/sap/EmmCommonProcedureInitiated.c		\
    EURECOM-NAS/src/emm/sap/emm_esmDef.h						\
    EURECOM-NAS/src/emm/sap/EmmRegistered.c						\
    EURECOM-NAS/src/emm/sap/emm_send.h							\
    EURECOM-NAS/src/emm/sap/EmmDeregisteredAttachNeeded.c		\
    EURECOM-NAS/src/emm/sap/emm_esm.h							\
    EURECOM-NAS/src/emm/sap/EmmRegisteredImsiDetachInitiated.c	\
    EURECOM-NAS/src/emm/sap/EmmServiceRequestInitiated.c		\
    EURECOM-NAS/src/emm/sap/EmmDeregisteredAttemptingToAttach.c	\
    EURECOM-NAS/src/emm/sap/emm_fsm.c							\
    EURECOM-NAS/src/emm/sap/EmmRegisteredInitiated.c			\
    EURECOM-NAS/src/emm/sap/EmmTrackingAreaUpdatingInitiated.c	\
    EURECOM-NAS/src/emm/sap/EmmDeregistered.c					\
    EURECOM-NAS/src/emm/sap/emm_fsm.h							\
    EURECOM-NAS/src/emm/sap/EmmRegisteredLimitedService.c		\
    EURECOM-NAS/src/emm/sap/EmmDeregisteredInitiated.c			\
    EURECOM-NAS/src/emm/sap/EmmNull.c							\
    EURECOM-NAS/src/emm/sap/EmmRegisteredNoCellAvailable.c		\
    EURECOM-NAS/src/emm/sap/EmmDeregisteredLimitedService.c		\
    EURECOM-NAS/src/emm/sap/emm_recv.c							\
    EURECOM-NAS/src/emm/sap/EmmRegisteredNormalService.c		\
    EURECOM-NAS/src/emm/sap/EmmDeregisteredNoCellAvailable.c	\
    EURECOM-NAS/src/emm/sap/emm_recv.h							\
    EURECOM-NAS/src/emm/sap/EmmRegisteredPlmnSearch.c

libnas_esm_SRCS =												\
    EURECOM-NAS/src/esm/esm_ebr_context.c						\
    EURECOM-NAS/src/esm/PdnConnectivity.c						\
    EURECOM-NAS/src/esm/sap/esm_recv.c							\
    EURECOM-NAS/src/esm/sap/esm_send.c							\
    EURECOM-NAS/src/esm/sap/esm_sap.c							\
    EURECOM-NAS/src/esm/esm_ebr.c								\
    EURECOM-NAS/src/esm/EpsBearerContextDeactivation.c			\
    EURECOM-NAS/src/esm/DefaultEpsBearerContextActivation.c		\
    EURECOM-NAS/src/esm/DedicatedEpsBearerContextActivation.c	\
    EURECOM-NAS/src/esm/PdnDisconnect.c							\
    EURECOM-NAS/src/esm/EsmStatusHdl.c							\
    EURECOM-NAS/src/esm/esm_main.c								\
    EURECOM-NAS/src/esm/esm_pt.c								\
    EURECOM-NAS/src/esm/esm_proc.h								\
    EURECOM-NAS/src/esm/esm_main.h								\
    EURECOM-NAS/src/esm/sap/esm_recv.h							\
    EURECOM-NAS/src/esm/sap/esm_sap.h							\
    EURECOM-NAS/src/esm/sap/esm_send.h							\
    EURECOM-NAS/src/esm/sap/esm_sapDef.h						\
    EURECOM-NAS/src/esm/esm_ebr_context.h						\
    EURECOM-NAS/src/esm/esmData.h								\
    EURECOM-NAS/src/esm/esm_pt.h								\
    EURECOM-NAS/src/esm/esm_ebr.h								\
    EURECOM-NAS/src/esm/esm_ip.c

libnas_esm_msg_SRCS =													\
    EURECOM-NAS/src/esm/msg/EsmInformationRequest.h						\
    EURECOM-NAS/src/esm/msg/EsmStatus.h									\
    EURECOM-NAS/src/esm/msg/DeactivateEpsBearerContextAccept.h			\
    EURECOM-NAS/src/esm/msg/BearerResourceAllocationReject.h			\
    EURECOM-NAS/src/esm/msg/ActivateDefaultEpsBearerContextReject.h		\
    EURECOM-NAS/src/esm/msg/BearerResourceModificationReject.h			\
    EURECOM-NAS/src/esm/msg/PdnConnectivityReject.h						\
    EURECOM-NAS/src/esm/msg/PdnDisconnectRequest.h						\
    EURECOM-NAS/src/esm/msg/DeactivateEpsBearerContextRequest.h			\
    EURECOM-NAS/src/esm/msg/PdnConnectivityRequest.h					\
    EURECOM-NAS/src/esm/msg/BearerResourceModificationRequest.h			\
    EURECOM-NAS/src/esm/msg/PdnDisconnectReject.h						\
    EURECOM-NAS/src/esm/msg/ActivateDedicatedEpsBearerContextRequest.h	\
    EURECOM-NAS/src/esm/msg/ActivateDefaultEpsBearerContextAccept.h		\
    EURECOM-NAS/src/esm/msg/ActivateDedicatedEpsBearerContextReject.h	\
    EURECOM-NAS/src/esm/msg/esm_msg.h									\
    EURECOM-NAS/src/esm/msg/ModifyEpsBearerContextAccept.h				\
    EURECOM-NAS/src/esm/msg/ModifyEpsBearerContextReject.h				\
    EURECOM-NAS/src/esm/msg/EsmInformationResponse.h					\
    EURECOM-NAS/src/esm/msg/ModifyEpsBearerContextRequest.h				\
    EURECOM-NAS/src/esm/msg/ActivateDedicatedEpsBearerContextAccept.h	\
    EURECOM-NAS/src/esm/msg/esm_cause.h									\
    EURECOM-NAS/src/esm/msg/BearerResourceAllocationRequest.h			\
    EURECOM-NAS/src/esm/msg/ActivateDefaultEpsBearerContextRequest.h	\
    EURECOM-NAS/src/esm/msg/esm_msgDef.h								\
    EURECOM-NAS/src/esm/msg/ModifyEpsBearerContextRequest.c				\
    EURECOM-NAS/src/esm/msg/DeactivateEpsBearerContextRequest.c			\
    EURECOM-NAS/src/esm/msg/BearerResourceAllocationReject.c			\
    EURECOM-NAS/src/esm/msg/BearerResourceModificationRequest.c			\
    EURECOM-NAS/src/esm/msg/ActivateDedicatedEpsBearerContextRequest.c	\
    EURECOM-NAS/src/esm/msg/EsmInformationResponse.c					\
    EURECOM-NAS/src/esm/msg/PdnDisconnectReject.c						\
    EURECOM-NAS/src/esm/msg/ActivateDedicatedEpsBearerContextAccept.c	\
    EURECOM-NAS/src/esm/msg/PdnDisconnectRequest.c						\
    EURECOM-NAS/src/esm/msg/BearerResourceModificationReject.c			\
    EURECOM-NAS/src/esm/msg/ActivateDefaultEpsBearerContextAccept.c		\
    EURECOM-NAS/src/esm/msg/ActivateDedicatedEpsBearerContextReject.c	\
    EURECOM-NAS/src/esm/msg/ModifyEpsBearerContextAccept.c				\
    EURECOM-NAS/src/esm/msg/DeactivateEpsBearerContextAccept.c			\
    EURECOM-NAS/src/esm/msg/esm_msg.c									\
    EURECOM-NAS/src/esm/msg/EsmInformationRequest.c						\
    EURECOM-NAS/src/esm/msg/ActivateDefaultEpsBearerContextRequest.c	\
    EURECOM-NAS/src/esm/msg/ModifyEpsBearerContextReject.c				\
    EURECOM-NAS/src/esm/msg/EsmStatus.c									\
    EURECOM-NAS/src/esm/msg/PdnConnectivityReject.c						\
    EURECOM-NAS/src/esm/msg/ActivateDefaultEpsBearerContextReject.c		\
    EURECOM-NAS/src/esm/msg/PdnConnectivityRequest.c					\
    EURECOM-NAS/src/esm/msg/BearerResourceAllocationRequest.c

libnas_esm_sap_SRCS =							\
    EURECOM-NAS/src/esm/sap/esm_recv.c			\
    EURECOM-NAS/src/esm/sap/esm_send.c			\
    EURECOM-NAS/src/esm/sap/esm_sap.c

libnas_ies_SRCS =													\
    EURECOM-NAS/src/ies/UeSecurityCapability.c						\
    EURECOM-NAS/src/ies/SecurityHeaderType.c						\
    EURECOM-NAS/src/ies/RadioPriority.c								\
    EURECOM-NAS/src/ies/EpsAttachResult.c							\
    EURECOM-NAS/src/ies/ImeisvRequest.c								\
    EURECOM-NAS/src/ies/EmergencyNumberList.c						\
    EURECOM-NAS/src/ies/SupportedCodecList.c						\
    EURECOM-NAS/src/ies/EsmCause.c									\
    EURECOM-NAS/src/ies/NetworkName.c								\
    EURECOM-NAS/src/ies/AuthenticationResponseParameter.c			\
    EURECOM-NAS/src/ies/Cli.c										\
    EURECOM-NAS/src/ies/AccessPointName.c							\
    EURECOM-NAS/src/ies/DaylightSavingTime.c						\
    EURECOM-NAS/src/ies/LcsIndicator.c								\
    EURECOM-NAS/src/ies/PagingIdentity.c							\
    EURECOM-NAS/src/ies/EpsMobileIdentity.c							\
    EURECOM-NAS/src/ies/GutiType.c									\
    EURECOM-NAS/src/ies/NasKeySetIdentifier.c						\
    EURECOM-NAS/src/ies/CsfbResponse.c								\
    EURECOM-NAS/src/ies/PacketFlowIdentifier.c						\
    EURECOM-NAS/src/ies/ApnAggregateMaximumBitRate.c				\
    EURECOM-NAS/src/ies/EpsBearerContextStatus.c					\
    EURECOM-NAS/src/ies/MobileIdentity.c							\
    EURECOM-NAS/src/ies/AuthenticationFailureParameter.c			\
    EURECOM-NAS/src/ies/EpsUpdateResult.c							\
    EURECOM-NAS/src/ies/TimeZone.c									\
    EURECOM-NAS/src/ies/MessageType.c								\
    EURECOM-NAS/src/ies/EmmCause.c									\
    EURECOM-NAS/src/ies/MobileStationClassmark2.c					\
    EURECOM-NAS/src/ies/AuthenticationParameterRand.c				\
    EURECOM-NAS/src/ies/PdnAddress.c								\
    EURECOM-NAS/src/ies/MobileStationClassmark3.c					\
    EURECOM-NAS/src/ies/ServiceType.c								\
    EURECOM-NAS/src/ies/NasRequestType.c							\
    EURECOM-NAS/src/ies/PlmnList.c									\
    EURECOM-NAS/src/ies/EpsQualityOfService.c						\
    EURECOM-NAS/src/ies/NasMessageContainer.c						\
    EURECOM-NAS/src/ies/EsmInformationTransferFlag.c				\
    EURECOM-NAS/src/ies/TrafficFlowTemplate.c						\
    EURECOM-NAS/src/ies/LinkedEpsBearerIdentity.c					\
    EURECOM-NAS/src/ies/NasSecurityAlgorithms.c						\
    EURECOM-NAS/src/ies/LlcServiceAccessPointIdentifier.c			\
    EURECOM-NAS/src/ies/ShortMac.c									\
    EURECOM-NAS/src/ies/TrafficFlowAggregateDescription.c			\
    EURECOM-NAS/src/ies/PTmsiSignature.c							\
    EURECOM-NAS/src/ies/ProtocolDiscriminator.c						\
    EURECOM-NAS/src/ies/EpsBearerIdentity.c							\
    EURECOM-NAS/src/ies/TimeZoneAndTime.c							\
    EURECOM-NAS/src/ies/TrackingAreaIdentityList.c					\
    EURECOM-NAS/src/ies/QualityOfService.c							\
    EURECOM-NAS/src/ies/EpsUpdateType.c								\
    EURECOM-NAS/src/ies/PdnType.c									\
    EURECOM-NAS/src/ies/UeRadioCapabilityInformationUpdateNeeded.c	\
    EURECOM-NAS/src/ies/AuthenticationParameterAutn.c				\
    EURECOM-NAS/src/ies/EpsAttachType.c								\
    EURECOM-NAS/src/ies/IdentityType2.c								\
    EURECOM-NAS/src/ies/LcsClientIdentity.c							\
    EURECOM-NAS/src/ies/DetachType.c								\
    EURECOM-NAS/src/ies/LocationAreaIdentification.c				\
    EURECOM-NAS/src/ies/TrackingAreaIdentity.c						\
    EURECOM-NAS/src/ies/CipheringKeySequenceNumber.c				\
    EURECOM-NAS/src/ies/MsNetworkCapability.c						\
    EURECOM-NAS/src/ies/UeNetworkCapability.c						\
    EURECOM-NAS/src/ies/ProtocolConfigurationOptions.c				\
    EURECOM-NAS/src/ies/AdditionalUpdateType.c						\
    EURECOM-NAS/src/ies/TmsiStatus.c								\
    EURECOM-NAS/src/ies/EsmMessageContainer.c						\
    EURECOM-NAS/src/ies/Nonce.c										\
    EURECOM-NAS/src/ies/ProcedureTransactionIdentity.c				\
    EURECOM-NAS/src/ies/TransactionIdentifier.c						\
    EURECOM-NAS/src/ies/AdditionalUpdateResult.c					\
    EURECOM-NAS/src/ies/GprsTimer.c									\
    EURECOM-NAS/src/ies/EpsNetworkFeatureSupport.c					\
    EURECOM-NAS/src/ies/SsCode.c									\
    EURECOM-NAS/src/ies/DrxParameter.c								\
    EURECOM-NAS/src/ies/KsiAndSequenceNumber.c						\
    EURECOM-NAS/src/ies/EpsBearerContextStatus.h					\
    EURECOM-NAS/src/ies/MobileStationClassmark2.h					\
    EURECOM-NAS/src/ies/LcsClientIdentity.h							\
    EURECOM-NAS/src/ies/ServiceType.h								\
    EURECOM-NAS/src/ies/Cli.h										\
    EURECOM-NAS/src/ies/GprsTimer.h									\
    EURECOM-NAS/src/ies/AuthenticationParameterAutn.h				\
    EURECOM-NAS/src/ies/EmmCause.h									\
    EURECOM-NAS/src/ies/PdnAddress.h								\
    EURECOM-NAS/src/ies/QualityOfService.h							\
    EURECOM-NAS/src/ies/LocationAreaIdentification.h				\
    EURECOM-NAS/src/ies/EmergencyNumberList.h						\
    EURECOM-NAS/src/ies/UeRadioCapabilityInformationUpdateNeeded.h	\
    EURECOM-NAS/src/ies/SupportedCodecList.h						\
    EURECOM-NAS/src/ies/AuthenticationFailureParameter.h			\
    EURECOM-NAS/src/ies/MobileIdentity.h							\
    EURECOM-NAS/src/ies/UeNetworkCapability.h						\
    EURECOM-NAS/src/ies/ShortMac.h									\
    EURECOM-NAS/src/ies/MessageType.h								\
    EURECOM-NAS/src/ies/AdditionalUpdateType.h						\
    EURECOM-NAS/src/ies/NasMessageContainer.h						\
    EURECOM-NAS/src/ies/TrafficFlowAggregateDescription.h			\
    EURECOM-NAS/src/ies/NasKeySetIdentifier.h						\
    EURECOM-NAS/src/ies/NasSecurityAlgorithms.h						\
    EURECOM-NAS/src/ies/GutiType.h									\
    EURECOM-NAS/src/ies/EpsMobileIdentity.h							\
    EURECOM-NAS/src/ies/EpsUpdateType.h								\
    EURECOM-NAS/src/ies/TrackingAreaIdentity.h						\
    EURECOM-NAS/src/ies/TimeZoneAndTime.h							\
    EURECOM-NAS/src/ies/AccessPointName.h							\
    EURECOM-NAS/src/ies/PdnType.h									\
    EURECOM-NAS/src/ies/LlcServiceAccessPointIdentifier.h			\
    EURECOM-NAS/src/ies/DaylightSavingTime.h						\
    EURECOM-NAS/src/ies/PagingIdentity.h							\
    EURECOM-NAS/src/ies/CsfbResponse.h								\
    EURECOM-NAS/src/ies/LcsIndicator.h								\
    EURECOM-NAS/src/ies/EsmInformationTransferFlag.h				\
    EURECOM-NAS/src/ies/EsmMessageContainer.h						\
    EURECOM-NAS/src/ies/ImeisvRequest.h								\
    EURECOM-NAS/src/ies/PTmsiSignature.h							\
    EURECOM-NAS/src/ies/SsCode.h									\
    EURECOM-NAS/src/ies/EpsAttachResult.h							\
    EURECOM-NAS/src/ies/UeSecurityCapability.h						\
    EURECOM-NAS/src/ies/TimeZone.h									\
    EURECOM-NAS/src/ies/EpsBearerIdentity.h							\
    EURECOM-NAS/src/ies/IdentityType2.h								\
    EURECOM-NAS/src/ies/LinkedEpsBearerIdentity.h					\
    EURECOM-NAS/src/ies/PacketFlowIdentifier.h						\
    EURECOM-NAS/src/ies/RadioPriority.h								\
    EURECOM-NAS/src/ies/Nonce.h										\
    EURECOM-NAS/src/ies/KsiAndSequenceNumber.h						\
    EURECOM-NAS/src/ies/NasRequestType.h							\
    EURECOM-NAS/src/ies/ProtocolConfigurationOptions.h				\
    EURECOM-NAS/src/ies/AuthenticationResponseParameter.h			\
    EURECOM-NAS/src/ies/MobileStationClassmark3.h					\
    EURECOM-NAS/src/ies/EpsQualityOfService.h						\
    EURECOM-NAS/src/ies/DrxParameter.h								\
    EURECOM-NAS/src/ies/EsmCause.h									\
    EURECOM-NAS/src/ies/ProcedureTransactionIdentity.h				\
    EURECOM-NAS/src/ies/TransactionIdentifier.h						\
    EURECOM-NAS/src/ies/ProtocolDiscriminator.h						\
    EURECOM-NAS/src/ies/EpsAttachType.h								\
    EURECOM-NAS/src/ies/AdditionalUpdateResult.h					\
    EURECOM-NAS/src/ies/SecurityHeaderType.h						\
    EURECOM-NAS/src/ies/TrafficFlowTemplate.h						\
    EURECOM-NAS/src/ies/NetworkName.h								\
    EURECOM-NAS/src/ies/PlmnList.h									\
    EURECOM-NAS/src/ies/TrackingAreaIdentityList.h					\
    EURECOM-NAS/src/ies/DetachType.h								\
    EURECOM-NAS/src/ies/AuthenticationParameterRand.h				\
    EURECOM-NAS/src/ies/EpsUpdateResult.h							\
    EURECOM-NAS/src/ies/MsNetworkCapability.h						\
    EURECOM-NAS/src/ies/TmsiStatus.h								\
    EURECOM-NAS/src/ies/CipheringKeySequenceNumber.h				\
    EURECOM-NAS/src/ies/ApnAggregateMaximumBitRate.h				\
    EURECOM-NAS/src/ies/EpsNetworkFeatureSupport.h					\
    EURECOM-NAS/src/ies/VoiceDomainPreferenceAndUeUsageSetting.c	\
    EURECOM-NAS/src/ies/VoiceDomainPreferenceAndUeUsageSetting.h
    
libnas_utils_SRCS =								\
    EURECOM-NAS/src/util/nas_log.h				\
    EURECOM-NAS/src/util/nas_log.c				\
    EURECOM-NAS/src/util/nas_timer.h			\
    EURECOM-NAS/src/util/nas_timer.c			\
    EURECOM-NAS/src/util/TLVEncoder.h			\
    EURECOM-NAS/src/util/TLVEncoder.c			\
    EURECOM-NAS/src/util/TLVDecoder.h			\
    EURECOM-NAS/src/util/TLVDecoder.c			\
    EURECOM-NAS/src/util/OctetString.h			\
    EURECOM-NAS/src/util/OctetString.c

libnas_SRCS =									\
    EURECOM-NAS/src/nas_network.c				\
    EURECOM-NAS/src/nas_network.h				\
    EURECOM-NAS/src/nas_parser.c				\
    EURECOM-NAS/src/nas_parser.h				\
    EURECOM-NAS/src/nas_proc.c					\
    EURECOM-NAS/src/nas_proc.h					\
    EURECOM-NAS/src/nas_user.c					\
    EURECOM-NAS/src/nas_user.h					\
    $(libnas_utils_SRCS)						\
    $(libnas_emm_SRCS)							\
    $(libnas_emm_msg_SRCS)						\
    $(libnas_emm_sap_SRCS)						\
    $(libnas_esm_SRCS)							\
    $(libnas_esm_msg_SRCS)						\
    $(libnas_esm_sap_SRCS)						\
    $(libnas_ies_SRCS)  \
    $(libnas_api_SRCS)
