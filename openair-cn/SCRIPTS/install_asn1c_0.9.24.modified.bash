#!/bin/bash
# Tested on ubuntu 12.04 with updates 07 november 2013


rm -Rf /usr/local/src/asn1c-r1516
svn co https://github.com/vlm/asn1c/trunk  /usr/local/src/asn1c-r1516 -r 1516
cd /usr/local/src/asn1c-r1516
patch -p0 < $OPENAIRCN_DIR/S1AP/MESSAGES/ASN1/asn1cpatch.p0
./configure
make
make install
cd -

