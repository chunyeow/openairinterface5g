#!/bin/bash
# To be used by distributions using compressed modules

COMPRESSION_FOUND="n"
COUNT=0;

for i in $(modprobe -l mac80211); do
	let COUNT=$COUNT+1
	i=${i##*/}
	if [ "$i" = "mac80211.ko.gz" ]; then
		COMPRESSION_FOUND="y"
		continue
	fi
done

if [ $COUNT -gt 2 ]; then
	echo "More than two mac80211 modules are detected, please report this."
	exit
fi

if [ $COMPRESSION_FOUND = "n" ]; then
	exit
fi

DIRS="$KLIB/$KMODDIR/net/mac80211/"
# This handles both drivers/net/ and drivers/net/wireless/
DIRS="$DIRS $KLIB/$KMODDIR/net/"
DIRS="$DIRS $KLIB/$KMODDIR/drivers/ssb/"
DIRS="$DIRS $KLIB/$KMODDIR/drivers/net/usb/"
DIRS="$DIRS $KLIB/$KMODDIR/drivers/net/wireless/"
DIRS="$DIRS $KLIB/$KMODDIR/drivers/misc/eeprom/"

for i in $DIRS; do
	if [ ! -d $i ]; then
		continue;
	fi
	for driver in $(find $i -type f -name *.ko); do
		gzip -9 $driver
	done
done
