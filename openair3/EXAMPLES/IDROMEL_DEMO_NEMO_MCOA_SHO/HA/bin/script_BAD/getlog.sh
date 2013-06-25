#!/bin/sh
TIME=`date +%R`
HOUR=${TIME%:*}
MIN=${TIME#*:}
FOLDER="log`date +%Y``date +%m``date +%d`_$HOUR$MIN"
CN=cn
LFN=lfn_rd1
MR=mr
UMTSAR=umts_ar
WLANAR=wlan_ar
LOG_LFN=logLFN.log
LOG_CN=logCN.log
LOG_HA_TMP=/var/log/kernel/info
LOG_HA=logHA.log
LOG_MR_TMP=/var/log/kmessages
LOG_MR=logMR.log
LOG_3GAR="log3G.log"
LOG_WLANAR="logWLAN.log"
CN_DIR="/opt/TESTBED_NEMO_MCOA_SHO/log"
LFN_DIR="/opt/TESTBED_NEMO_MCOA_SHO/log"
MR_DIR="/opt/TESTBED_NEMO_MCOA_SHO/log"
AR_DIR="/opt/TESTBED_NEMO_MCOA_SHO/log"
SCRIPT_LFN="deinitLFN.sh"
SCRIPT_CN="deinitCN.sh"
PRESCRIPT_AR="initAR.sh"
CMD_LFN="cd $LFN_DIR;sh $SCRIPT_LFN"
CMD_CN="cd $CN_DIR;sh $SCRIPT_CN"
CMD_MR="cd $MR_DIR;at -f $SCRIPT_MR $START_TIME"
PRECMD_AR="cd $AR_DIR;sh $PRESCRIPT_AR"

# on HA
PID_TCPDUMP=`ps -A|grep tcpdump|cut -c1-5`
kill -9 $PID_TCPDUMP
# on CN
scp -26  root@$CN:$CN_DIR/$LOG_CN $LOG_CN
scp -26  $SCRIPT_CN root@$CN:$CN_DIR/.
ssh -26  -l root $CN $CMD_CN
scp -26  root@$CN:$CN_DIR/"logCNtcpdump.log" .

# on ARs
ssh -26  -l    root $WLANAR $PRECMD_AR
ssh -26  -l    root $UMTSAR $PRECMD_AR
scp -26  root@$UMTSAR:$AR_DIR/$LOG_3GAR $LOG_3GAR
scp -26  root@$WLANAR:$AR_DIR/$LOG_WLANAR $LOG_WLANAR
# on LFN
scp -26  $SCRIPT_LFN root@$LFN:$LFN_DIR/.
ssh -26  -l root $LFN $CMD_LFN
scp -26  root@$LFN:$LFN_DIR/$LOG_LFN $LOG_LFN
scp -26  root@$LFN:$LFN_DIR/"logLFNtcpdump.log" .
# on MR
scp -26  root@$MR:$LOG_MR_TMP $LOG_MR

#rename
mkdir $FOLDER
mv $LOG_CN             ./$FOLDER/$LOG_CN
mv $LOG_LFN            ./$FOLDER/$LOG_LFN
mv "logLFNtcpdump.log" ./$FOLDER/.
mv $LOG_HA_TMP         ./$FOLDER/$LOG_HA
mv $LOG_MR             ./$FOLDER/$LOG_MR
mv "logHAtcpdump.log"  ./$FOLDER/.
mv $LOG_3GAR           ./$FOLDER/.
mv $LOG_WLANAR         ./$FOLDER/.
mv "logCNtcpdump.log"  ./$FOLDER/.

