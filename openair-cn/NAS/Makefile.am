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
noinst_LTLIBRARIES = libnas.la

libnas_la_LDFLAGS = -all-static

AM_CFLAGS =											\
	@ADD_CFLAGS@									\
	-DNAS_MME										\
	-DEPC_BUILD										\
	-DENABLE_ITTI									\
	-I$(top_srcdir)/COMMON							\
	-I$(top_srcdir)/SECU							\
	-I$(top_srcdir)/INTERTASK_INTERFACE				\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/api/user	\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/api/mme		\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/api/network	\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/emm/msg		\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/emm/sap		\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/emm			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/esm/msg		\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/esm/sap/msg	\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/esm/sap		\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/esm			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/ies			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/include		\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/util		\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/			\
	-I$(top_srcdir)/UTILS

if DISABLE_USE_NAS
libnas_la_SOURCES =	\
	nas_main.c
else
include Makefile.inc
libnas_la_SOURCES =								\
	nas_main.c									\
	nas_itti_messaging.c						\
	nas_itti_messaging.h						\
	$(libnas_SRCS)
endif