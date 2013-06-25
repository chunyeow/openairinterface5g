#
# Utilities for Proxy Mobile IPv6
# Author Huu-Nghia.Nguyen@eurecom.fr
#

#==========================================================================
proc progress { start end } {
        global ns
        set now [$ns now]
        if {$now <= $end && $now >= $start} {
                set percent [expr floor(($now-$start)*100/($end-$start))]
                puts "Progress: $percent%"
                $ns at [expr $now + 1] "progress $start $end"
        } else {
                puts "Progres: 100% Completed!"
        }
}
#==========================================================================
#Install radvd.conf in to the virtual machine <vmid> = {MN, AR1, AR2, CN, Router}
#You need to create the file <vmid>_radvd.conf first (Attention: No check for file existence)
# 
proc install-radvd vmid {
	global vms
	exec cp $vmid\_radvd.conf [$vms($vmid) set opt(uml_dir)]/hostfs/radvd.conf
	job $vmid cp /mnt/hostfs/radvd.conf /etc/radvd.conf
}
#==========================================================================
proc start-all-pmip6d { options } {
        job CH1 /mnt/hostfs/pmip6d $options -i -c -L 2001:100::1 -A 2001:200::1
        job MR1 /mnt/hostfs/pmip6d $options -i -m -s -L 2001:100::1 -E 2001:100::2 -N 2001:1::1

        job CH2 /mnt/hostfs/pmip6d $options -i -c -L 2001:200::1 -A 2001:100::1
        job MR2 /mnt/hostfs/pmip6d $options -i -m -s -L 2001:200::1 -E 2001:200::2 -N 2001:1::2
}

#==========================================================================
proc stop-all-pmip6d {} {
        job CH1 killall -INT pmip6d
        job CH2 killall -INT pmip6d
        job MR1 killall -INT pmip6d
        job MR2 killall -INT pmip6d
        job MN1 ifconfig eth0 down
        job MN2 ifconfig eth0 down
}

#==========================================================================
proc config-chorist {} {
	job RELAY sysctl -w net.ipv6.conf.all.forwarding=1
	install-radvd MR1
	install-radvd MR2
	job MR1 sysctl -w net.ipv6.conf.eth0.proxy_ndp=1
	job MR2 sysctl -w net.ipv6.conf.eth0.proxy_ndp=1
        job MR1 service radvd restart
        job MR2 service radvd restart

	global vms
	exec cp /usr/bin/telnet [$vms(CH1) set opt(uml_dir)]/hostfs/
	exec cp /usr/bin/telnet [$vms(CH2) set opt(uml_dir)]/hostfs/
	exec cp /usr/bin/telnet [$vms(MR1) set opt(uml_dir)]/hostfs/
	exec cp /usr/bin/telnet [$vms(MR2) set opt(uml_dir)]/hostfs/
}
#==========================================================================
proc progress { start end } {
	global ns
	set now [$ns now]
	if {$now <= $end && $now >= $start} {
		set percent [expr floor(($now-$start)*100/($end-$start))]
		puts "Progress: $percent%"
		$ns at [expr $now + 1] "progress $start $end"
	} else {
		puts "Progres: 100% Completed!"
	}
}
#==========================================================================
proc start-test-ro { options {filename "ro.log"} } {
        global ns
        set now [$ns now]

        #Step 1:        
        progress $now [expr $now + 25]
        setdest_1 150 100 100
        setdest_2 450 100 100
        job MN1 ifconfig eth0 down
        job MN2 ifconfig eth0 down

        #Start pmip6d
        $ns at [expr $now+5] "start-all-pmip6d $options"

        #MN1 joins the network 
        $ns at [expr $now+10] "job MN1 ifconfig eth0 up"
        $ns at [expr $now +11] "job MN2 ifconfig eth0 up"
	$ns at [expr $now+25] "job MN2 ping6 2001:1::fcfd:ff:fe00:600"


}
#==========================================================================
proc stop-test-ro {} {
        global ns
        set now [$ns now]

        progress $now [expr $now + 6]
        $ns at [expr $now+5] "job MN1 ifconfig eth0 down"
	$ns at [expr $now+5] "job MN2 ifconfig eth0 down"
        #Finish
        $ns at [expr $now+5] "puts \"Finish, stop all processes and services\""
        $ns at [expr $now+5] "stop-all-pmip6d"
	$ns at [expr $now+6] "job MN2 killall -INT ping6"
}
#==========================================================================
proc chorist-mobility { {filename "mobility.log"} } {
	global ns	
	set now [$ns now]

	#Step 1:	
	progress $now [expr $now + 200]
	setdest_1 150 100 100 
	setdest_2 450 100 100
	job MN1 ifconfig eth0 down
	job MN2 ifconfig eth0 down

	#Observation
	$ns at [expr $now+5] "start-all-pmip6d"

	#MN1 joins the network 
	$ns at [expr $now+10] "job MN1 ifconfig eth0 up"
	$ns at [expr $now +10] "job MN2 ifconfig eth0 up"

	#MN1 is moving to MR2 
	$ns at [expr $now+25] "setdest_1 450 100 100"

	#MN1 is moving back MR1
	$ns at [expr $now+100] "setdest_1 100 100 100"

	#MN1 power off
	$ns at [expr $now+175] "job MN1 ifconfig eth0 down"

	#Finish
	$ns at [expr $now+200] "puts \"Finish, stop all processes and services\""
	#$ns at [expr $now+150] "stop-all-pmip6d"
	$ns at [expr $now+201] "puts \"chorist-mobility: DONE!\""
}
#==========================================================================
proc chorist-basic { {filename "basic.log"} } {
	global ns	
	set now [$ns now]

	#Step 1:	
	progress $now [expr $now + 150]
	setdest_1 150 100 100 
	job MN1 ifconfig eth0 down

	#Observation
	$ns at [expr $now+5] "start-all-pmip6d"
	$ns at [expr $now+5] "job CH tcpdump -s 256 -i eth1 -v -w /mnt/hostfs/$filename "
 	$ns at [expr $now+5] "job MR1 tcpdump -s 256 -i eth0 -v -w /mnt/hostfs/$filename "
 	$ns at [expr $now+5] "job MR2 tcpdump -s 256 -i eth0 -v -w /mnt/hostfs/$filename "

	#MN1 joins the network & CN start to ping down; MN1 ping up
	$ns at [expr $now+10] "job MN1 ifconfig eth0 up"
	$ns at [expr $now+15] "job CN ping6 2001:1::fcfd:ff:fe00:500"
	$ns at [expr $now+15] "job MN1 ping6 2000::1"

	#MN1 is moving to MR2 
	$ns at [expr $now+25] "setdest_1 450 100 100"

	#MN1 is moving back MR1
	$ns at [expr $now+75] "setdest_1 100 100 100"

	#MN1 power off
	$ns at [expr $now+125] "job MN1 ifconfig eth0 down"

	#Finish
	$ns at [expr $now+150] "puts \"Finish, stop all processes and services\""
	$ns at [expr $now+150] "job CN killall -INT ping6"
	$ns at [expr $now+150] "job MN1 killall -INT ping6"
	$ns at [expr $now+150] "job CH killall -INT tcpdump"
	$ns at [expr $now+150] "job MR1 killall -INT tcpdump"
	$ns at [expr $now+150] "job MR2 killall -INT tcpdump"
	#$ns at [expr $now+150] "stop-all-pmip6d"
	$ns at [expr $now+151] "puts \"chorist-basic: DONE!\""
}

proc chorist-tcp-basic { {filename "tcp_basic.log"} {copt "-l 4M"} {options ""} } {
        global ns
        set now [$ns now]

        #Step 1:        
        progress $now [expr $now + 150]
        setdest_1 150 100 100
        job MN1 ifconfig eth0 down
	job CN ifconfig eth0 mtu 1300

        #Observation
        $ns at [expr $now+5] "start-all-pmip6d"
	$ns at [expr $now+5] "job CN iperf -V -s $options"
        $ns at [expr $now+5] "job CH tcpdump -s 256 -i eth1 -v -w /mnt/hostfs/$filename "
        $ns at [expr $now+5] "job MR1 tcpdump -s 256 -i eth0 -v -w /mnt/hostfs/$filename "
        $ns at [expr $now+5] "job MR2 tcpdump -s 256 -i eth0 -v -w /mnt/hostfs/$filename "
        $ns at [expr $now+5] "job CN tcpdump -s 256 -i eth0 -v -w /mnt/hostfs/$filename "

        #MN1 joins the network & send tcp traffic to CN
        $ns at [expr $now+10] "job MN1 ifconfig eth0 up mtu 1300"
        $ns at [expr $now+20] "job MN1 tcpdump -s 256 -i eth0 -v -w /mnt/hostfs/$filename "
        $ns at [expr $now+20] "job MN1 iperf -V -c 2000::1 $copt $options"

        #MN1 is moving to MR2 
        $ns at [expr $now+45] "setdest_1 450 100 100"

        #MN1 is moving back MR1
        $ns at [expr $now+75] "setdest_1 100 100 100"

        #MN1 power off
        $ns at [expr $now+124] "job MN1 killall -INT tcpdump"
        $ns at [expr $now+125] "job MN1 ifconfig eth0 down"

        #Finish
        $ns at [expr $now+150] "puts \"Finish, stop all processes and services\""
        $ns at [expr $now+150] "job CN killall -INT iperf"
        $ns at [expr $now+149] "job MN1 killall -INT iperf"
        $ns at [expr $now+150] "job CH killall -INT tcpdump"
        $ns at [expr $now+150] "job MR1 killall -INT tcpdump"
        $ns at [expr $now+150] "job MR2 killall -INT tcpdump"
        $ns at [expr $now+150] "job CN killall -INT tcpdump"
        #$ns at [expr $now+150] "stop-all-pmip6d"
        $ns at [expr $now+151] "puts \"chorist-basic: DONE!\""
}

proc chorist-udp-basic { {filename "udp_basic.log"} {copt "-t 100"} {options "-u"} } {
        global ns
        set now [$ns now]

        #Step 1:        
        progress $now [expr $now + 150]
        setdest_1 150 100 100
        job MN1 ifconfig eth0 down
        job CN ifconfig eth0 mtu 1300

        #Observation
        $ns at [expr $now+5] "start-all-pmip6d"
        $ns at [expr $now+5] "job CH tcpdump -s 256 -i eth1 -v -w /mnt/hostfs/$filename "
        $ns at [expr $now+5] "job MR1 tcpdump -s 256 -i eth0 -v -w /mnt/hostfs/$filename "
        $ns at [expr $now+5] "job MR2 tcpdump -s 256 -i eth0 -v -w /mnt/hostfs/$filename "
        $ns at [expr $now+5] "job CN tcpdump -s 256 -i eth0 -v -w /mnt/hostfs/$filename "

        #MN1 joins the network & send tcp traffic to CN
        $ns at [expr $now+10] "job MN1 ifconfig eth0 up mtu 1300"
        $ns at [expr $now+20] "job MN1 iperf -V -s $options"
        $ns at [expr $now+20] "job MN1 tcpdump -s 256 -i eth0 -v -w /mnt/hostfs/$filename "
        $ns at [expr $now+25] "job CN iperf -V -c 2001:1::fcfd:ff:fe00:500 $copt $options"

        #MN1 is moving to MR2 
        $ns at [expr $now+45] "setdest_1 450 100 100"

        #MN1 is moving back MR1
        $ns at [expr $now+75] "setdest_1 100 100 100"

        #MN1 power off
        $ns at [expr $now+124] "job MN1 killall -INT tcpdump"
	$ns at [expr $now+125] "job MN1 ifconfig eth0 down"

        #Finish
        $ns at [expr $now+150] "puts \"Finish, stop all processes and services\""
        $ns at [expr $now+150] "job CN killall -INT iperf"
        $ns at [expr $now+149] "job MN1 killall -INT iperf"
        $ns at [expr $now+150] "job CH killall -INT tcpdump"
        $ns at [expr $now+150] "job MR1 killall -INT tcpdump"
        $ns at [expr $now+150] "job MR2 killall -INT tcpdump"
        $ns at [expr $now+150] "job CN killall -INT tcpdump"
        #$ns at [expr $now+150] "stop-all-pmip6d"
        $ns at [expr $now+151] "puts \"chorist-udp-basic: DONE!\""
}

#==========================================================================
proc chorist-intra { {filename "intra.log"} } {
	global ns stdout
	set now [$ns now]

	#Step 1:	
	progress $now [expr $now + 201]
	setdest_1 150 100 100 
	job MN1 ifconfig eth0 down
	job MN2 ifconfig eth0 down

	#Observation
	set stdout "/mnt/hostfs/pmip6d.log"
	$ns at [expr $now+5] "start-all-pmip6d"
	$ns at [expr $now+5] "job CH tcpdump -s 256 -i eth1 -v -w /mnt/hostfs/$filename "
 	$ns at [expr $now+5] "job MR1 tcpdump -s 256 -i eth0 -v -w /mnt/hostfs/$filename "
 	$ns at [expr $now+5] "job MR2 tcpdump -s 256 -i eth0 -v -w /mnt/hostfs/$filename "

	set stdout "/dev/null"
	#MN1 & MN2 joins the network 
	$ns at [expr $now+10] "job MN1 ifconfig eth0 up"
	$ns at [expr $now+10] "job MN2 ifconfig eth0 up"

	#MN1 & MN2 ping each other 
	set stdout "/mnt/hostfs/ping6.log"
	$ns at [expr $now+20] "job MN2 ping6 2001:1::fcfd:ff:fe00:500"
	$ns at [expr $now+20] "job MN1 ping6 2001:1::fcfd:ff:fe00:600"

	#MN1 is moving to MR2 
	$ns at [expr $now+35] "setdest_1 450 100 100"

	#MN1 is moving back MR1
	$ns at [expr $now+100] "setdest_1 100 100 100"

	#MN1 & MN2 power off
	set stdout "/dev/null"
	$ns at [expr $now+175] "job MN1 ifconfig eth0 down"
	$ns at [expr $now+175] "job MN2 ifconfig eth0 down"

	#Finish
	$ns at [expr $now+200] "puts \"Finish, stop all processes and services\""
	$ns at [expr $now+200] "job MN2 killall -INT ping6"
	$ns at [expr $now+200] "job MN1 killall -INT ping6"
	$ns at [expr $now+200] "job CH killall -INT tcpdump"
	$ns at [expr $now+200] "job MR1 killall -INT tcpdump"
	$ns at [expr $now+200] "job MR2 killall -INT tcpdump"
	#$ns at [expr $now+200] "stop-all-pmip6d"
	$ns at [expr $now+200] "puts \"chorist-intra: DONE!\""

	set stdout "/mnt/hostfs/stdout"
}


proc chorist-tcp-intra { {filename "tcp_intra.log"} {copt "-l 4M"} {options ""} } {
        global ns
        set now [$ns now]

        #Step 1:
        progress $now [expr $now + 150]
        setdest_1 150 100 100
	setdest_2 450 100 100
        job MN1 ifconfig eth0 down
        job MN2 ifconfig eth0 down

        #Observation
        $ns at [expr $now+5] "start-all-pmip6d"
        $ns at [expr $now+5] "job CH tcpdump -s 256 -i eth1 -v -w /mnt/hostfs/$filename "
        $ns at [expr $now+5] "job MR1 tcpdump -s 256 -i eth0 -v -w /mnt/hostfs/$filename "
        $ns at [expr $now+5] "job MR2 tcpdump -s 256 -i eth0 -v -w /mnt/hostfs/$filename "

        #MN1 joins the network & CN start to ping down; MN1 ping up
        $ns at [expr $now+10] "job MN1 ifconfig eth0 up mtu 1300"
        $ns at [expr $now+10] "job MN2 ifconfig eth0 up mtu 1300"
        $ns at [expr $now+20] "job MN2 iperf -V -s $options"
        $ns at [expr $now+20] "job MN2 tcpdump -s 256 -i eth0 -v -w /mnt/hostfs/$filename "
        $ns at [expr $now+25] "job MN1 iperf -V -c 2001:1::fcfd:ff:fe00:600 $copt $options"

        #MN1 is moving to MR2
        $ns at [expr $now+35] "setdest_1 450 100 100"

        #MN1 is moving back MR1
        $ns at [expr $now+75] "setdest_1 100 100 100"

        #MN1 MN2 power off
        $ns at [expr $now+124] "job MN2 killall -INT tcpdump"
        $ns at [expr $now+125] "job MN1 ifconfig eth0 down"
        $ns at [expr $now+125] "job MN2 ifconfig eth0 down"

        #Finish
        $ns at [expr $now+150] "puts \"Finish, stop all processes and services\""
        $ns at [expr $now+150] "job MN2 killall -INT iperf"
        $ns at [expr $now+149] "job MN1 killall -INT iperf"
        $ns at [expr $now+150] "job CH killall -INT tcpdump"
        $ns at [expr $now+150] "job MR1 killall -INT tcpdump"
        $ns at [expr $now+150] "job MR2 killall -INT tcpdump"
        #$ns at [expr $now+150] "stop-all-pmip6d"
        $ns at [expr $now+151] "puts \"chorist-basic: DONE!\""
}

