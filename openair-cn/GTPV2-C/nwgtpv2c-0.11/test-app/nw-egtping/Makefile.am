AM_CFLAGS = -D__WITH_LIBEVENT__	\
	-I$(top_srcdir)/shared	\
	-I$(top_srcdir)/include	\
	-I$(top_srcdir)/test-app/nw-udp	\
	-I$(top_srcdir)/test-app/nw-log	\
	@AM_CFLAGS@

if ENABLE_TESTS_APP
bin_PROGRAMS = egtping

egtping_SOURCES =    \
			NwEvt.h\
			NwMiniLogMgrEntity.h\
			NwMiniTmrMgrEntity.h\
			NwMiniUlpEntity.h\
			NwMiniUdpEntity.h\
			NwMiniLogMgrEntity.c\
			NwMiniTmrMgrEntity.c\
			NwMiniUlpEntity.c\
			NwMiniUdpEntity.c\
                        NwEgtPingMain.c

egtping_LDADD =  $(top_builddir)/src/libNwGtpv2c.a -levent $(AM_LDFLAGS)
endif