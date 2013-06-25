#!/bin/bash

source params.sh
sudo sysctl -w net.ipv6.conf.all.forwarding=1
#./del_mpls.sh
./mr1_conf_nas_eads.sh 
./mr1_mpls_nas.sh

sudo sysctl -w net.ipv6.conf.eth0.proxy_ndp=1
sudo sysctl -w net.ipv6.conf.nasmesh0.proxy_ndp=1

sudo ifconfig $ETH_MR1 promisc
sudo ip -6 addr add $MR1_EG6_ADDR/64 dev $ETH_MR1
sudo $OPENAIR3_PMIP6D_PATH/pmip6d -m -s -L $CH1_IN6_ADDR -N $MR1_EG6_ADDR -E $MR1_IN6_ADDR
./del_mpls.sh





