export PROCESS = MME

include ../../Makerules
include ../../Makefile.inc

PROJDIR	= $(PWD)/../..

export LD_RUN_PATH = $(LIBDIR):$(LIBPROCESS)

LIBS	 = -lutil -lapi -lEMMmsg -lESMmsg -lies -lrt
INCLUDES = -I. -I$(SRCDIR) -I$(INCDIR) -I$(UTILDIR) \
	-I$(EMMMSGDIR) -I$(ESMMSGDIR) -I$(IESDIR)

TARGET = NetworkProcess

all: $(TARGET)

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)
	@$(CP) $@ $(BINDIR)

clean:
	$(RM) $(OBJS) *.bak *~

veryclean: clean
	$(RM) $(TARGET)

depend:
	makedepend -- ${CFLAGS} -- ${SRCS}

# DO NOT DELETE THIS LINE -- make depend depends on it.

