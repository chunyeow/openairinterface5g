ACLOCAL_AMFLAGS = -I m4

AM_CFLAGS = @ADD_CFLAGS@ 							\
	-I$(top_srcdir)/COMMON							\
	-I$(top_srcdir)/UTILS							\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/api/network	\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/include		\
	-I$(top_srcdir)/INTERTASK_INTERFACE

if HAVE_GCCXML
BUILT_SOURCES =		\
	messages.xml	\
	messages_xml.h

messages_hdr =	 												\
	$(top_srcdir)/INTERTASK_INTERFACE/intertask_messages_def.h	\
	$(top_srcdir)/INTERTASK_INTERFACE/timer_messages_def.h		\
	$(top_srcdir)/INTERTASK_INTERFACE/timer_messages_types.h	\
	$(top_srcdir)/COMMON/tasks_def.h							\
	$(top_srcdir)/COMMON/messages_def.h							\
	$(top_srcdir)/COMMON/messages_types.h						\
	$(top_srcdir)/COMMON/gtpv1_u_messages_def.h					\
	$(top_srcdir)/COMMON/gtpv1_u_messages_types.h				\
	$(top_srcdir)/COMMON/nas_messages_def.h						\
	$(top_srcdir)/COMMON/nas_messages_types.h					\
	$(top_srcdir)/COMMON/s11_messages_def.h						\
	$(top_srcdir)/COMMON/s11_messages_types.h					\
	$(top_srcdir)/COMMON/s1ap_messages_def.h					\
	$(top_srcdir)/COMMON/s1ap_messages_types.h					\
	$(top_srcdir)/COMMON/s6a_messages_def.h						\
	$(top_srcdir)/COMMON/s6a_messages_types.h					\
	$(top_srcdir)/COMMON/sgw_lite_def.h							\
	$(top_srcdir)/COMMON/sgw_lite_messages_types.h				\
	$(top_srcdir)/COMMON/sctp_messages_def.h					\
	$(top_srcdir)/COMMON/sctp_messages_types.h					\
	$(top_srcdir)/COMMON/udp_messages_def.h						\
	$(top_srcdir)/COMMON/udp_messages_types.h

messages.xml:	$(top_srcdir)/INTERTASK_INTERFACE/intertask_interface_types.h $(messages_hdr)
	gccxml $(AM_CFLAGS) $< -fxml=$(top_builddir)/$@

messages_xml.h: messages.xml
	sed -e 's/[ ]*//' -e 's/"/\\"/g' -e 's/^/"/' -e 's/$$/"/' messages.xml > messages_xml.h
endif

SUBDIRS = 				\
	.					\
	GTPV1-U				\
	GTPV2-C				\
	INTERTASK_INTERFACE \
	SGI					\
	NAS					\
	SCTP				\
	S11					\
	S1AP				\
	S6A		        	\
	SECU				\
	SGW-LITE			\
	UTILS				\
	UDP					\
	MME_APP				\
	TEST				\
	OAISIM_MME			\
	OAI_EPC				\
	OAI_SGW