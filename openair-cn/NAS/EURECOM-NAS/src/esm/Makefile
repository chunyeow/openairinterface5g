ifndef PROJDIR
PROJDIR = $(PWD)/../..
endif

include $(PROJDIR)/Makerules
include $(PROJDIR)/Makefile.inc

INCLUDES = -I. -I$(INCDIR) -I$(UTILDIR) -I$(IESDIR) -I$(MMEAPIDIR) \
	-I$(EMMSAPDIR) -I$(ESMSAPDIR) -I$(ESMMSGDIR) -I$(EMMDIR)

all: $(OBJS)
	@$(CD) $(ESMMSGDIR) && $(MAKE)
	@$(CD) $(ESMSAPDIR) && $(MAKE)

clean:
	$(RM) $(OBJS) *.bak *~
	@$(CD) $(ESMSAPDIR) && $(MAKE) $@

veryclean: clean
	@$(CD) $(ESMSAPDIR) && $(MAKE) $@
	@$(CD) $(ESMMSGDIR) && $(MAKE) $@
	$(RM) $(TARGET)

%.o: %.c Makefile $(PROJDIR)/Makerules $(PROJDIR)/Makefile.inc
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

depend:
	makedepend -- ${CFLAGS} -- ${SRCS}

# DO NOT DELETE THIS LINE -- make depend depends on it.

DedicatedEpsBearerContextActivation.o: esm_proc.h
DedicatedEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
DedicatedEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
DedicatedEpsBearerContextActivation.o: /usr/include/stdint.h
DedicatedEpsBearerContextActivation.o: /usr/include/features.h
DedicatedEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
DedicatedEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
DedicatedEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/esm_cause.h
DedicatedEpsBearerContextActivation.o: esm_ebr.h
DedicatedEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
DedicatedEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsBearerIdentity.h
DedicatedEpsBearerContextActivation.o: /usr/include/stdio.h
DedicatedEpsBearerContextActivation.o: /usr/include/libio.h
DedicatedEpsBearerContextActivation.o: /usr/include/_G_config.h
DedicatedEpsBearerContextActivation.o: /usr/include/wchar.h
DedicatedEpsBearerContextActivation.o: /usr/include/stdlib.h
DedicatedEpsBearerContextActivation.o: /usr/include/alloca.h
DedicatedEpsBearerContextActivation.o: esm_ebr_context.h
DedicatedEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_sap.h
DedicatedEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_regDef.h
DedicatedEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_esmDef.h
DedicatedEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_asDef.h
DefaultEpsBearerContextActivation.o: esm_proc.h
DefaultEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
DefaultEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
DefaultEpsBearerContextActivation.o: /usr/include/stdint.h
DefaultEpsBearerContextActivation.o: /usr/include/features.h
DefaultEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
DefaultEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
DefaultEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/esm_cause.h
DefaultEpsBearerContextActivation.o: esm_ebr.h
DefaultEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
DefaultEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsBearerIdentity.h
DefaultEpsBearerContextActivation.o: /usr/include/stdio.h
DefaultEpsBearerContextActivation.o: /usr/include/libio.h
DefaultEpsBearerContextActivation.o: /usr/include/_G_config.h
DefaultEpsBearerContextActivation.o: /usr/include/wchar.h
DefaultEpsBearerContextActivation.o: /usr/include/stdlib.h
DefaultEpsBearerContextActivation.o: /usr/include/alloca.h esm_ebr_context.h
DefaultEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_sap.h
DefaultEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_regDef.h
DefaultEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_esmDef.h
DefaultEpsBearerContextActivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_asDef.h
EpsBearerContextDeactivation.o: esm_proc.h
EpsBearerContextDeactivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
EpsBearerContextDeactivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
EpsBearerContextDeactivation.o: /usr/include/stdint.h /usr/include/features.h
EpsBearerContextDeactivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
EpsBearerContextDeactivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
EpsBearerContextDeactivation.o: esmData.h /usr/include/stdio.h
EpsBearerContextDeactivation.o: /usr/include/libio.h /usr/include/_G_config.h
EpsBearerContextDeactivation.o: /usr/include/wchar.h
EpsBearerContextDeactivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/esm_cause.h
EpsBearerContextDeactivation.o: esm_ebr.h
EpsBearerContextDeactivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
EpsBearerContextDeactivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsBearerIdentity.h
EpsBearerContextDeactivation.o: /usr/include/stdlib.h /usr/include/alloca.h
EpsBearerContextDeactivation.o: esm_ebr_context.h esm_main.h
EpsBearerContextDeactivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_sap.h
EpsBearerContextDeactivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_regDef.h
EpsBearerContextDeactivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_esmDef.h
EpsBearerContextDeactivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_asDef.h
EpsBearerContextDeactivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/sap/esm_sap.h
EpsBearerContextDeactivation.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/sap/esm_sapDef.h
esm_ebr.o: esm_ebr.h
esm_ebr.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
esm_ebr.o: /usr/include/stdint.h /usr/include/features.h
esm_ebr.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
esm_ebr.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
esm_ebr.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsBearerIdentity.h
esm_ebr.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
esm_ebr.o: /usr/include/wchar.h /usr/include/stdlib.h /usr/include/alloca.h
esm_ebr.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
esm_ebr.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
esm_ebr.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/api/mme/mme_api.h
esm_ebr.o: /usr/include/string.h /usr/include/xlocale.h
esm_ebr_context.o: esm_ebr_context.h
esm_ebr_context.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
esm_ebr_context.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
esm_ebr_context.o: /usr/include/stdint.h /usr/include/features.h
esm_ebr_context.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
esm_ebr_context.o: esmData.h
esm_ebr_context.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
esm_ebr_context.o: /usr/include/stdio.h /usr/include/libio.h
esm_ebr_context.o: /usr/include/_G_config.h /usr/include/wchar.h esm_ebr.h
esm_ebr_context.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
esm_ebr_context.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsBearerIdentity.h
esm_ebr_context.o: /usr/include/stdlib.h /usr/include/alloca.h
esm_ebr_context.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_sap.h
esm_ebr_context.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_regDef.h
esm_ebr_context.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_esmDef.h
esm_ebr_context.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_asDef.h
esm_ebr_context.o: /usr/include/string.h /usr/include/xlocale.h
esm_ip.o: esmData.h
esm_ip.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
esm_ip.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
esm_ip.o: /usr/include/stdint.h /usr/include/features.h /usr/include/stdio.h
esm_ip.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
esm_main.o: esm_main.h
esm_main.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
esm_main.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
esm_main.o: /usr/include/stdint.h /usr/include/features.h
esm_main.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
esm_main.o: esmData.h
esm_main.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
esm_main.o: /usr/include/stdio.h /usr/include/libio.h
esm_main.o: /usr/include/_G_config.h /usr/include/wchar.h esm_pt.h
esm_main.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
esm_main.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ProcedureTransactionIdentity.h
esm_main.o: /usr/include/stdlib.h /usr/include/alloca.h esm_ebr.h
esm_main.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/EpsBearerIdentity.h
esm_pt.o: esm_pt.h
esm_pt.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
esm_pt.o: /usr/include/stdint.h /usr/include/features.h
esm_pt.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
esm_pt.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ProcedureTransactionIdentity.h
esm_pt.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
esm_pt.o: /usr/include/wchar.h /usr/include/stdlib.h /usr/include/alloca.h
esm_pt.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
esm_pt.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
esm_pt.o: /usr/include/string.h /usr/include/xlocale.h
EsmStatusHdl.o: esm_proc.h
EsmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
EsmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
EsmStatusHdl.o: /usr/include/stdint.h /usr/include/features.h
EsmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
EsmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
EsmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/esm_cause.h
EsmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_sap.h
EsmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_regDef.h
EsmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_esmDef.h
EsmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_asDef.h
PdnConnectivity.o: esm_proc.h
PdnConnectivity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
PdnConnectivity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
PdnConnectivity.o: /usr/include/stdint.h /usr/include/features.h
PdnConnectivity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
PdnConnectivity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
PdnConnectivity.o: esmData.h /usr/include/stdio.h /usr/include/libio.h
PdnConnectivity.o: /usr/include/_G_config.h /usr/include/wchar.h
PdnConnectivity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/esm_cause.h
PdnConnectivity.o: esm_pt.h
PdnConnectivity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
PdnConnectivity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ProcedureTransactionIdentity.h
PdnConnectivity.o: /usr/include/stdlib.h /usr/include/alloca.h
PdnConnectivity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_sap.h
PdnConnectivity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_regDef.h
PdnConnectivity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_esmDef.h
PdnConnectivity.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_asDef.h
PdnConnectivity.o: /usr/include/string.h /usr/include/xlocale.h
PdnDisconnect.o: esm_proc.h
PdnDisconnect.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
PdnDisconnect.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
PdnDisconnect.o: /usr/include/stdint.h /usr/include/features.h
PdnDisconnect.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
PdnDisconnect.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
PdnDisconnect.o: esmData.h /usr/include/stdio.h /usr/include/libio.h
PdnDisconnect.o: /usr/include/_G_config.h /usr/include/wchar.h
PdnDisconnect.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/msg/esm_cause.h
PdnDisconnect.o: esm_pt.h
PdnDisconnect.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
PdnDisconnect.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/ies/ProcedureTransactionIdentity.h
PdnDisconnect.o: /usr/include/stdlib.h /usr/include/alloca.h
PdnDisconnect.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/sap/esm_sap.h
PdnDisconnect.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/sap/esm_sapDef.h
PdnDisconnect.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_sap.h
PdnDisconnect.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_regDef.h
PdnDisconnect.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_esmDef.h
PdnDisconnect.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_asDef.h
