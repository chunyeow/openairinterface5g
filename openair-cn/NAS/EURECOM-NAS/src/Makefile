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
		-I$(IESDIR) -I$(MMEAPIDIR)

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

