#!/usr/bin/env bash
if [ -d net ] ; then
	make clean
fi
rm -rf net
rm -rf drivers
rm -rf include
rm -rf compat
rm -rf udev
rm -f master-tag
rm -f compat_base_tree
rm -f compat_base_tree_version
rm -f compat_version
rm -f code-metrics.txt
echo "Cleaned wireless-bt-compat-2.6"
