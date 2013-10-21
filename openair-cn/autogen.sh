#! /bin/sh

(cd GTPV2-C/nwgtpv2c-0.11 && libtoolize && autoreconf --install --force)

aclocal \
&& autoheader \
&& automake --add-missing \
&& autoconf