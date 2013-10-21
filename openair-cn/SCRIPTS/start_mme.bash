#!/bin/bash
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


