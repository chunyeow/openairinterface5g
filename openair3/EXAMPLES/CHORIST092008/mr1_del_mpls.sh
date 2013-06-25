#!/bin/bash
#Script by Irina Dumitrascu (dumitrascu.irina@gmail.com) and Adrian Popa (adrian.popa.gh@gmail.com)
#June 2006
#Released under GPL

#Added by phil, Huu Nghia, 8 July 08
ip -6 route del $MN2_ADDR/128 via $CH_ADDR 
ip -6 route del $MN1_ADDR/128 dev eth0

echo Deleting all mpls - xc settings
xc_output="`mpls xc show | cut -c 9-75`"

number_of_lines=`echo $xc_output|wc -l`

for i in `seq 1 $number_of_lines` 
do
  xc_output_line="`echo $xc_output| head -$i | tail -1`"
  echo Deleting: mpls xc del $xc_output_line
  mpls xc del $xc_output
  
  #increment i
  i=`expr $i+1`
done
  
echo Deleting all mpls - ip route settings
nr_of_lines=` ip route show |grep mpls |wc -l`
for i in `seq 1 $nr_of_lines `
do
 output=`ip route show |grep mpls |head -1`
 echo Deleting: ip route del $output
 ip route del $output
 i=`expr $i+1`
 done
 

var1=`ip route show table 1`
  if [ ! -z "$var1" ]; then
       ip route del table 1
       #var1=`ip route show table 1`
       #ip route del $var1 table 1
       ip route flush table 1
      echo "clearing ip route table 1"
   fi

var1=`ip route show table 2`
  if [ ! -z "$var1" ]; then
     ip route del table 2
#     var1=`ip route show table 2`
 #    ip route del $var1 table 2
 ip route flush table 2
     echo "clearing ip route table 2"
  fi
					    
#echo Flushing iptables chains
#iptables -F
#echo Flushing iptables -t nat
#iptables -t nat -F
#echo Flushing iptables -t mangle
#iptables -t mangle -F

echo Deleting all mpls - nhlfe settings
nr_of_lines=` mpls nhlfe show |grep key |wc -l`
for i in `seq 1 $nr_of_lines `
do
 output=`mpls nhlfe show |grep key |cut -c 17-26 |head -1`
 echo Deleting: mpls nhlfe del key $output
 mpls nhlfe del key $output
 i=`expr $i+1`
done

echo Deleting all mpls - labelspace settings
nr_of_lines=`mpls labelspace show |grep -v "labelspace -1" |wc -l`
for i in `seq 1 $nr_of_lines `
do
 output=`mpls labelspace show |grep -v "labelspace -1"|cut -c 17-37 |head -1`
 echo Deleting: mpls labelspace set $output -1
 mpls labelspace set $output -1
 i=`expr $i+1`
done

echo Deleting all mpls - ilm settings
nr_of_lines=`mpls ilm show |grep ILM |wc -l`
for i in `seq 1 $nr_of_lines `
do
 output=`mpls ilm show |grep ILM |cut -c 10-37 |head -1`
 echo Deleting: mpls ilm del $output
 mpls ilm del $output
 i=`expr $i+1`
done
		    
var1=`tc qdisc show |grep eth0 |grep htb`
if [ ! -z "$var1" ]; then 
 tc qdisc del dev eth0 root
 echo "tc clear eth0"
fi

var2=`tc qdisc show |grep eth3 |grep htb`
if [ ! -z "$var2" ]; then 
 tc qdisc del dev eth3 root
 echo "tc clear eth3"
fi

var3=`tc qdisc show |grep eth1 |grep htb`
if [ ! -z "$var3" ]; then
 tc qdisc del dev eth1 root
  echo "tc clear eth1"
  fi

  var4=`tc qdisc show |grep eth2 |grep htb`
  if [ ! -z "$var4" ]; then
   tc qdisc del dev eth2 root
    echo "tc clear eth2"
    fi
