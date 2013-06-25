cd ../USERSPACE_TOOLS/OPENAIR_RF
./openair_rf_cbmimo1_lte 0 4 1
cd ../..
sudo rmmod nasmesh
sudo make remove_cbmimo1
cd LAUNCH_SCRIPTS
