noinst_LTLIBRARIES = libnas.la

libnas_la_LDFLAGS = -all-static

AM_CFLAGS =	\
	@ADD_CFLAGS@	                        	    \
	-DNAS_MME					    \
	-DEPC_BUILD					    \
	-I$(top_srcdir)/COMMON				    \
	-I$(top_srcdir)/INTERTASK_INTERFACE		    \
        -I$(top_srcdir)/NAS/EURECOM-NAS/src/api/user        \
        -I$(top_srcdir)/NAS/EURECOM-NAS/src/api/mme         \
        -I$(top_srcdir)/NAS/EURECOM-NAS/src/api/network     \
        -I$(top_srcdir)/NAS/EURECOM-NAS/src/emm/msg         \
        -I$(top_srcdir)/NAS/EURECOM-NAS/src/emm/sap         \
        -I$(top_srcdir)/NAS/EURECOM-NAS/src/emm             \
        -I$(top_srcdir)/NAS/EURECOM-NAS/src/esm/msg         \
        -I$(top_srcdir)/NAS/EURECOM-NAS/src/esm/sap/msg     \
        -I$(top_srcdir)/NAS/EURECOM-NAS/src/esm/sap         \
        -I$(top_srcdir)/NAS/EURECOM-NAS/src/esm             \
        -I$(top_srcdir)/NAS/EURECOM-NAS/src/ies             \
        -I$(top_srcdir)/NAS/EURECOM-NAS/src/include         \
        -I$(top_srcdir)/NAS/EURECOM-NAS/src/util            \
        -I$(top_srcdir)/NAS/EURECOM-NAS/src/                \
	-I$(top_srcdir)/UTILS

if DISABLE_USE_NAS
libnas_la_SOURCES =	\
	nas_main.c
else
include Makefile.inc
libnas_la_SOURCES = \
	nas_main.c	\
	nas_itti_messaging.c	nas_itti_messaging.h	\
	$(libnas_SRCS)
endif