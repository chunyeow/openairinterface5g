AM_CFLAGS = @ADD_CFLAGS@ 	\
	-I$(top_srcdir)/UTILS	\
	-I$(top_srcdir)/INTERTASK_INTERFACE	\
	-I$(top_srcdir)/COMMON

noinst_LTLIBRARIES = libsecu.la

libsecu_la_LDFLAGS = -all-static
libsecu_la_SOURCES = \
	kdf.c	\
	key_nas_deriver.c	\
	nas_stream_eea2.c	\
	nas_stream_eia2.c