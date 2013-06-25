cd ../USERSPACE_TOOLS/OPENAIR_RF
./openair_rf_cbmimo1_lte 0 4 1
sudo rmmod -f openair_rf
cd ../..
sudo make install_cbmimo1_softmodem OPENAIR2=0 FIRMWARE2010=1
cd LAUNCH_SCRIPTS
