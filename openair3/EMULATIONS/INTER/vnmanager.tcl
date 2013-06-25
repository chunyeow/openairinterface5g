#
# Management Script for Virtual Networking User-Mode-Linux
# Author: Huu-Nghia Nguyen
# Email: Huu-Nghia.Nguyen@eurecom.fr
# Eurecom Institute
#

Class VirtualMachine
proc flatten list {return [string map {\{ "" \} ""} $list]}

VirtualMachine instproc init {vm_opt_list} {
        $self instvar opt sname
	array set sname {}
	array set opt $vm_opt_list
	set dir [split $opt(uml_dir) "/"]
	set len [llength $dir]
        set opt(vmid) [lindex $dir [expr $len-2]]
        set opt(scenario) [lindex $dir [expr $len-4]]
        puts "VM ($opt(scenario).$opt(vmid)) running in $opt(uml_dir) is created"
}

VirtualMachine instproc mconsole args {
	$self instvar opt
	set dir $opt(uml_dir)$opt(umid)
	set cmdline "cd $dir && uml_mconsole mconsole [flatten $args]"
	puts "execute the command: $cmdline"
	set result [exec sh -c "cd $dir && uml_mconsole mconsole [flatten $args]" 2>/dev/null]
	return $result
}

VirtualMachine instproc getinfo {param idx} {
	$self instvar opt
	return [lindex [split $opt($param) ","] $idx]
}

########################################################################
#								       	
#                    PARSE THE ENVIRONMENT                             
#								       
########################################################################
set vm_cmds [exec  ps -ef | grep linux | sed s/.*linux//g | sort | uniq ]

## Split into vm_cmd_lines
set vm_cmd_lines [split $vm_cmds "\n"]
array unset vms
set stdout "/mnt/hostfs/stdout"
set stderr "/mnt/hostfs/stdout"
set filestdout ""

## Iterate over the vm_cmd_lines
foreach cmd  $vm_cmd_lines {
	#puts "---------\nAnalyse the command line:"
	#puts $cmd
	set params [split $cmd " "]
	array unset opt
	foreach param  $params {
		set fields [split $param "="]
		set optname [lindex $fields 0]
		set optval [lindex $fields 1] 
		if {$optval != ""} {
			set opt($optname) $optval
		}
	}

	if {[array size opt] > 0} {
		set vm [new VirtualMachine [array get opt]]
		set vms([$vm set opt(vmid)]) $vm		
	}
}

########################################################################
#								       	
#                    COMMAND IMPLEMENTATION                               
#								       
########################################################################

set helpstr "Command list:\n\
		\tconfig <vm> <device\[=options\]>: querry/add a new device to <vm>\n\
		\tjob <vm> <command>: execute a command in <vm> using sh -c\n\
		\tresult <vm>: return result of last job\n\
		\tterm  <vm> <conid>: open the console <conid> of <vm>\n\
		\tmconsole <vm> <args>: run uml_mconsole for <vm>\n\
		\tsetdest <x> <y> <speed>: \n\t\tset destination (<x>, <y>) for the MN with the speed of <speed> m/s\n"
proc help {} {
	global helpstr
	puts $helpstr
}


proc config {vmid args} {
	global vms
	set vm $vms($vmid)
	return [$vm mconsole config $args]
}

proc job {vmid args} {
	global vms filestdout
	global stdout stderr
	set vm $vms($vmid)
	catch { exec rm opt(uml_dir)]/hostfs/$stdout }
	return [$vm mconsole exec "'" $args ">$stdout 2>$stderr '"]
        regsub {(.*)/(.*)} $stdout {\2} filename
        set filestdout [$vm set opt(uml_dir)]/hostfs/$filename
        exec cat $filestdout

}

proc lastresult {} {
	exec cat $filestdout
}

proc result {vmid} {
        global vms
        global stdout stderr
        set vm $vms($vmid)
	regsub {(.*)/(.*)} $stdout {\2} filename
	set filestdout [$vm set opt(uml_dir)]/hostfs/$filename
        exec cat $filestdout 
}

proc term {vmid conid} {
	global vms
	set vm $vms($vmid)
	
	#Create screen name 
	set result [$vm mconsole config $conid]
	regsub {(.*):(.*)} $result {\2} device
	regsub {(.*)/(\d+)} $device {\2} devid
	set screenname "$conid$devid$vmid"

	#Try to look up in the env if the screen exist?
	set filename /tmp/vnman-screen-[pid].cfg
	catch {	exec screen -ls > $filename } {}
	set output [split [exec cat $filename] "\n"]
	if {[lsearch -regex $output ".*$screenname.*"] < 0} {exec screen -d -m -S $screenname $device}
	catch { exec rm -f $filename}

	#Show the terminal
	exec konsole -e screen -d -r $screenname &
	
}

proc mconsole {vmid args} {
	global vms
	set vm $vms($vmid)
	return [$vm mconsole $args]
} 
########################################################################
#								       	
#                    COMMAND LINE INTERFACE                              
#								       
########################################################################

puts "VM lists: [array names vms]"
help
source vninteractive.tcl
source wirelesstopo.tcl
stop



