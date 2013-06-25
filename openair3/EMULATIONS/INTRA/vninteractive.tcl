#http://wiki.tcl.tk/1968
#from Jeffrey Hobbs

 puts -nonewline "vnman (0) % "
 if {!$tcl_interactive} {
     set long_command ""
     set verbose_history 0
     if {![catch {rename unknown tcl_unknown}]} {
 	proc unknown {cmdname args} {
 	    if {[regexp "^!..*" $cmdname]} {
 		banghist [string range $cmdname 1 end]
 	    } else {
 		tcl_unknown $cmdname $args
 	    }
 	}
     }

     proc banghist {val} {
 	global verbose_history
 	if {![string compare $val "!"]} {set val ""}
 	if {$verbose_history} {puts "[history event $val]"}
 	history redo $val
     }

     if {![info exists tcl_prompt1]} {
 	set tcl_prompt1 {puts -nonewline "vnman ([history nextid]) % "}
     }
     proc read_stdin {} {
 	global eventLoop tcl_prompt1 long_command
 	set l [gets stdin]
 	if {[eof stdin]} {
 	    set eventLoop "done"     ;# terminate the vwait eventloop
 	} else {
 	    if {[string compare $l {}]} {
 		append long_command "\n$l"
 		set l $long_command
 		if {[info complete $l]} {
 		    if {[catch {uplevel \#0 history add [list $l] exec} err]} {
 			puts stderr $err
 		    } elseif {[string compare $err {}]} {
 			puts $err
 		    }
 		    set long_command ""
 		    catch $tcl_prompt1
 		} else {
 		    puts -nonewline "> "
 		}
 	    } elseif {![string compare $long_command {}]} {
 		catch $tcl_prompt1
 	    } else {
 		puts -nonewline "> "
 	    }
 	    flush stdout
 	}
     }

     # set up our keyboard read event handler:
     # Vector stdin data to the socket
     fileevent stdin readable read_stdin

     catch $tcl_prompt1
     flush stdout
     # wait for and handle or stdin events...
     #vwait eventLoop
 }

