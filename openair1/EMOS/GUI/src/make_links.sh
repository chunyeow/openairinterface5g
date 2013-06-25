#!/bin/bash

# Make all the symlinks required for the emos_dump project
# Leonardo Cardoso
#
# V20070924

OPENAIR_PATH="../../../"; 

ln -sf ${OPENAIR_PATH}PHY/CONFIG/emos.cfg config.cfg
ln -sf ${OPENAIR_PATH}PHY/CONFIG/emos.scn scenario.scn