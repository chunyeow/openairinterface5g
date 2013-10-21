export PROCESS = MME

include ../../Makerules
include ../../Makefile.inc

PROJDIR	= $(PWD)/../..

export LD_RUN_PATH = $(LIBDIR):$(LIBPROCESS)

LIBS	 = -lutil -lapi -lEMMmsg -lESMmsg -lies
INCLUDES = -I. -I$(INCDIR) -I$(UTILDIR) -I$(NETAPIDIR) \
	-I$(EMMMSGDIR) -I$(ESMMSGDIR) -I$(IESDIR)

TARGET = ASprocess

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

as_data.o: as_data.h
as_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/api/network/as_message.h
as_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
as_data.o: /usr/include/stdint.h /usr/include/features.h
as_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
as_data.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
as_data.o: /usr/include/wchar.h /usr/include/time.h /usr/include/xlocale.h
as_process.o: as_process.h
as_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/api/network/as_message.h
as_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
as_process.o: /usr/include/stdint.h /usr/include/features.h
as_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
as_process.o: nas_process.h as_data.h /usr/include/stdio.h
as_process.o: /usr/include/libio.h /usr/include/_G_config.h
as_process.o: /usr/include/wchar.h /usr/include/string.h
as_process.o: /usr/include/xlocale.h
as_simulator.o: as_simulator_parser.h as_data.h as_process.h
as_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/api/network/as_message.h
as_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
as_simulator.o: /usr/include/stdint.h /usr/include/features.h
as_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
as_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/socket.h
as_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
as_simulator.o: /usr/include/stdio.h /usr/include/libio.h
as_simulator.o: /usr/include/_G_config.h /usr/include/wchar.h
as_simulator.o: /usr/include/errno.h /usr/include/netdb.h
as_simulator.o: /usr/include/netinet/in.h /usr/include/endian.h
as_simulator.o: /usr/include/rpc/netdb.h /usr/include/stdlib.h
as_simulator.o: /usr/include/alloca.h /usr/include/string.h
as_simulator.o: /usr/include/xlocale.h /usr/include/poll.h
as_simulator.o: /usr/include/signal.h /usr/include/time.h
as_simulator.o: /usr/include/pthread.h /usr/include/sched.h
as_simulator_parser.o: as_simulator_parser.h
as_simulator_parser.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/parser.h
nas_data.o: nas_data.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsAttachType.h
nas_data.o: /usr/include/stdio.h /usr/include/features.h /usr/include/libio.h
nas_data.o: /usr/include/_G_config.h /usr/include/wchar.h
nas_data.o: /usr/include/stdlib.h /usr/include/alloca.h /usr/include/stdint.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/DetachType.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/NasKeySetIdentifier.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsMobileIdentity.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/MobileIdentity.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/IdentityType2.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/RequestType.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PdnType.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PdnAddress.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/NasSecurityAlgorithms.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/GprsTimer.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TrackingAreaIdentityList.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EmmCause.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EsmCause.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/emm_msgDef.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/esm_msgDef.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/emm_cause.h
nas_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/esm_cause.h
nas_process.o: nas_process.h nas_data.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsAttachType.h
nas_process.o: /usr/include/stdio.h /usr/include/features.h
nas_process.o: /usr/include/libio.h /usr/include/_G_config.h
nas_process.o: /usr/include/wchar.h /usr/include/stdlib.h
nas_process.o: /usr/include/alloca.h /usr/include/stdint.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/DetachType.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/NasKeySetIdentifier.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsMobileIdentity.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/MobileIdentity.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/IdentityType2.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/RequestType.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PdnType.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PdnAddress.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/NasSecurityAlgorithms.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/GprsTimer.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TrackingAreaIdentityList.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EmmCause.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EsmCause.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/api/network/nas_message.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/emm_msg.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/emm_msgDef.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AttachRequest.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ProtocolDiscriminator.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/SecurityHeaderType.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/MessageType.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/UeNetworkCapability.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EsmMessageContainer.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PTmsiSignature.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TrackingAreaIdentity.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/DrxParameter.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/MsNetworkCapability.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/LocationAreaIdentification.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TmsiStatus.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/MobileStationClassmark2.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/MobileStationClassmark3.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/SupportedCodecList.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AdditionalUpdateType.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/GutiType.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AttachAccept.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsAttachResult.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PlmnList.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EmergencyNumberList.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsNetworkFeatureSupport.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AdditionalUpdateResult.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AttachComplete.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AttachReject.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/DetachRequest.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/DetachAccept.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/TrackingAreaUpdateRequest.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsUpdateType.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/CipheringKeySequenceNumber.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/Nonce.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/UeRadioCapabilityInformationUpdateNeeded.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsBearerContextStatus.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/TrackingAreaUpdateAccept.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsUpdateResult.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/TrackingAreaUpdateComplete.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/TrackingAreaUpdateReject.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/ExtendedServiceRequest.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ServiceType.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/CsfbResponse.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/ServiceRequest.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/KsiAndSequenceNumber.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ShortMac.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/ServiceReject.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/GutiReallocationCommand.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/GutiReallocationComplete.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AuthenticationRequest.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AuthenticationParameterRand.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AuthenticationParameterAutn.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AuthenticationResponse.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AuthenticationResponseParameter.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AuthenticationReject.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/AuthenticationFailure.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AuthenticationFailureParameter.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/IdentityRequest.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/IdentityResponse.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/SecurityModeCommand.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/UeSecurityCapability.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ImeisvRequest.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/SecurityModeComplete.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/SecurityModeReject.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/EmmStatus.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/EmmInformation.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/NetworkName.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TimeZone.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TimeZoneAndTime.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/DaylightSavingTime.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/DownlinkNasTransport.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/NasMessageContainer.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/UplinkNasTransport.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/CsServiceNotification.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PagingIdentity.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/Cli.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/SsCode.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/LcsIndicator.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/LcsClientIdentity.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/esm_msg.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/esm_msgDef.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ActivateDedicatedEpsBearerContextRequest.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsBearerIdentity.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ProcedureTransactionIdentity.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/LinkedEpsBearerIdentity.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsQualityOfService.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TrafficFlowTemplate.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TransactionIdentifier.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/QualityOfService.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/LlcServiceAccessPointIdentifier.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/RadioPriority.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/PacketFlowIdentifier.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ProtocolConfigurationOptions.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ActivateDedicatedEpsBearerContextAccept.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ActivateDedicatedEpsBearerContextReject.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ActivateDefaultEpsBearerContextRequest.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/AccessPointName.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ApnAggregateMaximumBitRate.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ActivateDefaultEpsBearerContextAccept.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ActivateDefaultEpsBearerContextReject.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ModifyEpsBearerContextRequest.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ModifyEpsBearerContextAccept.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/ModifyEpsBearerContextReject.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/DeactivateEpsBearerContextRequest.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/DeactivateEpsBearerContextAccept.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/PdnDisconnectRequest.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/PdnDisconnectReject.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/PdnConnectivityRequest.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EsmInformationTransferFlag.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/PdnConnectivityReject.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/BearerResourceAllocationRequest.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/TrafficFlowAggregateDescription.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/BearerResourceAllocationReject.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/BearerResourceModificationRequest.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/BearerResourceModificationReject.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/EsmInformationRequest.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/EsmInformationResponse.h
nas_process.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/EsmStatus.h
nas_process.o: /usr/include/string.h /usr/include/xlocale.h
