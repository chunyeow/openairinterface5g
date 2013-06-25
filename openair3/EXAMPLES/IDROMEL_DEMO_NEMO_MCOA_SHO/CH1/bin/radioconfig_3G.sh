#!/bin/sh
# transfered from HA
#DOWNWARD HO
#ER_START="30.0"
#ER_END="1"

#UPWARD HO
ER_START="10"
ER_END="30"
###############
OUTBOUNDIF=eth0
INBOUNDIF=eth1
LOG_AR="log3G.log"
DURATION=10000
TIMESLICE=100
#LGtc qdisc change dev $INBOUNDIF  root netem loss $ER_START%
#LGtc qdisc change dev $OUTBOUNDIF root netem loss $ER_START%
#LG./waitfortime $1
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

