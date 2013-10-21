include ../../Makerules
include ../../Makefile.inc

PROJDIR	= $(PWD)/../..

export LD_RUN_PATH = $(LIBDIR)

LIBS	 = -lutil -lpthread -lrt
INCLUDES = -I. -I$(SRCDIR)

TARGET = UserProcess

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

user_parser.o: user_parser.h
user_parser.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/parser.h
user_simulator.o: user_parser.h
user_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/include/commonDef.h
user_simulator.o: /usr/include/stdint.h /usr/include/features.h
user_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/socket.h
user_simulator.o: /homes/roux/lte-epc/openair3/OPENAIRMME/NAS/EURECOM-NAS/src/util/device.h
user_simulator.o: /usr/include/stdio.h /usr/include/libio.h
user_simulator.o: /usr/include/_G_config.h /usr/include/wchar.h
user_simulator.o: /usr/include/xlocale.h /usr/include/errno.h
user_simulator.o: /usr/include/netdb.h /usr/include/netinet/in.h
user_simulator.o: /usr/include/endian.h /usr/include/rpc/netdb.h
user_simulator.o: /usr/include/ctype.h /usr/include/stdlib.h
user_simulator.o: /usr/include/alloca.h /usr/include/signal.h
user_simulator.o: /usr/include/time.h /usr/include/string.h
user_simulator.o: /usr/include/pthread.h /usr/include/sched.h
user_simulator.o: /usr/include/poll.h
