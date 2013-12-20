ifndef PROJDIR
PROJDIR = $(PWD)/../../..
endif

include $(PROJDIR)/Makerules
include $(PROJDIR)/Makefile.inc

INCLUDES = -I. -I$(INCDIR) -I$(UTILDIR) -I$(IESDIR) -I$(MMEAPIDIR) \
	-I$(EMMDIR) -I$(EMMMSGDIR) -I$(ESMDIR) -I$(ESMMSGDIR) -I$(NETAPIDIR)

TARGET = $(LIBEMMSAP)
TARGETS = $(TARGET).a $(TARGET).so

all: $(TARGETS)

%.o: %.c Makefile $(PROJDIR)/Makerules $(PROJDIR)/Makefile.inc
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).a: $(OBJS)
	@$(RM) $@
	@$(AR) $(ARFLAGS) $@ $(OBJS)
	@echo Replacing $@ to $(LIBPROCESS)
	@$(RM) $(LIBPROCESS)/$@
	@$(CP) $@ $(LIBPROCESS)

$(TARGET).so: $(OBJS)
	@$(LD) -G -o $@ $(OBJS)
	@echo Replacing $@ to $(LIBPROCESS)
	@$(RM) $(LIBPROCESS)/$@
	@$(CP) $@ $(LIBPROCESS)

clean:
	$(RM) $(OBJS) *.bak *~

veryclean: clean
	$(RM) $(TARGETS)

depend:
	makedepend -- ${CFLAGS} -- ${SRCS}

# DO NOT DELETE THIS LINE -- make depend depends on it.
