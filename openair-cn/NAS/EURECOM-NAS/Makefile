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
