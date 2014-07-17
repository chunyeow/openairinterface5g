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
SUBDIRS = ./MESSAGES/ .

AM_CFLAGS =				\
	@ADD_CFLAGS@ 			\
	-I$(top_srcdir)/COMMON	\
	-I$(top_builddir)/S1AP		\
	-I$(top_builddir)/S1AP/MESSAGES	\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/api/network		\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/include			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/ies				\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/emm/msg			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/esm/msg			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/util			\
	-I$(top_srcdir)/INTERTASK_INTERFACE	\
	-I$(top_srcdir)/SCTP	\
	-I$(top_srcdir)/UTILS	\
	-DEMIT_ASN_DEBUG_EXTERN

noinst_LTLIBRARIES = libs1ap.la
libs1ap_la_LDFLAGS = -all-static
libs1ap_la_LIBADD = ./MESSAGES/libs1apmessages.la

libs1ap_la_SOURCES = \
	s1ap_common.h s1ap_common.c 		\
	s1ap_mme_encoder.h s1ap_mme_encoder.c	\
	s1ap_mme_decoder.h s1ap_mme_decoder.c	\
	s1ap_mme_handlers.h s1ap_mme_handlers.c	\
	s1ap_mme_nas_procedures.c s1ap_mme_nas_procedures.h		\
	s1ap_mme.h s1ap_mme.c			\
	s1ap_mme_itti_messaging.c	s1ap_mme_itti_messaging.h	\
	s1ap_mme_retransmission.c	s1ap_mme_retransmission.h	\
	s1ap_mme_ta.c	s1ap_mme_ta.h	\
	$(top_builddir)/S1AP/s1ap_decoder.c	\
	$(top_builddir)/S1AP/s1ap_encoder.c	\
	$(top_builddir)/S1AP/s1ap_xer_print.c	\
	$(top_builddir)/S1AP/s1ap_ies_defs.h

clean-local:
	rm -f $(top_builddir)/S1AP/s1ap_ies_defs.h
	rm -f $(top_builddir)/S1AP/s1ap_decoder.c
	rm -f $(top_builddir)/S1AP/s1ap_encoder.c
	rm -f $(top_builddir)/S1AP/s1ap_xer_print.c
