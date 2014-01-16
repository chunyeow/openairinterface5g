AM_CFLAGS = @ADD_CFLAGS@
AM_YFLAGS = -d
AM_LFLAGS = -o$(LEX_OUTPUT_ROOT).c

BUILT_SOURCE = hss_parser.h

noinst_LTLIBRARIES = libutils.la
libutils_la_LDFLAGS = -all-static
libutils_la_SOURCES = \
	hss_parser.y	hss_scanner.l	\
	hss_config.h	hss_config.c	\
	conversion.h	conversion.c
