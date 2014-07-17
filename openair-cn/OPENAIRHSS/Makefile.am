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
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = access_restriction auc db s6a utils tests .

AM_CFLAGS = @ADD_CFLAGS@ \
    -I$(top_srcdir)/access_restriction  \
	-I$(top_srcdir)/auc	\
	-I$(top_srcdir)/db	\
	-I$(top_srcdir)/s6a	\
	-I$(top_srcdir)/utils

bin_PROGRAMS = openair-hss

openair_hssdir=$(sysconfdir)/openair-hss
openair_hss_DATA = $(top_builddir)/conf/hss.conf $(top_srcdir)/conf/hss_fd.conf

openair_hss_LDADD = \
	$(top_builddir)/s6a/libs6a.la	\
	$(top_builddir)/db/libdb.la	\
	$(top_builddir)/auc/libauc.la	\
	$(top_builddir)/access_restriction/libaccess_restriction.la	\
	$(top_builddir)/utils/libutils.la

openair_hss_SOURCES = \
	hss_config.h	\
	hss_main.c
