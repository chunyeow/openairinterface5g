export PROCESS = UE

ifndef PROJDIR
PROJDIR = $(PWD)/..
endif

include $(PROJDIR)/Makerules
include $(PROJDIR)/Makefile.inc

export LD_RUN_PATH = $(LIBDIR):$(LIBPROCESS)

LIBS		= -lapi -lutil -lEMMmsg -lESMmsg -lies -lrt
INCLUDES	= -I. -I$(INCDIR) -I$(UTILDIR) -I$(USIMAPIDIR) -I$(EMMDIR)

LIBSUTIL	= $(LIBDIR)/$(LIBUTIL).a $(LIBDIR)/$(LIBUTIL).so

USIM_OBJ	= usim_data.o
UE_OBJ		= ue_data.o

USIM_TARGET	= usim_data
UE_TARGET	= ue_data

TARGETS = $(USIM_TARGET) $(UE_TARGET)

all: $(TARGETS)

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

$(USIM_TARGET): $(USIM_OBJ) $(LIBSUTIL)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	@echo Replacing $@ to $(BINDIR)
	@$(RM) $(BINDIR)/$@
	@$(CP) $@ $(BINDIR)

$(UE_TARGET): $(UE_OBJ) $(LIBSUTIL)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)
	@echo Replacing $@ to $(BINDIR)
	@$(RM) $(BINDIR)/$@
	@$(CP) $@ $(BINDIR)

clean:
	$(RM) $(OBJS) *.bak *~

veryclean: clean
	$(RM) $(TARGETS)

depend:
	makedepend -- ${CFLAGS} -- ${SRCS}

# DO NOT DELETE THIS LINE -- make depend depends on it.

ue_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/userDef.h
ue_data.o: /usr/include/stdint.h /usr/include/features.h
ue_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/memory.h
ue_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/emm/emmData.h
ue_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
ue_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
ue_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/securityDef.h
ue_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
ue_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/nas_timer.h
ue_data.o: /usr/include/stdio.h /usr/include/libio.h /usr/include/_G_config.h
ue_data.o: /usr/include/wchar.h /usr/include/stdlib.h /usr/include/alloca.h
ue_data.o: /usr/include/string.h /usr/include/xlocale.h
usim_data.o: network.h
usim_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
usim_data.o: /usr/include/stdint.h /usr/include/features.h
usim_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/networkDef.h
usim_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/api/usim/usim_api.h
usim_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/OctetString.h
usim_data.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/memory.h
usim_data.o: /usr/include/stdio.h /usr/include/libio.h
usim_data.o: /usr/include/_G_config.h /usr/include/wchar.h
usim_data.o: /usr/include/stdlib.h /usr/include/alloca.h
usim_data.o: /usr/include/string.h /usr/include/xlocale.h
