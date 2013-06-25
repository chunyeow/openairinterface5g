#!/bin/sh
# transfered from HA
IF_UMTS=eth0
IF_WLAN=eth2

./waitfortime $1

#Make-Before-Break HO
#sleep 8
#ifconfig $IF_WLAN up
#ifconfig $IF_UMTS up

#SHO(UPWARD)
sleep 3
ifconfig $IF_WLAN up
sleep 10
ifconfig $IF_UMTS down
sleep 10
ifconfig $IF_UMTS up
sleep 10
ifconfig $IF_WLAN down
sleep 10
ifconfig $IF_WLAN up
sleep 10
ifconfig $IF_UMTS down

#SHO(DOWNWARD)
#sleep 3
#ifconfig $IF_UMTS up
#sleep 11
#ifconfig $IF_WLAN down

