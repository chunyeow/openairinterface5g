#!/bin/bash
#
# Copyright 2012	Luis R. Rodriguez <mcgrof@frijolero.org>
# Copyright 2011	Hauke Mehrtens <hauke@hauke-m.de>
# Copyright 2011	John W. Linville <linville@tuxdriver.com>
#
# Use this to parse a small .config equivalent looking file to generate
# our own autoconf.h. This file has defines for each config option
# just like the kernels include/linux/autoconf.h
#
# XXX: consider using scripts/kconfig/confdata.c instead.
# On the downside this would require the user to have libc though.

# This indicates which is the oldest kernel we support
# Update this if you are adding support for older kernels.
OLDEST_KERNEL_SUPPORTED="2.6.24"

if [ $# -ne 1 ]; then
	echo "Usage $0 config-file"
	exit
fi

COMPAT_CONFIG="$1"

if [ ! -f $COMPAT_CONFIG ]; then
	echo "File $1 is not a file"
	exit
fi

# Defines a CONFIG_ option if not defined yet, this helps respect
# linux/autoconf.h
function define_config {
	VAR=$1
	VALUE=$2
	case $VALUE in
	n) # Try to undefine it
		echo "#undef $VAR"
		;;
	y)
		echo "#ifndef $VAR"
		echo "#define $VAR 1"
		echo "#endif /* $VAR */"
		;;
	m)
		echo "#ifndef $VAR"
		echo "#define $VAR 1"
		echo "#endif /* $VAR */"
		;;
	*) # Assume string
		# XXX: add better checks to make sure what was on
		# the right was indeed a string
		echo "#ifndef $VAR"
		echo "#define $VAR \"$VALUE\""
		echo "#endif /* $VAR */"
		;;
	esac
}

function kernel_version_req {
	VERSION=$(echo $1 | sed -e 's/\./,/g')
	echo "#if (LINUX_VERSION_CODE < KERNEL_VERSION($VERSION))"
	echo "#error compat requirement: Linux >= $VERSION"
	echo "#endif /* (LINUX_VERSION_CODE < KERNEL_VERSION($VERSION) */"
}

cat <<EOF
#ifndef COMPAT_AUTOCONF_INCLUDED
#define COMPAT_AUTOCONF_INCLUDED
/*
 * Automatically generated C config: don't edit
 */
EOF

# Checks user is compiling against a kernel we support
kernel_version_req $OLDEST_KERNEL_SUPPORTED

# For each CONFIG_FOO=x option
for i in $(egrep '^CONFIG_|^ifdef CONFIG_|^ifndef CONFIG_|^endif #CONFIG_|^else #CONFIG_' $COMPAT_CONFIG | sed 's/ /+/'); do
	case $i in
	'ifdef+CONFIG_'* )
		echo "#$i" | sed -e 's/+/ /' -e 's/\(ifdef CONFIG_COMPAT_KERNEL_3_\)\([0-9]*\)/if (LINUX_VERSION_CODE < KERNEL_VERSION(3,\2,0))/' -e 's/\(ifdef CONFIG_COMPAT_KERNEL_2_6_\)\([0-9]*\)/if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,\2))/' -e 's/\(ifdef CONFIG_COMPAT_RHEL_\)\([0-9]*\)_\([0-9]*\)/if (defined(RHEL_MAJOR) \&\& RHEL_MAJOR == \2 \&\& RHEL_MINOR >= \3)/' -e 's/\(#ifdef \)\(CONFIG_[^:space:]*\)/#if defined(\2) || defined(\2_MODULE)/'
		continue
		;;
	'ifndef+CONFIG_'* )
		echo "#$i" | sed -e 's/+/ /' -e 's/\(ifndef CONFIG_COMPAT_KERNEL_3_\)\([0-9]*\)/if (LINUX_VERSION_CODE >= KERNEL_VERSION(3,\2,0))/' -e 's/\(ifndef CONFIG_COMPAT_KERNEL_2_6_\)\([0-9]*\)/if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,\2))/' -e 's/\(ifndef CONFIG_COMPAT_RHEL_\)\([0-9]*\)_\([0-9]*\)/if (!defined(RHEL_MAJOR) || RHEL_MAJOR != \2 || RHEL_MINOR < \3)/' -e 's/\(#ifndef \)\(CONFIG_[^:space:]*\)/#if !defined(\2) \&\& !defined(\2_MODULE)/'
		continue
		;;
	'else+#CONFIG_'* | 'endif+#CONFIG_'* )
		echo "#$i */" |sed -e 's/+#/ \/* /g'
		continue
		;;
	CONFIG_* )
		# Get the element on the left of the "="
		VAR=$(echo $i | cut -d"=" -f 1)
		# Get the element on the right of the "="
		VALUE=$(echo $i | cut -d"=" -f 2)

		# Any other module which can *definitely* be built as a module goes here
		define_config $VAR $VALUE
		continue
		;;
	esac
done

echo "#endif /* COMPAT_AUTOCONF_INCLUDED */"
