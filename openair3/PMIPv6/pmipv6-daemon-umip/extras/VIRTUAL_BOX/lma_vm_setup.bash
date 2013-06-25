#!/bin/bash

export VM_NAME='LMA-UBUNTU-12.04.2-server-i386'
export OS_INSTALL_IMAGE="/home/yang/Downloads/ubuntu-12.04.2-server-i386.iso"
export OS_TYPE="Ubuntu"

export HOST_BRIDGED_IF_NAME="eth1"
export NAMESERVER="192.168.12.100"
export INTERNET_GATEWAY_FOR_GUESTS="192.168.14.145"
export DEFAULT_VIRTUAL_BOX_VM_PATH=$HOME/'VirtualBox VMs'
#export SVN_USERNAME=gauthier
export SVN_USERNAME=

###########################################
# PMIP CONFIG FILES                       #
###########################################

LMA_CONFIG_FILE=extras/example-lma-ovs.conf

#
# +-----------+-----------------------------------------+---------------------+
# | COMPUTER 1|                +------+                 |            Physical | towards
# +-----------+       (nic1)   |eth?  +-----------------+            Network  +---->
# |                          +-+------+--+              |            Adapter  | INTERNET_GATEWAY_FOR_GUESTS
# |                          |    LMA    |              |HOST_BRIDGED_IF_NAME |
# |                          |  (VM)     |              +---------------------+
# |                          |           |                                    |
# |                          |           |                                    |
# |                          +--+------+-+                                    |
# |                   (nic2)    |eth?  |                                      |
# |                             +--+---+                                      |
# |                                |                                          |
# |                                |                                          |
# |                                | "pmip_egress"                            |
# |      +-----------+             |                  +-----------+           |
# |      |  MAG 1    +------+      |           +------+    MAG 2  |           |
# |      |  (VM)     |eth?  +------+-----------+eth?  |  (VM)     |           |
# |      |           +------+   internal       +------+           |           |
# |      | PMIP      |          network               | PMIP      |           |
# |      |  +        |       192.168.33/24            |  +        |           |
# |      | LTE eNB 1 |                                | LTE eNB 2 |           |
# |      | LTE UEs   |                                | LTE UEs   |           |
# |      +-----------+                                +-----------+           |
# |      |oai0 / eth?|                                |oai0 / eth?|           |
# |      +------+----+                                +------+----+           |
# |             |  "pmip_ingress1"            "pmip_ingress2"|                |
# |         +---+--+                                     +---+--+             |
# |         |oai0  |                                     |oai0  |             |
# |       +-----------+                                +-----------+          |
# |       |  UE1      |                                |  UE2      |          |
# |       |  (VM)     |                                |  (VM)     |          |
# |       +-----------+                                +-----------+          |
# |                                                                           |
# |                                                                           |
# +---------------------------------------------------------------------------+
#




black='\E[30m'
red='\E[31m'
green='\E[32m'
yellow='\E[33m'
blue='\E[34m'
magenta='\E[35m'
cyan='\E[36m'
white='\E[37m'

ROOT_UID=0
E_NOTROOT=67

trim ()
{
    echo "$1" | sed -n '1h;1!H;${;g;s/^[ \t]*//g;s/[ \t]*$//g;p;}'
}


cecho()   # Color-echo
# arg1 = message
# arg2 = color
{
    local default_msg="No Message."
    message=${1:-$default_msg}
    color=${2:-$black}
    echo -e "$color"
    echo -n "$message"
    tput sgr0
    return
}

echo_error() {
    local my_string=""
    until [ -z "$1" ]
    do
        my_string="$my_string$1"
        shift
        done
        cecho "$my_string" $red
}

echo_warning() {
    local my_string=""
    until [ -z "$1" ]
    do
        my_string="$my_string$1"
        shift
    done
    cecho "$my_string" $yellow
}

echo_success() {
    local my_string=""
    until [ -z "$1" ]
    do
        my_string="$my_string$1"
        shift
    done
    cecho "$my_string" $green
}

bash_exec() {
    output=$($1 2>&1)
    result=$?
    if [ $result -eq 0 ]
    then
        echo_success "$1"
    else
        echo_error "$1: $output"
    fi
}

set_openair() {
    path=`pwd`
    declare -i length_path
    declare -i index
    length_path=${#path}

    index=`echo $path | grep -b -o 'targets' | cut -d: -f1`
    #echo ${path%$token*}
    if [[ $index -lt $length_path  && index -gt 0 ]]
       then
           declare -x OPENAIR_DIR
           index=`expr $index - 1`
           openair_path=`echo $path | cut -c1-$index`
           #openair_path=`echo ${path:0:$index}`
           export OPENAIR_DIR=$openair_path
           export OPENAIR1_DIR=$openair_path/openair1
           export OPENAIR2_DIR=$openair_path/openair2
           export OPENAIR3_DIR=$openair_path/openair3
           export OPENAIR_TARGETS=$openair_path/targets
           return 0
    fi
    index=`echo $path | grep -b -o 'openair3' | cut -d: -f1`
    if [[ $index -lt $length_path  && index -gt 0 ]]
       then
           declare -x OPENAIR_DIR
           index=`expr $index - 1`
           openair_path=`echo $path | cut -c1-$index`
           #openair_path=`echo ${path:0:$index}`
           export OPENAIR_DIR=$openair_path
           export OPENAIR1_DIR=$openair_path/openair1
           export OPENAIR2_DIR=$openair_path/openair2
           export OPENAIR3_DIR=$openair_path/openair3
           export OPENAIR_TARGETS=$openair_path/targets
           return 0
    fi
    return -1
}

set_openair
cecho "OPENAIR_DIR     = $OPENAIR_DIR" $green
cecho "OPENAIR1_DIR    = $OPENAIR1_DIR" $green
cecho "OPENAIR2_DIR    = $OPENAIR2_DIR" $green
cecho "OPENAIR3_DIR    = $OPENAIR3_DIR" $green
cecho "OPENAIR_TARGETS = $OPENAIR_TARGETS" $green

if [ $OS_TYPE ]; then
    echo_success "OS_TYPE: $OS_TYPE"
else
    echo_error "OS_TYPE bash variable must be set to any of the following values (The type of OS you want to install on your VM): `VBoxManage list ostypes | grep ID | cut -d: -f2 | tr -d ' ' | sed -n -e 'H;${x;s/\n/,/g;s/^,//;p;}'`"
    exit -1
fi

if [ $VM_NAME ]; then
    echo_success "VM_NAME: $VM_NAME"
else
    echo_error "VM_NAME bash variable must be set"
    exit -1
fi

if [ $OS_INSTALL_IMAGE ]; then
    if [[ ! -e $OS_INSTALL_IMAGE ]] ; then  
        echo_error "$OS_INSTALL_IMAGE: File not found"
        exit -1
    else
        echo_success "OS_INSTALL_IMAGE: $OS_INSTALL_IMAGE"
    fi
else
    echo_error "OS_INSTALL_IMAGE bash variable must be set to a valid ISO OS installation file"
    exit -1
fi



 
#cd /tmp
#wget http://download.virtualbox.org/virtualbox/4.1.12/Oracle_VM_VirtualBox_Extension_Pack-4.1.12.vbox-extpack
#sudo VBoxManage extpack install Oracle_VM_VirtualBox_Extension_Pack-4.1.12.vbox-extpack

bash_exec "VBoxManage unregistervm $VM_NAME --delete"
 
bash_exec "VBoxManage createvm --name $VM_NAME --register"

UUID=`VBoxManage showvminfo  $VM_NAME | grep Hardware\ UUID | cut -d: -f2 | tr -d ' '` 

echo UUID=$UUID


bash_exec "VBoxManage modifyvm  $UUID --ostype $OS_TYPE --memory 512 --vram 12 --cpus 1 \
                                          --rtcuseutc on --cpuhotplug off --cpuexecutioncap 100 --pae on --hpet on \
                                          --hwvirtex on --nestedpaging on \
                                          --firmware bios --biosbootmenu messageandmenu --boot1 dvd --boot2 disk \
                                          --nic1 bridged        --nic2 intnet        --nic3 intnet         --nic4 null \
                                          --nictype1 82545EM    --nictype2 82545EM   --nictype3 82545EM   --nictype4 82545EM \
                                          --cableconnected1 on  --cableconnected2 on --cableconnected3 on --cableconnected4 off \
                                          --bridgeadapter1 $HOST_BRIDGED_IF_NAME --intnet2 pmip_egress     --intnet3 pmip_ingress \
                                          --audio none \
                                          --usb off --usbehci off"
                                          

BRIDGED_MAC_ADDRESS=`vboxmanage showvminfo $UUID | grep Bridged\ Interface |  sed 's/^.*MAC:\ //'  |  cut --delimiter=',' -f 1`
PMIP_MAC_ADDRESS=`vboxmanage showvminfo $UUID | grep pmip_egress |  sed 's/^.*MAC:\ //'  |  cut --delimiter=',' -f 1`

mkdir "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO
rm -Rf "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/*
cp lma_guest_setup.bash "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/lma_guest_setup.bash

##################################
# PARSE LMA CONFIGURATION FILE   #
##################################
rm -f /tmp/source.txt
cat $OPENAIR3_DIR/PMIPv6/pmipv6-daemon-umip/$LMA_CONFIG_FILE  | grep \; | tr -d ';' | sed -e 's/  */ /gp;' | sed -e "s/^ *//;s/ *$//;s/ \{1,\}/ /g" | tr ' ' '='  > /tmp/source.txt
source /tmp/source.txt

IPV4_BYTE=1

echo "# This file describes the network interfaces available on your system" > "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo "# and how to activate them. For more information, see interfaces(5)."  >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo " "                                                                     >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo "# The loopback network interface"                                      >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo "auto lo"                                                               >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo "iface lo inet loopback"                                                >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo " "                                                                     >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo "# BRIDGED_MAC_ADDRESS $BRIDGED_MAC_ADDRESS"                            >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo "auto BRIDGED_IF_TO_FIND"                                               >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo "iface BRIDGED_IF_TO_FIND inet static"                                  >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo "address 192.168.14.19"                                                 >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo "netmask 255.255.255.0"                                                 >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo "broadcast 192.168.14.255"                                              >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo "gateway $INTERNET_GATEWAY_FOR_GUESTS"                                  >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo "nameserver $NAMESERVER"                                                >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo " "                                                                     >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo "# PMIP_NETWORK_MAC_ADDRESS $PMIP_MAC_ADDRESS"                          >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo "auto PMIP_IF_TO_FIND"                                                  >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo "iface PMIP_IF_TO_FIND inet static"                                     >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo "address 192.168.33.$IPV4_BYTE"                                         >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo "netmask 255.255.255.0"                                                 >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma
echo "broadcast 192.168.33.255"                                              >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/if_lma

rm -f /tmp/source.txt
###################################
# GET OPENAIR CODE
###################################
cd "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO 
if [[ "$SVN_USERNAME"x == "x" ]]; then
    # svn read only
    svn co http://svn.eurecom.fr/openair4G/trunk
else
    # svn read/write
    svn co http://svn.eurecom.fr/openairsvn/openair4G/trunk --username $SVN_USERNAME
fi

tar -cjf trunk.tar.bz2 trunk
rm -Rf trunk 
#####################
# GET ASN1C CODE
#####################
svn co https://asn1c.svn.sourceforge.net/svnroot/asn1c/trunk asn1c
tar -cjf asn1c.tar.bz2 asn1c
rm -Rf asn1c


chmod -Rf 777 "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO
mkisofs -max-iso9660-filenames -untranslated-filenames -no-iso-translate -o "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/cd_guest_setup.iso "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/MKISO/



                                          
                                          
VBoxManage createhd --filename "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/"$VM_NAME".vdi --size 16384 --format VDI
                                          
VBoxManage storagectl $UUID --name "IDE Controller" --add ide --controller PIIX4 --hostiocache on --bootable on
VBoxManage storageattach $UUID --storagectl "IDE Controller" --type dvddrive --passthrough off --port 1 --device 0 --medium $OS_INSTALL_IMAGE



VBoxManage storageattach $UUID --storagectl "IDE Controller" --type dvddrive --passthrough off --port 1 --device 1 --medium "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/cd_guest_setup.iso

VBoxManage storagectl $UUID --name "SATA Controller" --add sata --controller IntelAhci --sataportcount 4 --hostiocache off --bootable on
VBoxManage storageattach $UUID --storagectl "SATA Controller" --type hdd --port 0 --device 0 --medium "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$VM_NAME"/"$VM_NAME".vdi

echo_success '+---------------------------------------------------------------------------+'
echo_success '| LMA VIRTUAL MACHINE CREATED                                               |'
echo_success '| NOW BOOT YOUR VIRTUAL MACHINE AND PROCEED TO OS INSTALLATION              |'
echo_success '| YOU CAN CONFIGURE THE BRIDGED INTERFACE DURING THE INSTALLATION PROCESS   |'
echo_success '| - SELECT "Configure network manually" WHEN PROMPED.                       |'
echo_success '| THEN THE NETWORK MAY NOT WORK, CINCE YOUR CONFIGURED NIC 1 INTERFACE      |'
echo_success '| DOES NOT CORRESPOND TO ETH0, AND SO ON FOR NIC 2, NIC #...                |'
echo_success '| BUT A LATER SRIPT WILL HANDLE THESE CASES AND WILL RECOVER FROM UDEV NET  |'
echo_success '| RULES.                                                                    |'
echo_success '| - Select keyboard: ...                                                    |'
echo_success '| - Primary network interface:                                              |'
echo_success '|                           SELECT "eth0"                                   |'
echo_success '| - Network configuration method:                                           |'
echo_success '|                           SELECT "Configure network manually"             |'
echo_success '| - IP address: An address valid on the network where your host bridged     |'
echo_success '|               interface is plugged.                                       |'
echo_success '| - Gateway: A valid IP address of the internet gateway                     |'
echo_success '| - Nameserver adresses: DNS addresses (192.168.12.100 for EUR exp network) |'
echo_success '| - Hostname:   lma                                                         |'
echo_success '| - Domainname:   Leave the input field blank                               |'
echo_success '| - Full name for the new user: pmip                                        |'
echo_success '| - User name for your account: pmip                                        |'
echo_success '| - Choose a password for the new user: (whatever, linux by default)        |'
echo_success '| - Encrypt your home directory:                                            |'
echo_success '|                           SELECT "No"                                     |'
echo_success '| - Partitionning method:   SELECT "Guided - use entire disk"               |'
echo_success '| - HTTP proxy information: LEAVE THE INPUT FIELD BLANK                     |'
echo_success '| - How do you want to manage upgrades on this system:                      |'
echo_success '|                           SELECT "No automatic updates"                   |'
echo_success '| - Choose software to install:                                             |'
echo_success '|                           SELECT "OpenSSH server"                         |'
echo_success '| - Install the GRUB boot loader to the master boot record:                 |'
echo_success '|                           SELECT "Yes"                                    |'
echo_success '|---------------------------------------------------------------------------|'
echo_warning '| AFTER THE BOOT OF THE O.S.:                                               |'
echo_warning '| Login with the user account created during the installation process:      |'
echo_success '| #lma login: pmip                                                          |'
echo_success '| #Password:                                                                |'
echo_success '| #pmip@lma:~$ sudo passwd                                                  |'
echo_success '| #[sudo] password for pmip:                                                |'
echo_success '| #Enter new UNIX password:                                                 |'
echo_success '| #Retype new UNIX password:                                                |'
echo_success '| #passwd: password updated successfully                                    |'
echo_success '| #pmip@lma:~$ exit                                                         |'
echo_warning '| Loggin as root,                                                           |'
echo_success '| #lma login: root                                                          |'
echo_success '| #Password:                                                                |'
echo_warning '| Execute the bash script to install the PMIP environment and software:     |'
echo_success '| #root@lma:~# mount -t iso9660 /dev/dvd  /mnt                              |'
echo_success '| #mount: block device /dev/sr1 is write-protected, mounting read-only      |'
echo_success '| #root@lma:~# /mnt/lma_guest_setup.bash                                    |'
echo_success '| For kernel configuration prompted options, select the default or whatever.|
echo_warning '| Shutdown this VM:                                                         |'
echo_success '| #root@lma:~# shutdown -P now                                              |'
echo_success '|---------------------------------------------------------------------------|'
echo_success '| THEN THIS VIRTUAL MACHINE WILL BE DUPLICATED FOR MAGS WITH THE SCRIPT     |'
echo_success '| mag_vm_setup.bash                                                         |'
echo_success '+---------------------------------------------------------------------------+'


