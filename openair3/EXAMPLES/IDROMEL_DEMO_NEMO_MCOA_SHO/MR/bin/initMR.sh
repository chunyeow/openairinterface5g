#!/bin/bash
KERNEL_LOGMR=/var/log/kmessages
if [ -f $KERNEL_LOGMR ] ; then 
    rm $KERNEL_LOGMR
fi
/etc/init.d/sysklogd restart
ntpdate -b 2001:4::1
