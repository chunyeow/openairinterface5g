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
SECUDIR     = $(OPENAIRCN_DIR)/SECU
SRCDIR      = $(PROJDIR)/src
INCDIR		= $(SRCDIR)/include
UTILDIR		= $(SRCDIR)/util
APIDIR		= $(SRCDIR)/api
USERAPIDIR  = $(APIDIR)/user
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
