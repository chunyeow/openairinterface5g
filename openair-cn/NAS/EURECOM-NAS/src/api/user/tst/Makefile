ifndef PROJDIR
PROJDIR = $(PWD)/../../../..
endif

include $(PROJDIR)/Makerules
include $(PROJDIR)/Makefile.inc

LIBS		= -lutil -lapi -lEMMmsg -lESMmsg -lies
INCLUDES	= -I. -I$(INCDIR) -I$(UTILDIR) -I$(USERAPIDIR)

LIBSAPI		= $(LIBDIR)/$(LIBAPI).a $(LIBDIR)/$(LIBAPI).so

TST_OBJ		= at_parser.o

TST_TARGET	= at_parser

TARGETS = $(TST_TARGET)

all: $(TARGETS)

%.o: %.c Makefile
	$(CC) $(CFLAGS) -c $< -o $@ 

$(TST_TARGET): $(TST_OBJ)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	$(RM) $(OBJS) $(TARGETS) *.bak *~

depend:
	makedepend -- ${CFLAGS} -- ${SRCS}

# DO NOT DELETE THIS LINE -- make depend depends on it.

at_parser.o: $(USERAPIDIR)/at_command.h
at_parser.o: $(INCDIR)/commonDef.h $(INCDIR)/userDef.h
at_parser.o: $(INCDIR)/networkDef.h $(UTILDIR)/log.h
