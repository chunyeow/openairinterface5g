AM_CFLAGS = @ADD_CFLAGS@ 	\
    -I$(top_srcdir)/COMMON  \
    -I$(top_srcdir)/SGW-LITE  \
    -I$(top_srcdir)/UTILS   \
    -I$(top_srcdir)/UTILS/HASHTABLE   \
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/api/network		\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/include			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/ies				\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/emm/msg			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/esm/msg			\
	-I$(top_srcdir)/NAS/EURECOM-NAS/src/util			\
    -I$(top_srcdir)/INTERTASK_INTERFACE 
    
noinst_LTLIBRARIES = libsgi.la
libsgi_la_LDFLAGS = -all-static 
libsgi_la_SOURCES = \
	sgi_task.c	sgi.h	\
    sgi_socket.c \
    sgi_egress.c \
    sgi_util.c 

if ENABLE_USE_PCAP_FOR_SGI
    libsgi_la_SOURCES +=     sgi_pcap.c  \
                         /usr/lib/libpcap.a 

endif


if ENABLE_USE_NETFILTER_FOR_SGI
    libsgi_la_SOURCES +=     sgi_nf.c  \
        /usr/lib/libnetfilter_queue.a \
        /usr/lib/libnfnetlink.la
endif 


