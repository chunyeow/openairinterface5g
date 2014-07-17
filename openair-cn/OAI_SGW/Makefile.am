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
if !STANDALONE_EPC
AM_CFLAGS = @ADD_CFLAGS@ \
	-I$(top_srcdir)/COMMON				\
	-I$(top_srcdir)/GTPV1-U				\
	-I$(top_srcdir)/GTPV1-U/nw-gtpv1u/include	\
	-I$(top_srcdir)/GTPV1-U/nw-gtpv1u/shared	\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/api/network		\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/include			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/ies				\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/emm/msg			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/esm/msg			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/util			\
	-I$(top_srcdir)/INTERTASK_INTERFACE		\
	-I$(top_srcdir)/S11				\
	-I$(top_srcdir)/SGI				\
	-I$(top_srcdir)/SGW-LITE			\
	-I$(top_srcdir)/UDP				\
	-I$(top_srcdir)/UTILS				\
	-I$(top_srcdir)/UTILS/HASHTABLE

oai_sgw_LDADD = \
	$(top_builddir)/GTPV1-U/libgtpv1u.la	\
	$(top_builddir)/INTERTASK_INTERFACE/libitti.la	\
	$(top_builddir)/S11/libs11.la	\
	$(top_builddir)/GTPV2-C/nwgtpv2c-0.11/src/libNwGtpv2c.a	\
	$(top_builddir)/SGI/libsgi.la	\
	$(top_builddir)/SGW-LITE/libsgw_lite.la	\
	$(top_builddir)/UTILS/HASHTABLE/libhashtable.la \
	$(top_builddir)/UDP/libudpserver.la	\
	$(top_builddir)/SECU/libsecu.la	\
	$(top_builddir)/UTILS/libutils.la	\
	$(abs_top_builddir)/INTERTASK_INTERFACE/UTIL/LFDS/liblfds6.1.1/liblfds611/bin/liblfds611.a

oai_sgw_SOURCES = \
	oai_sgw_log.c	\
	oai_sgw.c

bin_PROGRAMS = oai_sgw
endif
