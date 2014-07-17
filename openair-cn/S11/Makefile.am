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
noinst_LTLIBRARIES = libs11.la

libs11_la_LDFLAGS = -all-static

AM_CFLAGS =	\
	@ADD_CFLAGS@	\
	-I$(top_srcdir)/COMMON	\
	-I$(top_srcdir)/GTPV2-C/nwgtpv2c-0.11/shared	\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/api/network		\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/include			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/ies				\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/emm/msg			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/esm/msg			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/util			\
	-I$(top_srcdir)/INTERTASK_INTERFACE	\
	-I$(top_srcdir)/SGW-LITE	\
	-I$(top_srcdir)/UTILS

libs11_la_SOURCES = \
	s11_common.c		  s11_common.h			\
	s11_ie_formatter.c 	  s11_ie_formatter.h		\
	s11_mme_task.c 		  s11_mme.h			\
	s11_sgw.c 		  s11_sgw.h 			\
	s11_mme_session_manager.c s11_mme_session_manager.h	\
	s11_sgw_session_manager.c s11_sgw_session_manager.h	\
	s11_sgw_bearer_manager.c  s11_sgw_bearer_manager.h