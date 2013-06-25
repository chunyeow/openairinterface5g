AM_CFLAGS = @ADD_CFLAGS@ \
	-I$(top_srcdir)/INTERTASK_INTERFACE	\
	-I$(top_srcdir)/UTILS

noinst_LTLIBRARIES = libsgw_lite.la
libsgw_lite_la_LDFLAGS = -all-static

libsgw_lite_la_SOURCES=	\
	sgw_lite_task.c		\
	sgw_lite_handlers.c		sgw_lite_handlers.h	\
	sgw_lite_context_manager.c	sgw_lite_context_manager.h
