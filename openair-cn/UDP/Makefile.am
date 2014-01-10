noinst_LTLIBRARIES = libudpserver.la
libudpserver_la_LDFLAGS = -all-static

AM_CFLAGS =	\
	@ADD_CFLAGS@	\
	-I$(top_srcdir)/COMMON	\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/api/network		\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/include			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/ies				\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/emm/msg			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/esm/msg			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/util			\
	-I$(top_srcdir)/INTERTASK_INTERFACE	\
	-I$(top_srcdir)/UTILS

libudpserver_la_SOURCES =	\
	udp_primitives_server.h udp_primitives_server.c