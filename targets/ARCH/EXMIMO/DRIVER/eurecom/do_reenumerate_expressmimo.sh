#!/bin/sh

# Re-Enumerate FPGA card

# After resetting or powering up the card or after reloading the FPGA bitstream,
# run this script to re-enumerate the PCIe device in Linux.

# You may need to change the device path. Check lspci output for this.

# You need to run this as root:
# sudo ./do_reenumerate_expressmimo.sh

# Matthias <ihmig@eurecom.fr>, 2013

rmmod openair_rf
echo 1 > /sys/bus/pci/devices/0000\:01\:00.0/remove
echo 1 > /sys/bus/pci/devices/0000\:05\:00.0/remove
echo 1 > /sys/bus/pci/rescan

insmod openair_rf.ko
