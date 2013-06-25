#!/bin/sh
# transfered from HA
#DOWNWARD HO
#ER_START="0.004"
#ER_END="0.016"
#UPWARD HO
ER_START="0.014"
ER_END="0.004"
###############
OUTBOUNDIF=eth1
INBOUNDIF=eth0
LOG_AR="logWLAN.log"
DURATION=10000
TIMESLICE=100
#tc qdisc change dev $INBOUNDIF  root netem loss $ER_START%
#tc qdisc change dev $OUTBOUNDIF root netem loss $ER_START%
pwd
./waitfortime $1
TIMESECOND=`date +%s`
TIMENANO=`date +%N`
echo $TIMESECOND.$TIMENANO $ER_START > $LOG_AR
sleep 5
#./radio_emulation $OUTBOUNDIF $ER_START $ER_END $INBOUNDIF $ER_START $ER_END $DURATION $TIMESLICE >> $LOG_AR
sleep 5
TIMESECOND=`date +%s`
TIMENANO=`date +%N`
echo $TIMESECOND.$TIMENANO $ER_END >> $LOG_AR
exit 0



