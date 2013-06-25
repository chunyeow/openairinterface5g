#!/bin/bash
MODULES="$MODULES ath3k bcm203x bluecard_cs bnep bpa10x bt3c_cs btmrvl btmrvl_sdio btsdio"
MODULES="$MODULES btusb btuart_cs cmtp dtl1_cs hidp hci_vhci hci_uart rfcomm sco bluetooth l2cap"
echo Stoping bluetooth service..
/etc/init.d/bluetooth stop
/etc/init.d/bluetooth status

for i in $MODULES; do
	grep ^$i /proc/modules 2>&1 > /dev/null
	if [ $? -eq 0 ]; then
		echo Unloading $i...
		modprobe -r --ignore-remove $i
	fi
done
