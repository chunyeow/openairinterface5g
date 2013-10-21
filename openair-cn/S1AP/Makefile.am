SUBDIRS = ./MESSAGES/ .

AM_CFLAGS =				\
	@ADD_CFLAGS@ 			\
	-I$(top_srcdir)/COMMON	\
	-I$(top_builddir)/S1AP		\
	-I$(top_builddir)/S1AP/MESSAGES	\
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
