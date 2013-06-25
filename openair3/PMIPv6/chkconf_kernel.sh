#!/bin/sh

# Simple shell script to check kernel configuration sanity
# for use with MIPL Mobile IPv6
#
# Author: Antti Tuominen <anttit@tcs.hut.fi>
#
# $Id: chkconf_kernel.sh 1.3 05/05/16 12:15:16+03:00 vnuorval@tcs.hut.fi $
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version
# 2 of the License, or (at your option) any later version.


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

if [ "$1" = "" ] ; then
    KERNELCONFIG="/proc/config.gz"
    CAT=zcat
else
    LINUX=$1
    KERNELCONFIG="$LINUX/.config"
    CAT=cat
fi

if [ ! -f $KERNELCONFIG ] ; then
    echo "$KERNELCONFIG does not exist."
    exit 1
fi

WARN=0;

echo
echo "Checking kernel configuration..."
echo "Using $KERNELCONFIG";

for TAG in $TAGS ; do
    VAL=`$CAT $KERNELCONFIG | sed -ne "/$TAG[= ]/s/^CONFIG_$TAG[= ]//gp"`;
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
