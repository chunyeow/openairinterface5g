AM_CFLAGS = @ADD_CFLAGS@ 	\
	-I$(top_srcdir)/UTILS

noinst_LTLIBRARIES = libitti.la

libitti_la_LDFLAGS = -all-static
libitti_la_SOURCES = \
	gtpv1_u_messages_def.h	gtpv1_u_messages_types.h	\
	timer_messages_def.h	timer_messages_types.h	\
	s1ap_messages_def.h	s1ap_messages_types.h	\
	s6a_messages_def.h	s6a_messages_types.h	\
	sgw_lite_def.h		sgw_lite_messages_types.h	\
	sctp_messages_def.h	sctp_messages_types.h	\
	udp_message_def.h	udp_messages_types.h	\
	intertask_interface.c 	intertask_interface.h	\
	intertask_interface_dump.c intertask_interface_dump.h