if [ "$1" == "" ]; then
    echo "Please provide a TX power level (0-8)"
    exit
fi

if [ "$2" == "" ]; then
    echo "Please provide a Node ID (0-7 for CH, 8-15 for MR/UE)"
    exit
fi

if [ "$3" != "" ]; then
    echo "Setting timing advance to" $3
else 
    echo "Using standard timing advance"
fi

if [ "$OPENAIR1_DIR" == "" ] ; then
    echo "Please set OPENAIR1_DIR environment variable"
    exit
else
    echo "OPENAIR1_DIR Set to $OPENAIR1_DIR"
fi

cd $OPENAIR1_DIR/USERSPACE_TOOLS/OPENAIR_RF
source ./txgains.sh

case $1 in 
    0) 
	CBMIMO1_POWER=$CBMIMO1_m20_dBm
	echo "Configuring CBMIMO1 for -20dBm ($CBMIMO1_POWER)"
	;;
    1) 
	CBMIMO1_POWER=$CBMIMO1_m15_dBm
	echo Configuring CBMIMO1 for -15dBm
	;;
    2) 
	CBMIMO1_POWER=$CBMIMO1_m10_dBm
	echo Configuring CBMIMO1 for -10dBm
	;;
    3) 
	CBMIMO1_POWER=$CBMIMO1_m5_dBm
	echo Configuring CBMIMO1 for -5dBm
	;;
    4) 
	CBMIMO1_POWER=$CBMIMO1_0_dBm
	echo Configuring CBMIMO1 for 0dBm
	;;
    5) 
	CBMIMO1_POWER=$CBMIMO1_5_dBm
	echo Configuring CBMIMO1 for 5dBm
	;;
    6) 
	CBMIMO1_POWER=$CBMIMO1_10_dBm
	echo Configuring CBMIMO1 for 10dBm
	;;
    7) 
	CBMIMO1_POWER=$CBMIMO1_15_dBm
	echo Configuring CBMIMO1 for 15dBm
	;;
    8) 
	CBMIMO1_POWER=$CBMIMO1_20_dBm
	echo Configuring CBMIMO1 for 20dBm
	;;
    *)
	echo Invalid TX power
	exit
esac


echo Removing openair_rf,openair_l2 and openair_rrc modules
sudo rmmod -f openair_rrc
sudo rmmod -f openair_l2
sudo rmmod -f openair_rf

sudo insmod $OPENAIR1_DIR/ARCH/CBMIMO1/DEVICE_DRIVER/openair_rf_l2.ko
OARF_RET=`lsmod | grep -e openair_rf`

if [ "$OARF_RET" = "" ]; then
    echo Error loading openair1 module
    exit
fi

sudo insmod $OPENAIR2_DIR/LAYER2/openair_layer2.ko
OA2_RET=`lsmod | grep -e openair_l2`

if [ "$OA2_RET" = "" ]; then
    echo Error loading openair2 module
    exit
fi


sudo insmod $OPENAIR2_DIR/RRC/MESH/openair_RRC.ko
OARRC_RET=`lsmod | grep -e openair_rrc`
if [ "$OARRC_RET" = "" ]; then
    echo Error loading openair RRC module
    exit
fi

OA_DEV="/dev/openair0"

if [ -c "$OA_DEV" ]; then
    echo "$OA_DEV exists"
else
    echo "creating $OA_DEV"
    sudo mknod $OA_DEV c 127 0
    sudo chmod a+rw /dev/openair0
fi

./openair_rf_cbmimo1 1 0
./openair_rf_cbmimo1 1 6 $CBMIMO1_POWER 
if [ "$3" != "" ]; then
    ./openair_rf_cbmimo1 1 25 $3
fi
if [ $2 -eq "0" ]; then 
    ./openair_rf_cbmimo1 1 1 1 $2
elif [ $2 -lt "8" ]; then
    ./openair_rf_cbmimo1 1 2 1 $2
elif [ $2 -lt "16" ]; then
    ./openair_rf_cbmimo1 1 3 1 $2
else
    echo Illegal Node ID, exiting
    exit
fi

sleep 1


# Watch the status of the radio to check for hardware exit event

#OA1_STATE=`cat /proc/openair1/openair1_state`

#while [ "$OA1_STATE" != "EXITED" ]; do
#    OA1_STATE=`cat /proc/openair1/openair1_state`
#    sleep .5
#done

#./openair_rf_cbmimo1 1 4 1
#echo CBMIMO1 Hardware exit, restarting ...
