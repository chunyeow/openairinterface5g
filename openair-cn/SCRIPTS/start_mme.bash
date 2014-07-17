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
# start MME without configuring anything in networking

###############################
# Include misc functions
###############################
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
source $THIS_SCRIPT_PATH/utils.bash

# check_for_root_rights
check_for_mme_executable
check_epc_config

###############################
# Test reachability of HSS
###############################
ping -c 1 hss.eur || { echo "hss.eur does not respond to ping" >&2 ; exit ; }

###############################
# Test reachability of MME
###############################
rm -f /tmp/source.txt
cat $OPENAIR3_DIR/OPENAIRMME/UTILS/CONF/epc_$HOSTNAME.conf | tr -d " " > /tmp/source.txt
source /tmp/source.txt

ping -c 1 $MME_IP_ADDRESS_FOR_S1_MME  || { echo "MME_IP_ADDRESS_FOR_S1_MME=$MME_IP_ADDRESS_FOR_S1_MME does not respond to ping" >&2 ; exit ; }
ping -c 1 $SGW_IP_ADDRESS_FOR_S11 || { echo "SGW_IP_ADDRESS_FOR_S11=$SGW_IP_ADDRESS_FOR_S11 does not respond to ping" >&2 ; exit ; }


###############################
cecho "Starting MME on host $HOSTNAME" $blue
###############################
$OPENAIR3_DIR/OPENAIRMME/objs/OAISIM_MME/oaisim_mme -c $OPENAIR3_DIR/OPENAIRMME/UTILS/CONF/epc_$HOSTNAME.conf


