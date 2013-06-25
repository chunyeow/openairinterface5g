## $Id: Makefile.am 1.12 06/05/07 17:37:11+03:00 anttit@tcs.hut.fi $
## Process this file with automake to produce Makefile.in

AUTOMAKE_OPTIONS = gnu
EXTRA_DIST = BUGS INSTALL.kernel README.IPsec chkconf_kernel.sh \
	     extras/example-cn.conf extras/example-ha.conf \
             extras/example-mn.conf licenses

MAINTAINERCLEANFILES =  aclocal.m4 config.guess config.sub \
			configure config.h.in \
			ylwrap Makefile.in $(DIST_ARCHIVES)

if MISSING
LIBMISSING = libmissing
endif
SUBDIRS = $(LIBMISSING) libnetlink src man
