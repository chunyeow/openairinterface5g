AM_CFLAGS =				\
	@ADD_CFLAGS@ 			\
	-I$(top_srcdir)/COMMON   \
	-I$(top_srcdir)/INTERTASK_INTERFACE	\
	-I$(top_srcdir)/UTILS

noinst_LTLIBRARIES = libs6a.la
libs6a_la_LDFLAGS = -all-static

libs6a_la_SOURCES = \
	s6a_defs.h	s6a_messages.h	\
	s6a_auth_info.c	\
	s6a_dict.c	\
	s6a_error.c	\
	s6a_peer.c	\
	s6a_subscription_data.c	\
	s6a_task.c	\
	s6a_up_loc.c
