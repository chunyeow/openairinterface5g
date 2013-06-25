#!/bin/bash

source params.sh
sudo sysctl -w net.ipv6.conf.all.forwarding=1
#sudo sh -c 'echo 0 >/sys/mpls/debug
./del_mpls.sh
./mr2_conf_nas_eads.sh 

#MN1->MN2
./mpls_nas.sh $MR2_CH1_LABEL_IN $CH2_IN6_ADDR $MR2_CH2_LABEL_OUT
#MN2->MN1
./mpls_nas.sh $MR2_CH2_LABEL_IN $CH1_IN6_ADDR $MR2_CH1_LABEL_OUT

#CH1->CH2
./mpls_nas.sh $CH1_MR2_CH2_LABEL_OUT $CH2_IN6_ADDR $CH2_MR2_CH1_LABEL_IN
#CH2->CH1
./mpls_nas.sh $CH2_MR2_CH1_LABEL_OUT $CH1_IN6_ADDR $CH1_MR2_CH2_LABEL_IN


sudo sysctl -w net.ipv6.conf.eth0.proxy_ndp=1
sudo sysctl -w net.ipv6.conf.nasmesh0.proxy_ndp=1




