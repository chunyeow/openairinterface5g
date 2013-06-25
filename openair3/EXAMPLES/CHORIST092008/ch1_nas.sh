#Cluster Head (1) – with NAS_MPLS_PMIP working

#!/bin/bash
#Phil, 25 july 08

source params.sh
./ch1_conf_nas.sh 
#MN1MN2
./mpls_nas.sh $MR1_LABEL_OUT $MR2_IN6_ADDR1 $MR2_CH1_LABEL_IN
#MN2MN1
./mpls_nas.sh $MR2_CH1_LABEL_OUT $MR1_IN6_ADDR $MR1_LABEL_IN
#echo $OPENAIR3_HOME/pmip6d/pmip6d -c -L $CH1_IN6_ADDR
#$OPENAIR3_HOME/pmip6d/pmip6d -c -L $CH1_IN6_ADDR
#./del_mpls.sh
