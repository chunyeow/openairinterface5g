#!/bin/bash
#Lamia, Philippe huu nghia, 10 Oct 08
source params.sh

sudo sysctl -w net.ipv6.conf.all.forwarding=1
#./mr2_del_mpls.sh
./mr2_conf_nas.sh 
./mr2_mpls_nas.sh
sudo sysctl -w net.ipv6.conf.eth0.proxy_ndp=1
sudo sysctl -w net.ipv6.conf.nasmesh0.proxy_ndp=1

sudo ifconfig eth0 promisc
sudo ip -6 addr add $MR2_EG_ADDR/64 dev eth0
sudo xterm -hold -e "$OPENAIR3_PMIP6D_PATH/pmip6d -m -s -L $CH_ADDR -N $MR2_EG_ADDR -E $MR2_IN_ADDR" &
#$OPENAIR3_SCRIPTS_PATH/mr2_del_mpls.sh

#telnet localhost 7777 and then type pmip to see all binding entries
#ip -6 rule
#ip -6 route


watch -n.1 "cat /proc/openair2/lchan_stats ; cat /proc/openair1/bch_stats"

./stop_rf.sh
./ch_del_mpls.sh
