noinst_LTLIBRARIES = libudpserver.la
libudpserver_la_LDFLAGS = -all-static

AM_CFLAGS = @ADD_CFLAGS@

AM_CPPFLAGS =	\
	-I$(top_srcdir)/INTERTASK_INTERFACE	\
	-I$(top_srcdir)/UTILS

libudpserver_la_SOURCES =	\
	udp_primitives_server.h udp_primitives_server.c