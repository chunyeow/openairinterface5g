AM_CFLAGS = @ADD_CFLAGS@ 	\
	-I$(top_srcdir)/INTERTASK_INTERFACE	\
	-I$(top_srcdir)/GTPV1-U/nw-gtpv1u/include	\
	-I$(top_srcdir)/GTPV1-U/nw-gtpv1u/shared	\
	-I$(top_srcdir)/UTILS	\
	-I$(top_srcdir)/UTILS/TIMER

noinst_LTLIBRARIES = libgtpv1u.la

libgtpv1u_la_LDFLAGS = -all-static

libgtpv1u_la_SOURCES = \
	gtpv1_u_task.c	gtpv1_u.h	\
	nw-gtpv1u/shared/NwTypes.h	\
	nw-gtpv1u/shared/NwUtils.h	\
	nw-gtpv1u/shared/NwGtpv1uError.h	\
	nw-gtpv1u/shared/NwLog.h		\
	nw-gtpv1u/shared/NwGtpv1uIe.h	\
	nw-gtpv1u/shared/NwGtpv1uMsg.h	\
	nw-gtpv1u/shared/NwGtpv1u.h	\
	nw-gtpv1u/include/NwGtpv1uPrivate.h	\
	nw-gtpv1u/include/NwGtpv1uLog.h	\
	nw-gtpv1u/include/NwGtpv1uTrxn.h	\
	nw-gtpv1u/include/NwGtpv1uTunnelEndPoint.h	\
	nw-gtpv1u/src/NwGtpv1uTunnelEndPoint.c	\
	nw-gtpv1u/src/NwGtpv1uTrxn.c	\
	nw-gtpv1u/src/NwGtpv1uMsg.c	\
	nw-gtpv1u/src/NwGtpv1u.c