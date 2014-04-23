#!/bin/bash
################################################################################
# Eurecom OpenAirInterface core network
# Copyright(c) 1999 - 2014 Eurecom
#
# This program is free software; you can redistribute it and/or modify it
# under the terms and conditions of the GNU General Public License,
# version 2, as published by the Free Software Foundation.
#
# This program is distributed in the hope it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
#
# The full GNU General Public License is included in this distribution in
# the file called "COPYING".
#
# Contact Information
# Openair Admin: openair_admin@eurecom.fr
# Openair Tech : openair_tech@eurecom.fr
# Forums       : http://forums.eurecom.fsr/openairinterface
# Address      : EURECOM,
#                Campus SophiaTech,
#                450 Route des Chappes,
#                CS 50193
#                06904 Biot Sophia Antipolis cedex,
#                FRANCE
################################################################################
# file start_enb.bash
# brief
# author Lionel Gauthier
# company Eurecom
# email: lionel.gauthier@eurecom.fr
###########################################
# INPUT OF THIS SCRIPT:
# THE DIRECTORY WHERE ARE LOCATED THE CONFIGURATION FILES
#########################################
# This script start  ENB+UE (all in one executable, on one host)
# Depending on configuration files, it can be instanciated a virtual switch 
# setting or a VLAN setting.
# MME+SP-GW executable have to be launched on the same host by your own (start_epc.bash) before this script is invoked.
#
###########################################################################################################################
#                                    VIRTUAL SWITCH SETTING
###########################################################################################################################
#
#                                                                           hss.eur
#                                                                             |
#        +-----------+          +------+              +-----------+           v   +----------+
#        |  eNB      +------+   |  ovs | VLAN 1+------+    MME    +----+      +---+   HSS    |
#        |           |cpenb0+------------------+cpmme0|           |    +------+   |          |
#        |           +------+   |bridge|       +------+           +----+      +---+          |
#        |           |upenb0+-------+  |              |           |               +----------+
#        +-----------+------+   |   |  |              +-----------+
#                               +---|--+                    |                   router.eur
#                                   |                 +-----------+              |   +--------------+
#                                   |                 |  S+P-GW   |              v   |   ROUTER     |
#                                   |  VLAN2   +------+           +-------+     +----+              +----+
#                                   +----------+upsgw0|           |sgi    +-...-+    |              |    +---...Internet
#                                              +------+           +-------+     +----+              +----+
#                                                     |           |      11 VLANS    |              |
#                                                     +-----------+   ids=[5..15]    +--------------+
#
#
###########################################################################################################################
#                                    VLAN SETTING
###########################################################################################################################
#                                                                           hss.eur
#                                                                             |
#        +-----------+                                +-----------+           v   +----------+
#        |  eNB      +------+            VLAN 1+------+    MME    +----+      +---+   HSS    |
#        |           |ethx.1+------------------+ethy.1|           |    +------+   |          |
#        |           +------+                  +------+           +----+      +---+          |
#        |           |ethx.2+-------+                 |           |               +----------+
#        |           +------+       |                 +-+-------+-+
#        |           |              |                   | s11mme|    
#        |           |              |                   +---+---+    
#        |           |              |             (optional)|   VLAN 3
#        +-----------+              |                   +---+---+    
#                                   |                   | s11sgw|            router.eur
#                                   |                 +-+-------+-+              |   +--------------+
#                                   |                 |  S+P-GW   |              v   |   ROUTER     |
#                                   |  VLAN2   +------+           +-------+     +----+              +----+
#                                   +----------+ethy.2|           |sgi    +-...-+    |              |    +---...Internet
#                                              +------+           +-------+     +----+              +----+
#                                                     |           |      11 VLANS    |              |
#                                                     +-----------+   ids=[5..15]    +--------------+

###########################################################
# Parameters
###########################################################
declare -x EMULATION_DEV_INTERFACE="eth1"
declare MAKE_LTE_ACCESS_STRATUM_TARGET="oaisim DEBUG=1 ENABLE_ITTI=1 USE_MME=R10 NAS=1 Rel10=1"
declare MAKE_IP_DRIVER_TARGET="ue_ip.ko"
declare IP_DRIVER_NAME="ue_ip"
declare LTEIF="oip1"
declare UE_IPv4="10.0.0.8"
declare UE_IPv6="2001:1::8"
declare UE_IPv6_CIDR=$UE_IPv6"/64"
declare UE_IPv4_CIDR=$UE_IPv4"/24"

###########################################################
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
source $THIS_SCRIPT_PATH/utils.bash
###########################################################

#check_install_epc_software

cd $THIS_SCRIPT_PATH

EMULATION_DEV_ADDRESS=`ifconfig $EMULATION_DEV_INTERFACE | grep 'inet addr:'| grep -v '127.0.0.1' | cut -d: -f2 | awk '{ print $1}'`

#######################################################
# USIM, NVRAM files
#######################################################
export NVRAM_DIR=$THIS_SCRIPT_PATH

if [ ! -f $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/ue_data ]; then
    make --directory=$OPENAIRCN_DIR/NAS/EURECOM-NAS veryveryclean
    make --directory=$OPENAIRCN_DIR/NAS/EURECOM-NAS PROCESS=UE
    rm .ue.nvram
fi
if [ ! -f $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/usim_data ]; then
    make --directory=$OPENAIRCN_DIR/NAS/EURECOM-NAS veryveryclean
    make --directory=$OPENAIRCN_DIR/NAS/EURECOM-NAS PROCESS=UE
    rm .usim.nvram
fi
if [ ! -f .ue.nvram ]; then
    # generate .ue_emm.nvram .ue.nvram
    $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/ue_data --gen
fi

if [ ! -f .usim.nvram ]; then
    # generate .usim.nvram
    $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/usim_data --gen
fi
$OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/ue_data --print
$OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/usim_data --print

##################################################
# LAUNCH eNB + UE executable
##################################################
echo "Bringup UE interface"
pkill oaisim
bash_exec "rmmod $IP_DRIVER_NAME" > /dev/null 2>&1

cecho "make $MAKE_IP_DRIVER_TARGET $MAKE_LTE_ACCESS_STRATUM_TARGET ....." $green
#bash_exec "make --directory=$OPENAIR2_DIR $MAKE_IP_DRIVER_TARGET "
make --directory=$OPENAIR2_DIR $MAKE_IP_DRIVER_TARGET || exit 1

#bash_exec "make --directory=$OPENAIR_TARGETS/SIMU/USER $MAKE_LTE_ACCESS_STRATUM_TARGET "
make --directory=$OPENAIR_TARGETS/SIMU/USER $MAKE_LTE_ACCESS_STRATUM_TARGET -j`grep -c ^processor /proc/cpuinfo ` || exit 1


bash_exec "insmod  $OPENAIR2_DIR/NETWORK_DRIVER/UE_IP/$IP_DRIVER_NAME.ko"

bash_exec "ip route flush cache"

sleep 1

bash_exec "sysctl -w net.ipv4.conf.all.log_martians=1"
assert "  `sysctl -n net.ipv4.conf.all.log_martians` -eq 1" $LINENO

echo "   Disabling reverse path filtering"
bash_exec "sysctl -w net.ipv4.conf.all.rp_filter=0"
assert "  `sysctl -n net.ipv4.conf.all.rp_filter` -eq 0" $LINENO


bash_exec "ip route flush cache"

# Check table 200 lte in /etc/iproute2/rt_tables
fgrep lte /etc/iproute2/rt_tables  > /dev/null 
if [ $? -ne 0 ]; then
    echo "200 lte " >> /etc/iproute2/rt_tables
fi
ip rule add fwmark 5 table lte
ip route add default dev $LTEIF table lte

ip route add 239.0.0.160/28 dev $EMULATION_DEV_INTERFACE


ITTI_LOG_FILE=./itti_ue.$HOSTNAME.log
rotate_log_file $ITTI_LOG_FILE
STDOUT_LOG_FILE=./stdout_ue.log

rotate_log_file $STDOUT_LOG_FILE
rotate_log_file $STDOUT_LOG_FILE.filtered

cd $THIS_SCRIPT_PATH

nohup xterm -e $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/UserProcess &

gdb --args $OPENAIR_TARGETS/SIMU/USER/oaisim -a -l9 -u1 -b0 -M1 -p2 -g1 -D $EMULATION_DEV_ADDRESS -K $ITTI_LOG_FILE  2>&1 | tee $STDOUT_LOG_FILE 

cat $STDOUT_LOG_FILE |  grep -v '[PHY]' | grep -v '[MAC]' | grep -v '[EMU]' | \
                        grep -v '[OCM]' | grep -v '[OMG]' | \
                        grep -v 'RLC not configured' | grep -v 'check if serving becomes' | \
                        grep -v 'mac_rrc_data_req'   | grep -v 'BCCH request =>' > $STDOUT_LOG_FILE.filtered
