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
