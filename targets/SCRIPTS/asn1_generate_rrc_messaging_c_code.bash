#!/bin/bash

# generate .c and .h files
which asn1c > /dev/null
if [ $? -eq 0 ] ; then echo "asn1c is installed" ; else echo "Please install asn1c (version 0.9.22 or greater)" ; fi
cd $OPENAIR2_DIR/RRC/LITE/MESSAGES && asn1c -gen-PER -fcompound-names -fnative-types -fskeletons-copy $OPENAIR2_DIR/RRC/LITE/MESSAGES/asn1c/ASN1_files/EUTRA-RRC-Definitions.asn


# generate ASN constants

rm -f $OPENAIR2_DIR/RRC/LITE/MESSAGES/asn1_constants.h
cat $OPENAIR2_DIR/RRC/LITE/MESSAGES/asn1c/ASN1_files/EUTRA-RRC-Definitions.asn | grep   INTEGER\ \:\: | sed  's/INTEGER ::=//g' | sed 's/--/\/\//g' | sed 's/^/#define /' | sed 's/\-1/_minus_1/g' | sed 's/\-/_/g' > $OPENAIR2_DIR/RRC/LITE/MESSAGES/asn1_constants.tmp 
rm -f /tmp/EUTRA-RRC-Definitions.tmp
cat $OPENAIR2_DIR/RRC/LITE/MESSAGES/asn1c/ASN1_files/EUTRA-RRC-Definitions.asn | grep  --invert-match SEQUENCE | grep INTEGER\ \( | uniq | grep \:\:\= | sed  's/INTEGER\ //g'  | sed 's/[()]//g' | tr "." " " | sed  's/\:\:\=//g' | tr '\t' " "  > /tmp/EUTRA-RRC-Definitions.tmp 
cat /tmp/EUTRA-RRC-Definitions.tmp | sed 's/^ *//g' | sed 's/ \{1,\}/ /g' | cut --complement -d ' '  -f3 | sed 's/^/#define min_val_/' | sed 's/\ -/\ +/g' | sed 's/-/_/g' | sed  's/\ +/\ -/g' >> $OPENAIR2_DIR/RRC/LITE/MESSAGES/asn1_constants.tmp 
cat /tmp/EUTRA-RRC-Definitions.tmp | sed 's/^ *//g' | sed 's/ \{1,\}/ /g' | cut --complement -d ' '  -f2 | sed 's/^/#define max_val_/' | sed 's/\ -/\ +/g' | sed 's/-/_/g' | sed  's/\ +/\ -/g' >> $OPENAIR2_DIR/RRC/LITE/MESSAGES/asn1_constants.tmp;
echo "#ifndef __ASN1_CONSTANTS_H__" > $OPENAIR2_DIR/RRC/LITE/MESSAGES/asn1_constants.h
echo "#define __ASN1_CONSTANTS_H__" >> $OPENAIR2_DIR/RRC/LITE/MESSAGES/asn1_constants.h
cat $OPENAIR2_DIR/RRC/LITE/MESSAGES/asn1_constants.tmp >> $OPENAIR2_DIR/RRC/LITE/MESSAGES/asn1_constants.h
echo "#endif " >> $OPENAIR2_DIR/RRC/LITE/MESSAGES/asn1_constants.h
rm -f $OPENAIR2_DIR/RRC/LITE/MESSAGES/asn1_constants.tmp;
