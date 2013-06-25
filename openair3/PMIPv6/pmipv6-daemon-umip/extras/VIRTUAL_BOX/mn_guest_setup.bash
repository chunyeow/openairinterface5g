#!/bin/bash


GATEWAY=`cat /mnt/if_mn | grep gateway | sed 's/^.*gateway//' | sed 's/[\t ]//g;;/^$/d'`
NAMESERVER=`cat /mnt/if_mn | grep nameserver | sed 's/^.*nameserver//' | sed 's/[\t ]//g;;/^$/d'`

# GET MAC ADDRESS IN FILE
MAC=`cat /mnt/if_mn | grep NIC1_MAC_ADDRESS | sed 's/^.*ADDRESS//' | sed 's/[\t ]//g;;/^$/d'`
# BUILD MAC ADDRESS WITH ":" SEPARATOR
MAC2=`expr substr $MAC 1 2`:`expr substr $MAC 3 2`:`expr substr $MAC 5 2`:`expr substr $MAC 7 2`:`expr substr $MAC 9 2`:`expr substr $MAC 11 2` 
# FIND THE CORRESPONDING INTERFACE NAME TO THE MAC ADDRESS
NIC1_NAME=`cat /etc/udev/rules.d/70-persistent-net.rules | grep -i $MAC2 | sed 's/^.*NAME=//' | tr -d "\""`
echo "ADDRESS NIC1 " $NIC1_NAME "="$MAC2 

# GET MAC ADDRESS IN FILE
MAC=`cat /mnt/if_mn | grep NIC2_MAC_ADDRESS | sed 's/^.*ADDRESS//' | sed 's/[\t ]//g;;/^$/d'`
# BUILD MAC ADDRESS WITH ":" SEPARATOR
MAC2=`expr substr $MAC 1 2`:`expr substr $MAC 3 2`:`expr substr $MAC 5 2`:`expr substr $MAC 7 2`:`expr substr $MAC 9 2`:`expr substr $MAC 11 2` 
# FIND THE CORRESPONDING INTERFACE NAME TO THE MAC ADDRESS
NIC2_NAME=`cat /etc/udev/rules.d/70-persistent-net.rules | grep -i $MAC2 | sed 's/^.*NAME=//' | tr -d "\""`
echo "ADDRESS NIC2 " $NIC2_NAME "="$MAC2 



cp -f /mnt/if_mn /etc/network/interfaces
sed -i "s/IF1_TO_FIND/"$NIC1_NAME"/" /etc/network/interfaces
sed -i "s/IF2_TO_FIND/"$NIC2_NAME"/" /etc/network/interfaces


# delete default route
ip r d `ip r s | grep default`
# delete old routes for the current ip address (routed to wrong interface)
ip r d `ip r s | grep eth0` > /dev/null 2>&1
ip r d `ip r s | grep eth1` > /dev/null 2>&1
ip r d `ip r s | grep eth2` > /dev/null 2>&1
ip r d `ip r s | grep eth3` > /dev/null 2>&1
ip r d `ip r s | grep eth4` > /dev/null 2>&1
ip r d `ip r s | grep eth5` > /dev/null 2>&1
ip r d `ip r s | grep eth6` > /dev/null 2>&1
ip r d `ip r s | grep eth7` > /dev/null 2>&1
ip r d `ip r s | grep eth8` > /dev/null 2>&1
ip r d `ip r s | grep eth9` > /dev/null 2>&1
ip r d `ip r s | grep eth10` > /dev/null 2>&1


cp /etc/resolv.conf /etc/resolv.conf.save

echo mn > /etc/hostname


/etc/init.d/networking restart
echo "nameserver $NAMESERVER" >> /etc/resolv.conf
sync



