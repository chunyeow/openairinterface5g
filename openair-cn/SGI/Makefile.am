################################################################################
#   OpenAirInterface
#   Copyright(c) 1999 - 2014 Eurecom
#
#    OpenAirInterface is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#
#    OpenAirInterface is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with OpenAirInterface.The full GNU General Public License is
#    included in this distribution in the file called "COPYING". If not,
#    see <http://www.gnu.org/licenses/>.
#
#  Contact Information
#  OpenAirInterface Admin: openair_admin@eurecom.fr
#  OpenAirInterface Tech : openair_tech@eurecom.fr
#  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
#
#  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.
#
################################################################################
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
    -I$(top_srcdir)/NAS/EURECOM-NAS/src/util            \
    -I$(top_srcdir)/GTPV1-U            \
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


