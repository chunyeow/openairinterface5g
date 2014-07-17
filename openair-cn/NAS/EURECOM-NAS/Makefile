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
# To be run as: make or make PROCESS=UE or make PROCESS=MME
# make and make PROCESS=UE build ./bin/UEprocess
# make PROCESS=MME builds ./bin/MMEprocess

include Makerules
include Makefile.inc

export PROJDIR	= $(CURDIR)

all:
ifeq ($(PROCESS), UE)
	@$(CD) $(SECUDIR)  && $(MAKE)
endif
	@$(CD) $(UTILDIR) && $(MAKE)
	@$(CD) $(APIDIR)  && $(MAKE)
	@$(CD) $(IESDIR)  && $(MAKE)
	@$(CD) $(EMMDIR)  && $(MAKE)
	@$(CD) $(ESMDIR)  && $(MAKE)
	@$(CD) $(SRCDIR)  && $(MAKE)
ifeq ($(PROCESS), UE)
	@$(CD) $(TOOLSDIR)  && $(MAKE)
	@$(CD) $(USRTSTDIR) && $(MAKE)
endif
ifeq ($(PROCESS), MME)
	@$(CD) $(NETTSTDIR) && $(MAKE)
	@$(CD) $(ASSIMUDIR) && $(MAKE)
endif
# SR include all objects in a single archive
	$(AR) rcs libUE/libNasUE.a `find . -name *.o` `find $(OPENAIRCN_DIR)/SECU -name *.o`

clean:
	@$(CD) $(SRCDIR)  && $(MAKE) $@
	@$(CD) $(APIDIR)  && $(MAKE) $@
	@$(CD) $(EMMDIR)  && $(MAKE) $@
	@$(CD) $(ESMDIR)  && $(MAKE) $@

veryclean: clean
	@$(CD) $(SRCDIR)    && $(MAKE) $@
	@$(CD) $(APIDIR)    && $(MAKE) $@
	@$(CD) $(EMMDIR)    && $(MAKE) $@
	@$(CD) $(ESMDIR)    && $(MAKE) $@
	@$(CD) $(UTILDIR)   && $(MAKE) $@
	@$(CD) $(IESDIR)    && $(MAKE) $@
	@$(CD) $(TOOLSDIR)  && $(MAKE) $@
	@$(CD) $(USRTSTDIR) && $(MAKE) $@
	@$(CD) $(NETTSTDIR) && $(MAKE) $@
	@$(CD) $(ASSIMUDIR) && $(MAKE) $@
	@$(RM) libUE/libNasUE.a

veryveryclean: veryclean
	@$(CD) $(BINDIR)    && $(RM) * .*.nvram
	@$(CD) $(LIBDIR)    && $(RM) *
	@$(CD) $(LIBDIR)UE  && $(RM) *
	@$(CD) $(LIBDIR)MME && $(RM) *

depend:
	@$(CD) $(SRCDIR)    && $(MAKE) depend
	@$(CD) $(APIDIR)    && $(MAKE) depend
	@$(CD) $(EMMDIR)    && $(MAKE) depend
	@$(CD) $(ESMDIR)    && $(MAKE) depend
	@$(CD) $(UTILDIR)   && $(MAKE) depend
	@$(CD) $(IESDIR)    && $(MAKE) depend
	@$(CD) $(TOOLSDIR)  && $(MAKE) depend
	@$(CD) $(USRTSTDIR) && $(MAKE) depend
	@$(CD) $(NETTSTDIR) && $(MAKE) depend
	@$(CD) $(ASSIMUDIR) && $(MAKE) depend

# DO NOT DELETE
