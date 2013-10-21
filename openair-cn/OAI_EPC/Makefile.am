if STANDALONE_EPC
AM_CFLAGS = @ADD_CFLAGS@ \
	-I$(top_srcdir)/COMMON				\
	-I$(top_srcdir)/GTPV1-U				\
	-I$(top_srcdir)/GTPV1-U/nw-gtpv1u/include	\
	-I$(top_srcdir)/GTPV1-U/nw-gtpv1u/shared		\
	-I$(top_srcdir)/SCTP				\
	-I$(top_srcdir)/INTERTASK_INTERFACE		\
	-I$(top_srcdir)/SGI				\
	-I$(top_srcdir)/MME_APP				\
	-I$(top_srcdir)/NAS				\
	-I$(top_builddir)/S1AP/MESSAGES			\
	-I$(top_srcdir)/S1AP				\
	-I$(top_srcdir)/S6A				\
	-I$(top_srcdir)/SECU				\
	-I$(top_srcdir)/SGW-LITE			\
	-I$(top_srcdir)/UDP				\
	-I$(top_srcdir)/UTILS				\
	-I$(top_srcdir)/UTILS/HASHTABLE			\
	-I$(top_srcdir)/UTILS/TIMER

oai_epc_LDADD = \
	$(top_builddir)/GTPV1-U/libgtpv1u.la	\
	$(top_builddir)/GTPV2-C/nwgtpv2c-0.11/src/libNwGtpv2c.a	\
	$(top_builddir)/INTERTASK_INTERFACE/libitti.la	\
	$(top_builddir)/SGI/libsgi.la	\
	$(top_builddir)/NAS/libnas.la	\
	$(top_builddir)/S6A/libs6a.la	\
	$(top_builddir)/SGW-LITE/libsgw_lite.la	\
	$(top_builddir)/SCTP/libsctpserver.la	\
	$(top_builddir)/UTILS/HASHTABLE/libhashtable.la \
	$(top_builddir)/UDP/libudpserver.la	\
	$(top_builddir)/MME_APP/libmmeapp.la	\
	$(top_builddir)/SECU/libsecu.la	\
	$(top_builddir)/UTILS/libutils.la	\
	$(top_builddir)/S1AP/libs1ap.la

oai_epc_SOURCES = \
	oai_epc_log.c	\
	oai_epc.c

bin_PROGRAMS = oai_epc
endif
