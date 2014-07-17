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
AM_CFLAGS =				\
	@ADD_CFLAGS@ 			\
	-I$(top_srcdir)/COMMON								\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/api/network		\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/include			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/ies				\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/emm/msg			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/esm/msg			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/util			\
	-I$(top_srcdir)/INTERTASK_INTERFACE					\
	-I$(top_srcdir)/S1AP								\
	-I$(top_srcdir)/SGW-LITE							\
	-I$(top_srcdir)/S6A									\
	-I$(top_srcdir)/SCTP								\
	-I$(top_srcdir)/SECU								\
	-I$(top_srcdir)/UTILS

noinst_LTLIBRARIES = libmmeapp.la
libmmeapp_la_LDFLAGS = -all-static

libmmeapp_la_SOURCES = \
    mme_app_capabilities.c                          \
	mme_app_context.c								\
	mme_app_main.c	mme_app_defs.h					\
	mme_app_bearer.c								\
	mme_app_authentication.c						\
    mme_app_location.c                              \
	mme_app_statistics.c	mme_app_statistics.h	\
	mme_app_defs.h	mme_app_extern.h				\
	mme_app_itti_messaging.h						\
	s6a_2_nas_cause.c
