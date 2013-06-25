#!/bin/bash
date +%s
START_TIME=`date +%s`
START_TIME=`expr $START_TIME + 30`
CN=cn
LFN=lfn_rd1
MR=mr
UMTSAR=umts_ar
WLANAR=wlan_ar
START_CN="tcp_send.sh"
#START_CN="tcp_send_CBR.sh"
INIT1_LFN="tcp_recv.sh"
INIT2_LFN="tcp_recv_dump.sh"
INIT_MR="initMR.sh"
SCRIPT_MR="HOtrigger.sh"
INIT_HA="initHA.sh"
INIT_AR="initAR.sh"
RADIOCONFIG_UMTSAR="radioconfig_3G.sh"
RADIOCONFIG_WLANAR="radioconfig_WLAN.sh"
echo "START TIME=$START_TIME"
echo "set variables"
CN_DIR="/opt/TESTBED_NEMO_MCOA_SHO/bin"
LFN_DIR="/opt/TESTBED_NEMO_MCOA_SHO/bin"
MR_DIR="/opt/TESTBED_NEMO_MCOA_SHO/bin"
AR_DIR="/opt/TESTBED_NEMO_MCOA_SHO/bin"
CMD_CN="cd $CN_DIR && sh $START_CN $START_TIME $LFN"
CMD_MR="cd $MR_DIR;sh $SCRIPT_MR $START_TIME"
PRECMD_MR="cd $MR_DIR;sh $INIT_MR"
CMD1_LFN="cd $LFN_DIR;sh $INIT1_LFN"
CMD2_LFN="cd $LFN_DIR;sh $INIT2_LFN"
PRECMD_AR="cd $AR_DIR;sh $INIT_AR"
CMD_UMTSAR="cd $AR_DIR;sh $RADIOCONFIG_UMTSAR $START_TIME"
CMD_WLANAR="cd $AR_DIR && sh $RADIOCONFIG_WLANAR $START_TIME"

KERNEL_LOGHA=/var/log/kernel/info

#start measurements
echo "-----Start Measurement"
# on HA
echo "on HA"
if [ -f $KERNEL_LOGHA ] ; then 
    rm /var/log/kernel/info
fi
/etc/init.d/sysklogd restart
sh $INIT_HA

#on ARs
echo "on ARs"
ssh -l    root $WLANAR $PRECMD_AR
ssh -l    root $UMTSAR $PRECMD_AR
ssh -f -l root $UMTSAR $CMD_UMTSAR
ssh -f -l root $WLANAR $CMD_WLANAR
#on CN
echo "on CN"
ssh -f -l root $CN $CMD_CN
#on MR
echo "on MR"
ssh -l    root $MR $PRECMD_MR
ssh -f -l root $MR $CMD_MR
#on LFN
echo "on LFN"
ssh -f -l root $LFN $CMD1_LFN
ssh -f -l root $LFN $CMD2_LFN

echo "starting measurements on $1"

