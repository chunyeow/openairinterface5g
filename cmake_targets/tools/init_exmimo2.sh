#!/bin/bash

PCI=`lspci -m | grep Xilinx`
if [ -z "$PCI" ]; then
 echo "No card found. Stopping!"
 return
fi

## This part corrects the wrong configuration of the endpoint done by the bios in some machines
echo "$PCI" | while read config_reg; do
SLOT_NUMBER=`echo $config_reg | awk -F\" '{print $1}'`
sudo setpci -s $SLOT_NUMBER 60.b=10
done

load_module() {
  mod_name=${1##*/}
  mod_name=${mod_name%.*}
  if awk "/$mod_name/ {found=1 ;exit} END {if (found!=1) exit 1}" /proc/modules
    then
      echo "module $mod_name already loaded: I remove it first"
      sudo rmmod $mod_name
  fi
  echo loading $mod_name
  sudo insmod $1
}

load_module $OPENAIR_DIR/cmake_targets/bin/openair_rf.ko
sleep 1

if [ ! -e /dev/openair0 ]; then 
 sudo mknod /dev/openair0 c 127 0
 sudo chmod a+rw /dev/openair0
fi

DEVICE=`echo $PCI | awk -F\" '{print $(NF-1)}' | awk '{print $2}'`
DEVICE_SWID=${DEVICE:2:2}
if [ $DEVICE_SWID == '0a' ]; then
   echo "Using firware version 10"
   $OPENAIR_DIR/cmake_targets/bin/updatefw -s 0x43fffff0 -b -f $OPENAIR_TARGETS/ARCH/EXMIMO/USERSPACE/OAI_FW_INIT/sdr_expressmimo2_v10
else
   echo 'No corresponding firmware found'
   return
fi
