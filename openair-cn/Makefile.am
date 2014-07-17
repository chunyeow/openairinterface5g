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

AM_CFLAGS = @ADD_CFLAGS@ 								\
	-I$(top_srcdir)/COMMON								\
	-I$(top_srcdir)/UTILS								\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/api/network		\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/include			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/ies				\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/emm/msg			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/esm/msg			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/util			\
	-I$(top_srcdir)/INTERTASK_INTERFACE

if HAVE_GCCXML
BUILT_SOURCES =		\
	messages.xml	\
	messages_xml.h

messages_hdr =	 							\
	$(top_srcdir)/INTERTASK_INTERFACE/intertask_messages_def.h	\
	$(top_srcdir)/INTERTASK_INTERFACE/timer_messages_def.h		\
	$(top_srcdir)/INTERTASK_INTERFACE/timer_messages_types.h	\
	$(top_srcdir)/COMMON/tasks_def.h				\
	$(top_srcdir)/COMMON/messages_def.h				\
	$(top_srcdir)/COMMON/messages_types.h				\
	$(top_srcdir)/COMMON/gtpv1_u_messages_def.h			\
	$(top_srcdir)/COMMON/gtpv1_u_messages_types.h			\
	$(top_srcdir)/COMMON/nas_messages_def.h				\
	$(top_srcdir)/COMMON/nas_messages_types.h			\
	$(top_srcdir)/COMMON/s11_messages_def.h				\
	$(top_srcdir)/COMMON/s11_messages_types.h			\
	$(top_srcdir)/COMMON/s1ap_messages_def.h			\
	$(top_srcdir)/COMMON/s1ap_messages_types.h			\
	$(top_srcdir)/COMMON/s6a_messages_def.h				\
	$(top_srcdir)/COMMON/s6a_messages_types.h			\
	$(top_srcdir)/COMMON/sgw_lite_def.h				\
	$(top_srcdir)/COMMON/sgw_lite_messages_types.h			\
	$(top_srcdir)/COMMON/sctp_messages_def.h			\
	$(top_srcdir)/COMMON/sctp_messages_types.h			\
	$(top_srcdir)/COMMON/udp_messages_def.h				\
	$(top_srcdir)/COMMON/udp_messages_types.h

messages.xml:	$(top_srcdir)/INTERTASK_INTERFACE/intertask_interface_types.h $(messages_hdr)
	@echo "Generating $<"
	@gccxml $(AM_CFLAGS) $< -fxml=$(top_builddir)/$@

messages_xml.h: messages.xml
	@echo "Generating $<"
	@sed -e 's/[ ]*//' -e 's/"/\\"/g' -e 's/^/"/' -e 's/$$/\\n"/' messages.xml > messages_xml.h

clean-local:
	@rm -rf -v messages.xml messages_xml.h
endif

SUBDIRS = 				\
	.				\
	GTPV1-U				\
	GTPV2-C				\
	INTERTASK_INTERFACE		\
	SGI				\
	NAS				\
	SCTP				\
	S11				\
	S1AP				\
	S6A		        	\
	SECU				\
	SGW-LITE			\
	UTILS				\
	UDP				\
	MME_APP				\
	TEST				\
	OAISIM_MME			\
	OAI_EPC				\
	OAI_SGW