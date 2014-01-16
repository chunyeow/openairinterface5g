AM_CFLAGS = @ADD_CFLAGS@ 	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/s6a	\
	-I$(top_srcdir)/utils

noinst_LTLIBRARIES = libdb.la

libdb_la_LDFLAGS = -all-static
libdb_la_SOURCES = \
	db_proto.h	db_connector.c	\
	db_subscription_data.c	\
	db_epc_equipment.c