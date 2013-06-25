#!/bin/bash
CN=cn_ha
LFN=lfn_rd1
MR=mr
UMTSAR=umts_ar
WLANAR=wlan_ar
START_CN="tcp_send.sh"
#START_CN="tcp_send_CBR.sh"
INIT1_LFN="tcp_recv.sh"
INIT2_LFN="tcp_recv_dump.sh"
INIT_MR="initMR.sh"
INIT_HA="initHA.sh"
INIT_AR="initAR.sh"
WAIT_FOR_TIME="waitfortime"
SCRIPT_MR="HOtrigger.sh"
RADIOCONFIG_UMTSAR="radioconfig_3G.sh"
RADIOCONFIG_WLANAR="radioconfig_WLAN.sh"

echo "set variables"
#CN_DIR="/opt/TESTBED_NEMO_MCOA_SHO"
CN_DIR="/opt/TESTBED_NEMO_MCOA_SHO/bin"
#LFN_DIR="/opt/TESTBED_NEMO_MCOA_SHO"
LFN_DIR="/opt/TESTBED_NEMO_MCOA_SHO/bin"
#MR_DIR="/opt/TESTBED_NEMO_MCOA_SHO"
MR_DIR="/opt/TESTBED_NEMO_MCOA_SHO/bin"
#AR_DIR="/opt/TESTBED_NEMO_MCOA_SHO"
AR_DIR="/opt/TESTBED_NEMO_MCOA_SHO/bin"

CMD1_AR="tc qdisc change dev eth0 root netem loss 0%"
CMD2_AR="tc qdisc change dev eth1 root netem loss 0%"

#preparation 
echo "-----Preparation"
echo "-----Changing Radio Emulation Settings on 3G_AR"
ssh -l root $UMTSAR $CMD1_AR
ssh -l root $UMTSAR $CMD2_AR
echo "-----Changing Radio Emulation Settings on WLAN_AR"
ssh -l root $WLANAR $CMD1_AR
ssh -l root $WLANAR $CMD2_AR
# on CN
echo "-----copying file to CN"
scp $WAIT_FOR_TIME    root@$CN:$CN_DIR
scp $START_CN    root@$CN:$CN_DIR
# on AR1
echo "-----copying file to ARs"
scp $WAIT_FOR_TIME   root@$UMTSAR:$AR_DIR
scp $WAIT_FOR_TIME root@$WLANAR:$AR_DIR
scp $RADIOCONFIG_UMTSAR   root@$UMTSAR:$AR_DIR
scp $RADIOCONFIG_WLANAR root@$WLANAR:$AR_DIR
scp $INIT_AR  root@$UMTSAR:$AR_DIR
scp $INIT_AR  root@$WLANAR:$AR_DIR
# on MR
echo "-----copying file to MR"
scp $WAIT_FOR_TIME    root@$MR:$MR_DIR
scp $SCRIPT_MR    root@$MR:$MR_DIR
scp $INIT_MR root@$MR:$MR_DIR
# on LFN
echo "-----copying file to LFN"
scp $WAIT_FOR_TIME   root@$LFN:$LFN_DIR
scp $INIT1_LFN   root@$LFN:$LFN_DIR
scp $INIT2_LFN   root@$LFN:$LFN_DIR

echo "-----Preparation was done"
