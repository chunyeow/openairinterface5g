sudo sh ../xio_script.sh
cd ../USERSPACE_TOOLS/OPENAIR_RF
./openair_rf_cbmimo1_lte 2 4 1
cd ../../
sudo make remove_cbmimo1
sudo make install_cbmimo1_softmodem
cd USERSPACE_TOOLS/OPENAIR_RF
./openair_rf_cbmimo1_lte 2 0 0 1
./openair_rf_cbmimo1_lte 2 6 120 120 100 100
#./openair_rf_cbmimo1_lte 2 26 100   #Card v2_10 synched to v2_19
./openair_rf_cbmimo1_lte 2 14 0
./openair_rf_cbmimo1_lte 2 32 1
./openair_rf_cbmimo1_lte 2 1 0 0
cd ../../LAUNCH_SCRIPTS
