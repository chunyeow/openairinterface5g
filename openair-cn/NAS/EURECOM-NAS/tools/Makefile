export PROCESS = UE

ifndef PROJDIR
PROJDIR = $(PWD)/..
endif

include $(PROJDIR)/Makerules
include $(PROJDIR)/Makefile.inc

export LD_RUN_PATH = $(LIBDIR):$(LIBPROCESS)

LIBS		= -lapi -lutil -lEMMmsg -lESMmsg -lies -lrt 
INCLUDES	= -I. -I$(INCDIR) -I$(UTILDIR) -I$(USIMAPIDIR) -I$(EMMDIR) -I$(ESMDIR) -I$(IESDIR)

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
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) $(SECUDIR)/*.o -lnettle -lcrypto
	@echo Replacing $@ to $(BINDIR)
	@$(RM) $(BINDIR)/$@
	@$(CP) $@ $(BINDIR)

$(UE_TARGET): $(UE_OBJ) $(LIBSUTIL)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS) $(SECUDIR)/*.o -lnettle -lcrypto
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

