#!/bin/bash
#Phil,lamia 10 Oct 08

###Without PMIP
source params.sh
sudo ./ch_conf_nas.sh 
sudo ./ch_mpls_nas.sh $MR1_LABEL_OUT $MR2_IN_ADDR $MR2_LABEL_IN
sudo ./ch_mpls_nas.sh $MR2_LABEL_OUT $MR1_IN_ADDR $MR1_LABEL_IN
echo /openair3/pmip6d/pmip6d -c -L $CH_ADDR
sudo xterm -hold -e "$OPENAIR3_PMIP6D_PATH/pmip6d -c -L $CH_ADDR" & 
#./ch_del_mpls.sh

watch -n .1 "cat /proc/openair2/lchan_stats ; cat /proc/openair1/bch_stats ; cat /proc/openair1/openair1_state"


./stop_rf.sh
