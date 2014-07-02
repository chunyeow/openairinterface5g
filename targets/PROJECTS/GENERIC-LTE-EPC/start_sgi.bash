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
# file start_sgi.bash
# brief
# author Lionel Gauthier
# company Eurecom
# email: lionel.gauthier@eurecom.fr
#

#########################################
# INPUT OF THIS SCRIPT:
# THE DIRECTORY WHERE ARE LOCATED THE CONFIGURATION FILES
#########################################


###########################################################
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
. $THIS_SCRIPT_PATH/utils.bash
. $THIS_SCRIPT_PATH/interfaces.bash
. $THIS_SCRIPT_PATH/networks.bash
###########################################################

if [ $# -eq 1 ]; then
    declare -x CONFIG_FILE_DIR=$1
    if [ ! -d $CONFIG_FILE_DIR ]; then
        echo_error "ERROR while invoking this script, as first argument to this script you have to provide the path to a directory (./CONF/VLAN.VIRTUAL.$HOSTNAME for example) containing valid epc and enb config files"
        exit 1
    fi
else
    echo_error "ERROR while invoking this script, as first argument to this script  you have to provide the path to a directory  (./CONF/VLAN.VIRTUAL.$HOSTNAME for example) containing valid epc and enb config files"
    exit 1
fi


cd $THIS_SCRIPT_PATH
#######################################################
# FIND CONFIG FILE
#######################################################
SEARCHED_CONFIG_FILE_EPC="epc*$HOSTNAME.conf"
CONFIG_FILE_EPC=`find $CONFIG_FILE_DIR -iname $SEARCHED_CONFIG_FILE_EPC`
if [ -f $CONFIG_FILE_EPC ]; then
    echo_warning "EPC config file found is now $CONFIG_FILE_EPC"
else
    echo_error "EPC config file not found, exiting (searched for $SEARCHED_CONFIG_FILE_EPC)"
    exit 1
fi



#######################################################
# SOURCE CONFIG FILE
#######################################################
rm -f /tmp/source.txt
VARIABLES="
           PGW_INTERFACE_NAME_FOR_SGI\|\
           PGW_IPV4_ADDR_FOR_SGI"

VARIABLES=$(echo $VARIABLES | sed -e 's/\\r//g')
VARIABLES=$(echo $VARIABLES | tr -d ' ')
cat $CONFIG_FILE_EPC | grep -w "$VARIABLES"| tr -d " " | tr -d ";" > /tmp/source.txt
source /tmp/source.txt

declare PGW_IPV4_NETMASK_FOR_SGI=$(          echo $PGW_IPV4_ADDR_FOR_SGI              | cut -f2 -d '/')
PGW_IPV4_ADDR_FOR_SGI=$(                     echo $PGW_IPV4_ADDR_FOR_SGI              | cut -f1 -d '/')


create_sgi_vlans
