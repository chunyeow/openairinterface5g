AM_CFLAGS = @ADD_CFLAGS@ \
	-I$(top_srcdir)/SCTP			\
	-I$(top_srcdir)/INTERTASK_INTERFACE	\
	-I$(top_builddir)/S1AP/MESSAGES		\
	-I$(top_builddir)/S1AP			\
	-I$(top_srcdir)/S1AP			\
	-I$(top_srcdir)/UTILS

oaisim_mme_list_benchmark_LDADD = \
	$(top_builddir)/INTERTASK_INTERFACE/libitti.la	\
	$(top_builddir)/S1AP/libs1ap.la	\
	$(top_builddir)/SCTP/libsctpclient.la	\
	$(top_builddir)/UTILS/libutils.la

bin_PROGRAMS = oaisim_mme_list_benchmark