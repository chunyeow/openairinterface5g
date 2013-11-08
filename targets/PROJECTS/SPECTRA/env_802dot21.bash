#!/bin/bash


#####################################################
# VARIABLES TO BE FILLED WITH RIGHT VALUES:
#####################################################
export BOOST_ROOT=/usr/local/src/boost_1_48_0
export ODTONE_ROOT=/usr/local/src/odtone_spectra_v1
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
            echo_success "Program b2 is now reachable by the PATH variable." >&2
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

