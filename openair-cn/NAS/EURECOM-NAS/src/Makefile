ifndef PROJDIR
PROJDIR = $(PWD)/..
endif

include $(PROJDIR)/Makerules
include $(PROJDIR)/Makefile.inc

export LD_RUN_PATH = $(LIBDIR):$(LIBPROCESS)

TARGET = $(PROCESS)process

# Define the version number
DEFINES += -D'FIRMWARE_VERSION="$(SVN_REV) - $(DATE_REV)"'

ifeq ($(TARGET), MMEprocess)
  SRCS = MMEprocess.c nas_network.c nas_parser.c nas_proc.c
endif
ifeq ($(TARGET), UEprocess)
  SRCS = UEprocess.c nas_network.c nas_parser.c nas_proc.c nas_user.c
endif

LIBS		= -lutil -lapi -lEMMmsg -lESMmsg -lEMMsap -lESMsap -lies -lrt
INCLUDES	= -I. -I$(INCDIR) -I$(UTILDIR) -I$(USERAPIDIR) -I$(NETAPIDIR) \
		-I$(EMMDIR) -I$(EMMMSGDIR) -I$(EMMSAPDIR) \
		-I$(ESMDIR) -I$(ESMMSGDIR) -I$(ESMSAPDIR) \
		-I$(IESDIR)

.PHONY: $(TARGET)

%.o: %.c Makefile $(PROJDIR)/Makerules $(PROJDIR)/Makefile.inc
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(EMMDIR)/*.o $(ESMDIR)/*.o $(LIBS)
	@echo Replacing $@ to $(BINDIR)
	@$(RM) $(BINDIR)/$@
	@$(CP) $@ $(BINDIR)

# Always build the main object file which contains the version number
$(TARGET).o: .FORCE
.FORCE:

clean:
	$(RM) $(OBJS) *.bak *~

veryclean: clean
	$(RM) UE$(TARGET) MME$(TARGET)

depend:
	makedepend -- ${CFLAGS} -- ${SRCS}

# DO NOT DELETE THIS LINE -- make depend depends on it.

MMEprocess.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
MMEprocess.o: /usr/include/stdint.h /usr/include/features.h
MMEprocess.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
MMEprocess.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
MMEprocess.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/api/network/network_api.h
MMEprocess.o: nas_network.h nas_parser.h /usr/include/stdlib.h
MMEprocess.o: /usr/include/alloca.h /usr/include/poll.h /usr/include/string.h
MMEprocess.o: /usr/include/xlocale.h /usr/include/signal.h
MMEprocess.o: /usr/include/time.h /usr/include/pthread.h
MMEprocess.o: /usr/include/endian.h /usr/include/sched.h
nas_network.o: nas_network.h
nas_network.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
nas_network.o: /usr/include/stdint.h /usr/include/features.h
nas_network.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
nas_network.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/api/network/as_message.h
nas_network.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
nas_network.o: nas_proc.h
nas_parser.o: nas_parser.h
nas_parser.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/parser.h
nas_parser.o: /usr/include/stdio.h /usr/include/features.h
nas_parser.o: /usr/include/libio.h /usr/include/_G_config.h
nas_parser.o: /usr/include/wchar.h /usr/include/stdlib.h
nas_parser.o: /usr/include/alloca.h
nas_proc.o: nas_proc.h
nas_proc.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
nas_proc.o: /usr/include/stdint.h /usr/include/features.h
nas_proc.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
nas_proc.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
nas_proc.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/emm_main.h
nas_proc.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_sap.h
nas_proc.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_regDef.h
nas_proc.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_esmDef.h
nas_proc.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
nas_proc.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/sap/emm_asDef.h
nas_proc.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/esm_main.h
nas_proc.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/sap/esm_sap.h
nas_proc.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/esm/sap/esm_sapDef.h
nas_proc.o: /usr/include/stdio.h /usr/include/libio.h
nas_proc.o: /usr/include/_G_config.h /usr/include/wchar.h
nas_user.o: nas_user.h
nas_user.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
nas_user.o: /usr/include/stdint.h /usr/include/features.h
nas_user.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
nas_user.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/userDef.h
nas_user.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
nas_user.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/memory.h
nas_user.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/api/user/at_command.h
nas_user.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/api/user/at_response.h
nas_user.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/api/user/at_error.h
nas_user.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/api/user/user_indication.h
nas_user.o: nas_proc.h /usr/include/string.h /usr/include/xlocale.h
nas_user.o: /usr/include/stdlib.h /usr/include/alloca.h
UEprocess.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
UEprocess.o: /usr/include/stdint.h /usr/include/features.h
UEprocess.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_log.h
UEprocess.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
UEprocess.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/api/user/user_api.h
UEprocess.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
UEprocess.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/api/network/network_api.h
UEprocess.o: nas_user.h nas_network.h nas_parser.h /usr/include/stdlib.h
UEprocess.o: /usr/include/alloca.h /usr/include/poll.h /usr/include/string.h
UEprocess.o: /usr/include/xlocale.h /usr/include/signal.h /usr/include/time.h
UEprocess.o: /usr/include/pthread.h /usr/include/endian.h
UEprocess.o: /usr/include/sched.h
