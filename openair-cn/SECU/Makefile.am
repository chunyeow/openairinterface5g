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
	-I$(top_srcdir)/UTILS	\
	-I$(top_srcdir)/INTERTASK_INTERFACE	\
	-I$(top_srcdir)/COMMON

noinst_LTLIBRARIES = libsecu.la

libsecu_la_LDFLAGS = -all-static -lm
libsecu_la_SOURCES = \
	kdf.c	\
	key_nas_deriver.c	\
    nas_stream_eea1.c   \
    nas_stream_eia1.c   \
    nas_stream_eea2.c   \
    nas_stream_eia2.c   \
    rijndael.c   \
    rijndael.h   \
    snow3g.c   \
    snow3g.h   \
	secu_defs.h