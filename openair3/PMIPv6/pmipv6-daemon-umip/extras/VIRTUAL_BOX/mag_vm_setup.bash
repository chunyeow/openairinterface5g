#!/bin/bash

###############################################################
export LMA_VM_NAME='LMA-UBUNTU-12.04.2-server-i386'
export MAG_VM_BASENAME='MAG-UBUNTU-12.04.2-server-i386'
MAG_CONFIG_FILES=( extras/example-mag1.conf extras/example-mag2.conf )
export NAMESERVER="192.168.12.100"
###############################################################

export DEFAULT_VIRTUAL_BOX_VM_PATH=$HOME/'VirtualBox VMs'

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

##########################################################
# BEFORE CLONING LMA VM, REMOVE ATTACHED DVD/CD MEDIUM
##########################################################

LMA_UUID=`VBoxManage showvminfo  $LMA_VM_NAME | grep Hardware\ UUID | cut -d: -f2 | tr -d ' '` 
echo LMA_UUID=$LMA_UUID
ISO_UUID=`vboxmanage showvminfo $LMA_UUID | grep IDE\ Controller\ \(1\,\ 0\) | grep UUID | sed 's/^.*UUID:\ //' |  sed 's/[\t() ]//g;;/^$/d'`
if [ "$ISO_UUID"x != x ]; then
    bash_exec "vboxmanage closemedium dvd  $ISO_UUID --delete"
fi
ISO_UUID=`vboxmanage showvminfo $LMA_UUID | grep IDE\ Controller\ \(1\,\ 1\) | grep UUID | sed 's/^.*UUID:\ //' |  sed 's/[\t() ]//g;;/^$/d'`
if [ "$ISO_UUID"x != x ]; then
    bash_exec "vboxmanage closemedium dvd  $ISO_UUID --delete"
fi



IPV4_BYTE=1

##################################
# PARSE MAG CONFIGURATION FILES  #
##################################
for (( i = 0 ; i < ${#MAG_CONFIG_FILES[@]} ; i++ )) 
do
    MAG_VM_NAME=`echo $MAG_VM_BASENAME | sed "s/MAG/MAG"$i"/"`
    echo "MAG_VM_NAME="$MAG_VM_NAME
    ##########################################################
    # CLONE LMA VM
    ##########################################################
    echo_success "Removing old MAG VM if any..."
    bash_exec "VBoxManage unregistervm $MAG_VM_NAME --delete"
    rm -Rf "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/*

    echo_success "Cloning LMA VM..."
    bash_exec "VBoxManage clonevm  $LMA_VM_NAME --mode machine  --name $MAG_VM_NAME --register"

    MAG_UUID=`VBoxManage showvminfo  $MAG_VM_NAME | grep Hardware\ UUID | cut -d: -f2 | tr -d ' '` 
    echo MAG_UUID=$MAG_UUID

    echo_success "Modifying MAG VM..."
    bash_exec "VBoxManage modifyvm  $MAG_UUID --nic1 intnet        --nic2 intnet        --nic3 null         --nic4 null \
                                          --nictype1 82545EM    --nictype2 82545EM   --nictype3 82545EM   --nictype4 82545EM \
                                          --cableconnected1 on  --cableconnected2 on --cableconnected3 off --cableconnected4 off \
                                          --intnet1 pmip_egress     --intnet2 pmip_ingress$i \
                                          --macaddress1 00123456789$i --macaddress2 001122334455 \
                                          --nictrace1 on --nictrace2 on \
                                          --nictracefile1 mag_egress$i --nictracefile1 mag_ingress$i "


    PMIP_EGRESS_MAC_ADDRESS=` vboxmanage showvminfo $MAG_UUID | grep pmip_egress  | sed 's/^.*MAC:\ //' | cut --delimiter=',' -f 1`
    PMIP_INGRESS_MAC_ADDRESS=`vboxmanage showvminfo $MAG_UUID | grep pmip_ingress | sed 's/^.*MAC:\ //' | cut --delimiter=',' -f 1`

    mkdir "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO
    rm -Rf "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/*
    cp mag_guest_setup.bash "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/mag_guest_setup.bash



    let "IPV4_BYTE=IPV4_BYTE+1"
    rm -f /tmp/source.txt
    cat $OPENAIR3_DIR/PMIPv6/pmipv6-daemon-umip/${MAG_CONFIG_FILES[$i]}  | grep \; | tr -d ';' | sed -e 's/  */ /gp;' | sed -e "s/^ *//;s/ *$//;s/ \{1,\}/ /g" | tr ' ' '='  > /tmp/source.txt
    source /tmp/source.txt
    
    echo_success "Generating /etc/network/interface file..."
    echo "# This file describes the network interfaces available on your system" >  "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo "# and how to activate them. For more information, see interfaces(5)."  >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo " "                                                                     >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo "# The loopback network interface"                                      >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo "auto lo"                                                               >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo "iface lo inet loopback"                                                >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo " "                                                                     >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo "# EGRESS_MAC_ADDRESS $PMIP_EGRESS_MAC_ADDRESS"                         >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo "auto PMIP_INGRESS_IF_TO_FIND"                                          >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo "iface PMIP_INGRESS_IF_TO_FIND inet static"                             >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo "address 192.168.11.1"                                                  >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo "netmask 255.255.255.0"                                                 >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo "broadcast 192.168.11.255"                                              >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo "gateway 192.168.33.1"                                                  >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo "nameserver $NAMESERVER"                                                >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo " "                                                                     >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo "# INGRESS_MAC_ADDRESS $PMIP_INGRESS_MAC_ADDRESS"                       >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo "auto PMIP_EGRESS_IF_TO_FIND"                                           >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo "iface PMIP_EGRESS_IF_TO_FIND inet static"                              >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo "address 192.168.33.$IPV4_BYTE"                                         >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo "netmask 255.255.255.0"                                                 >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    echo "broadcast 192.168.33.255"                                              >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/if_mag
    rm -f /tmp/source.txt

    chmod -Rf 777 "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO
    echo_success "Creating ISO file..."
    mkisofs -max-iso9660-filenames -untranslated-filenames -no-iso-translate -o "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/cd_mag_guest_setup.iso "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/MKISO/

    echo_success "Attaching ISO file as a DVD to VM..."
    VBoxManage storageattach $MAG_UUID --storagectl "IDE Controller" --type dvddrive --passthrough off --port 1 --device 0 --medium "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MAG_VM_NAME"/cd_mag_guest_setup.iso

done


echo_success '+---------------------------------------------------------------------------+'
echo_warning '| AFTER THE BOOT OF THE O.S.:                                               |'
echo_warning '| Login with the root account                                               |'
echo_warning '| Execute the bash script to install the PMIP environment and software:     |'
echo_success '| #root@lma:~# mount -t iso9660 /dev/dvd1  /mnt                             |'
echo_success '| #mount: block device /dev/sr0 is write-protected, mounting read-only      |'
echo_success '| #root@lma:~# /mnt/mag_guest_setup.bash                                    |'
echo_success '+---------------------------------------------------------------------------+'
