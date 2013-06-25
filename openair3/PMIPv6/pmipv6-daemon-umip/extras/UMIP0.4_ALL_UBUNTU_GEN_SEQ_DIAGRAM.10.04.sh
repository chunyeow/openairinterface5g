#!/bin/bash
####################################################################################
# This script starts automatically configures a PMIP demonstration. To run this
# script the target machines will need to have installed:
#  - ssh  (apt-get install ssh)
#     * After installing the ssh, if there is no no public key spread, you are
#       using different users per machine or even the users are not nis users,
#       new public/private key pair should be created (ssh-keygen -t dsa) and
#       this key pair copied over the target machines.  This process should be
#       done just once to enable the script to log in the other machines without
#       demanding the password.
#       (a@A:~> cat .ssh/id_rsa.pub | ssh b@B 'cat >> .ssh/authorized_keys'
#       b@B's password:)
#
# * Pay attention to change the apropriate configuration fields in the top of
#   this script and in the top of the clean_script.sh
#
# * The script is prepared to run as root in all the machines, this can also be
#   changed through changing the USER variable, however, pay attention because
#   the user will need to be suduer because some of the scripts needs to start
#   root priviledged commands. The user should be sudoer in all the target
#   machines.
#
# * The only option of the script is if one wants it to perform the version
#   control or not. If you do not need/want to save the previous configuration
#   you can pass the "no_copy" option to the script. In this way the script
#   will neither change the configuration deploied in the machines nor save it.
#
# * This script is prepared to run for a 3 nodes setup (one LMA and two MRDs),
#   however, appart from the last steps, the routing, to add a new PMIP machine is
#   just a mater of adding new lines to the initial vectors (machine, LOG_FILE,
#   OPEN_AIR_DIR, CONFIG_DIR, CONFIG_DIR_LOCAL, OPEN_AIR_CONF, PLATFORM_LOG_FILE) with the
#   appropriate values.  The routing, on the other hand, should be carefully
#   made for the scenario at hand.
#
####################################################################################

###########################
# Configuration Variables
###########################
export PMIP_SRC_DIR=$( cd "$( dirname "$0" )/.." && pwd )
export USER=root

export LMA_Ind=0
export MAG1_Ind=1
export MAG2_Ind=2

export LMA=192.168.12.128
export MAG1=192.168.12.126
export MAG2=192.168.12.167

export NET_ROOT=192.168.12.0

export DEFAULT_ROUTER=192.168.12.100

export ALL_MULTICAST=ff00::0/8

# Accessible ssh addresses for the machines
machine[$LMA_Ind]=$LMA
machine[$MAG1_Ind]=$MAG1
machine[$MAG2_Ind]=$MAG2

echo "######### Version Control ##########"
export run_version=`cat $PMIP_SRC_DIR/logs/.last_version.count`
let "run_version=$run_version+1"
echo $run_version>$PMIP_SRC_DIR/logs/.last_version.count


PMIP_SCRIPT[$LMA_Ind]="python $PMIP_SRC_DIR/extras/UMIP0.4_LMA_UBUNTU.10.04.py --pcap=yes --runversion=$run_version --pmipdir=$PMIP_SRC_DIR"
PMIP_SCRIPT[$MAG1_Ind]="python $PMIP_SRC_DIR/extras/UMIP0.4_MAG1_UBUNTU.10.04.py --pcap=yes --runversion=$run_version --pmipdir=$PMIP_SRC_DIR"
PMIP_SCRIPT[$MAG2_Ind]="python $PMIP_SRC_DIR/extras/UMIP0.4_MAG2_UBUNTU.10.04.py --pcap=yes --runversion=$run_version --pmipdir=$PMIP_SRC_DIR"

PMIP_LOG[$LMA_Ind]=$PMIP_SRC_DIR/logs/LMA_PMIP.$run_version.log
PMIP_LOG[$MAG1_Ind]=$PMIP_SRC_DIR/logs/MAG1_PMIP.$run_version.log
PMIP_LOG[$MAG2_Ind]=$PMIP_SRC_DIR/logs/MAG2_PMIP.$run_version.log

PCAP_FIRST_FILE_LOG[$LMA_Ind]=$PMIP_SRC_DIR/logs/lma2mags.$run_version.pcap
PCAP_FIRST_FILE_LOG[$MAG1_Ind]=$PMIP_SRC_DIR/logs/mag12ap.$run_version.pcap
PCAP_FIRST_FILE_LOG[$MAG2_Ind]=$PMIP_SRC_DIR/logs/mag22ap.$run_version.pcap

PCAP_SECOND_FILE_LOG[$LMA_Ind]=$PMIP_SRC_DIR/logs/lma2cn.$run_version.pcap
PCAP_SECOND_FILE_LOG[$MAG1_Ind]=$PMIP_SRC_DIR/logs/mag12lma.$run_version.pcap
PCAP_SECOND_FILE_LOG[$MAG2_Ind]=$PMIP_SRC_DIR/logs/mag22lma.$run_version.pcap

RADIUS_LOG=$PMIP_SRC_DIR/logs/RADIUS_PMIP.$run_version.log

echo ""
echo "######### Clean config ##########"
index=0
element_count=${#machine[@]}
while [ "$index" -lt "$element_count" ]
do

   #echo "ssh $USER@${machine[$index]}  ip route add $DEFAULT_ROUTER dev eth0"
   #ssh $USER@${machine[$index]} "ip route add $DEFAULT_ROUTER dev eth0"
   #echo "$USER@${machine[$index]}  ip route add default via $DEFAULT_ROUTER dev eth0"
   #ssh $USER@${machine[$index]}  "ip route add default via $DEFAULT_ROUTER dev eth0"
   echo "ssh $USER@${machine[$index]}  pkill -9 pmip6d"
   ssh $USER@${machine[$index]}  "pkill -9 pmip6d"
   echo "ssh $USER@${machine[$index]}  pkill -9 radiusd"
   ssh $USER@${machine[$index]}  "pkill -9 radiusd"
   echo "ssh $USER@${machine[$index]}  pkill -15 wireshark"
   ssh $USER@${machine[$index]}  "pkill -15 wireshark"

  let "index = $index + 1"
done

echo ""
echo "############# Machines Setup ############# "
index=0
element_count=3
while [ "$index" -lt "$element_count" ]
do

   echo "ssh $USER@${machine[$index]}  sudo echo 1 > /proc/sys/net/ipv4/ip_forward"
   ssh $USER@${machine[$index]}  "sudo echo 1 > /proc/sys/net/ipv4/ip_forward"

   echo "ssh $USER@${machine[$index]} sudo echo 0 > /proc/sys/net/ipv4/conf/all/rp_filter"
   ssh $USER@${machine[$index]} "sudo echo 0 > /proc/sys/net/ipv4/conf/all/rp_filter"

   echo "ssh $USER@${machine[$index]} sudo echo 0 > /proc/sys/net/ipv4/conf/eth0/rp_filter"
   ssh $USER@${machine[$index]} "sudo echo 0 > /proc/sys/net/ipv4/conf/eth0/rp_filter"

   echo "ssh $USER@${machine[$index]} sudo echo 0 > /proc/sys/net/ipv4/conf/eth1/rp_filter"
   ssh $USER@${machine[$index]} "sudo echo 0 > /proc/sys/net/ipv4/conf/eth1/rp_filter"

   let "index = $index + 1"
done


 echo "#############################################"
 echo "# LAUCHING PMIP PROCESS ON ALL NODES"
 echo "#############################################"

 index=0
 element_count=${#machine[@]}
 while [ "$index" -lt "$element_count" ]
 do
   #if [ `ifconfig | grep ${machine[$index]} | wc -m` -eq 0 ]; then
       echo "ssh $USER@${machine[$index]} ${PMIP_SCRIPT[$index]} > ${PMIP_LOG[$index]}  2>&1  &"
       ssh $USER@${machine[$index]} "${PMIP_SCRIPT[$index]} > ${PMIP_LOG[$index]} 2>&1" &
       if [ $index -eq $LMA_Ind ]; then
           echo "ssh $USER@${machine[$LMA_Ind]} radiusd -X > $RADIUS_LOG  2>&1 &"
           ssh -X $USER@${machine[$LMA_Ind]} "radiusd -X > $RADIUS_LOG  2>&1"&
           #sleep 2
       fi
   #else
   #    echo "${PMIP_SCRIPT[$index]} > ${PMIP_LOG[$index]}  2>&1  &"
   #    ${PMIP_SCRIPT[$index]} > ${PMIP_LOG[$index]} 2>&1 &
   #    sleep 1
   #    #bash xterm -e tail -f ${PMIP_LOG[$index]} &
   #    if [ $index -eq $LMA_Ind ]; then
   #        echo "radiusd -X > $RADIUS_LOG  2>&1 &"
   #        radiusd -X > $RADIUS_LOG  2>&1 &
   #        sleep 2
   #    fi
   #fi
   #sleep 1

   let "index = $index + 1"
 done
 echo ""


 read -p "Press any key to stop... " -n1 -s
 echo "######### Clean config ##########"
 index=0
 element_count=${#machine[@]}
 while [ "$index" -lt "$element_count" ]
 do

    echo "ssh $USER@${machine[$index]}  pkill -15 pmip6d"
    ssh $USER@${machine[$index]}  "pkill -15 pmip6d"
    echo "ssh $USER@${machine[$index]}  pkill -9 pmip6d"
    ssh $USER@${machine[$index]}  "pkill -9 pmip6d"
    echo "ssh $USER@${machine[$index]}  pkill -15 radiusd"
    ssh $USER@${machine[$index]}  "pkill -15 radiusd"
    echo "ssh $USER@${machine[$index]}  pkill -9 radiusd"
    ssh $USER@${machine[$index]}  "pkill -9 radiusd"
    echo "ssh $USER@${machine[$index]}  pkill -15 wireshark"
    ssh $USER@${machine[$index]}  "pkill -15 wireshark"

   let "index = $index + 1"
 done

 index=0
 element_count=3
 while [ "$index" -lt "$element_count" ]
 do
    echo " ${machine[$index]}  - > $index"

    if [ `ifconfig | grep ${machine[$index]} | wc -m` -eq 0 ]; then
        echo "scp -p $USER@${machine[$index]}:$PMIP_SRC_DIR/logs/*.pcap $PMIP_SRC_DIR/logs/"
        scp -p $USER@${machine[$index]}:$PMIP_SRC_DIR/logs/*.pcap $PMIP_SRC_DIR/logs/
        echo "scp -p $USER@${machine[$index]}:$PMIP_SRC_DIR/logs/*.log $PMIP_SRC_DIR/logs/"
        scp -p $USER@${machine[$index]}:$PMIP_SRC_DIR/logs/*.log $PMIP_SRC_DIR/logs/
    fi
    let "index = $index + 1"
 done

 $PMIP_SRC_DIR/extras/output-process/pmip_mscgen.sh $PMIP_SRC_DIR






