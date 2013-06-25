#!/bin/bash

###############################################################
export LMA_VM_NAME='LMA-UBUNTU-12.04.2-server-i386'
export MN_VM_NAME='MN-UBUNTU-12.04.2-server-i386'
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


##########################################################
# CLONE LMA VM
##########################################################
echo_success "Removing old MN VM if any..."
bash_exec "VBoxManage unregistervm $MN_VM_NAME --delete"
rm -Rf "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/*

echo_success "Cloning LMA VM..."
bash_exec "VBoxManage clonevm  $LMA_VM_NAME --mode machine  --name $MN_VM_NAME --register"

MN_UUID=`VBoxManage showvminfo  $MN_VM_NAME | grep Hardware\ UUID | cut -d: -f2 | tr -d ' '` 
echo MN_UUID=$MN_UUID

echo_success "Modifying MAG VM..."
bash_exec "VBoxManage modifyvm  $MN_UUID   --nic1 intnet              --nic2 intnet              \
                                          --nictype1 82545EM          --nictype2 82545EM         \
                                          --cableconnected1 on        --cableconnected2 on       \
                                          --intnet1 pmip_ingress0     --intnet2 pmip_ingress1    \
                                          --macaddress1 001122330011  --macaddress2 001122330022 \
                                          --nictrace1 on              --nictrace2 on \
                                          --nictracefile1 mn_ingress0 --nictracefile2 mn_ingress1"


    NIC1_MAC_ADDRESS=`vboxmanage showvminfo $MN_UUID | grep NIC\ 1 | sed 's/^.*MAC:\ //' | cut --delimiter=',' -f 1`
    NIC2_MAC_ADDRESS=`vboxmanage showvminfo $MN_UUID | grep NIC\ 2 | sed 's/^.*MAC:\ //' | cut --delimiter=',' -f 1`


    mkdir "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO
    rm -Rf "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/*
    cp mn_guest_setup.bash "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/mn_guest_setup.bash



    let "IPV4_BYTE=IPV4_BYTE+1"

    echo_success "Generating /etc/network/interface file..."
    echo "# This file describes the network interfaces available on your system" >  "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo "# and how to activate them. For more information, see interfaces(5)."  >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo " "                                                                     >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo "# The loopback network interface"                                      >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo "auto lo"                                                               >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo "iface lo inet loopback"                                                >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo " "                                                                     >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo "# NIC1_MAC_ADDRESS $NIC1_MAC_ADDRESS"                                  >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo "auto IF1_TO_FIND"                                                      >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo "iface IF1_TO_FIND inet static"                                         >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo "address 192.168.11.$IPV4_BYTE"                                         >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo "netmask 255.255.255.0"                                                 >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo "broadcast 192.168.11.255"                                              >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo "gateway 192.168.11.1"                                                  >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo "nameserver $NAMESERVER"                                                >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo " "                                                                     >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    let "IPV4_BYTE=IPV4_BYTE+1"
    echo "# NIC2_MAC_ADDRESS $NIC2_MAC_ADDRESS"                                  >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo "auto IF2_TO_FIND"                                                      >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo "iface IF2_TO_FIND inet static"                                         >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo "address 192.168.11.$IPV4_BYTE"                                         >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo "netmask 255.255.255.0"                                                 >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo "broadcast 192.168.11.255"                                              >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn
    echo " "                                                                     >> "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/if_mn


    chmod -Rf 777 "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO
    echo_success "Creating ISO file..."
    mkisofs -max-iso9660-filenames -untranslated-filenames -no-iso-translate -o "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/cd_mn_guest_setup.iso "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/MKISO/

    echo_success "Attaching ISO file as a DVD to VM..."
    VBoxManage storageattach $MN_UUID --storagectl "IDE Controller" --type dvddrive --passthrough off --port 1 --device 0 --medium "$DEFAULT_VIRTUAL_BOX_VM_PATH"/"$MN_VM_NAME"/cd_mn_guest_setup.iso


echo_success '+---------------------------------------------------------------------------+'
echo_warning '| AFTER THE BOOT OF THE O.S.:                                               |'
echo_warning '| Login with the root account                                               |'
echo_warning '| Execute the bash script to install the PMIP environment and software:     |'
echo_success '| #root@lma:~# mount -t iso9660 /dev/dvd1  /mnt                             |'
echo_success '| #mount: block device /dev/sr0 is write-protected, mounting read-only      |'
echo_success '| #root@lma:~# /mnt/mn_guest_setup.bash                                     |'
echo_success '+---------------------------------------------------------------------------+'
