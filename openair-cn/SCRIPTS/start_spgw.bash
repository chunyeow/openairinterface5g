#!/bin/bash
################################################################################
#   OpenAirInterface
#   Copyright(c) 1999 - 2014 Eurecom
#
#    OpenAirInterface is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#
#    OpenAirInterface is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with OpenAirInterface.The full GNU General Public License is
#    included in this distribution in the file called "COPYING". If not,
#    see <http://www.gnu.org/licenses/>.
#
#  Contact Information
#  OpenAirInterface Admin: openair_admin@eurecom.fr
#  OpenAirInterface Tech : openair_tech@eurecom.fr
#  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
#
#  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.
#
################################################################################
# start S+P-GW without configuring anything in networking

###############################
# Include misc functions
###############################
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
source $THIS_SCRIPT_PATH/utils.bash

echo "Starting S+P-GW on host $HOSTNAME"

# Some file and executable checks
check_for_root_rights
check_epc_config
check_for_sgw_executable

###############################
# Test reachability of MME
###############################
rm -f /tmp/source.txt
cat $OPENAIR3_DIR/OPENAIRMME/UTILS/CONF/epc_$HOSTNAME.conf | tr -d " " > /tmp/source.txt
source /tmp/source.txt

# echo 'Testing reachability of S11 MME'
# ping -c 1 $MME_IP_ADDRESS_FOR_S11_MME || { echo "MME_IP_ADDRESS_FOR_S11_MME=$MME_IP_ADDRESS_FOR_S11_MME does not respond to ping" >&2 ; exit ; }

###############################
# Test reachability of MME
###############################

#IP_ROUTER=`python -c 'import socket; print socket.gethostbyname("router.eur")'`
#export MAC_ROUTER=`ip neigh show | grep $IP_ROUTER | cut -d ' '  -f5 | tr -d ':'`
#echo_success "ROUTER MAC ADDRESS= $MAC_ROUTER"

export MAC_ROUTER="000000000000"


###############################
# CREATE SGI VLANs
###############################
bash_exec "modprobe 8021q"

for i in 5 6 7 8 9 10 11 12 13 14 15
do
    # create vlan interface
    bash_exec "vconfig rem $PGW_INTERFACE_NAME_FOR_SGI.$i"
    sync
    bash_exec "vconfig add $PGW_INTERFACE_NAME_FOR_SGI $i"
    sync
    # configure vlan interface
    #CIDR=$NETWORK'.'$i'/24'
    base=200
    NET=$(( $i + $base ))
    CIDR='10.0.'$NET'.2/8'
    bash_exec "ip -4 addr add  $CIDR dev $PGW_INTERFACE_NAME_FOR_SGI.$i"
    bash_exec "ip link set dev $PGW_INTERFACE_NAME_FOR_SGI.$i up"
done

bash_exec "ip link set $PGW_INTERFACE_NAME_FOR_SGI promisc on"

# custom for hades
ip r d default via 192.168.21.1 dev wlan0
ip r d default via 192.168.21.1 dev wlan0
pkill synergyc
synergyc 192.168.12.17

###############################
echo 'starting SGW'
###############################
gdb --args $OPENAIR3_DIR/OPENAIRMME/objs/OAI_SGW/oai_sgw -c $OPENAIR3_DIR/OPENAIRMME/UTILS/CONF/epc_$HOSTNAME.conf
