#!/bin/sh

# Module loading
sudo depmod -a
sudo modprobe mac80211_eurecom
sudo insmod ${OPENAIRITS_DIR}/phy/DRIVERS/ieee80211p.ko
