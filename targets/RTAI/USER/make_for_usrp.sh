#!/bin/bash

############### make nasmesh.ko ###############
sudo rmmod nasmesh
#cd ${OPENAIR2_DIR} && make nasmesh_netlink.ko
#cd ${OPENAIR2_DIR}/NAS/DRIVER/MESH/RB_TOOL/ && make
#make all
sudo insmod $OPENAIR2_DIR/NAS/DRIVER/MESH/nasmesh.ko

############## Ethernet config  ####################
sudo ifconfig eth0 mtu 4000
sudo sysctl -w net.core.wmem_max=1048576
sudo sysctl -w net.core.rmem_max=50000000

############## rtai modules ###################
if test \! -c /dev/rtai_shm; then
        sudo mknod -m 666 /dev/rtai_shm c 10 254
fi
for n in `seq 0 9`; do
        f=/dev/rtf$n
        if test \! -c $f; then
                sudo mknod -m 666 $f c 150 $n
        fi
done
sudo modprobe rtai_hal
sudo modprobe rtai_sched
sudo modprobe rtai_fifos
sudo modprobe rtai_sem
sudo modprobe rtai_mbx
sudo modprobe rtai_msg

############## make  ###################
make lte-softmodem-usrp NAS=1 USRP=1 XFORMS=1 RTAI=1 HARD_RT=1 #DRIVER2013=1
#make lte-softmodem NAS=1 XFORMS=1 USRP=0 RTAI=1 DRIVER2013=1
echo DONE!
exit 0
