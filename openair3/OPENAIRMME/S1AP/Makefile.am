SUBDIRS = ./MESSAGES/ .

AM_CFLAGS =				\
	@ADD_CFLAGS@ 			\
	-I$(top_builddir)/S1AP		\
	-I$(top_builddir)/S1AP/MESSAGES	\
	-I$(top_srcdir)/INTERTASK_INTERFACE	\
	-I$(top_srcdir)/SCTP	\
	-I$(top_srcdir)/UTILS

noinst_LTLIBRARIES = libs1ap.la
libs1ap_la_LDFLAGS = -all-static
libs1ap_la_LIBADD = ./MESSAGES/libs1apmessages.la

libs1ap_la_SOURCES = \
	s1ap_common.h s1ap_common.c 		\
	s1ap_eNB_decoder.h s1ap_eNB_decoder.c	\
	s1ap_eNB_encoder.h s1ap_eNB_encoder.c	\
	s1ap_eNB_handlers.h s1ap_eNB_handlers.c \
	s1ap_mme_encoder.h s1ap_mme_encoder.c	\
	s1ap_mme_decoder.h s1ap_mme_decoder.c	\
	s1ap_mme_handlers.h s1ap_mme_handlers.c	\
	s1ap_mme.h s1ap_mme.c			\
	$(top_builddir)/S1AP/s1ap_decoder.c	\
	$(top_builddir)/S1AP/s1ap_encoder.c	\
	$(top_builddir)/S1AP/s1ap_ies_defs.h

clean-local:
	rm -f $(top_builddir)/S1AP/s1ap_ies_defs.h
	rm -f $(top_builddir)/S1AP/s1ap_decoder.c
	rm -f $(top_builddir)/S1AP/s1ap_encoder.c
