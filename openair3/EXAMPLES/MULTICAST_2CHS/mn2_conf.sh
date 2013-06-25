#!/bin/bash
#Huu-Nghia, Philippe, Lamia 28 june 2008

#echo Deleting MN1 autoconfigured addresse 
#$OPENAIR3_SCRIPTS_PATH/mn1_del.sh

#ip -6 route add 2001:660:5502::25/128 via 2001:660:5502::10 dev eth0


ip addr add $MN2_IN_ADDR/32 dev eth0

ip -6 addr del 2001:660:5502::25/64 dev eth0
ip -6 addr add 2001:660:5502::25/64 dev eth0
