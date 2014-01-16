ACLOCAL_AMFLAGS = -I m4

SUBDIRS = access_restriction auc db s6a utils tests .

AM_CFLAGS = @ADD_CFLAGS@ \
    -I$(top_srcdir)/access_restriction  \
	-I$(top_srcdir)/auc	\
	-I$(top_srcdir)/db	\
	-I$(top_srcdir)/s6a	\
	-I$(top_srcdir)/utils

bin_PROGRAMS = openair-hss

openair_hssdir=$(sysconfdir)/openair-hss
openair_hss_DATA = $(top_builddir)/conf/hss.conf $(top_srcdir)/conf/hss_fd.conf

openair_hss_LDADD = \
	$(top_builddir)/s6a/libs6a.la	\
	$(top_builddir)/db/libdb.la	\
	$(top_builddir)/auc/libauc.la	\
	$(top_builddir)/access_restriction/libaccess_restriction.la	\
	$(top_builddir)/utils/libutils.la

openair_hss_SOURCES = \
	hss_config.h	\
	hss_main.c
