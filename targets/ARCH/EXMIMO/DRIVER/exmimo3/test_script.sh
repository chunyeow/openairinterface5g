#!/bin/bash

PCI=`lspci -m | grep Xilinx`
if [ -z "$PCI" ]; then
 echo "No card found. Stopping!"
 return
fi

SLOT_NUMBER=`echo $PCI | awk -F\" '{print $1}'`

setpci -s $SLOT_NUMBER 60.b=10

