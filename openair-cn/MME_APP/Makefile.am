AM_CFLAGS =				\
	@ADD_CFLAGS@ 			\
	-I$(top_srcdir)/COMMON								\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/api/network		\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/include			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/ies				\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/emm/msg			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/esm/msg			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/util			\
	-I$(top_srcdir)/INTERTASK_INTERFACE					\
	-I$(top_srcdir)/S1AP								\
	-I$(top_srcdir)/SGW-LITE							\
	-I$(top_srcdir)/S6A									\
	-I$(top_srcdir)/SCTP								\
	-I$(top_srcdir)/SECU								\
	-I$(top_srcdir)/UTILS

noinst_LTLIBRARIES = libmmeapp.la
libmmeapp_la_LDFLAGS = -all-static

libmmeapp_la_SOURCES = \
    mme_app_capabilities.c                          \
	mme_app_context.c								\
	mme_app_main.c	mme_app_defs.h					\
	mme_app_bearer.c								\
	mme_app_authentication.c						\
    mme_app_location.c                              \
	mme_app_statistics.c	mme_app_statistics.h	\
	mme_app_defs.h	mme_app_extern.h				\
	mme_app_itti_messaging.h						\
	s6a_2_nas_cause.c
