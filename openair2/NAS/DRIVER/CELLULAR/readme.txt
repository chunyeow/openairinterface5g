         ---------------------------------
         | UMTS-TDD (TD-CDMA) NAS driver |
         ---------------------------------


  
1 - COMPILATION
---------------
Pre-Req :
 - RTAI installed and running
 - IPv6 patched as described below (see 4 - Patching IPv6)
  
From RHODOS26 directory:
  
 - make nasrg : creates the NAS RG driver (nasrg.ko) and copies it to the RHODOS26/bin directory
  
 - make cleannasrg : deletes all object and temporary files from the nasrg directory
  
 - make nasmt : creates the NAS MT driver (nasmt.ko) and copies it to the RHODOS26/bin directory
  
 - make cleannasmt : deletes all object and temporary files from the nasmt directory
  
  
2 - OPERATION - RG side
-----------------------
  
A1 - Compile the Access Stratum (for IF mode)
  
Pre-Req : RTAI installed and running
          ICC 8.0 installed and running
  
  > make rgif
  > make asrguserif
  > make rrmumts 

A2 - Compile the Access Stratum (for RF mode)

Pre-Req : RTAI installed and running
          ICC 8.0 installed and running

  > make guimakefiles
  > make rgrf
  > make asrguserrfa2
  > make rrmumts


B - Start the Access Stratum
  
 (as root      )  > cd  <..>RHODOS26/bin
  
 (first install)  > create_rtfifos
 (first install)  > mknod -m 666 /dev/daq0 c 127 1
 (after cleanas)  > cp
../src/rrm_umts/congestion_control/resources_manager/rrm_master.properties .
  
 (--           )  > ./start_rtos
 (--           )  > ./runrgif [for IF]   [or ./runrga2 for RF]
  
 ==> to check correct operation:
 (as root      ) cat /dev/rtf62
 (as root      ) tail -f /var/log/messages
  
C - Start the NAS Driver
  
 (as root      ) > cd  <..>RHODOS26/bin
 (--           ) > insmod nasrg.ko
 (--           ) > ./access_router.up
  
D - Stop the NAS Driver and the Access Stratum
  
 (--           ) > ./access_router.down
 (--           ) > rmmod nasrg
 (--           ) > <CTRL+C> on Acccess stratum window
 (--           ) > ./stop_rtos
  
E - Clean Access Stratum compilation
  
 > make cleanas
  
3 - OPERATION - MT side
-----------------------
  
A1 - Compile the Access Stratum (for IF mode)

Pre-Req : RTAI installed and running
          ICC 8.0 installed and running
          file RHODOS26/src/access_stratum/l1/low/control/mmx.h copied in /usr/include

  > make mtif
  > make asmtuserif
  > make fpgatest

A2 - Compile the Access Stratum (for RF mode)

Pre-Req : RTAI installed and running
          ICC 8.0 installed and running
          file RHODOS26/src/access_stratum/l1/low/control/mmx.h copied in /usr/include

  > make guimakefiles
  > make mtrf
  > make asmtuserrf
  > make fpgatest


B - Start the Access Stratum

 (as root      )  > cd  <..>RHODOS26/bin

 (first install)  > create_rtfifos
 (first install)  > mknod -m 666 /dev/daq0 c 127 1

 (--           )  > ./start_rtos
 (--           )  > ./runrgif [for IF]   [or ./runrga2 for RF]

 ==> to check correct operation:
 (as root      ) cat /dev/rtf62
 (as root      ) tail -f /var/log/messages

C - Start the NAS Driver

 (as root      ) > cd  <..>RHODOS26/bin
 (--           ) > insmod nasmt.ko
 (--           ) > ./mobile_node.up

D - Stop the NAS Driver and the Access Stratum

 (--           ) > ./mobile_node.down
 (--           ) > rmmod nasmt
 (--           ) > <CTRL+C> on Acccess stratum window
 (--           ) > ./stop_rtos

E - Clean Access Stratum compilation

 > make cleanas


4 - Patching IPv6
-----------------------

For Linux with 2.6.xx kernel version (xx = 10 or 81):

- move RHODOS26/non_access_stratum/driver/src/addrconf.c.26.xx to /usr/src/linux/net/ipv6/addrconf.c
- move RHODOS26/non_access_stratum/driver/src/if_arp.h.26.xx   to /usr/src/linux/include/linux/if_arp.h
- recompile IPv6 (inside the kernel or as a module, as usual)
  
5 - Start the MT connection
-----------------------

You can use either the RAL-UMTS (RAL-CDMA) -- see corresponding documentation --

* OR *

the tool provided with the driver (directory RHODOS26/src/non_access_stratum/driver/mt_tools).
Install the tool, update the Cell_id field if needed and click on the "Attach" button.
  

 