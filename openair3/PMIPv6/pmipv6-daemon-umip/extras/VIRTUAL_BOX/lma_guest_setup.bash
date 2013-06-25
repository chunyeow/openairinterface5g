#!/bin/bash


GATEWAY=`cat /mnt/if_lma | grep gateway | sed 's/^.*gateway//' | sed 's/[\t ]//g;;/^$/d'`
NAMESERVER=`cat /mnt/if_lma | grep nameserver | sed 's/^.*nameserver//' | sed 's/[\t ]//g;;/^$/d'`

MAC=`cat /mnt/if_lma | grep BRIDGED_MAC_ADDRESS | sed 's/^.*ADDRESS//' | sed 's/[\t ]//g;;/^$/d'`
MAC2=`expr substr $MAC 1 2`:`expr substr $MAC 3 2`:`expr substr $MAC 5 2`:`expr substr $MAC 7 2`:`expr substr $MAC 9 2`:`expr substr $MAC 11 2` 
BRIDGE_IF_NAME=`cat /etc/udev/rules.d/70-persistent-net.rules | grep -i $MAC2 | sed 's/^.*NAME=//' | tr -d "\""`

MAC=`cat /mnt/if_lma | grep PMIP_NETWORK_MAC_ADDRESS | sed 's/^.*ADDRESS//' | sed 's/[\t ]//g;;/^$/d'`
MAC2=`expr substr $MAC 1 2`:`expr substr $MAC 3 2`:`expr substr $MAC 5 2`:`expr substr $MAC 7 2`:`expr substr $MAC 9 2`:`expr substr $MAC 11 2` 
PMIP_IF_NAME=`cat /etc/udev/rules.d/70-persistent-net.rules | grep -i $MAC2 | sed 's/^.*NAME=//' | tr -d "\""`

cp -f /mnt/if_lma /etc/network/interfaces
sed   -i "s/BRIDGED_IF_TO_FIND/"$BRIDGE_IF_NAME"/" /etc/network/interfaces
sed   -i "s/PMIP_IF_TO_FIND/"$PMIP_IF_NAME"/" /etc/network/interfaces
# delete default route
ip r d `ip r s | grep default`
# delete old routes for the current ip address (routed to wrong interface)
ip r d `ip r s | grep eth0`
ip r d `ip r s | grep eth1`
ip r d `ip r s | grep eth2`
ip r d `ip r s | grep eth3`


cp /etc/resolv.conf /etc/resolv.conf.save
/etc/init.d/networking restart
ip r a default via $GATEWAY dev $BRIDGE_IF_NAME
echo "nameserver $NAMESERVER" >> /etc/resolv.conf
sync

#################################################
# RETRIEVE PACKAGES FOR COMPILATION AND RUNNING #
#################################################
apt-get update
apt-get install -y subversion make gcc g++ indent openssl libssl-dev autoconf libpcap-dev bison byacc libtool flex


apt-get install -y libblas-dev libxml2 libxml2-dev libforms-bin libforms-dev libatlas-base-dev libatlas-dev 
apt-get install -y libpcap-dev  iproute-dev libc6-dev macchanger python-netaddr ndisc6 tshark

cd /usr/local/src 
cp -rupv /mnt/trunk.tar.bz2 .
sync
tar -xjf trunk.tar.bz2
sync
###########################################
## FREE RADIUS SERVER INSTALLATION FOR LMA
###########################################
cd /usr/local/src 
wget ftp://ftp.freeradius.org/pub/freeradius/freeradius-server-2.1.12.tar.bz2
tar xjf freeradius-server-2.1.12.tar.bz2
cd freeradius-server-2.1.12
./configure
make
make install

#####################
# ASN1C INSTALLATION
#####################
cd /usr/local/src/trunk/openair2/RRC/LITE/MESSAGES/asn1c
cp /mnt/asn1c* .
tar -xjf asn1c*
cd asn1c 
./configure
make
make install

#########################################
# GENERATE RRC MESSAGE STRUCTS FROM ASN1
#########################################
cd /usr/local/src/trunk/openair2/RRC/LITE/MESSAGES
asn1c -gen-PER -fcompound-names -fnative-types -fskeletons-copy /usr/local/src/trunk/openair2/RRC/LITE/MESSAGES/asn1c/ASN1_files/EUTRA-RRC-Definitions-86.asn


#########################################
# INSTALL MODIFIED (IPV6) FREERADIUS CLIENT LIBRARY
#########################################
cd /usr/local/src/trunk/openair3/PMIPv6/freeradius-client-1.1.6/
./configure
make
make install

#########################################
# INSTALL PMIP DAEMON
#########################################
cd /usr/local/src/trunk/openair3/PMIPv6/pmipv6-daemon-umip/
autoreconf -i
# --with-pmip-use-radius
./configure --enable-vt
make
make install

#########################################
# BUIL PMIP KERNEL
#########################################
apt-get install -y linux-source
cd /usr/src/linux-source-*
tar xjf linux-source-*

cd linux-source-*
make oldconfig && make prepare

# NOW SET DEFAULT SETTING FOR KERNEL
KERNEL_CONFIG_FILE=.config

# Default settings for kernel
EXPERIMENTAL=y
SYSVIPC=y
PROC_FS=y
NET=y
NET_KEY=y
NET_KEY_MIGRATE=y
INET=y 
IPV6=y
INET6_ESP=y
IPV6_TUNNEL=y
IPV6_MULTIPLE_TABLES=y
IPV6_SUBTREES=y
IPV6_MIP6=y
XFRM=y
XFRM_USER=y
XFRM_SUB_POLICY=y
INET6_XFRM_MODE_ROUTEOPTIMIZATION=y

COMMON="EXPERIMENTAL SYSVIPC PROC_FS NET INET IPV6 IPV6_MIP6 \
    XFRM XFRM_USER XFRM_SUB_POLICY INET6_XFRM_MODE_ROUTEOPTIMIZATION"

MNHA="IPV6_TUNNEL IPV6_MULTIPLE_TABLES"

MN="IPV6_SUBTREES"

IPSEC="INET6_ESP"

PFKEY="NET_KEY NET_KEY_MIGRATE"

TAGS="$COMMON $MNHA $MN $IPSEC $PFKEY"

if [ "$KERNEL_CONFIG_FILE" = "" ] ; then
    echo "Please provide a path to the config file of the kernel source tree..."
    exit -1
fi

if [ ! -f $KERNEL_CONFIG_FILE ] ; then
    echo "The config file of the kernel source tree does not exist."
    exit 1
fi

WARN=0;

echo
echo "Checking kernel configuration..."
echo "Using $KERNEL_CONFIG_FILE";

for TAG in $TAGS ; do
    CONFIG_TAG="CONFIG_"$TAG
    #echo "CONFIG_TAG="$CONFIG_TAG "=" "${!TAG}"
    sed   -i "s/.*"$CONFIG_TAG" is not set.*/"$CONFIG_TAG"="${!TAG}"/" $KERNEL_CONFIG_FILE 
    sed   -i "s/.*"$CONFIG_TAG"=.*/"$CONFIG_TAG"="${!TAG}"/" $KERNEL_CONFIG_FILE 
done

echo
echo "Checking kernel configuration..."
echo "Using $KERNEL_CONFIG_FILE";

for TAG in $TAGS ; do
    VAL=`cat $KERNEL_CONFIG_FILE | sed -ne "/$TAG[= ]/s/^CONFIG_$TAG[= ]//gp"`;
    eval "DFLT=\$$TAG";
    if [ "$VAL" != "$DFLT" ] ; then
    if [ -z "$VAL" ] ; then
        VERDICT="not supported";
    else
        VERDICT="$VAL";
    fi
    echo " Warning: CONFIG_$TAG should be set to $DFLT ($VERDICT)";
    let WARN=$WARN+1;
    fi
done

echo
if [ $WARN -eq 0 ] ; then
    echo "All kernel options are as they should.";
else
    echo "Above $WARN options may conflict with MIPL.";
    echo "If you are not sure, use the recommended setting.";
fi
echo

#######################################
# COMPILE AND INSTALL THE PMIP KERNEL
make
make modules_install
make install

# no need to run grub-install

#######################################
# update default entry in /boot/grub/grub.cfg

#FIND EXACT VERSION OF KERNEL SOURCE CODE
LINE==`cat .config | grep Kernel\ Configuration | sed 's/\ Kernel.*//'`
export LINUX_SOURCE_VERSION=`echo ${LINE##* }`

#FIND THE INDEX OF THE CORRESPONDING LINUX ENTRY IN grub.cfg...
ENTRY_INDEX=`cat /boot/grub/grub.cfg | grep menuentry | sed -e "/"$LINUX_SOURCE_VERSION"/,+10d" | wc -l`
echo $ENTRY_INDEX

#...AND MAKE IT THE DEFAULT ENTRY
sed -i "s/set default=\"0\"/set default=\""$ENTRY_INDEX"\"/"   /boot/grub/grub.cfg

#######################################
# REBOOT ON PMIP KERNEL
#######################################
shutdown -P now







