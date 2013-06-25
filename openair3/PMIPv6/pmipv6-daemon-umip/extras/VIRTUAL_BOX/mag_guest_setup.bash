#!/bin/bash


GATEWAY=`cat /mnt/if_mag | grep gateway | sed 's/^.*gateway//' | sed 's/[\t ]//g;;/^$/d'`
NAMESERVER=`cat /mnt/if_mag | grep nameserver | sed 's/^.*nameserver//' | sed 's/[\t ]//g;;/^$/d'`

# GET MAC ADDRESS IN FILE
MAC=`cat /mnt/if_mag | grep INGRESS_MAC_ADDRESS | sed 's/^.*ADDRESS//' | sed 's/[\t ]//g;;/^$/d'`
# BUILD MAC ADDRESS WITH ":" SEPARATOR
MAC2=`expr substr $MAC 1 2`:`expr substr $MAC 3 2`:`expr substr $MAC 5 2`:`expr substr $MAC 7 2`:`expr substr $MAC 9 2`:`expr substr $MAC 11 2` 
# FIND THE CORRESPONDING INTERFACE NAME TO THE MAC ADDRESS
PMIP_INGRESS_IF_NAME=`cat /etc/udev/rules.d/70-persistent-net.rules | grep -i $MAC2 | sed 's/^.*NAME=//' | tr -d "\""`
echo "ADDRESS MAC INGRESS IF " $PMIP_INGRESS_IF_NAME "="$MAC2 

# GET MAC ADDRESS IN FILE
MAC=`cat /mnt/if_mag | grep EGRESS_MAC_ADDRESS | sed 's/^.*ADDRESS//' | sed 's/[\t ]//g;;/^$/d'`
# BUILD MAC ADDRESS WITH ":" SEPARATOR
MAC2=`expr substr $MAC 1 2`:`expr substr $MAC 3 2`:`expr substr $MAC 5 2`:`expr substr $MAC 7 2`:`expr substr $MAC 9 2`:`expr substr $MAC 11 2` 
# FIND THE CORRESPONDING INTERFACE NAME TO THE MAC ADDRESS
PMIP_EGRESS_IF_NAME=`cat /etc/udev/rules.d/70-persistent-net.rules | grep -i $MAC2 | sed 's/^.*NAME=//' | tr -d "\""`
echo "ADDRESS MAC EGRESS IF " $PMIP_EGRESS_IF_NAME "="$MAC2 

cp -f /mnt/if_mag /etc/network/interfaces
sed -i "s/PMIP_INGRESS_IF_TO_FIND/"$PMIP_INGRESS_IF_NAME"/" /etc/network/interfaces
sed -i "s/PMIP_EGRESS_IF_TO_FIND/"$PMIP_EGRESS_IF_NAME"/"   /etc/network/interfaces
# delete default route
ip r d `ip r s | grep default`
# delete old routes for the current ip address (routed to wrong interface)
ip r d `ip r s | grep eth0`
ip r d `ip r s | grep eth1`
ip r d `ip r s | grep eth2`
ip r d `ip r s | grep eth3`
ip r d `ip r s | grep eth4`
ip r d `ip r s | grep eth5`
ip r d `ip r s | grep eth6`
ip r d `ip r s | grep eth7`
ip r d `ip r s | grep eth8`


cp /etc/resolv.conf /etc/resolv.conf.save

echo mag > /etc/hostname


/etc/init.d/networking restart
ip r a default via $GATEWAY dev $PMIP_EGRESS_IF_NAME
echo "nameserver $NAMESERVER" >> /etc/resolv.conf
sync



