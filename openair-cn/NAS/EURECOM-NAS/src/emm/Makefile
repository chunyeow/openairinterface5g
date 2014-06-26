ifndef PROJDIR
PROJDIR = $(PWD)/../..
endif

include $(PROJDIR)/Makerules
include $(PROJDIR)/Makefile.inc

INCLUDES = -I. \
    -I$(INCDIR) \
    -I$(UTILDIR) \
    -I$(USIMAPIDIR) \
    -I$(MMEAPIDIR) \
	-I$(ESMDIR) \
	-I$(EMMSAPDIR) \
	-I$(ESMDIR) \
	-I$(ESMSAPDIR) \
	-I$(EMMMSGDIR) \
	-I$(IESDIR) \
    -I$(OPENAIRCN_DIR)/COMMON \
    -I$(OPENAIRCN_DIR)/SECU
	

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

