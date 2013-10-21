noinst_LTLIBRARIES = libs11.la

libs11_la_LDFLAGS = -all-static

AM_CFLAGS =	\
	@ADD_CFLAGS@	\
	-I$(top_srcdir)/COMMON	\
	-I$(top_srcdir)/GTPV2-C/nwgtpv2c-0.11/shared	\
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