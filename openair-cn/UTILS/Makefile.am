SUBDIRS = . HASHTABLE

AM_CFLAGS = @ADD_CFLAGS@	\
	-I$(top_srcdir)/COMMON	\
	-I$(top_srcdir)/INTERTASK_INTERFACE

AM_YFLAGS = -d

noinst_LTLIBRARIES = libutils.la
libutils_la_LDFLAGS = -all-static
libutils_la_SOURCES = \
	assertions.h	\
	backtrace.c	backtrace.h	\
	conversions.h	conversions.c	\
	enum_string.h	enum_string.c	\
	log.c log.h	\
	mme_parser.y mme_scanner.l	\
	mme_config.c mme_config.h	\
	mme_default_values.h	\
	queue.h	tree.h	\
	signals.c	signals.h
