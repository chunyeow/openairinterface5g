sh /opt/XIO2000/xio_script.sh
cd ../USERSPACE_TOOLS/OPENAIR_RF
./openair_rf_cbmimo1_lte 0 4 1
rmmod -f openair_rf
cd ../../
make install_cbmimo1_softmodem_lte_emos_l2
cd USERSPACE_TOOLS/OPENAIR_RF
./openair_rf_cbmimo1_lte 0 0 1 1
./openair_rf_cbmimo1_lte 0 6 120 120 100 100
./openair_rf_cbmimo1_lte 0 26 100   #Card v2_10 synched to v2_19
./openair_rf_cbmimo1_lte 0 14 0
./openair_rf_cbmimo1_lte 0 32 2
./openair_rf_cbmimo1_lte 0 1 1 0
cd ../../LAUNCH_SCRIPTS
