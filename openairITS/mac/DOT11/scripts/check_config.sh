#!/usr/bin/env bash
# This script checks the compat-wireless configuration file and if changes were made
# regenerates the compat_autoconf header.

# These variables are expected to be exported:
#COMPAT_CONFIG=".config"
#COMPAT_CONFIG_CW="config"
#CONFIG_CHECK=".${COMPAT_CONFIG}.md5"
#COMPAT_AUTOCONF="include/linux/compat_autoconf.h"

function gen_compat_autoconf {
	echo "./scripts/gen-compat-autoconf.sh $COMPAT_CONFIG $COMPAT_CONFIG_CW > $COMPAT_AUTOCONF"
	./scripts/gen-compat-autoconf.sh $COMPAT_CONFIG $COMPAT_CONFIG_CW > $COMPAT_AUTOCONF
	md5sum $COMPAT_CONFIG $COMPAT_CONFIG_CW > $CONFIG_CHECK
}

which md5sum 2>&1 > /dev/null
if [ $? -ne 0 ]; then
	echo "md5sum required to detect changes on config file"
	exit -1
fi

if [ ! -f $CONFIG_CHECK ]; then
	gen_compat_autoconf
	exit
fi

md5sum -c $CONFIG_CHECK 2> /dev/null 1>/dev/null

if [ $? -ne 0 ]; then
	echo "Changes to compat-wireless's configuration was detected, regenerating autoconf..."
	gen_compat_autoconf
fi

