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
AM_CFLAGS = @ADD_CFLAGS@ 	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/access_restriction	\
	-I$(top_srcdir)/auc	\
	-I$(top_srcdir)/db	\
	-I$(top_srcdir)/utils

noinst_LTLIBRARIES = libs6a.la

libs6a_la_LDFLAGS = -all-static
libs6a_la_SOURCES = \
	s6a_common.c	\
	s6a_fd.c s6a_proto.h	\
	s6a_auth_info.c	\
	s6a_up_loc.c	\
	s6a_purge_ue.c	\
	s6a_error.c	\
	s6a_subscription_data.c	\
	s6a_in_addr.c	\
	s6a_peers.c	\
	s6a_supported_features.h	s6a_supported_features.c