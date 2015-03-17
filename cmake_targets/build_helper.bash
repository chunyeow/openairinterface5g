################################################################################
#   OpenAirInterface
#   Copyright(c) 1999 - 2014 Eurecom
#
#    OpenAirInterface is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#
#    OpenAirInterface is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with OpenAirInterface.The full GNU General Public License is
#    included in this distribution in the file called "COPYING". If not,
#    see <http://www.gnu.org/licenses/>.
#
#  Contact Information
#  OpenAirInterface Admin: openair_admin@eurecom.fr
#  OpenAirInterface Tech : openair_tech@eurecom.fr
#  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
#
#  Address      : Eurecom, Campus SophiaTech, 450 Route des Chappes, CS 50193 - 06904 Biot Sophia Antipolis cedex, FRANCE
#
################################################################################
# file build_cmake.bash
# brief
# author Laurent Thomas
#
#######################################
SUDO=sudo

###############################
## echo and  family
###############################
black='\E[30m'
red='\E[31m'
green='\E[32m'
yellow='\E[33m'
blue='\E[34m'
magenta='\E[35m'
cyan='\E[36m'
white='\E[37m'
reset_color='\E[00m'
COLORIZE=1

cecho()  {  
    # Color-echo
    # arg1 = message
    # arg2 = color
    local default_msg="No Message."
    message=${1:-$default_msg}
    color=${2:-$green}
    [ "$COLORIZE" = "1" ] && message="$color$message$reset_color"
    echo -e "$message"
    return
}

echo_error()   { cecho "$*" $red          ;}
echo_fatal()   { cecho "$*" $red; exit -1 ;}
echo_warning() { cecho "$*" $yellow       ;}
echo_success() { cecho "$*" $green        ;}
echo_info()    { cecho "$*" $blue         ;}

print_help() {
echo_info '
This program installs OpenAirInterface Software
You should have ubuntu 14.xx, updated, and the Linux kernel >= 3.14
Options
-h
   This help
-c | --clean
   Erase all files made by previous compilation, installation" 
--clean-kernel
   Erase previously installed features in kernel: iptables, drivers, ...
-C | --config-file
   The configuration file to install
-I | --install-external-packages 
   Installs required packages such as LibXML, asn1.1 compiler, freediameter, ...
   This option will require root password
-g | --run-with-gdb
   Add debugging symbols to compilation directives
--eNB
   Makes the eNB LTE softmodem
--UE
   Makes the UE softmodem
--EPC
   Makes the EPC
-r | --3gpp-release
   default is Rel10, 
   Rel8 limits the implementation to 3GPP Release 8 version
-w | --hardware
   EXMIMO (Default), USRP, None
   Adds this RF board support (in external packages installation and in compilation)
--oaisim
   Makes the oaisim simulator
--phy_simulators
   Makes the unitary tests Layer 1 simulators
--core_simulators
   Makes the core security features unitary simulators
-s | --check
   runs a set of auto-tests based on simulators and several compilation tests
-V | --vcd
   Adds a debgging facility to the binary files: GUI with major internal synchronization events
-x | --xforms
   Adds a software oscilloscope feature to the produced binaries
--install-system-files
   Install OpenArInterface required files in Linux system 
   (will ask root password)
Typical Options for a quick startup with a COTS UE and Eurecom RF board: build_oai.bash -I -g -eNB -EPC -x --install-system-files'
}

###########################
# Cleaners
###########################

clean_kernel() {
    $SUDO modprobe ip_tables
    $SUDO modprobe x_tables
    $SUDO iptables -P INPUT ACCEPT
    $SUDO iptables -F INPUT
    $SUDO iptables -P OUTPUT ACCEPT
    $SUDO iptables -F OUTPUT
    $SUDO iptables -P FORWARD ACCEPT
    $SUDO iptables -F FORWARD
    $SUDO iptables -t nat -F
    $SUDO iptables -t mangle -F
    $SUDO iptables -t filter -F
    $SUDO iptables -t raw -F
    echo_info "Flushed iptables"
    $SUDO rmmod nasmesh > /dev/null 2>&1
    $SUDO rmmod oai_nw_drv  > /dev/null 2>&1
    $SUDO rmmod openair_rf > /dev/null 2>&1
    $SUDO rmmod ue_ip > /dev/null 2>&1
    echo_info "removed drivers from kernel"
}

clean_all_files() {
 dir=$OPENAIR_DIR/cmake
 rm -rf $dir/log $dir/bin $dir/autotests/bin $dir/autotests/log $dir/autotests/*/buid $dir/build_*/build
}

###################################
# Compilers
###################################

compilations() {
  cd $OPENAIR_DIR/cmake_targets/$1/build
  {
    rm -f $3
    make -j4 $2
  } > $dlog/$2.$REL.txt 2>&1
  if [ -s $3 ] ; then
     cp $3 $dbin/$2.$REL
     echo_success "$2 compiled"
  else
     echo_error "$2 compilation failed"
  fi
}

run_tests() {
   $1 > $2 2>&1
   grep 
}

test_compile() {
    mkdir -p $tdir/$1/build
    cd $tdir/$1/build
    {
	cmake ..
	rm -f $3
	make -j4 $2
    } > $tdir/log/$1.txt
    if [ -s $3 ] ; then
     cp $3 $tdir/bin/$3.$1
     echo_success "$1 test compiled"
  else
     echo_error "$1 test compilation failed"
  fi
}

run_compilation_autotests() {
    tdir=$OPENAIR_DIR/cmake_targets/autotests
    mkdir -p $tdir/bin $tdir/log
    updated=$(svn st -q $OPENAIR_DIR)
    if [ "$updated" != "" ] ; then
	echo_warning "some files are not in svn: $updated"
    fi
    compilations \
        test.0101 oaisim \
        oaisim  $tdir/bin/oaisim.r8

    compilations \
        test.0102 oaisim \
        oaisim  $tdir/bin/oaisim.r8.nas

    compilations \
        test.0103 oaisim \
        oaisim  $tdir/bin/oaisim.r8.rf

    compilations \
        test.0104 dlsim \
        dlsim  $tdir/bin/dlsim

    compilations \
        test.0104 ulsim \
        ulsim  $tdir/bin/ulsim

    compilations \
        test.0106 oaisim \
        oaisim  $tdir/bin/oaisim.r8.itti

    compilations \
        test.0107 oaisim \
        oaisim  $tdir/bin/oaisim.r10

    compilations \
        test.0108 oaisim \
        oaisim  $tdir/bin/oaisim.r10.itti

    compilations \
        test.0114 oaisim \
        oaisim  $tdir/bin/oaisim.r8.itti.ral

    compilations \
        test.0115 oaisim \
        oaisim  $tdir/bin/oaisim.r10.itti.ral

    compilations \
        test.0102 nasmesh \
        CMakeFiles/nasmesh/nasmesh.ko $tdir/bin/nasmesh.ko

}

##########################################
# X.509 certificates
##########################################

make_one_cert() {
    openssl genrsa -out $1.key.pem 1024
    openssl req -new -batch -out $1.csr.pem -key $1.key.pem -subj /CN=$1.eur/C=FR/ST=PACA/L=Aix/O=Eurecom/OU=CM
    openssl ca -cert cacert.pem -keyfile cakey.pem -in $1.csr.pem -out $1.cert.pem -outdir . -batch
}

make_certs(){

    # certificates are stored in diameter config directory
    if [ ! -d /usr/local/etc/freeDiameter ];  then
        echo "Creating non existing directory: /usr/local/etc/freeDiameter/"
        $SUDO mkdir -p /usr/local/etc/freeDiameter/ || echo_error "can't create: /usr/local/etc/freeDiameter/"
    fi

    cd /usr/local/etc/freeDiameter
    echo "creating the CA certificate"
    echo_warning "erase all existing certificates as long as the CA is regenerated"
    $SUDO rm -f /usr/local/etc/freeDiameter/

    # CA self certificate
    $SUDO openssl req  -new -batch -x509 -days 3650 -nodes -newkey rsa:1024 -out cacert.pem -keyout cakey.pem -subj /CN=eur/C=FR/ST=PACA/L=Aix/O=Eurecom/OU=CM
    
    # generate hss certificate and sign it
    $SUDO make_one_cert hss
    $SUDO make_one_cert mme

    # legacy config is using a certificate named 'user'
    $SUDO make_one_cert user

}

############################################
# External packages installers
############################################

install_nettle_from_source() {
    cd /tmp
    echo "Downloading nettle archive"
    wget ftp://ftp.lysator.liu.se/pub/security/lsh/nettle-2.5.tar.gz 
    tar -xzf nettle-2.5.tar.gz
    cd nettle-2.5/
    ./configure --disable-openssl --enable-shared --prefix=/usr 
    echo "Compiling nettle"
    make -j4
    make check 
    $SUDO make install 
    rm -rf /tmp/nettle-2.5.tar.gz /tmp/nettle-2.5
}

install_gnutls_from_source(){
    cd /tmp 
    echo "Downloading gnutls archive"
    wget ftp://ftp.gnutls.org/gcrypt/gnutls/v3.1/gnutls-3.1.23.tar.xz 
    tar -xzf gnutls-3.1.23.tar.xz
    cd gnutls-3.1.23/
    ./configure --prefix=/usr
    echo "Compiling gnutls"
    make -j4
    $SUDO make install 
    rm -rf /tmp/gnutls-3.1.23.tar.xz /tmp/gnutls-3.1.23
}

install_freediameter_from_source() {
    cd /tmp
    echo "Downloading freeDiameter archive"
    wget http://www.freediameter.net/hg/freeDiameter/archive/1.1.5.tar.gz 
    tar xf 1.1.5.tar.gz
    cd freeDiameter-1.1.5
    patch -p1 < $OPENAIRCN_DIR/S6A/freediameter/freediameter-1.1.5.patch 
    mkdir build
    cd build
    cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ../ 
    echo "Compiling freeDiameter"
    make -j4
    make test 
    $SUDO make install 
    rm -rf /tmp/1.1.5.tar.gz /tmp/freeDiameter-1.1.5
}

check_install_usrp_uhd_driver(){
    if [ ! -f /etc/apt/sources.list.d/ettus.list ] ; then 
        $SUDO bash -c 'echo "deb http://files.ettus.com/binaries/uhd/repo/uhd/ubuntu/`lsb_release -cs` `lsb_release -cs` main" >> /etc/apt/sources.list.d/ettus.list'
        $SUDO apt-get update
     fi
        $SUDO apt-get -y install  python libboost-all-dev libusb-1.0-0-dev
        $SUDO apt-get -y install -t `lsb_release -cs` uhd
}

check_install_oai_software() {
    
    $SUDO apt-get update
    $SUDO apt-get install -y 
        autoconf  \
	automake  \
	bison  \
	build-essential \
	check \
	cmake \
	cmake-curses-gui  \
	dialog \
	dkms \
	doxygen \
	ethtool \
	flex  \
	g++ \
	gawk \
	gcc \
	gccxml \
	gdb  \
	graphviz \
	gtkwave \
	guile-2.0-dev  \
	iperf \
	iproute \
	iptables \
	iptables-dev \
	libatlas-base-dev \
	libatlas-dev \
	libblas \
	libblas3gf \
	libblas-dev \
	libboost-all-dev \
	libconfig8-dev \
	libforms-bin \
	libforms-dev \
	libgcrypt11-dev \
	libgmp-dev \
	libgtk-3-dev \
	libidn11-dev  \
	libidn2-0-dev  \
	libmysqlclient-dev  \
	libpgm-5.1 \
	libpgm-dev \
	libpthread-stubs0-dev \
	libsctp1  \
	libsctp-dev  \
	libssl-dev  \
	libtasn1-3-dev  \
	libtool  \
	libusb-1.0-0-dev \
	libxml2 \
	libxml2-dev  \
	linux-headers-`uname -r` \
	make \
	mysql-client  \
	mysql-server \
	openssh-client \
	openssh-server \
	openssl \
	openvpn \
	phpmyadmin \
	pkg-config \
	python  \
	python-dev  \
	python-pexpect \
	sshfs \
	subversion \
	swig  \
	tshark \
	uml-utilities \
	unzip  \
	valgrind  \
	vlan
    if [ `lsb_release -rs` = '12.04' ] ; then
        install_nettle_from_source
	install_gnutls_from_source
    else
        $SUDO apt-get install -y libgnutls-dev nettle-dev nettle-bin 
    fi
    install_freediameter_from_source
    install_asn1c_from_source
}

install_asn1c_from_source(){
    mkdir /tmp/asn1c-r1516
    cd /tmp/asn1c-r1516
    svn co https://github.com/vlm/asn1c/trunk  /tmp/asn1c-r1516 -r 1516
    patch -p0 < $OPENAIRCN_DIR/S1AP/MESSAGES/ASN1/asn1cpatch.p0
    ./configure
    make
    $SUDO make install
}

#################################################
# 2. compile 
################################################
compile_hss() {
    cd $OPENAIRCN_DIR/OPENAIRHSS
    
    if [ "$CLEAN" = "1" ]; then
        echo_info "build a clean HSS"
        rm -rf obj* m4 .autom4* configure
    fi

    echo_success "Invoking autogen"
    ./autogen.sh || return 1
    mkdir -p objs ; cd objs
    echo_success "Invoking configure"
    ../configure || return 1
    if [ -f Makefile ];  then
        echo_success "Compiling..."
        make -j4
	# seems a bug in hss compilation: run make twice to work around
	make -j4
        if [ $? -ne 0 ]; then
            echo_error "Build failed, exiting"
            return 1
        else 
            return 0
        fi
    else
        echo_error "Configure failed, aborting"
    fi
    return 1
}

install_nas_tools() {
    cd $1
    if [ ! -f .ue.nvram ]; then
        echo_success "generate .ue_emm.nvram .ue.nvram"
        ./ue_data --gen
    fi

    if [ ! -f .usim.nvram ]; then
        echo_success "generate .usim.nvram"
        ./usim_data --gen
    fi

}

##################################
# create HSS DB
################################

# arg 1 is mysql user      (root)
# arg 2 is mysql password  (linux)
# arg 3 is hss username    (hssadmin)
# arg 4 is hss password    (admin)
# arg 5 is database name   (oai_db)
create_hss_database(){
    EXPECTED_ARGS=5
    if [ $# -ne $EXPECTED_ARGS ]
    then
        echo_error "Usage: $0 dbuser dbpass hssuser hsspass databasename"
	return 1
    fi
    
    Q1="GRANT ALL PRIVILEGES ON *.* TO '$3'@'localhost' IDENTIFIED BY '$4' WITH GRANT OPTION;"
    Q2="FLUSH PRIVILEGES;"
    mysql -u $1 --password=$2 -e "${Q1}${Q2}"
    if [ $? -ne 0 ]; then
	echo_error "$3 permissions creation failed"
	echo_error "verify root password for mysql is linux: mysql -u root --password=linux"
	echo_error "if not, reset it to "linux" with sudo dpkg-reconfigure mysql-server-5.5"
	return 1
    else
	echo_success "$3 permissions creation succeeded"
    fi
    
    Q3="CREATE DATABASE IF NOT EXISTS $5;"
    mysql -u $3 --password=$4 -e "${Q3}"
    if [ $? -ne 0 ]; then
	echo_error "$5 creation failed"
	return 1
    else
	echo_success "$5 creation succeeded"
    fi
    
    mysql -u $3 --password=$4 $5 < $OPENAIRCN_DIR/OPENAIRHSS/db/oai_db.sql
    if [ $? -ne 0 ]; then
        echo_error "$5 tables creation failed"
        return 1
    else
        echo_success "$5 tables creation succeeded"
    fi
    return 0
}

################################
# set_openair_env
###############################
set_openair_env(){
    fullpath=`readlink -f $BASH_SOURCE`
    [ -f "/.$fullpath" ] || fullpath=`readlink -f $PWD/$fullpath`
    openair_path=${fullpath%/cmake_targets/*}
    openair_path=${openair_path%/targets/*}
    openair_path=${openair_path%/openair-cn/*}
    openair_path=${openair_path%/openair[123]/*}    
    export OPENAIR_DIR=$openair_path
    export OPENAIR_HOME=$openair_path
    export OPENAIR1_DIR=$openair_path/openair1
    export OPENAIR2_DIR=$openair_path/openair2
    export OPENAIR3_DIR=$openair_path/openair3
    export OPENAIRCN_DIR=$openair_path/openair-cn
    export OPENAIR_TARGETS=$openair_path/targets
}

