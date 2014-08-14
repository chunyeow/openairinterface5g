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
# file env_802dot21.bash
# brief
# author Lionel Gauthier
# company Eurecom
# email: lionel.gauthier@eurecom.fr
#

#####################################################
# VARIABLES TO BE FILLED WITH RIGHT VALUES:
#####################################################
export BOOST_ROOT=/usr/local/src/boost_1_49_0
export ODTONE_ROOT=/usr/local/src/ODTONE

export MIH_F=odtone-mihf
export ENB_MIH_F_CONF_FILE=odtone_enb.conf
export UE_MIH_F_CONF_FILE=odtone_ue.conf

export ODTONE_MIH_USER_DIR=$ODTONE_ROOT/app/lte_test_user
export ODTONE_MIH_EXE_DIR=$ODTONE_ROOT/dist

export ENB_MIH_USER=enb_lte_user
export ENB_MIH_USER_CONF_FILE=enb_lte_user.conf

export UE_MIH_USER=ue_lte_user
export UE_MIH_USER_CONF_FILE=ue_lte_user.conf
#####################################################


ENV_SCRIPT_SOURCED="?"
ENV_SCRIPT_ERRORS="no"

if [[ $BASH_SOURCE != $0 ]]; then
    THIS_SCRIPT_PATH=${BASH_SOURCE%env_802dot21.bash}
    [[ x"$THIS_SCRIPT_PATH" == x ]] && THIS_SCRIPT_PATH="./"
    ENV_SCRIPT_SOURCED="yes"
else
    THIS_SCRIPT_PATH=$(dirname $(readlink -f $0))
fi

ENV_SCRIPT_STARTED="yes"
source $THIS_SCRIPT_PATH/utils.bash

if [ -d $ODTONE_ROOT ]; then
    echo_success "ODTONE_ROOT     = $ODTONE_ROOT." >&2
else
    echo_error "ODTONE_ROOT variable was not set correctly, please update ($ODTONE_ROOT)." >&2
fi
if [ ! -d $BOOST_ROOT ]; then
    echo_error "BOOST_ROOT variable was not set correctly, please update (may be you also need to install boost), exiting."
    ENV_SCRIPT_ERRORS="yes"
else
    command -v b2 >/dev/null 2>&1
    if [ $? -ne 0 ]; then
        echo_warning "Program b2 is not installed or not in the PATH variable. Trying to resolve..." >&2
        if [[ -x "$BOOST_ROOT/b2" ]]; then
            echo_success "Program b2 found in dir $BOOST_ROOT." >&2
            export PATH=$PATH:$BOOST_ROOT
        else
            echo_warning "Program b2 not found in dir $BOOST_ROOT. Trying to install..." >&2
            cd $BOOST_ROOT; ./bootstrap.sh;
            cd -
            export PATH=$PATH:$BOOST_ROOT
        fi
        command -v $BOOST_ROOT/b2 >/dev/null 2>&1
        if [ $? -eq 0 ]; then
            echo_success "Program b2 is now reachable by the PATH variable during the execution of this script." >&2
        else
            echo_error "Built of b2 failed. Please help yourself" >&2
        fi
    fi
fi


if [ ! -d $ODTONE_ROOT ]; then
    echo_error "ODTONE_ROOT variable was not set correctly, please update (may be you also need to install odtone), exiting."
    ENV_SCRIPT_ERRORS="yes"
fi


[[ x"$ENV_SCRIPT_ERRORS" == "xyes" ]] && [[ x"$ENV_SCRIPT_SOURCED" == "xyes" ]] && return 1
[[ x"$ENV_SCRIPT_ERRORS" == "xyes" ]] && [[ x"$ENV_SCRIPT_SOURCED" == "xno" ]] && exit 1

