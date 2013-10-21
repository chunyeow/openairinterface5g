ifndef PROJDIR
export PROJDIR = $(PWD)/../..
endif

include $(PROJDIR)/Makerules
include $(PROJDIR)/Makefile.inc

export INCLUDES = -I. -I$(INCDIR) -I$(UTILDIR) -I$(IESDIR) -I$(EMMMSGDIR) -I$(ESMMSGDIR)

TARGET = $(LIBAPI)
TARGETS = $(TARGET).a $(TARGET).so
OBJS = $(USERAPIDIR)/*.o $(NETAPIDIR)/*.o $(USIMAPIDIR)/*.o $(MMEAPIDIR)/*.o

all:
	@$(CD) $(NETAPIDIR)  && $(MAKE)
	@$(CD) $(USERAPIDIR) && $(MAKE)
	@$(CD) $(USIMAPIDIR) && $(MAKE)
	@$(CD) $(MMEAPIDIR)  && $(MAKE)
	@$(MAKE) $(TARGETS)

$(TARGET).a: $(OBJS)
	@$(RM) $@
	@$(AR) $(ARFLAGS) $@ $^
	@echo Replacing $@ to $(LIBPROCESS)
	@$(RM) $(LIBPROCESS)/$@
	@$(CP) $@ $(LIBPROCESS)

$(TARGET).so: $(OBJS)
	@$(LD) -G -o $@ $^
	@echo Replacing $@ to $(LIBPROCESS)
	@$(RM) $(LIBPROCESS)/$@
	@$(CP) $@ $(LIBPROCESS)

clean:
	$(RM)  *.bak *~
	@$(CD) $(USERAPIDIR) && $(MAKE) $@
	@$(CD) $(NETAPIDIR)  && $(MAKE) $@
	@$(CD) $(USIMAPIDIR) && $(MAKE) $@
	@$(CD) $(MMEAPIDIR)  && $(MAKE) $@

veryclean: clean
	$(RM)  $(TARGETS)

depend:
	makedepend -- ${CFLAGS} -- ${SRCS}

# DO NOT DELETE THIS LINE -- make depend depends on it.
