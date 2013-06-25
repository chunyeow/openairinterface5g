cd $OPENAIR1_DIR/USERSPACE_TOOLS/OPENAIR_RF
echo STOP STARTS...
./openair_rf_cbmimo1 1 4 1
sleep 2
echo STOP RF DONE
sudo killall chbch_scope
echo STOP CHBCH SCOPE DONE
sudo killall sach_scope
echo STOP_SACH SCOPE DONE
sleep 1
sudo rmmod nasmesh
sleep 1
echo REMOVE NASMESH MODULE DONE
sudo rmmod openair_rrc
sleep 1
echo REMOVE RRC MODULE DONE
sudo rmmod openair_l2
sleep 1
echo REMOVE LAYER2 MODULE DONE
sudo rmmod openair_rf
echo REMOVE RF MODULE DONE
echo STOP FINISHED SUCCESSFULLY
