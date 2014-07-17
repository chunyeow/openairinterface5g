################################################################################
#   OpenAirInterface
#   Copyright(c) 1999 - 2014 Eurecom
#
#    OpenAirInterface is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#
#    OpenAirInterface is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with OpenAirInterface.The full GNU General Public License is
#    included in this distribution in the file called "COPYING". If not,
#    see <http://www.gnu.org/licenses/>.
#
#  Contact Information
#  OpenAirInterface Admin: openair_admin@eurecom.fr
#  OpenAirInterface Tech : openair_tech@eurecom.fr
#  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
#
#  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.
#
################################################################################
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

LIBS		= -lutil -lapi -lEMMmsg -lESMmsg -lEMMsap -lESMsap -lies -lrt -lnettle -lcrypto -lm
INCLUDES	= -I. -I$(INCDIR) -I$(UTILDIR) -I$(USERAPIDIR) -I$(NETAPIDIR) \
		-I$(EMMDIR) -I$(EMMMSGDIR) -I$(EMMSAPDIR) \
		-I$(ESMDIR) -I$(ESMMSGDIR) -I$(ESMSAPDIR) \
		-I$(IESDIR) -I$(MMEAPIDIR) 

.PHONY: $(TARGET)

%.o: %.c Makefile $(PROJDIR)/Makerules $(PROJDIR)/Makefile.inc
	@echo Compiling $<
	@$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(EMMDIR)/*.o $(ESMDIR)/*.o $(SECUDIR)/*.o $(LIBS)
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

