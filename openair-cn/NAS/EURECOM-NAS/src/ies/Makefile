ifndef PROJDIR
PROJDIR = $(PWD)/../..
endif

include $(PROJDIR)/Makerules
include $(PROJDIR)/Makefile.inc

INCLUDES = -I. -I$(UTILDIR)

#DEFINES	 += -DNAS_DEBUG

TARGET = $(LIBIES)
TARGETS = $(TARGET).a $(TARGET).so

all: $(TARGETS)

%.o: %.c %.h Makefile
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).a: $(OBJS)
	@$(RM) $@
	@$(AR) $(ARFLAGS) $@ $(OBJS)
	@echo Replacing $@ to $(LIBDIR)
	@$(RM) $(LIBDIR)/$@
	@$(CP) $@ $(LIBDIR)

$(TARGET).so: $(OBJS)
	@$(LD) -G -o $@ $(OBJS)
	@echo Replacing $@ to $(LIBDIR)
	@$(RM) $(LIBDIR)/$@
	@$(CP) $@ $(LIBDIR)

clean:
	$(RM) $(OBJS) *.bak *~

veryclean: clean
	$(RM) $(TARGETS)

depend:
	makedepend -- ${CFLAGS} -- ${SRCS}

# DO NOT DELETE THIS LINE -- make depend depends on it.

AccessPointName.o: /usr/include/stdio.h /usr/include/features.h
AccessPointName.o: /usr/include/libio.h /usr/include/_G_config.h
AccessPointName.o: /usr/include/wchar.h /usr/include/xlocale.h
AccessPointName.o: /usr/include/stdlib.h /usr/include/alloca.h
AccessPointName.o: /usr/include/stdint.h
AccessPointName.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
AccessPointName.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
AccessPointName.o: /usr/include/endian.h
AccessPointName.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
AccessPointName.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
AccessPointName.o: AccessPointName.h
AccessPointName.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
AdditionalUpdateResult.o: /usr/include/stdio.h /usr/include/features.h
AdditionalUpdateResult.o: /usr/include/libio.h /usr/include/_G_config.h
AdditionalUpdateResult.o: /usr/include/wchar.h /usr/include/xlocale.h
AdditionalUpdateResult.o: /usr/include/stdlib.h /usr/include/alloca.h
AdditionalUpdateResult.o: /usr/include/stdint.h
AdditionalUpdateResult.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
AdditionalUpdateResult.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
AdditionalUpdateResult.o: /usr/include/endian.h
AdditionalUpdateResult.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
AdditionalUpdateResult.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
AdditionalUpdateResult.o: AdditionalUpdateResult.h
AdditionalUpdateResult.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
AdditionalUpdateType.o: /usr/include/stdio.h /usr/include/features.h
AdditionalUpdateType.o: /usr/include/libio.h /usr/include/_G_config.h
AdditionalUpdateType.o: /usr/include/wchar.h /usr/include/xlocale.h
AdditionalUpdateType.o: /usr/include/stdlib.h /usr/include/alloca.h
AdditionalUpdateType.o: /usr/include/stdint.h
AdditionalUpdateType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
AdditionalUpdateType.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
AdditionalUpdateType.o: /usr/include/endian.h
AdditionalUpdateType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
AdditionalUpdateType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
AdditionalUpdateType.o: AdditionalUpdateType.h
AdditionalUpdateType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
ApnAggregateMaximumBitRate.o: /usr/include/stdio.h /usr/include/features.h
ApnAggregateMaximumBitRate.o: /usr/include/libio.h /usr/include/_G_config.h
ApnAggregateMaximumBitRate.o: /usr/include/wchar.h /usr/include/xlocale.h
ApnAggregateMaximumBitRate.o: /usr/include/stdlib.h /usr/include/alloca.h
ApnAggregateMaximumBitRate.o: /usr/include/stdint.h
ApnAggregateMaximumBitRate.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
ApnAggregateMaximumBitRate.o: /usr/include/arpa/inet.h
ApnAggregateMaximumBitRate.o: /usr/include/netinet/in.h /usr/include/endian.h
ApnAggregateMaximumBitRate.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
ApnAggregateMaximumBitRate.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
ApnAggregateMaximumBitRate.o: ApnAggregateMaximumBitRate.h
ApnAggregateMaximumBitRate.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
AuthenticationFailureParameter.o: /usr/include/stdio.h
AuthenticationFailureParameter.o: /usr/include/features.h
AuthenticationFailureParameter.o: /usr/include/libio.h
AuthenticationFailureParameter.o: /usr/include/_G_config.h
AuthenticationFailureParameter.o: /usr/include/wchar.h /usr/include/xlocale.h
AuthenticationFailureParameter.o: /usr/include/stdlib.h /usr/include/alloca.h
AuthenticationFailureParameter.o: /usr/include/stdint.h
AuthenticationFailureParameter.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
AuthenticationFailureParameter.o: /usr/include/arpa/inet.h
AuthenticationFailureParameter.o: /usr/include/netinet/in.h
AuthenticationFailureParameter.o: /usr/include/endian.h
AuthenticationFailureParameter.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
AuthenticationFailureParameter.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
AuthenticationFailureParameter.o: AuthenticationFailureParameter.h
AuthenticationFailureParameter.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
AuthenticationParameterAutn.o: /usr/include/stdio.h /usr/include/features.h
AuthenticationParameterAutn.o: /usr/include/libio.h /usr/include/_G_config.h
AuthenticationParameterAutn.o: /usr/include/wchar.h /usr/include/xlocale.h
AuthenticationParameterAutn.o: /usr/include/stdlib.h /usr/include/alloca.h
AuthenticationParameterAutn.o: /usr/include/stdint.h
AuthenticationParameterAutn.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
AuthenticationParameterAutn.o: /usr/include/arpa/inet.h
AuthenticationParameterAutn.o: /usr/include/netinet/in.h
AuthenticationParameterAutn.o: /usr/include/endian.h
AuthenticationParameterAutn.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
AuthenticationParameterAutn.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
AuthenticationParameterAutn.o: AuthenticationParameterAutn.h
AuthenticationParameterAutn.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
AuthenticationParameterRand.o: /usr/include/stdio.h /usr/include/features.h
AuthenticationParameterRand.o: /usr/include/libio.h /usr/include/_G_config.h
AuthenticationParameterRand.o: /usr/include/wchar.h /usr/include/xlocale.h
AuthenticationParameterRand.o: /usr/include/stdlib.h /usr/include/alloca.h
AuthenticationParameterRand.o: /usr/include/stdint.h
AuthenticationParameterRand.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
AuthenticationParameterRand.o: /usr/include/arpa/inet.h
AuthenticationParameterRand.o: /usr/include/netinet/in.h
AuthenticationParameterRand.o: /usr/include/endian.h
AuthenticationParameterRand.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
AuthenticationParameterRand.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
AuthenticationParameterRand.o: AuthenticationParameterRand.h
AuthenticationParameterRand.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
AuthenticationResponseParameter.o: /usr/include/stdio.h
AuthenticationResponseParameter.o: /usr/include/features.h
AuthenticationResponseParameter.o: /usr/include/libio.h
AuthenticationResponseParameter.o: /usr/include/_G_config.h
AuthenticationResponseParameter.o: /usr/include/wchar.h
AuthenticationResponseParameter.o: /usr/include/xlocale.h
AuthenticationResponseParameter.o: /usr/include/stdlib.h
AuthenticationResponseParameter.o: /usr/include/alloca.h
AuthenticationResponseParameter.o: /usr/include/stdint.h
AuthenticationResponseParameter.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
AuthenticationResponseParameter.o: /usr/include/arpa/inet.h
AuthenticationResponseParameter.o: /usr/include/netinet/in.h
AuthenticationResponseParameter.o: /usr/include/endian.h
AuthenticationResponseParameter.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
AuthenticationResponseParameter.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
AuthenticationResponseParameter.o: AuthenticationResponseParameter.h
AuthenticationResponseParameter.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
CipheringKeySequenceNumber.o: /usr/include/stdio.h /usr/include/features.h
CipheringKeySequenceNumber.o: /usr/include/libio.h /usr/include/_G_config.h
CipheringKeySequenceNumber.o: /usr/include/wchar.h /usr/include/xlocale.h
CipheringKeySequenceNumber.o: /usr/include/stdlib.h /usr/include/alloca.h
CipheringKeySequenceNumber.o: /usr/include/stdint.h
CipheringKeySequenceNumber.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
CipheringKeySequenceNumber.o: /usr/include/arpa/inet.h
CipheringKeySequenceNumber.o: /usr/include/netinet/in.h /usr/include/endian.h
CipheringKeySequenceNumber.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
CipheringKeySequenceNumber.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
CipheringKeySequenceNumber.o: CipheringKeySequenceNumber.h
CipheringKeySequenceNumber.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
Cli.o: /usr/include/stdio.h /usr/include/features.h /usr/include/libio.h
Cli.o: /usr/include/_G_config.h /usr/include/wchar.h /usr/include/xlocale.h
Cli.o: /usr/include/stdlib.h /usr/include/alloca.h /usr/include/stdint.h
Cli.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
Cli.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
Cli.o: /usr/include/endian.h
Cli.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
Cli.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
Cli.o: Cli.h
Cli.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
CsfbResponse.o: /usr/include/stdio.h /usr/include/features.h
CsfbResponse.o: /usr/include/libio.h /usr/include/_G_config.h
CsfbResponse.o: /usr/include/wchar.h /usr/include/xlocale.h
CsfbResponse.o: /usr/include/stdlib.h /usr/include/alloca.h
CsfbResponse.o: /usr/include/stdint.h
CsfbResponse.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
CsfbResponse.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
CsfbResponse.o: /usr/include/endian.h
CsfbResponse.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
CsfbResponse.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
CsfbResponse.o: CsfbResponse.h
CsfbResponse.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
DaylightSavingTime.o: /usr/include/stdio.h /usr/include/features.h
DaylightSavingTime.o: /usr/include/libio.h /usr/include/_G_config.h
DaylightSavingTime.o: /usr/include/wchar.h /usr/include/xlocale.h
DaylightSavingTime.o: /usr/include/stdlib.h /usr/include/alloca.h
DaylightSavingTime.o: /usr/include/stdint.h
DaylightSavingTime.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
DaylightSavingTime.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
DaylightSavingTime.o: /usr/include/endian.h
DaylightSavingTime.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
DaylightSavingTime.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
DaylightSavingTime.o: DaylightSavingTime.h
DaylightSavingTime.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
DetachType.o: /usr/include/stdio.h /usr/include/features.h
DetachType.o: /usr/include/libio.h /usr/include/_G_config.h
DetachType.o: /usr/include/wchar.h /usr/include/xlocale.h
DetachType.o: /usr/include/stdlib.h /usr/include/alloca.h
DetachType.o: /usr/include/stdint.h
DetachType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
DetachType.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
DetachType.o: /usr/include/endian.h
DetachType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
DetachType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
DetachType.o: DetachType.h
DetachType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
DrxParameter.o: /usr/include/stdio.h /usr/include/features.h
DrxParameter.o: /usr/include/libio.h /usr/include/_G_config.h
DrxParameter.o: /usr/include/wchar.h /usr/include/xlocale.h
DrxParameter.o: /usr/include/stdlib.h /usr/include/alloca.h
DrxParameter.o: /usr/include/stdint.h
DrxParameter.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
DrxParameter.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
DrxParameter.o: /usr/include/endian.h
DrxParameter.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
DrxParameter.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
DrxParameter.o: DrxParameter.h
DrxParameter.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
EmergencyNumberList.o: /usr/include/stdio.h /usr/include/features.h
EmergencyNumberList.o: /usr/include/libio.h /usr/include/_G_config.h
EmergencyNumberList.o: /usr/include/wchar.h /usr/include/xlocale.h
EmergencyNumberList.o: /usr/include/stdlib.h /usr/include/alloca.h
EmergencyNumberList.o: /usr/include/stdint.h
EmergencyNumberList.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
EmergencyNumberList.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
EmergencyNumberList.o: /usr/include/endian.h
EmergencyNumberList.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
EmergencyNumberList.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
EmergencyNumberList.o: EmergencyNumberList.h
EmergencyNumberList.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
EmmCause.o: /usr/include/stdio.h /usr/include/features.h /usr/include/libio.h
EmmCause.o: /usr/include/_G_config.h /usr/include/wchar.h
EmmCause.o: /usr/include/xlocale.h /usr/include/stdlib.h
EmmCause.o: /usr/include/alloca.h /usr/include/stdint.h
EmmCause.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
EmmCause.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
EmmCause.o: /usr/include/endian.h
EmmCause.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
EmmCause.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
EmmCause.o: EmmCause.h
EmmCause.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
EpsAttachResult.o: /usr/include/stdio.h /usr/include/features.h
EpsAttachResult.o: /usr/include/libio.h /usr/include/_G_config.h
EpsAttachResult.o: /usr/include/wchar.h /usr/include/xlocale.h
EpsAttachResult.o: /usr/include/stdlib.h /usr/include/alloca.h
EpsAttachResult.o: /usr/include/stdint.h
EpsAttachResult.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
EpsAttachResult.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
EpsAttachResult.o: /usr/include/endian.h
EpsAttachResult.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
EpsAttachResult.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
EpsAttachResult.o: EpsAttachResult.h
EpsAttachResult.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
EpsAttachType.o: /usr/include/stdio.h /usr/include/features.h
EpsAttachType.o: /usr/include/libio.h /usr/include/_G_config.h
EpsAttachType.o: /usr/include/wchar.h /usr/include/xlocale.h
EpsAttachType.o: /usr/include/stdlib.h /usr/include/alloca.h
EpsAttachType.o: /usr/include/stdint.h
EpsAttachType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
EpsAttachType.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
EpsAttachType.o: /usr/include/endian.h
EpsAttachType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
EpsAttachType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
EpsAttachType.o: EpsAttachType.h
EpsAttachType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
EpsBearerContextStatus.o: /usr/include/stdio.h /usr/include/features.h
EpsBearerContextStatus.o: /usr/include/libio.h /usr/include/_G_config.h
EpsBearerContextStatus.o: /usr/include/wchar.h /usr/include/xlocale.h
EpsBearerContextStatus.o: /usr/include/stdlib.h /usr/include/alloca.h
EpsBearerContextStatus.o: /usr/include/stdint.h
EpsBearerContextStatus.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
EpsBearerContextStatus.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
EpsBearerContextStatus.o: /usr/include/endian.h
EpsBearerContextStatus.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
EpsBearerContextStatus.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
EpsBearerContextStatus.o: EpsBearerContextStatus.h
EpsBearerContextStatus.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
EpsBearerIdentity.o: /usr/include/stdio.h /usr/include/features.h
EpsBearerIdentity.o: /usr/include/libio.h /usr/include/_G_config.h
EpsBearerIdentity.o: /usr/include/wchar.h /usr/include/xlocale.h
EpsBearerIdentity.o: /usr/include/stdlib.h /usr/include/alloca.h
EpsBearerIdentity.o: /usr/include/stdint.h
EpsBearerIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
EpsBearerIdentity.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
EpsBearerIdentity.o: /usr/include/endian.h
EpsBearerIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
EpsBearerIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
EpsBearerIdentity.o: EpsBearerIdentity.h
EpsBearerIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
EpsMobileIdentity.o: /usr/include/stdio.h /usr/include/features.h
EpsMobileIdentity.o: /usr/include/libio.h /usr/include/_G_config.h
EpsMobileIdentity.o: /usr/include/wchar.h /usr/include/xlocale.h
EpsMobileIdentity.o: /usr/include/stdlib.h /usr/include/alloca.h
EpsMobileIdentity.o: /usr/include/stdint.h
EpsMobileIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
EpsMobileIdentity.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
EpsMobileIdentity.o: /usr/include/endian.h
EpsMobileIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
EpsMobileIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
EpsMobileIdentity.o: EpsMobileIdentity.h
EpsMobileIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
EpsNetworkFeatureSupport.o: /usr/include/stdio.h /usr/include/features.h
EpsNetworkFeatureSupport.o: /usr/include/libio.h /usr/include/_G_config.h
EpsNetworkFeatureSupport.o: /usr/include/wchar.h /usr/include/xlocale.h
EpsNetworkFeatureSupport.o: /usr/include/stdlib.h /usr/include/alloca.h
EpsNetworkFeatureSupport.o: /usr/include/stdint.h
EpsNetworkFeatureSupport.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
EpsNetworkFeatureSupport.o: /usr/include/arpa/inet.h
EpsNetworkFeatureSupport.o: /usr/include/netinet/in.h /usr/include/endian.h
EpsNetworkFeatureSupport.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
EpsNetworkFeatureSupport.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
EpsNetworkFeatureSupport.o: EpsNetworkFeatureSupport.h
EpsNetworkFeatureSupport.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
EpsQualityOfService.o: /usr/include/stdio.h /usr/include/features.h
EpsQualityOfService.o: /usr/include/libio.h /usr/include/_G_config.h
EpsQualityOfService.o: /usr/include/wchar.h /usr/include/xlocale.h
EpsQualityOfService.o: /usr/include/stdlib.h /usr/include/alloca.h
EpsQualityOfService.o: /usr/include/stdint.h
EpsQualityOfService.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
EpsQualityOfService.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
EpsQualityOfService.o: /usr/include/endian.h
EpsQualityOfService.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
EpsQualityOfService.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
EpsQualityOfService.o: EpsQualityOfService.h
EpsUpdateResult.o: /usr/include/stdio.h /usr/include/features.h
EpsUpdateResult.o: /usr/include/libio.h /usr/include/_G_config.h
EpsUpdateResult.o: /usr/include/wchar.h /usr/include/xlocale.h
EpsUpdateResult.o: /usr/include/stdlib.h /usr/include/alloca.h
EpsUpdateResult.o: /usr/include/stdint.h
EpsUpdateResult.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
EpsUpdateResult.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
EpsUpdateResult.o: /usr/include/endian.h
EpsUpdateResult.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
EpsUpdateResult.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
EpsUpdateResult.o: EpsUpdateResult.h
EpsUpdateResult.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
EpsUpdateType.o: /usr/include/stdio.h /usr/include/features.h
EpsUpdateType.o: /usr/include/libio.h /usr/include/_G_config.h
EpsUpdateType.o: /usr/include/wchar.h /usr/include/xlocale.h
EpsUpdateType.o: /usr/include/stdlib.h /usr/include/alloca.h
EpsUpdateType.o: /usr/include/stdint.h
EpsUpdateType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
EpsUpdateType.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
EpsUpdateType.o: /usr/include/endian.h
EpsUpdateType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
EpsUpdateType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
EpsUpdateType.o: EpsUpdateType.h
EpsUpdateType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
EsmCause.o: /usr/include/stdio.h /usr/include/features.h /usr/include/libio.h
EsmCause.o: /usr/include/_G_config.h /usr/include/wchar.h
EsmCause.o: /usr/include/xlocale.h /usr/include/stdlib.h
EsmCause.o: /usr/include/alloca.h /usr/include/stdint.h
EsmCause.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
EsmCause.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
EsmCause.o: /usr/include/endian.h
EsmCause.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
EsmCause.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
EsmCause.o: EsmCause.h
EsmCause.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
EsmInformationTransferFlag.o: /usr/include/stdio.h /usr/include/features.h
EsmInformationTransferFlag.o: /usr/include/libio.h /usr/include/_G_config.h
EsmInformationTransferFlag.o: /usr/include/wchar.h /usr/include/xlocale.h
EsmInformationTransferFlag.o: /usr/include/stdlib.h /usr/include/alloca.h
EsmInformationTransferFlag.o: /usr/include/stdint.h
EsmInformationTransferFlag.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
EsmInformationTransferFlag.o: /usr/include/arpa/inet.h
EsmInformationTransferFlag.o: /usr/include/netinet/in.h /usr/include/endian.h
EsmInformationTransferFlag.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
EsmInformationTransferFlag.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
EsmInformationTransferFlag.o: EsmInformationTransferFlag.h
EsmInformationTransferFlag.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
EsmMessageContainer.o: /usr/include/stdio.h /usr/include/features.h
EsmMessageContainer.o: /usr/include/libio.h /usr/include/_G_config.h
EsmMessageContainer.o: /usr/include/wchar.h /usr/include/xlocale.h
EsmMessageContainer.o: /usr/include/stdlib.h /usr/include/alloca.h
EsmMessageContainer.o: /usr/include/stdint.h
EsmMessageContainer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
EsmMessageContainer.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
EsmMessageContainer.o: /usr/include/endian.h
EsmMessageContainer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
EsmMessageContainer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
EsmMessageContainer.o: EsmMessageContainer.h
EsmMessageContainer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
GprsTimer.o: /usr/include/stdio.h /usr/include/features.h
GprsTimer.o: /usr/include/libio.h /usr/include/_G_config.h
GprsTimer.o: /usr/include/wchar.h /usr/include/xlocale.h
GprsTimer.o: /usr/include/stdlib.h /usr/include/alloca.h
GprsTimer.o: /usr/include/stdint.h
GprsTimer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
GprsTimer.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
GprsTimer.o: /usr/include/endian.h
GprsTimer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
GprsTimer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
GprsTimer.o: GprsTimer.h
GprsTimer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
GutiType.o: /usr/include/stdio.h /usr/include/features.h /usr/include/libio.h
GutiType.o: /usr/include/_G_config.h /usr/include/wchar.h
GutiType.o: /usr/include/xlocale.h /usr/include/stdlib.h
GutiType.o: /usr/include/alloca.h /usr/include/stdint.h
GutiType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
GutiType.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
GutiType.o: /usr/include/endian.h
GutiType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
GutiType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
GutiType.o: GutiType.h
GutiType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
IdentityType2.o: /usr/include/stdio.h /usr/include/features.h
IdentityType2.o: /usr/include/libio.h /usr/include/_G_config.h
IdentityType2.o: /usr/include/wchar.h /usr/include/xlocale.h
IdentityType2.o: /usr/include/stdlib.h /usr/include/alloca.h
IdentityType2.o: /usr/include/stdint.h
IdentityType2.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
IdentityType2.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
IdentityType2.o: /usr/include/endian.h
IdentityType2.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
IdentityType2.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
IdentityType2.o: IdentityType2.h
IdentityType2.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
ImeisvRequest.o: /usr/include/stdio.h /usr/include/features.h
ImeisvRequest.o: /usr/include/libio.h /usr/include/_G_config.h
ImeisvRequest.o: /usr/include/wchar.h /usr/include/xlocale.h
ImeisvRequest.o: /usr/include/stdlib.h /usr/include/alloca.h
ImeisvRequest.o: /usr/include/stdint.h
ImeisvRequest.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
ImeisvRequest.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
ImeisvRequest.o: /usr/include/endian.h
ImeisvRequest.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
ImeisvRequest.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
ImeisvRequest.o: ImeisvRequest.h
ImeisvRequest.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
KsiAndSequenceNumber.o: /usr/include/stdio.h /usr/include/features.h
KsiAndSequenceNumber.o: /usr/include/libio.h /usr/include/_G_config.h
KsiAndSequenceNumber.o: /usr/include/wchar.h /usr/include/xlocale.h
KsiAndSequenceNumber.o: /usr/include/stdlib.h /usr/include/alloca.h
KsiAndSequenceNumber.o: /usr/include/stdint.h
KsiAndSequenceNumber.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
KsiAndSequenceNumber.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
KsiAndSequenceNumber.o: /usr/include/endian.h
KsiAndSequenceNumber.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
KsiAndSequenceNumber.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
KsiAndSequenceNumber.o: KsiAndSequenceNumber.h
KsiAndSequenceNumber.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
LcsClientIdentity.o: /usr/include/stdio.h /usr/include/features.h
LcsClientIdentity.o: /usr/include/libio.h /usr/include/_G_config.h
LcsClientIdentity.o: /usr/include/wchar.h /usr/include/xlocale.h
LcsClientIdentity.o: /usr/include/stdlib.h /usr/include/alloca.h
LcsClientIdentity.o: /usr/include/stdint.h
LcsClientIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
LcsClientIdentity.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
LcsClientIdentity.o: /usr/include/endian.h
LcsClientIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
LcsClientIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
LcsClientIdentity.o: LcsClientIdentity.h
LcsClientIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
LcsIndicator.o: /usr/include/stdio.h /usr/include/features.h
LcsIndicator.o: /usr/include/libio.h /usr/include/_G_config.h
LcsIndicator.o: /usr/include/wchar.h /usr/include/xlocale.h
LcsIndicator.o: /usr/include/stdlib.h /usr/include/alloca.h
LcsIndicator.o: /usr/include/stdint.h
LcsIndicator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
LcsIndicator.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
LcsIndicator.o: /usr/include/endian.h
LcsIndicator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
LcsIndicator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
LcsIndicator.o: LcsIndicator.h
LcsIndicator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
LinkedEpsBearerIdentity.o: /usr/include/stdio.h /usr/include/features.h
LinkedEpsBearerIdentity.o: /usr/include/libio.h /usr/include/_G_config.h
LinkedEpsBearerIdentity.o: /usr/include/wchar.h /usr/include/xlocale.h
LinkedEpsBearerIdentity.o: /usr/include/stdlib.h /usr/include/alloca.h
LinkedEpsBearerIdentity.o: /usr/include/stdint.h
LinkedEpsBearerIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
LinkedEpsBearerIdentity.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
LinkedEpsBearerIdentity.o: /usr/include/endian.h
LinkedEpsBearerIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
LinkedEpsBearerIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
LinkedEpsBearerIdentity.o: LinkedEpsBearerIdentity.h
LinkedEpsBearerIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
LlcServiceAccessPointIdentifier.o: /usr/include/stdio.h
LlcServiceAccessPointIdentifier.o: /usr/include/features.h
LlcServiceAccessPointIdentifier.o: /usr/include/libio.h
LlcServiceAccessPointIdentifier.o: /usr/include/_G_config.h
LlcServiceAccessPointIdentifier.o: /usr/include/wchar.h
LlcServiceAccessPointIdentifier.o: /usr/include/xlocale.h
LlcServiceAccessPointIdentifier.o: /usr/include/stdlib.h
LlcServiceAccessPointIdentifier.o: /usr/include/alloca.h
LlcServiceAccessPointIdentifier.o: /usr/include/stdint.h
LlcServiceAccessPointIdentifier.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
LlcServiceAccessPointIdentifier.o: /usr/include/arpa/inet.h
LlcServiceAccessPointIdentifier.o: /usr/include/netinet/in.h
LlcServiceAccessPointIdentifier.o: /usr/include/endian.h
LlcServiceAccessPointIdentifier.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
LlcServiceAccessPointIdentifier.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
LlcServiceAccessPointIdentifier.o: LlcServiceAccessPointIdentifier.h
LlcServiceAccessPointIdentifier.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
LocationAreaIdentification.o: /usr/include/stdio.h /usr/include/features.h
LocationAreaIdentification.o: /usr/include/libio.h /usr/include/_G_config.h
LocationAreaIdentification.o: /usr/include/wchar.h /usr/include/xlocale.h
LocationAreaIdentification.o: /usr/include/stdlib.h /usr/include/alloca.h
LocationAreaIdentification.o: /usr/include/stdint.h
LocationAreaIdentification.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
LocationAreaIdentification.o: /usr/include/arpa/inet.h
LocationAreaIdentification.o: /usr/include/netinet/in.h /usr/include/endian.h
LocationAreaIdentification.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
LocationAreaIdentification.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
LocationAreaIdentification.o: LocationAreaIdentification.h
LocationAreaIdentification.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
MessageType.o: /usr/include/stdio.h /usr/include/features.h
MessageType.o: /usr/include/libio.h /usr/include/_G_config.h
MessageType.o: /usr/include/wchar.h /usr/include/xlocale.h
MessageType.o: /usr/include/stdlib.h /usr/include/alloca.h
MessageType.o: /usr/include/stdint.h
MessageType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
MessageType.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
MessageType.o: /usr/include/endian.h
MessageType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
MessageType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
MessageType.o: MessageType.h
MessageType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
MobileIdentity.o: /usr/include/stdio.h /usr/include/features.h
MobileIdentity.o: /usr/include/libio.h /usr/include/_G_config.h
MobileIdentity.o: /usr/include/wchar.h /usr/include/xlocale.h
MobileIdentity.o: /usr/include/stdlib.h /usr/include/alloca.h
MobileIdentity.o: /usr/include/stdint.h
MobileIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
MobileIdentity.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
MobileIdentity.o: /usr/include/endian.h
MobileIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
MobileIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
MobileIdentity.o: MobileIdentity.h
MobileIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
MobileStationClassmark2.o: /usr/include/stdio.h /usr/include/features.h
MobileStationClassmark2.o: /usr/include/libio.h /usr/include/_G_config.h
MobileStationClassmark2.o: /usr/include/wchar.h /usr/include/xlocale.h
MobileStationClassmark2.o: /usr/include/stdlib.h /usr/include/alloca.h
MobileStationClassmark2.o: /usr/include/stdint.h
MobileStationClassmark2.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
MobileStationClassmark2.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
MobileStationClassmark2.o: /usr/include/endian.h
MobileStationClassmark2.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
MobileStationClassmark2.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
MobileStationClassmark2.o: MobileStationClassmark2.h
MobileStationClassmark2.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
MobileStationClassmark3.o: /usr/include/stdio.h /usr/include/features.h
MobileStationClassmark3.o: /usr/include/libio.h /usr/include/_G_config.h
MobileStationClassmark3.o: /usr/include/wchar.h /usr/include/xlocale.h
MobileStationClassmark3.o: /usr/include/stdlib.h /usr/include/alloca.h
MobileStationClassmark3.o: /usr/include/stdint.h
MobileStationClassmark3.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
MobileStationClassmark3.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
MobileStationClassmark3.o: /usr/include/endian.h
MobileStationClassmark3.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
MobileStationClassmark3.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
MobileStationClassmark3.o: MobileStationClassmark3.h
MobileStationClassmark3.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
MsNetworkCapability.o: /usr/include/stdio.h /usr/include/features.h
MsNetworkCapability.o: /usr/include/libio.h /usr/include/_G_config.h
MsNetworkCapability.o: /usr/include/wchar.h /usr/include/xlocale.h
MsNetworkCapability.o: /usr/include/stdlib.h /usr/include/alloca.h
MsNetworkCapability.o: /usr/include/stdint.h
MsNetworkCapability.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
MsNetworkCapability.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
MsNetworkCapability.o: /usr/include/endian.h
MsNetworkCapability.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
MsNetworkCapability.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
MsNetworkCapability.o: MsNetworkCapability.h
MsNetworkCapability.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
NasKeySetIdentifier.o: /usr/include/stdio.h /usr/include/features.h
NasKeySetIdentifier.o: /usr/include/libio.h /usr/include/_G_config.h
NasKeySetIdentifier.o: /usr/include/wchar.h /usr/include/xlocale.h
NasKeySetIdentifier.o: /usr/include/stdlib.h /usr/include/alloca.h
NasKeySetIdentifier.o: /usr/include/stdint.h
NasKeySetIdentifier.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
NasKeySetIdentifier.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
NasKeySetIdentifier.o: /usr/include/endian.h
NasKeySetIdentifier.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
NasKeySetIdentifier.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
NasKeySetIdentifier.o: NasKeySetIdentifier.h
NasKeySetIdentifier.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
NasMessageContainer.o: /usr/include/stdio.h /usr/include/features.h
NasMessageContainer.o: /usr/include/libio.h /usr/include/_G_config.h
NasMessageContainer.o: /usr/include/wchar.h /usr/include/xlocale.h
NasMessageContainer.o: /usr/include/stdlib.h /usr/include/alloca.h
NasMessageContainer.o: /usr/include/stdint.h
NasMessageContainer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
NasMessageContainer.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
NasMessageContainer.o: /usr/include/endian.h
NasMessageContainer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
NasMessageContainer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
NasMessageContainer.o: NasMessageContainer.h
NasMessageContainer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
NasSecurityAlgorithms.o: /usr/include/stdio.h /usr/include/features.h
NasSecurityAlgorithms.o: /usr/include/libio.h /usr/include/_G_config.h
NasSecurityAlgorithms.o: /usr/include/wchar.h /usr/include/xlocale.h
NasSecurityAlgorithms.o: /usr/include/stdlib.h /usr/include/alloca.h
NasSecurityAlgorithms.o: /usr/include/stdint.h
NasSecurityAlgorithms.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
NasSecurityAlgorithms.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
NasSecurityAlgorithms.o: /usr/include/endian.h
NasSecurityAlgorithms.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
NasSecurityAlgorithms.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
NasSecurityAlgorithms.o: NasSecurityAlgorithms.h
NasSecurityAlgorithms.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
NetworkName.o: /usr/include/stdio.h /usr/include/features.h
NetworkName.o: /usr/include/libio.h /usr/include/_G_config.h
NetworkName.o: /usr/include/wchar.h /usr/include/xlocale.h
NetworkName.o: /usr/include/stdlib.h /usr/include/alloca.h
NetworkName.o: /usr/include/stdint.h
NetworkName.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
NetworkName.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
NetworkName.o: /usr/include/endian.h
NetworkName.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
NetworkName.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
NetworkName.o: NetworkName.h
NetworkName.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
Nonce.o: /usr/include/stdio.h /usr/include/features.h /usr/include/libio.h
Nonce.o: /usr/include/_G_config.h /usr/include/wchar.h /usr/include/xlocale.h
Nonce.o: /usr/include/stdlib.h /usr/include/alloca.h /usr/include/stdint.h
Nonce.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
Nonce.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
Nonce.o: /usr/include/endian.h
Nonce.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
Nonce.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
Nonce.o: Nonce.h
Nonce.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
PacketFlowIdentifier.o: /usr/include/stdio.h /usr/include/features.h
PacketFlowIdentifier.o: /usr/include/libio.h /usr/include/_G_config.h
PacketFlowIdentifier.o: /usr/include/wchar.h /usr/include/xlocale.h
PacketFlowIdentifier.o: /usr/include/stdlib.h /usr/include/alloca.h
PacketFlowIdentifier.o: /usr/include/stdint.h
PacketFlowIdentifier.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
PacketFlowIdentifier.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
PacketFlowIdentifier.o: /usr/include/endian.h
PacketFlowIdentifier.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
PacketFlowIdentifier.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
PacketFlowIdentifier.o: PacketFlowIdentifier.h
PacketFlowIdentifier.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
PagingIdentity.o: /usr/include/stdio.h /usr/include/features.h
PagingIdentity.o: /usr/include/libio.h /usr/include/_G_config.h
PagingIdentity.o: /usr/include/wchar.h /usr/include/xlocale.h
PagingIdentity.o: /usr/include/stdlib.h /usr/include/alloca.h
PagingIdentity.o: /usr/include/stdint.h
PagingIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
PagingIdentity.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
PagingIdentity.o: /usr/include/endian.h
PagingIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
PagingIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
PagingIdentity.o: PagingIdentity.h
PagingIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
PdnAddress.o: /usr/include/stdio.h /usr/include/features.h
PdnAddress.o: /usr/include/libio.h /usr/include/_G_config.h
PdnAddress.o: /usr/include/wchar.h /usr/include/xlocale.h
PdnAddress.o: /usr/include/stdlib.h /usr/include/alloca.h
PdnAddress.o: /usr/include/stdint.h
PdnAddress.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
PdnAddress.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
PdnAddress.o: /usr/include/endian.h
PdnAddress.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
PdnAddress.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
PdnAddress.o: PdnAddress.h
PdnAddress.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
PdnType.o: /usr/include/stdio.h /usr/include/features.h /usr/include/libio.h
PdnType.o: /usr/include/_G_config.h /usr/include/wchar.h
PdnType.o: /usr/include/xlocale.h /usr/include/stdlib.h /usr/include/alloca.h
PdnType.o: /usr/include/stdint.h
PdnType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
PdnType.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
PdnType.o: /usr/include/endian.h
PdnType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
PdnType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
PdnType.o: PdnType.h
PdnType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
PlmnList.o: /usr/include/stdio.h /usr/include/features.h /usr/include/libio.h
PlmnList.o: /usr/include/_G_config.h /usr/include/wchar.h
PlmnList.o: /usr/include/xlocale.h /usr/include/stdlib.h
PlmnList.o: /usr/include/alloca.h /usr/include/stdint.h
PlmnList.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
PlmnList.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
PlmnList.o: /usr/include/endian.h
PlmnList.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
PlmnList.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
PlmnList.o: PlmnList.h
PlmnList.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
ProcedureTransactionIdentity.o: /usr/include/stdio.h /usr/include/features.h
ProcedureTransactionIdentity.o: /usr/include/libio.h /usr/include/_G_config.h
ProcedureTransactionIdentity.o: /usr/include/wchar.h /usr/include/xlocale.h
ProcedureTransactionIdentity.o: /usr/include/stdlib.h /usr/include/alloca.h
ProcedureTransactionIdentity.o: /usr/include/stdint.h
ProcedureTransactionIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
ProcedureTransactionIdentity.o: /usr/include/arpa/inet.h
ProcedureTransactionIdentity.o: /usr/include/netinet/in.h
ProcedureTransactionIdentity.o: /usr/include/endian.h
ProcedureTransactionIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
ProcedureTransactionIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
ProcedureTransactionIdentity.o: ProcedureTransactionIdentity.h
ProcedureTransactionIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
ProtocolConfigurationOptions.o: /usr/include/stdio.h /usr/include/features.h
ProtocolConfigurationOptions.o: /usr/include/libio.h /usr/include/_G_config.h
ProtocolConfigurationOptions.o: /usr/include/wchar.h /usr/include/xlocale.h
ProtocolConfigurationOptions.o: /usr/include/stdlib.h /usr/include/alloca.h
ProtocolConfigurationOptions.o: /usr/include/stdint.h
ProtocolConfigurationOptions.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
ProtocolConfigurationOptions.o: /usr/include/arpa/inet.h
ProtocolConfigurationOptions.o: /usr/include/netinet/in.h
ProtocolConfigurationOptions.o: /usr/include/endian.h
ProtocolConfigurationOptions.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
ProtocolConfigurationOptions.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
ProtocolConfigurationOptions.o: ProtocolConfigurationOptions.h
ProtocolConfigurationOptions.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
ProtocolDiscriminator.o: /usr/include/stdio.h /usr/include/features.h
ProtocolDiscriminator.o: /usr/include/libio.h /usr/include/_G_config.h
ProtocolDiscriminator.o: /usr/include/wchar.h /usr/include/xlocale.h
ProtocolDiscriminator.o: /usr/include/stdlib.h /usr/include/alloca.h
ProtocolDiscriminator.o: /usr/include/stdint.h
ProtocolDiscriminator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
ProtocolDiscriminator.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
ProtocolDiscriminator.o: /usr/include/endian.h
ProtocolDiscriminator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
ProtocolDiscriminator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
ProtocolDiscriminator.o: ProtocolDiscriminator.h
ProtocolDiscriminator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
PTmsiSignature.o: /usr/include/stdio.h /usr/include/features.h
PTmsiSignature.o: /usr/include/libio.h /usr/include/_G_config.h
PTmsiSignature.o: /usr/include/wchar.h /usr/include/xlocale.h
PTmsiSignature.o: /usr/include/stdlib.h /usr/include/alloca.h
PTmsiSignature.o: /usr/include/stdint.h
PTmsiSignature.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
PTmsiSignature.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
PTmsiSignature.o: /usr/include/endian.h
PTmsiSignature.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
PTmsiSignature.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
PTmsiSignature.o: PTmsiSignature.h
PTmsiSignature.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
QualityOfService.o: /usr/include/stdio.h /usr/include/features.h
QualityOfService.o: /usr/include/libio.h /usr/include/_G_config.h
QualityOfService.o: /usr/include/wchar.h /usr/include/xlocale.h
QualityOfService.o: /usr/include/stdlib.h /usr/include/alloca.h
QualityOfService.o: /usr/include/stdint.h
QualityOfService.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
QualityOfService.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
QualityOfService.o: /usr/include/endian.h
QualityOfService.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
QualityOfService.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
QualityOfService.o: QualityOfService.h
QualityOfService.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
RadioPriority.o: /usr/include/stdio.h /usr/include/features.h
RadioPriority.o: /usr/include/libio.h /usr/include/_G_config.h
RadioPriority.o: /usr/include/wchar.h /usr/include/xlocale.h
RadioPriority.o: /usr/include/stdlib.h /usr/include/alloca.h
RadioPriority.o: /usr/include/stdint.h
RadioPriority.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
RadioPriority.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
RadioPriority.o: /usr/include/endian.h
RadioPriority.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
RadioPriority.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
RadioPriority.o: RadioPriority.h
RadioPriority.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
RequestType.o: /usr/include/stdio.h /usr/include/features.h
RequestType.o: /usr/include/libio.h /usr/include/_G_config.h
RequestType.o: /usr/include/wchar.h /usr/include/xlocale.h
RequestType.o: /usr/include/stdlib.h /usr/include/alloca.h
RequestType.o: /usr/include/stdint.h
RequestType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
RequestType.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
RequestType.o: /usr/include/endian.h
RequestType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
RequestType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
RequestType.o: RequestType.h
RequestType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
SecurityHeaderType.o: /usr/include/stdio.h /usr/include/features.h
SecurityHeaderType.o: /usr/include/libio.h /usr/include/_G_config.h
SecurityHeaderType.o: /usr/include/wchar.h /usr/include/xlocale.h
SecurityHeaderType.o: /usr/include/stdlib.h /usr/include/alloca.h
SecurityHeaderType.o: /usr/include/stdint.h
SecurityHeaderType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
SecurityHeaderType.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
SecurityHeaderType.o: /usr/include/endian.h
SecurityHeaderType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
SecurityHeaderType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
SecurityHeaderType.o: SecurityHeaderType.h
SecurityHeaderType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
ServiceType.o: /usr/include/stdio.h /usr/include/features.h
ServiceType.o: /usr/include/libio.h /usr/include/_G_config.h
ServiceType.o: /usr/include/wchar.h /usr/include/xlocale.h
ServiceType.o: /usr/include/stdlib.h /usr/include/alloca.h
ServiceType.o: /usr/include/stdint.h
ServiceType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
ServiceType.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
ServiceType.o: /usr/include/endian.h
ServiceType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
ServiceType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
ServiceType.o: ServiceType.h
ServiceType.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
ShortMac.o: /usr/include/stdio.h /usr/include/features.h /usr/include/libio.h
ShortMac.o: /usr/include/_G_config.h /usr/include/wchar.h
ShortMac.o: /usr/include/xlocale.h /usr/include/stdlib.h
ShortMac.o: /usr/include/alloca.h /usr/include/stdint.h
ShortMac.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
ShortMac.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
ShortMac.o: /usr/include/endian.h
ShortMac.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
ShortMac.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
ShortMac.o: ShortMac.h
ShortMac.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
SsCode.o: /usr/include/stdio.h /usr/include/features.h /usr/include/libio.h
SsCode.o: /usr/include/_G_config.h /usr/include/wchar.h
SsCode.o: /usr/include/xlocale.h /usr/include/stdlib.h /usr/include/alloca.h
SsCode.o: /usr/include/stdint.h
SsCode.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
SsCode.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
SsCode.o: /usr/include/endian.h
SsCode.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
SsCode.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
SsCode.o: SsCode.h
SsCode.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
SupportedCodecList.o: /usr/include/stdio.h /usr/include/features.h
SupportedCodecList.o: /usr/include/libio.h /usr/include/_G_config.h
SupportedCodecList.o: /usr/include/wchar.h /usr/include/xlocale.h
SupportedCodecList.o: /usr/include/stdlib.h /usr/include/alloca.h
SupportedCodecList.o: /usr/include/stdint.h
SupportedCodecList.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
SupportedCodecList.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
SupportedCodecList.o: /usr/include/endian.h
SupportedCodecList.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
SupportedCodecList.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
SupportedCodecList.o: SupportedCodecList.h
SupportedCodecList.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
TimeZoneAndTime.o: /usr/include/stdio.h /usr/include/features.h
TimeZoneAndTime.o: /usr/include/libio.h /usr/include/_G_config.h
TimeZoneAndTime.o: /usr/include/wchar.h /usr/include/xlocale.h
TimeZoneAndTime.o: /usr/include/stdlib.h /usr/include/alloca.h
TimeZoneAndTime.o: /usr/include/stdint.h
TimeZoneAndTime.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
TimeZoneAndTime.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
TimeZoneAndTime.o: /usr/include/endian.h
TimeZoneAndTime.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
TimeZoneAndTime.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
TimeZoneAndTime.o: TimeZoneAndTime.h
TimeZoneAndTime.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
TimeZone.o: /usr/include/stdio.h /usr/include/features.h /usr/include/libio.h
TimeZone.o: /usr/include/_G_config.h /usr/include/wchar.h
TimeZone.o: /usr/include/xlocale.h /usr/include/stdlib.h
TimeZone.o: /usr/include/alloca.h /usr/include/stdint.h
TimeZone.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
TimeZone.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
TimeZone.o: /usr/include/endian.h
TimeZone.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
TimeZone.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
TimeZone.o: TimeZone.h
TimeZone.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
TmsiStatus.o: /usr/include/stdio.h /usr/include/features.h
TmsiStatus.o: /usr/include/libio.h /usr/include/_G_config.h
TmsiStatus.o: /usr/include/wchar.h /usr/include/xlocale.h
TmsiStatus.o: /usr/include/stdlib.h /usr/include/alloca.h
TmsiStatus.o: /usr/include/stdint.h
TmsiStatus.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
TmsiStatus.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
TmsiStatus.o: /usr/include/endian.h
TmsiStatus.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
TmsiStatus.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
TmsiStatus.o: TmsiStatus.h
TmsiStatus.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
TrackingAreaIdentity.o: /usr/include/stdio.h /usr/include/features.h
TrackingAreaIdentity.o: /usr/include/libio.h /usr/include/_G_config.h
TrackingAreaIdentity.o: /usr/include/wchar.h /usr/include/xlocale.h
TrackingAreaIdentity.o: /usr/include/stdlib.h /usr/include/alloca.h
TrackingAreaIdentity.o: /usr/include/stdint.h
TrackingAreaIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
TrackingAreaIdentity.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
TrackingAreaIdentity.o: /usr/include/endian.h
TrackingAreaIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
TrackingAreaIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
TrackingAreaIdentity.o: TrackingAreaIdentity.h
TrackingAreaIdentity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
TrackingAreaIdentityList.o: /usr/include/stdio.h /usr/include/features.h
TrackingAreaIdentityList.o: /usr/include/libio.h /usr/include/_G_config.h
TrackingAreaIdentityList.o: /usr/include/wchar.h /usr/include/xlocale.h
TrackingAreaIdentityList.o: /usr/include/stdlib.h /usr/include/alloca.h
TrackingAreaIdentityList.o: /usr/include/stdint.h
TrackingAreaIdentityList.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
TrackingAreaIdentityList.o: /usr/include/arpa/inet.h
TrackingAreaIdentityList.o: /usr/include/netinet/in.h /usr/include/endian.h
TrackingAreaIdentityList.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
TrackingAreaIdentityList.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
TrackingAreaIdentityList.o: TrackingAreaIdentityList.h
TrackingAreaIdentityList.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
TrafficFlowAggregateDescription.o: /usr/include/stdio.h
TrafficFlowAggregateDescription.o: /usr/include/features.h
TrafficFlowAggregateDescription.o: /usr/include/libio.h
TrafficFlowAggregateDescription.o: /usr/include/_G_config.h
TrafficFlowAggregateDescription.o: /usr/include/wchar.h
TrafficFlowAggregateDescription.o: /usr/include/xlocale.h
TrafficFlowAggregateDescription.o: /usr/include/stdlib.h
TrafficFlowAggregateDescription.o: /usr/include/alloca.h
TrafficFlowAggregateDescription.o: /usr/include/stdint.h
TrafficFlowAggregateDescription.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
TrafficFlowAggregateDescription.o: /usr/include/arpa/inet.h
TrafficFlowAggregateDescription.o: /usr/include/netinet/in.h
TrafficFlowAggregateDescription.o: /usr/include/endian.h
TrafficFlowAggregateDescription.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
TrafficFlowAggregateDescription.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
TrafficFlowAggregateDescription.o: TrafficFlowAggregateDescription.h
TrafficFlowAggregateDescription.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
TrafficFlowTemplate.o: /usr/include/stdio.h /usr/include/features.h
TrafficFlowTemplate.o: /usr/include/libio.h /usr/include/_G_config.h
TrafficFlowTemplate.o: /usr/include/wchar.h /usr/include/xlocale.h
TrafficFlowTemplate.o: /usr/include/stdlib.h /usr/include/alloca.h
TrafficFlowTemplate.o: /usr/include/stdint.h
TrafficFlowTemplate.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
TrafficFlowTemplate.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
TrafficFlowTemplate.o: /usr/include/endian.h
TrafficFlowTemplate.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
TrafficFlowTemplate.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
TrafficFlowTemplate.o: TrafficFlowTemplate.h
TransactionIdentifier.o: /usr/include/stdio.h /usr/include/features.h
TransactionIdentifier.o: /usr/include/libio.h /usr/include/_G_config.h
TransactionIdentifier.o: /usr/include/wchar.h /usr/include/xlocale.h
TransactionIdentifier.o: /usr/include/stdlib.h /usr/include/alloca.h
TransactionIdentifier.o: /usr/include/stdint.h
TransactionIdentifier.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
TransactionIdentifier.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
TransactionIdentifier.o: /usr/include/endian.h
TransactionIdentifier.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
TransactionIdentifier.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
TransactionIdentifier.o: TransactionIdentifier.h
TransactionIdentifier.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
UeNetworkCapability.o: /usr/include/stdio.h /usr/include/features.h
UeNetworkCapability.o: /usr/include/libio.h /usr/include/_G_config.h
UeNetworkCapability.o: /usr/include/wchar.h /usr/include/xlocale.h
UeNetworkCapability.o: /usr/include/stdlib.h /usr/include/alloca.h
UeNetworkCapability.o: /usr/include/stdint.h
UeNetworkCapability.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
UeNetworkCapability.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
UeNetworkCapability.o: /usr/include/endian.h
UeNetworkCapability.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
UeNetworkCapability.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
UeNetworkCapability.o: UeNetworkCapability.h
UeNetworkCapability.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
UeRadioCapabilityInformationUpdateNeeded.o: /usr/include/stdio.h
UeRadioCapabilityInformationUpdateNeeded.o: /usr/include/features.h
UeRadioCapabilityInformationUpdateNeeded.o: /usr/include/libio.h
UeRadioCapabilityInformationUpdateNeeded.o: /usr/include/_G_config.h
UeRadioCapabilityInformationUpdateNeeded.o: /usr/include/wchar.h
UeRadioCapabilityInformationUpdateNeeded.o: /usr/include/xlocale.h
UeRadioCapabilityInformationUpdateNeeded.o: /usr/include/stdlib.h
UeRadioCapabilityInformationUpdateNeeded.o: /usr/include/alloca.h
UeRadioCapabilityInformationUpdateNeeded.o: /usr/include/stdint.h
UeRadioCapabilityInformationUpdateNeeded.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
UeRadioCapabilityInformationUpdateNeeded.o: /usr/include/arpa/inet.h
UeRadioCapabilityInformationUpdateNeeded.o: /usr/include/netinet/in.h
UeRadioCapabilityInformationUpdateNeeded.o: /usr/include/endian.h
UeRadioCapabilityInformationUpdateNeeded.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
UeRadioCapabilityInformationUpdateNeeded.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
UeRadioCapabilityInformationUpdateNeeded.o: UeRadioCapabilityInformationUpdateNeeded.h
UeRadioCapabilityInformationUpdateNeeded.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
UeSecurityCapability.o: /usr/include/stdio.h /usr/include/features.h
UeSecurityCapability.o: /usr/include/libio.h /usr/include/_G_config.h
UeSecurityCapability.o: /usr/include/wchar.h /usr/include/xlocale.h
UeSecurityCapability.o: /usr/include/stdlib.h /usr/include/alloca.h
UeSecurityCapability.o: /usr/include/stdint.h
UeSecurityCapability.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVEncoder.h
UeSecurityCapability.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
UeSecurityCapability.o: /usr/include/endian.h
UeSecurityCapability.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/TLVDecoder.h
UeSecurityCapability.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
UeSecurityCapability.o: UeSecurityCapability.h
UeSecurityCapability.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
