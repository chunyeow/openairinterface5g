AM_CFLAGS = @ADD_CFLAGS@ \
	-I$(top_srcdir)/GTPV1-U			\
	-I$(top_srcdir)/SCTP			\
	-I$(top_srcdir)/INTERTASK_INTERFACE	\
	-I$(top_builddir)/S1AP/MESSAGES		\
	-I$(top_srcdir)/S1AP			\
	-I$(top_srcdir)/S6A			\
	-I$(top_srcdir)/SGW-LITE		\
	-I$(top_srcdir)/UDP			\
	-I$(top_srcdir)/UTILS			\
	-I$(top_srcdir)/UTILS/TIMER

oaisim_mme_LDADD = \
	$(top_builddir)/GTPV1-U/libgtpv1u.la	\
	$(top_builddir)/INTERTASK_INTERFACE/libitti.la	\
	$(top_builddir)/S1AP/libs1ap.la	\
	$(top_builddir)/S6A/libs6a.la	\
	$(top_builddir)/SGW-LITE/libsgw_lite.la	\
	$(top_builddir)/SCTP/libsctpserver.la	\
	$(top_builddir)/UTILS/TIMER/libtimer.la	\
	$(top_builddir)/UTILS/libutils.la	\
	$(top_builddir)/UDP/libudpserver.la

bin_PROGRAMS = oaisim_mme