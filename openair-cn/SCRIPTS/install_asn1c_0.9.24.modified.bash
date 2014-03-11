#!/bin/bash
# Tested on ubuntu 12.04 with updates 07 november 2013

###########################################################
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
source $THIS_SCRIPT_PATH/utils.bash
###########################################################

if [ -f $OPENAIRCN_DIR/S1AP/MESSAGES/ASN1/asn1c-0.9.24.modified.tar.gz ]
then
    echo_success "Found file $OPENAIRCN_DIR/S1AP/MESSAGES/ASN1/asn1c-0.9.24.modified.tar.gz, installing..."
    cp $OPENAIRCN_DIR/S1AP/MESSAGES/ASN1/asn1c-0.9.24.modified.tar.gz /usr/local/src
    cd /usr/local/src
    extract /usr/local/src/asn1c-0.9.24.modified.tar.gz
    if [ $? -eq 0 ]; then
        cd asn1c-9.24
        autoreconf
        rm configure.in
        ./configure
        make
        make install
        exit 0
    else
        echo_error "Failed in extracting /usr/local/src/asn1c-0.9.24.modified.tar.gz, exiting"
    fi
else
    echo_error "File $OPENAIRCN_DIR/S1AP/MESSAGES/ASN1/asn1c-0.9.24.modified.tar.gz not found, please ask Eurecom developers how to get it, exiting"
fi
exit 1
