AM_CFLAGS = @ADD_CFLAGS@ 	\
	-I$(top_srcdir)		\
	-I$(top_srcdir)/utils

noinst_LTLIBRARIES = libauc.la

libauc_la_LDFLAGS = -all-static
libauc_la_SOURCES = \
	auc.h	\
	fx.c	\
	kdf.c	\
	random.c	\
	rijndael.c	\
	sequence_number.c