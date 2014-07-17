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
export PROJDIR = $(PWD)/../..
endif

include $(PROJDIR)/Makerules
include $(PROJDIR)/Makefile.inc

export INCLUDES = -I. -I$(INCDIR) -I$(UTILDIR) -I$(IESDIR) \
                  -I$(EMMDIR)    \
                  -I$(ESMDIR)    \
                  -I$(EMMMSGDIR) \
                  -I$(ESMMSGDIR) \
                  -I$(SRCDIR) \
                  -I$(OPENAIRCN_DIR)/COMMON \
                  -I$(OPENAIRCN_DIR)/SECU

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
	@echo target.a
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
