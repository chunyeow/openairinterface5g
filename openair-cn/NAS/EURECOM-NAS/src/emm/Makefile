ifndef PROJDIR
PROJDIR = $(PWD)/../..
endif

include $(PROJDIR)/Makerules
include $(PROJDIR)/Makefile.inc

INCLUDES = -I. -I$(INCDIR) -I$(UTILDIR) -I$(USIMAPIDIR) -I$(MMEAPIDIR) \
	-I$(ESMDIR) -I$(EMMSAPDIR) -I$(ESMSAPDIR) -I$(EMMMSGDIR) -I$(IESDIR)

all: $(OBJS)
	@$(CD) $(EMMMSGDIR) && $(MAKE)
	@$(CD) $(EMMSAPDIR) && $(MAKE)

clean:
	$(RM) $(OBJS) *.bak *~
	@$(CD) $(EMMSAPDIR) && $(MAKE) $@

veryclean: clean
	@$(CD) $(EMMSAPDIR) && $(MAKE) $@
	@$(CD) $(EMMMSGDIR) && $(MAKE) $@
	$(RM) $(TARGET)

%.o: %.c Makefile $(PROJDIR)/Makerules $(PROJDIR)/Makefile.inc
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

depend:
	makedepend -- ${CFLAGS} -- ${SRCS}

# DO NOT DELETE THIS LINE -- make depend depends on it.

Attach.o: emm_proc.h
Attach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
Attach.o: /usr/include/stdint.h /usr/include/features.h
Attach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
Attach.o: LowerLayer.h
Attach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
Attach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
Attach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
Attach.o: emmData.h
Attach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/securityDef.h
Attach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_sap.h
Attach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_regDef.h
Attach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_esmDef.h
Attach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_asDef.h
Attach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/sap/esm_sap.h
Attach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/sap/esm_sapDef.h
Attach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/emm_cause.h
Attach.o: /usr/include/string.h /usr/include/xlocale.h /usr/include/stdlib.h
Attach.o: /usr/include/alloca.h
Authentication.o: emm_proc.h
Authentication.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
Authentication.o: /usr/include/stdint.h /usr/include/features.h
Authentication.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
Authentication.o: LowerLayer.h
Authentication.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
Authentication.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
Authentication.o: emmData.h
Authentication.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
Authentication.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/securityDef.h
Authentication.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_sap.h
Authentication.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_regDef.h
Authentication.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_esmDef.h
Authentication.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_asDef.h
Authentication.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/emm_cause.h
Authentication.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/api/usim/usim_api.h
Authentication.o: /usr/include/stdlib.h /usr/include/alloca.h
Authentication.o: /usr/include/string.h /usr/include/xlocale.h
Detach.o: emm_proc.h
Detach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
Detach.o: /usr/include/stdint.h /usr/include/features.h
Detach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
Detach.o: LowerLayer.h
Detach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
Detach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
Detach.o: emmData.h
Detach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
Detach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/securityDef.h
Detach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_sap.h
Detach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_regDef.h
Detach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_esmDef.h
Detach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_asDef.h
Detach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/sap/esm_sap.h
Detach.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/sap/esm_sapDef.h
Detach.o: /usr/include/stdlib.h /usr/include/alloca.h
emm_data_ctx.o: /usr/include/stdlib.h /usr/include/features.h
emm_data_ctx.o: /usr/include/alloca.h /usr/include/string.h
emm_data_ctx.o: /usr/include/xlocale.h
emm_main.o: emm_main.h
emm_main.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
emm_main.o: /usr/include/stdint.h /usr/include/features.h
emm_main.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
emm_main.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
emm_main.o: emmData.h
emm_main.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/securityDef.h
emm_main.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
emm_main.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
emm_main.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/memory.h
emm_main.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/api/usim/usim_api.h
emm_main.o: IdleMode.h /usr/include/string.h /usr/include/xlocale.h
emm_main.o: /usr/include/stdio.h /usr/include/libio.h
emm_main.o: /usr/include/_G_config.h /usr/include/wchar.h
emm_main.o: /usr/include/stdlib.h /usr/include/alloca.h
EmmStatusHdl.o: emm_proc.h
EmmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
EmmStatusHdl.o: /usr/include/stdint.h /usr/include/features.h
EmmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
EmmStatusHdl.o: LowerLayer.h
EmmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
EmmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/emm_cause.h
EmmStatusHdl.o: emmData.h
EmmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
EmmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/securityDef.h
EmmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
EmmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_sap.h
EmmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_regDef.h
EmmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_esmDef.h
EmmStatusHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_asDef.h
Identification.o: emm_proc.h
Identification.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
Identification.o: /usr/include/stdint.h /usr/include/features.h
Identification.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
Identification.o: LowerLayer.h
Identification.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
Identification.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
Identification.o: emmData.h
Identification.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
Identification.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/securityDef.h
Identification.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_sap.h
Identification.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_regDef.h
Identification.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_esmDef.h
Identification.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_asDef.h
Identification.o: /usr/include/stdlib.h /usr/include/alloca.h
Identification.o: /usr/include/string.h /usr/include/xlocale.h
IdleMode.o: emm_proc.h
IdleMode.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
IdleMode.o: /usr/include/stdint.h /usr/include/features.h
IdleMode.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
IdleMode.o: LowerLayer.h
IdleMode.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
IdleMode.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_sap.h
IdleMode.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_regDef.h
IdleMode.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_esmDef.h
IdleMode.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_asDef.h
IdleMode.o: IdleMode.h emmData.h
IdleMode.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
IdleMode.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/securityDef.h
IdleMode.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
IdleMode.o: /usr/include/assert.h /usr/include/stdio.h /usr/include/libio.h
IdleMode.o: /usr/include/_G_config.h /usr/include/wchar.h
IdleMode.o: /usr/include/xlocale.h /usr/include/string.h
LowerLayer.o: LowerLayer.h
LowerLayer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
LowerLayer.o: /usr/include/stdint.h /usr/include/features.h
LowerLayer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
LowerLayer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
LowerLayer.o: emmData.h
LowerLayer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
LowerLayer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/securityDef.h
LowerLayer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
LowerLayer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_sap.h
LowerLayer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_regDef.h
LowerLayer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_esmDef.h
LowerLayer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_asDef.h
LowerLayer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/sap/esm_sap.h
LowerLayer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/sap/esm_sapDef.h
LowerLayer.o: /usr/include/string.h /usr/include/xlocale.h
SecurityModeControl.o: emm_proc.h
SecurityModeControl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
SecurityModeControl.o: /usr/include/stdint.h /usr/include/features.h
SecurityModeControl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
SecurityModeControl.o: LowerLayer.h
SecurityModeControl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
SecurityModeControl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
SecurityModeControl.o: emmData.h
SecurityModeControl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
SecurityModeControl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/securityDef.h
SecurityModeControl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_sap.h
SecurityModeControl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_regDef.h
SecurityModeControl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_esmDef.h
SecurityModeControl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_asDef.h
SecurityModeControl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/msg/emm_cause.h
SecurityModeControl.o: /usr/include/stdlib.h /usr/include/alloca.h
SecurityModeControl.o: /usr/include/string.h /usr/include/xlocale.h
ServiceRequestHdl.o: emm_proc.h
ServiceRequestHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
ServiceRequestHdl.o: /usr/include/stdint.h /usr/include/features.h
ServiceRequestHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
ServiceRequestHdl.o: LowerLayer.h
ServiceRequestHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
ServiceRequestHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
ServiceRequestHdl.o: emmData.h
ServiceRequestHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
ServiceRequestHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/securityDef.h
ServiceRequestHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_sap.h
ServiceRequestHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_regDef.h
ServiceRequestHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_esmDef.h
ServiceRequestHdl.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_asDef.h
TrackingAreaUpdate.o: emm_proc.h
TrackingAreaUpdate.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
TrackingAreaUpdate.o: /usr/include/stdint.h /usr/include/features.h
TrackingAreaUpdate.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
TrackingAreaUpdate.o: LowerLayer.h
TrackingAreaUpdate.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
TrackingAreaUpdate.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
TrackingAreaUpdate.o: emmData.h
TrackingAreaUpdate.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
TrackingAreaUpdate.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/securityDef.h
TrackingAreaUpdate.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_sap.h
TrackingAreaUpdate.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_regDef.h
TrackingAreaUpdate.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_esmDef.h
TrackingAreaUpdate.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_asDef.h
