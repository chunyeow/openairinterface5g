#!/bin/bash


#Script by Lamia Romdhani
#June 2008

echo Setting MPLS  
sudo modprobe mpls6

echo Number of params = $#
if [ $# -eq 3 ]; then
	LABEL_IN=$1
	IP_NEXT_HOP=$2
	LABEL_OUT=$3

	echo "IP Next Hop is $IP_NEXT_HOP"

	echo 'Starting mpls'
	sudo mpls labelspace set dev nasmesh0 labelspace 0
	sudo mpls ilm add label gen $LABEL_IN labelspace 0 proto ipv6
var=`mpls nhlfe add key 0 instructions push gen $LABEL_OUT nexthop nasmesh0 ipv6     $IP_NEXT_HOP |grep key | cut -c 17-26`
	sudo mpls xc add ilm_label gen $LABEL_IN ilm_labelspace 0 nhlfe_key $var
else
	echo usage: mpls_nas.sh LABEL_IN IP_NEXT_HOP LABEL_OUT
fi
