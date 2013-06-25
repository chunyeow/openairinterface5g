#!/bin/bash
# $Id: mk-pmgr_h.sh 1.1 05/02/21 14:45:42+02:00 anttit@tcs.hut.fi $

DEFAULTS=policy.h
INFILE=pmgr.h.in
OUTFILE=pmgr.h
TMP=tmp.$$

cat <<EOF > $TMP
/* Do not edit.  This file is automatically created during make. */
EOF

sed -ne '1,/##INSERT_FUNCTIONPOINTERS##/{p}' \
    $INFILE | grep -v '^/\* ##' >> $TMP
sed -ne '/int default_[^;]*$/,/);/{s/int default_\([^(]*\)/int (*\1)/;p}' \
    $DEFAULTS >> $TMP
sed -ne '/int default_[^;]*;$/{s/int default_\([^(]*\)/int (*\1)/;p}' \
    $DEFAULTS >> $TMP
sed -ne '/##INSERT_FUNCTIONPOINTERS##/,//{p}' \
    $INFILE | grep -v '^/\* ##' >> $TMP

indent -kr -i8 $TMP -o $OUTFILE
rm -f $TMP
