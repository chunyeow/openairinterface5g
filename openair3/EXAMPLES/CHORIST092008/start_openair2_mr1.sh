#!/bin/bash

source params.sh
cd $OPENAIR2_DIR/SIMULATION/USER_TOOLS/LAYER2_SIM
./mac_sim -m2 -t $OPENAIR2_DIR/SIMULATION/TOPOLOGIES/eth_5nodes.top
