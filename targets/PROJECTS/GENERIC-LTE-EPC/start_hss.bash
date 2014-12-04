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
#  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE
#
################################################################################
# file start_hss.bash
# brief
# author Lionel Gauthier
# company Eurecom
# email: lionel.gauthier@eurecom.fr
###########################################
# INPUT OF THIS SCRIPT: NONE
#########################################
# This script start HSS component

#

###########################################################
THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
. $THIS_SCRIPT_PATH/utils.bash
###########################################################

HSS_CONFIG_FILE="$OPENAIRCN_DIR/OPENAIRHSS/conf/hss.conf"

######################################
# CHECK MISC SOFTWARES AND LIBS      #
######################################
#check_install_hss_software

######################################
# compile HSS                        #
######################################
compile_hss

######################################
# Check realm in certificate         #
######################################
TMP_FILE="/tmp/$(basename $0).$RANDOM.txt"
sed -r 's/\s+//g' $HSS_CONFIG_FILE > $TMP_FILE
source $TMP_FILE
rm -f  $TMP_FILE

TMP_FILE="/tmp/$(basename $0).$RANDOM.txt"
sed -r 's/\s+//g' $OPENAIRCN_DIR/OPENAIRHSS/conf/$FD_conf | grep Identity | tr -d ";\"" | cut -d '=' -f2 > $TMP_FILE
HSS_IDENTITY=$(cat $TMP_FILE)
# we should replace 'hss' with hostname
REALM=${HSS_IDENTITY##hss}
REALM=${REALM##.}
echo "REALM FOUND=$REALM"
# arg is realm
check_hss_s6a_certificate $REALM

######################################
# LAUNCH HSS                         #
######################################
$OPENAIRCN_DIR/OPENAIRHSS/objs/openair-hss -c $HSS_CONFIG_FILE


