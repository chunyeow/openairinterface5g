AM_CFLAGS = @ADD_CFLAGS@ \
	-I$(top_srcdir)/COMMON	\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/api/network	\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/include	\
    -I$(top_srcdir)/INTERTASK_INTERFACE \
    -I$(top_srcdir)/MME_APP \
    -I$(top_srcdir)/UTILS \
    -I$(top_srcdir)/UTILS/HASHTABLE

noinst_LTLIBRARIES = libsgw_lite.la
libsgw_lite_la_LDFLAGS = -all-static

libsgw_lite_la_SOURCES=						\
	s11_causes.c		s11_causes.h			\
	sgw_lite_task.c						\
	sgw_lite_handlers.c		sgw_lite_handlers.h	\
	sgw_lite_context_manager.c	sgw_lite_context_manager.h \
	sgw_lite.h
