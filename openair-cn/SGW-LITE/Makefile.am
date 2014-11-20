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
AM_CFLAGS = @ADD_CFLAGS@ \
	-I$(top_srcdir)/COMMON	\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/api/network		\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/include			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/ies				\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/emm/msg			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/esm/msg			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/util			\
    -I$(top_srcdir)/INTERTASK_INTERFACE \
    -I$(top_srcdir)/GTPV1-U \
    -I$(top_srcdir)/MME_APP \
    -I$(top_srcdir)/UTILS \
    -I$(top_srcdir)/UTILS/HASHTABLE

noinst_LTLIBRARIES = libsgw_lite.la
libsgw_lite_la_LDFLAGS = -all-static -lconfig

libsgw_lite_la_SOURCES=							\
	spgw_config.c				spgw_config.h	\
	s11_causes.c				s11_causes.h	\
	sgw_lite_task.c								\
	sgw_lite_handlers.c			sgw_lite_handlers.h	\
	sgw_lite_context_manager.c	sgw_lite_context_manager.h \
	sgw_lite.h	sgw_lite_defs.h	sgw_lite_ie_defs.h	\
	pgw_lite_paa.c              pgw_lite_paa.h
