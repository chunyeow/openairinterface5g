#Cluster Head (2)– with NAS_MPLS_PMIP working
#CH2_NAS.SH
#!/bin/bash
#Phil, 25 july 08

source params.sh
./ch2_conf_nas.sh 
#MN1MN2
./mpls_nas.sh $MR2_CH2_LABEL_OUT $MR3_IN6_ADDR $MR3_LABEL_IN
#MN2MN1
./mpls_nas.sh $MR3_LABEL_OUT $MR2_IN6_ADDR2 $MR2_CH2_LABEL_IN
#echo $OPENAIR3_HOME/pmip6d/pmip6d -c -L $CH2_IN6_ADDR
#$OPENAIR3_HOME/pmip6d/pmip6d -c -L $CH2_IN6_ADDR
#./del_mpls.sh
