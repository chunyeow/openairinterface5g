AM_CFLAGS =				\
	@ADD_CFLAGS@ 			\
	-I$(top_srcdir)/INTERTASK_INTERFACE	\
	-I$(top_srcdir)/UTILS

noinst_LTLIBRARIES = libs6a.la
libs6a_la_LDFLAGS = -all-static

if ENABLE_USE_HSS
libs6a_la_SOURCES = \
	s6a_task.c	s6a_defs.h
else
libs6a_la_SOURCES =
endif
