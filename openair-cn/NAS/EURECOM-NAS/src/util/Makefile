ifndef PROJDIR
PROJDIR = $(PWD)/../..
endif

include $(PROJDIR)/Makerules
include $(PROJDIR)/Makefile.inc

INCLUDES = -I. -I$(INCDIR)

TARGET = $(LIBUTIL)
TARGETS = $(TARGET).a $(TARGET).so

all: $(TARGETS)

%.o: %.c Makefile
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

device.o: device.h
device.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
device.o: /usr/include/stdint.h /usr/include/features.h /usr/include/stdio.h
device.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
device.o: /usr/include/stdlib.h /usr/include/alloca.h /usr/include/string.h
device.o: /usr/include/xlocale.h /usr/include/unistd.h /usr/include/getopt.h
device.o: /usr/include/fcntl.h /usr/include/time.h
memory.o: memory.h
memory.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
memory.o: /usr/include/stdint.h /usr/include/features.h /usr/include/stdio.h
memory.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
memory.o: /usr/include/stdlib.h /usr/include/alloca.h /usr/include/string.h
memory.o: /usr/include/xlocale.h
nas_log.o: nas_log.h /usr/include/stdio.h /usr/include/features.h
nas_log.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
nas_timer.o: nas_timer.h
nas_timer.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
nas_timer.o: /usr/include/stdint.h /usr/include/features.h
nas_timer.o: /usr/include/assert.h /usr/include/pthread.h
nas_timer.o: /usr/include/endian.h /usr/include/sched.h /usr/include/time.h
nas_timer.o: /usr/include/signal.h /usr/include/string.h
nas_timer.o: /usr/include/xlocale.h /usr/include/stdlib.h
nas_timer.o: /usr/include/alloca.h
OctetString.o: /usr/include/stdio.h /usr/include/features.h
OctetString.o: /usr/include/libio.h /usr/include/_G_config.h
OctetString.o: /usr/include/wchar.h /usr/include/stdlib.h
OctetString.o: /usr/include/alloca.h /usr/include/string.h
OctetString.o: /usr/include/xlocale.h TLVEncoder.h /usr/include/arpa/inet.h
OctetString.o: /usr/include/netinet/in.h /usr/include/stdint.h
OctetString.o: /usr/include/endian.h TLVDecoder.h nas_log.h OctetString.h
parser.o: parser.h
parser.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
parser.o: /usr/include/stdint.h /usr/include/features.h /usr/include/stdio.h
parser.o: /usr/include/libio.h /usr/include/_G_config.h /usr/include/wchar.h
parser.o: /usr/include/string.h /usr/include/xlocale.h
socket.o: socket.h
socket.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
socket.o: /usr/include/stdint.h /usr/include/features.h /usr/include/stdlib.h
socket.o: /usr/include/alloca.h /usr/include/string.h /usr/include/xlocale.h
socket.o: /usr/include/unistd.h /usr/include/getopt.h /usr/include/errno.h
socket.o: /usr/include/netdb.h /usr/include/netinet/in.h
socket.o: /usr/include/endian.h /usr/include/rpc/netdb.h
stty.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
stty.o: /usr/include/stdint.h /usr/include/features.h /usr/include/termios.h
stty.o: /usr/include/unistd.h /usr/include/getopt.h /usr/include/string.h
stty.o: /usr/include/xlocale.h /usr/include/stdlib.h /usr/include/alloca.h
TLVDecoder.o: /usr/include/stdio.h /usr/include/features.h
TLVDecoder.o: /usr/include/libio.h /usr/include/_G_config.h
TLVDecoder.o: /usr/include/wchar.h /usr/include/stdlib.h
TLVDecoder.o: /usr/include/alloca.h /usr/include/string.h
TLVDecoder.o: /usr/include/xlocale.h TLVDecoder.h /usr/include/arpa/inet.h
TLVDecoder.o: /usr/include/netinet/in.h /usr/include/stdint.h
TLVDecoder.o: /usr/include/endian.h nas_log.h
TLVEncoder.o: /usr/include/stdio.h /usr/include/features.h
TLVEncoder.o: /usr/include/libio.h /usr/include/_G_config.h
TLVEncoder.o: /usr/include/wchar.h /usr/include/stdlib.h
TLVEncoder.o: /usr/include/alloca.h /usr/include/stdint.h TLVEncoder.h
TLVEncoder.o: /usr/include/arpa/inet.h /usr/include/netinet/in.h
TLVEncoder.o: /usr/include/endian.h
