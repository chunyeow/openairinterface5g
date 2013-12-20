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
	@$(CP) $(TARGET) $(BINDIR)

clean:
	$(RM) $(OBJS) *.bak *~

veryclean: clean
	$(RM) $(TARGET)

depend:
	makedepend -- ${CFLAGS} -- ${SRCS}

# DO NOT DELETE THIS LINE -- make depend depends on it.

