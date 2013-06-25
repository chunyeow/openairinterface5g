#!/bin/bash
# $Id: mk-pmgr_c.sh 1.1 05/02/21 14:45:42+02:00 anttit@tcs.hut.fi $

DEFAULTS=policy.h
INFILE=pmgr.c.in
OUTFILE=pmgr.c
TMP=tmp.$$

cat <<EOF > $TMP
/* Do not edit.  This file is automatically created during make. */
EOF

sed -ne '1,/##INSERT_DEFAULTS##/{p}' $INFILE | grep -v '^/\* ##' > $TMP
sed -ne '/int default_/{s/int default_\([^(]*\).*/lb->\1 = default_\1;/;p}' $DEFAULTS >> $TMP
sed -ne '/##INSERT_DEFAULTS##/,/##INSERT_ENTRYPOINTS##/{p}' \
    $INFILE | grep -v '^/\* ##' >> $TMP
sed -ne '/int default_/{s/int default_\([^(]*\).*/func.v = dlsym(h, "\1");\nif (dlerror() == NULL)\nep->\1 = func.i;/;p}' $DEFAULTS >> $TMP
sed -ne '/##INSERT_ENTRYPOINTS##/,//{p}' $INFILE | grep -v '^/\* ##' >> $TMP

indent -kr -ncs -i8 $TMP -o $OUTFILE
rm -f $TMP
