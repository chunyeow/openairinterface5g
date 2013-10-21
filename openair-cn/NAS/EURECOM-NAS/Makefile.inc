# list of generated libraries
LIBUTIL		= libutil
LIBAPI		= libapi
LIBIES		= libies
LIBEMMMSG	= libEMMmsg
LIBEMMSAP	= libEMMsap
LIBESMMSG	= libESMmsg
LIBESMSAP	= libESMsap

# Binary directories
BINDIR		= $(PROJDIR)/bin
LIBDIR		= $(PROJDIR)/lib
LIBPROCESS	= $(LIBDIR)$(PROCESS)

# Source directories
SRCDIR		= $(PROJDIR)/src
INCDIR		= $(SRCDIR)/include
UTILDIR		= $(SRCDIR)/util
APIDIR		= $(SRCDIR)/api
USERAPIDIR 	= $(APIDIR)/user
NETAPIDIR  	= $(APIDIR)/network
USIMAPIDIR 	= $(APIDIR)/usim
MMEAPIDIR 	= $(APIDIR)/mme
IESDIR		= $(SRCDIR)/ies
EMMDIR		= $(SRCDIR)/emm
ESMDIR		= $(SRCDIR)/esm
EMMMSGDIR	= $(EMMDIR)/msg
EMMSAPDIR	= $(EMMDIR)/sap
ESMMSGDIR	= $(ESMDIR)/msg
ESMSAPDIR	= $(ESMDIR)/sap

# Tools directory
TOOLSDIR	= $(PROJDIR)/tools

# Test directories
TSTDIR		= $(PROJDIR)/tst
USRTSTDIR	= $(TSTDIR)/user
NETTSTDIR	= $(TSTDIR)/network
ASSIMUDIR	= $(TSTDIR)/as_simulator
