export PROCESS = MME

include ../../Makerules
include ../../Makefile.inc

PROJDIR	= $(PWD)/../..

export LD_RUN_PATH = $(LIBDIR):$(LIBPROCESS)

LIBS	 = -lutil -lapi -lEMMmsg -lESMmsg -lies
INCLUDES = -I. -I$(SRCDIR) -I$(INCDIR) -I$(UTILDIR) \
	-I$(EMMMSGDIR) -I$(ESMMSGDIR) -I$(IESDIR)

TARGET = NetworkProcess

all: $(TARGET)

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	$(RM) $(OBJS) *.bak *~

veryclean: clean
	$(RM) $(TARGET)

depend:
	makedepend -- ${CFLAGS} -- ${SRCS}

# DO NOT DELETE THIS LINE -- make depend depends on it.

network_parser.o: network_parser.h
network_parser.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/parser.h
network_simulator.o: network_parser.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
network_simulator.o: /usr/include/stdint.h /usr/include/features.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/socket.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/api/network/as_message.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/api/network/nas_message.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/emm_msg.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/emm_msgDef.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AttachRequest.h
network_simulator.o: /usr/include/stdio.h /usr/include/libio.h
network_simulator.o: /usr/include/_G_config.h /usr/include/wchar.h
network_simulator.o: /usr/include/xlocale.h /usr/include/stdlib.h
network_simulator.o: /usr/include/alloca.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ProtocolDiscriminator.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/SecurityHeaderType.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/MessageType.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsAttachType.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/NasKeySetIdentifier.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsMobileIdentity.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/UeNetworkCapability.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EsmMessageContainer.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PTmsiSignature.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TrackingAreaIdentity.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/DrxParameter.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/MsNetworkCapability.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/LocationAreaIdentification.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TmsiStatus.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/MobileStationClassmark2.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/MobileStationClassmark3.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/SupportedCodecList.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AdditionalUpdateType.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/GutiType.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AttachAccept.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsAttachResult.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/GprsTimer.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TrackingAreaIdentityList.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/MobileIdentity.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EmmCause.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PlmnList.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EmergencyNumberList.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsNetworkFeatureSupport.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AdditionalUpdateResult.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AttachComplete.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AttachReject.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/DetachRequest.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/DetachType.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/DetachAccept.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/TrackingAreaUpdateRequest.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsUpdateType.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/CipheringKeySequenceNumber.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/Nonce.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/UeRadioCapabilityInformationUpdateNeeded.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsBearerContextStatus.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/TrackingAreaUpdateAccept.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsUpdateResult.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/TrackingAreaUpdateComplete.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/TrackingAreaUpdateReject.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/ExtendedServiceRequest.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ServiceType.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/CsfbResponse.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/ServiceRequest.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/KsiAndSequenceNumber.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ShortMac.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/ServiceReject.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/GutiReallocationCommand.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/GutiReallocationComplete.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AuthenticationRequest.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AuthenticationParameterRand.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AuthenticationParameterAutn.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AuthenticationResponse.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AuthenticationResponseParameter.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AuthenticationReject.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AuthenticationFailure.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AuthenticationFailureParameter.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/IdentityRequest.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/IdentityType2.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/IdentityResponse.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/SecurityModeCommand.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/NasSecurityAlgorithms.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/UeSecurityCapability.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ImeisvRequest.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/SecurityModeComplete.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/SecurityModeReject.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/EmmStatus.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/EmmInformation.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/NetworkName.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TimeZone.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TimeZoneAndTime.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/DaylightSavingTime.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/DownlinkNasTransport.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/NasMessageContainer.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/UplinkNasTransport.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/CsServiceNotification.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PagingIdentity.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/Cli.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/SsCode.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/LcsIndicator.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/LcsClientIdentity.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/esm_msg.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/esm_msgDef.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ActivateDedicatedEpsBearerContextRequest.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsBearerIdentity.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ProcedureTransactionIdentity.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/LinkedEpsBearerIdentity.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsQualityOfService.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TrafficFlowTemplate.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TransactionIdentifier.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/QualityOfService.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/LlcServiceAccessPointIdentifier.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/RadioPriority.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PacketFlowIdentifier.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ProtocolConfigurationOptions.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ActivateDedicatedEpsBearerContextAccept.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ActivateDedicatedEpsBearerContextReject.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EsmCause.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ActivateDefaultEpsBearerContextRequest.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AccessPointName.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PdnAddress.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ApnAggregateMaximumBitRate.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ActivateDefaultEpsBearerContextAccept.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ActivateDefaultEpsBearerContextReject.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ModifyEpsBearerContextRequest.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ModifyEpsBearerContextAccept.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ModifyEpsBearerContextReject.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/DeactivateEpsBearerContextRequest.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/DeactivateEpsBearerContextAccept.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/PdnDisconnectRequest.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/PdnDisconnectReject.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/PdnConnectivityRequest.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/RequestType.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PdnType.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EsmInformationTransferFlag.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/PdnConnectivityReject.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/BearerResourceAllocationRequest.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TrafficFlowAggregateDescription.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/BearerResourceAllocationReject.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/BearerResourceModificationRequest.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/BearerResourceModificationReject.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/EsmInformationRequest.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/EsmInformationResponse.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/EsmStatus.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/emm_cause.h
network_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/esm_cause.h
network_simulator.o: /usr/include/errno.h /usr/include/netdb.h
network_simulator.o: /usr/include/netinet/in.h /usr/include/endian.h
network_simulator.o: /usr/include/rpc/netdb.h /usr/include/signal.h
network_simulator.o: /usr/include/time.h /usr/include/string.h
network_simulator.o: /usr/include/unistd.h /usr/include/getopt.h
