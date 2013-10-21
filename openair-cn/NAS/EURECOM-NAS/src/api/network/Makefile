ifndef PROJDIR
PROJDIR  = $(PWD)/../../..
INCLUDES = -I. -I$(INCDIR) -I$(UTILDIR) -I$(IESDIR) -I$(EMMMSGDIR) -I$(ESMMSGDIR)
endif

include $(PROJDIR)/Makerules
include $(PROJDIR)/Makefile.inc

all: $(OBJS)

%.o: %.c Makefile
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS) *.bak *~

depend:
	makedepend -- ${CFLAGS} -- ${SRCS}

# DO NOT DELETE THIS LINE -- make depend depends on it.

as_message.o: as_message.h
as_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
as_message.o: /usr/include/stdint.h /usr/include/features.h
as_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
as_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
as_message.o: /usr/include/string.h /usr/include/xlocale.h
as_message.o: /usr/include/stdlib.h /usr/include/alloca.h
nas_message.o: nas_message.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
nas_message.o: /usr/include/stdint.h /usr/include/features.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/emm_msg.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/emm_msgDef.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AttachRequest.h
nas_message.o: /usr/include/stdio.h /usr/include/libio.h
nas_message.o: /usr/include/_G_config.h /usr/include/wchar.h
nas_message.o: /usr/include/stdlib.h /usr/include/alloca.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ProtocolDiscriminator.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/SecurityHeaderType.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/MessageType.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsAttachType.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/NasKeySetIdentifier.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsMobileIdentity.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/UeNetworkCapability.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EsmMessageContainer.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PTmsiSignature.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TrackingAreaIdentity.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/DrxParameter.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/MsNetworkCapability.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/LocationAreaIdentification.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TmsiStatus.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/MobileStationClassmark2.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/MobileStationClassmark3.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/SupportedCodecList.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AdditionalUpdateType.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/GutiType.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AttachAccept.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsAttachResult.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/GprsTimer.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TrackingAreaIdentityList.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/MobileIdentity.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EmmCause.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PlmnList.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EmergencyNumberList.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsNetworkFeatureSupport.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AdditionalUpdateResult.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AttachComplete.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AttachReject.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/DetachRequest.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/DetachType.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/DetachAccept.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/TrackingAreaUpdateRequest.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsUpdateType.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/CipheringKeySequenceNumber.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/Nonce.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/UeRadioCapabilityInformationUpdateNeeded.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsBearerContextStatus.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/TrackingAreaUpdateAccept.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsUpdateResult.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/TrackingAreaUpdateComplete.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/TrackingAreaUpdateReject.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/ExtendedServiceRequest.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ServiceType.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/CsfbResponse.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/ServiceRequest.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/KsiAndSequenceNumber.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ShortMac.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/ServiceReject.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/GutiReallocationCommand.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/GutiReallocationComplete.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AuthenticationRequest.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AuthenticationParameterRand.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AuthenticationParameterAutn.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AuthenticationResponse.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AuthenticationResponseParameter.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AuthenticationReject.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AuthenticationFailure.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AuthenticationFailureParameter.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/IdentityRequest.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/IdentityType2.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/IdentityResponse.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/SecurityModeCommand.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/NasSecurityAlgorithms.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/UeSecurityCapability.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ImeisvRequest.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/SecurityModeComplete.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/SecurityModeReject.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/EmmStatus.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/EmmInformation.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/NetworkName.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TimeZone.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TimeZoneAndTime.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/DaylightSavingTime.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/DownlinkNasTransport.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/NasMessageContainer.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/UplinkNasTransport.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/CsServiceNotification.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PagingIdentity.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/Cli.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/SsCode.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/LcsIndicator.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/LcsClientIdentity.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/esm_msg.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/esm_msgDef.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ActivateDedicatedEpsBearerContextRequest.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsBearerIdentity.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ProcedureTransactionIdentity.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/LinkedEpsBearerIdentity.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsQualityOfService.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TrafficFlowTemplate.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TransactionIdentifier.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/QualityOfService.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/LlcServiceAccessPointIdentifier.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/RadioPriority.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PacketFlowIdentifier.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ProtocolConfigurationOptions.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ActivateDedicatedEpsBearerContextAccept.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ActivateDedicatedEpsBearerContextReject.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EsmCause.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ActivateDefaultEpsBearerContextRequest.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AccessPointName.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PdnAddress.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ApnAggregateMaximumBitRate.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ActivateDefaultEpsBearerContextAccept.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ActivateDefaultEpsBearerContextReject.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ModifyEpsBearerContextRequest.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ModifyEpsBearerContextAccept.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ModifyEpsBearerContextReject.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/DeactivateEpsBearerContextRequest.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/DeactivateEpsBearerContextAccept.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/PdnDisconnectRequest.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/PdnDisconnectReject.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/PdnConnectivityRequest.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/RequestType.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PdnType.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EsmInformationTransferFlag.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/PdnConnectivityReject.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/BearerResourceAllocationRequest.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TrafficFlowAggregateDescription.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/BearerResourceAllocationReject.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/BearerResourceModificationRequest.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/BearerResourceModificationReject.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/EsmInformationRequest.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/EsmInformationResponse.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/EsmStatus.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
nas_message.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
nas_message.o: /usr/include/endian.h
nas_message.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
nas_message.o: /usr/include/string.h /usr/include/xlocale.h
network_api.o: network_api.h
network_api.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
network_api.o: /usr/include/stdint.h /usr/include/features.h
network_api.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
network_api.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/socket.h
network_api.o: as_message.h
network_api.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
network_api.o: /usr/include/string.h /usr/include/xlocale.h
network_api.o: /usr/include/netdb.h /usr/include/netinet/in.h
network_api.o: /usr/include/endian.h /usr/include/rpc/netdb.h
network_api.o: /usr/include/errno.h /usr/include/unistd.h
network_api.o: /usr/include/getopt.h
