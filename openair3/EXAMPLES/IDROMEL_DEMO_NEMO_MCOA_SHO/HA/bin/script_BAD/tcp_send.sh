#!/bin/sh
# transfered from HA
LOGFILE=logCN.log
TCPDUMPLOG=logCNtcpdump.log
WAITTIME=5
DURATION=60
tcpdump -i eth0 -w $TCPDUMPLOG -s 1500 > /dev/null &
echo "start" > $LOGFILE
date -R >> $LOGFILE
netstat -s|grep retrans >> $LOGFILE
ntpdate -b 2001:4::1 > /dev/null
./waitfortime $1


#WARNING

#iperf -V -c $2 -t $DURATION > /dev/null






netstat -s|grep retrans >> $LOGFILE
sleep $WAITTIME
echo "end" >> $LOGFILE
exit 0
