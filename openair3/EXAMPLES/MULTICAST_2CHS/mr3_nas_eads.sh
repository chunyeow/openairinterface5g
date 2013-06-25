#!/bin/bash

source params.sh
sudo sysctl -w net.ipv6.conf.all.forwarding=1
#./del_mpls.sh
./mr3_conf_nas_eads.sh 
./mr3_mpls_nas.sh


#/etc/init.d/radvd status
#/etc/init.d/radvd start
#radvdump 

sudo sysctl -w net.ipv6.conf.eth0.proxy_ndp=1
sudo sysctl -w net.ipv6.conf.nasmesh0.proxy_ndp=1

sudo ifconfig $ETH_MR3 promisc
sudo ip -6 addr add $MR3_EG6_ADDR/64 dev $ETH_MR3
sudo $OPENAIR3_PMIP6D_PATH/pmip6d -m -s -L $CH2_IN6_ADDR -N $MR3_EG6_ADDR -E $MR3_IN6_ADDR
./del_mpls.sh





