#! /bin/nse

# An example script for the usage of ns-2 in emulation mode:
# Uses:
# 1. Network/Daemon agents (by Huu-Nghia.Nguyen@eurecom.fr) to access to a uml_switch at level II 
# 2. Tap/Raw agents to map between real MAC addresses and NS-2 IP addresses
# 3. In-memory compression of the trace file to reduce disk write operations during simulation
#	(currently done via gzip)


# Common variables
# 

set scriptname		routingdemo
set val(chan)           Channel/WirelessChannel    ;# Channel Type
set val(prop)           Propagation/TwoRayGround   ;# radio-propagation model
set val(netif)          Phy/WirelessPhy            ;# network interface type
set val(mac)            Mac/802_11                 ;# MAC type
set val(ifq)            Queue/DropTail/PriQueue    ;# interface queue type
set val(ll)             LL                         ;# link layer type
set val(ant)            Antenna/OmniAntenna        ;# antenna model
set val(ifqlen)         50                         ;# max packet in ifq
set val(x)              800			   ;# x range in meters
set val(y)              600			   ;# y range in meters
set val(rp)		DumbAgent		   ;# routing protocol
set val(nn)		7			   ;# number of mobile nodes
#set val(stime)		200.0			   ;# simulation time
set val(stime)		360000.0		   ;# simulation time = 100 hours

set ns	[new Simulator]
$ns use-scheduler RealTime

set tracefd  [open $scriptname.tr w]
$ns trace-all $tracefd

#set namtrace [open $scriptname.nam w]
#set namtrace [open "|nam -r 0.1 -" w]
#$ns namtrace-all-wireless $namtrace $val(x) $val(y)

#Procedure needed when running nam in real-time
proc NamTime {} {
	#Send time to nam periodically
	global ns namtrace
	set now [$ns now]
	set next [expr $now + 0.05]
	puts $namtrace "T -t $now"
	flush $namtrace
	$ns at $next "NamTime"
}

#$ns at 1.0 "NamTime"

proc UniformErr {} {
    set err [new ErrorModel]
    $err unit packet
    $err set rate_ 0.01
    $err ranvar [new RandomVariable/Uniform]
    $err drop-target [new Agent/Null]
    return $err
}

set topo [new Topography]

$topo load_flatgrid $val(x) $val(y)

# Create GOD
create-god $val(nn)

# Create channel
set chan_1 [new $val(chan)]
set chan_2 [new $val(chan)]

# Configure node parameters
$ns node-config -adhocRouting $val(rp) \
	-llType $val(ll) \
	-macType $val(mac) \
	-ifqType $val(ifq) \
	-ifqLen $val(ifqlen) \
	-antType $val(ant) \
	-propType $val(prop) \
	-phyType $val(netif) \
	-topoInstance $topo \
	-agentTrace ON \
	-routerTrace ON \
	-macTrace ON \
	-movementTrace ON \
	-channel $chan_1 \
	-IncomingErrProc UniformErr

#Procedure to configure an ns-2 node initially
proc setup_node {id x y z color} {
	global ns node_
	set node_($id) [$ns node]
	$node_($id) set X_ $x
	$node_($id) set Y_ $y
	$node_($id) set Z_ $z
	$node_($id) color $color
	$ns at 0 "$node_($id) setdest $x $y 0"
	$ns at 0 "$node_($id) color $color"
	$node_($id) random-motion 0
}
			
#Topo table:
#set CN $vms(CN)
set CH $vms(CH)
set MR1 $vms(MR1)
set MR2 $vms(MR2)
set MN1 $vms(MN1)
set MN2 $vms(MN2)
set map(1) [list $CH eth0 $chan_1 300 100 0 "black"]

set map(2) [list $MR1 eth1 $chan_1 100 100 0 "green"]
set map(3) [list $MR1 eth0 $chan_2 100 100 0 "green"]
set map(4) [list $MR2 eth1 $chan_1 500 100 0 "green"]
set map(5) [list $MR2 eth0 $chan_2 500 100 0 "green"]

set map(6) [list $MN1 eth0 $chan_2 150 100 0 "red"]
set map(7) [list $MN2 eth0 $chan_2 450 100 0 "red"]
	
Agent/Tap set maxpkt_ 1500
for {set i 1} {$i <= $val(nn)} {incr i} {
	$ns node-config -channel [lindex $map($i) 2]
	puts  "setup_node $i [lindex $map($i) 3]  [lindex $map($i) 4]  [lindex $map($i) 5]  [lindex $map($i) 6]"
	setup_node $i [lindex $map($i) 3]  [lindex $map($i) 4]  [lindex $map($i) 5]  [lindex $map($i) 6]

	$ns at 0 "$node_($i) start";
    	$ns at $val(stime) "$node_($i) reset";

	#set raw($i) [new Network/Raw]
	set raw($i) [new Network/Daemon]
	set node [lindex $map($i) 0]
	set iff [lindex $map($i) 1]
	puts  [$node getinfo $iff 1]
	#$raw($i) open [$node getinfo $iff 1] readwrite
	$raw($i) open [$node getinfo $iff 3] readwrite
	
	#Tap Agent for each node
	puts  [$node getinfo $iff 2]
	#set tagent($i) [new Agent/Tap/Raw [$node getinfo $iff 2]]
#	if {$node == $MR2 && $iff == "eth0"} {
#		set tagent($i) [new Agent/Tap/Raw fe:fd:0:0:3:0]
#	} else { 
		set tagent($i) [new Agent/Tap/Raw [$node getinfo $iff 1]]
#	}
	$tagent($i) network $raw($i)
	$ns attach-agent $node_($i) $tagent($i)
}

$ns at $val(stime) "stop"
$ns at $val(stime) "puts \"NS EXITING ...\" ; $ns halt"

proc setdest_1 { x y speed} {
	global node_
	$node_(6) setdest $x $y $speed
}
proc setdest_2 { x y speed} {
	global node_
	$node_(7) setdest $x $y $speed
}

proc stop {} {
	global ns tracefd raw 
	$ns flush-trace
	close $tracefd
	for {set i 1} {$i <= $val(nn)} {incr i} {
		$raw($i) close
	}
}
	

source utils.tcl    
puts "okey"
	
$ns run
