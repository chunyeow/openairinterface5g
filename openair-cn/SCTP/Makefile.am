noinst_LTLIBRARIES = libsctpserver.la libsctpclient.la
libsctpclient_la_LDFLAGS = -all-static
libsctpserver_la_LDFLAGS = -all-static

AM_CFLAGS =	\
	@ADD_CFLAGS@	\
	-I$(top_srcdir)/COMMON	\
	-I$(top_srcdir)/INTERTASK_INTERFACE	\
	-I$(top_srcdir)/UTILS

libsctpserver_la_SOURCES=	\
	sctp_common.c sctp_common.h	\
	sctp_itti_messaging.h sctp_itti_messaging.c	\
	sctp_primitives_server.h sctp_primitives_server.c

libsctpclient_la_SOURCES=	\
	sctp_common.c sctp_common.h	\
	sctp_primitives_client.h sctp_primitives_client.c