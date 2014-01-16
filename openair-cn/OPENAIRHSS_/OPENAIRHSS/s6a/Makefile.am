AM_CFLAGS = @ADD_CFLAGS@ 	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/access_restriction	\
	-I$(top_srcdir)/auc	\
	-I$(top_srcdir)/db	\
	-I$(top_srcdir)/utils

noinst_LTLIBRARIES = libs6a.la

libs6a_la_LDFLAGS = -all-static
libs6a_la_SOURCES = \
	s6a_common.c	\
	s6a_fd.c s6a_proto.h	\
	s6a_auth_info.c	\
	s6a_up_loc.c	\
	s6a_purge_ue.c	\
	s6a_error.c	\
	s6a_subscription_data.c	\
	s6a_in_addr.c	\
	s6a_peers.c	\
	s6a_supported_features.h	s6a_supported_features.c