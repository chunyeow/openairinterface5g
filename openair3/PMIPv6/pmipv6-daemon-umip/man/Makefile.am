## $Id: Makefile.am 1.3 03/12/12 14:03:21+02:00 anttit@tcs.hut.fi $
## Process this file with automake to produce Makefile.in

dist_man1_MANS = mip6d.man
dist_man5_MANS = mip6d.conf.man
dist_man7_MANS = mipv6.man
BUILT_SOURCES = $(dist_man1_MANS) $(dist_man5_MANS) $(dist_man7_MANS)
CLEANFILES = $(dist_man1_MANS) $(dist_man5_MANS) $(dist_man7_MANS)
EXTRA_DIST = mip6d.tmpl mip6d.conf.tmpl mipv6.tmpl 
MAINTAINERCLEANFILES = Makefile.in

%.man: %.tmpl
	$(SED) -e 's#%DATE%#@PACKAGE_DATE@#' \
               -e 's#%VERSION%#@PACKAGE_VERSION@#' \
               -e 's#%etc%#@sysconfdir@#' $< > $@
