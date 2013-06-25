#!/bin/bash
# Copyright 2012        Luis R. Rodriguez <mcgrof@frijolero.org>
# Copyright 2012        Hauke Mehrtens <hauke@hauke-m.de>
#
# This generates a bunch of CONFIG_COMPAT_KERNEL_2_6_22
# CONFIG_COMPAT_KERNEL_3_0 .. etc for each kernel release you need an object
# for.
#
# Note: this is part of the compat.git project, not compat-wireless.git,
# send patches against compat.git.

if [[ ! -f ${KLIB_BUILD}/Makefile ]]; then
	exit
fi

# Actual kernel version
KERNEL_VERSION=$(${MAKE} -C ${KLIB_BUILD} kernelversion | sed -n 's/^\([0-9]\)\..*/\1/p')

# 3.0 kernel stuff
COMPAT_LATEST_VERSION="5"
KERNEL_SUBLEVEL="-1"

# Note that this script will export all variables explicitly,
# trying to export all with a blanket "export" statement at
# the top of the generated file causes the build to slow down
# by an order of magnitude.

if [[ ${KERNEL_VERSION} -eq "3" ]]; then
	KERNEL_SUBLEVEL=$(${MAKE} -C ${KLIB_BUILD} kernelversion | sed -n 's/^3\.\([0-9]\+\).*/\1/p')
else
	COMPAT_26LATEST_VERSION="39"
	KERNEL_26SUBLEVEL=$(${MAKE} -C ${KLIB_BUILD} kernelversion | sed -n 's/^2\.6\.\([0-9]\+\).*/\1/p')
	let KERNEL_26SUBLEVEL=${KERNEL_26SUBLEVEL}+1

	for i in $(seq ${KERNEL_26SUBLEVEL} ${COMPAT_26LATEST_VERSION}); do
		eval CONFIG_COMPAT_KERNEL_2_6_${i}=y
		echo "export CONFIG_COMPAT_KERNEL_2_6_${i}=y"
	done
fi

let KERNEL_SUBLEVEL=${KERNEL_SUBLEVEL}+1
for i in $(seq ${KERNEL_SUBLEVEL} ${COMPAT_LATEST_VERSION}); do
	eval CONFIG_COMPAT_KERNEL_3_${i}=y
	echo "export CONFIG_COMPAT_KERNEL_3_${i}=y"
done

# The purpose of these seem to be the inverse of the above other varibales.
# The RHEL checks seem to annotate the existance of RHEL minor versions.
RHEL_MAJOR=$(grep ^RHEL_MAJOR ${KLIB_BUILD}/Makefile | sed -n 's/.*= *\(.*\)/\1/p')
if [[ ! -z ${RHEL_MAJOR} ]]; then
	RHEL_MINOR=$(grep ^RHEL_MINOR $(KLIB_BUILD)/Makefile | sed -n 's/.*= *\(.*\)/\1/p')
	for i in $(seq 0 ${RHEL_MINOR}); do
		eval CONFIG_COMPAT_${RHEL_MAJOR}_${i}=y
		echo "export CONFIG_COMPAT_${RHEL_MAJOR}_${i}=y"
	done
fi

if [[ ${CONFIG_COMPAT_KERNEL_2_6_33} = "y" ]]; then
	echo "export CONFIG_COMPAT_FIRMWARE_CLASS=m"
fi

if [[ ${CONFIG_COMPAT_KERNEL_2_6_36} = "y" ]]; then
	echo "export CONFIG_COMPAT_KFIFO=y"
fi
