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

compilations() {
  cd $OPENAIR_DIR/cmake_targets/$1
  {
    [ "$CLEAN" = "1" ] && rm -rf build
    mkdir -p build
    cd build
    rm -f $3
    cmake ..
    make -j4 $2
  } > $5 2>&1
  if [ -s $3 ] ; then
     cp $3 $4
     echo_success "$6"
  else
     echo_error "$7"
  fi
}

run_tests() {
   $1 > $2 2>&1
   grep 
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
        oaisim  $tdir/bin/oaisim.r8 \
        $tdir/log/test0101.txt \
	"test 0101:oaisim Rel8 passed" \
        "test 0101:oaisim Rel8 failed"

    compilations \
        test.0102 oaisim \
        oaisim  $tdir/bin/oaisim.r8.nas \
        $tdir/log/test0102.oaisim.txt \
	"test 0102:oaisim Rel8 nas passed" \
        "test 0102:oaisim Rel8 nas failed"
    compilations \
        test.0103 oaisim \
        oaisim  $tdir/bin/oaisim.r8.rf \
        $tdir/log/test0103.txt \
	"test 0103:oaisim rel8 rf passed" \
        "test 0103:oaisim rel8 rf failed"
    compilations \
        test.0104 dlsim \
        dlsim  $tdir/bin/dlsim \
        $tdir/log/test0104.txt \
	"test 0104:dlsim passed" \
        "test 0104:dlsim failed"    
    compilations \
        test.0104 ulsim \
        ulsim  $tdir/bin/ulsim \
        $tdir/log/test0105.txt \
	"test 0105: ulsim passed" \
        "test 0105: ulsim failed"
    compilations \
        test.0106 oaisim \
        oaisim  $tdir/bin/oaisim.r8.itti \
        $tdir/log/test0106.txt \
	"test 0103:oaisim rel8 itti passed" \
        "test 0103:oaisim rel8 itti failed"
    compilations \
        test.0107 oaisim \
        oaisim  $tdir/bin/oaisim.r10 \
        $tdir/log/test0107.txt \
	"test 0103:oaisim rel10 passed" \
        "test 0103:oaisim rel10 failed"
    compilations \
        test.0108 oaisim \
        oaisim  $tdir/bin/oaisim.r10.itti \
        $tdir/log/test0108.txt \
	"test 0108:oaisim rel10 itti passed" \
        "test 0108:oaisim rel10 itti failed"
    compilations \
        test.0114 oaisim \
        oaisim  $tdir/bin/oaisim.r8.itti.ral \
        $tdir/log/test0114.txt \
	"test 0114:oaisim rel8 itti ral passed" \
        "test 0114:oaisim rel8 itti ral failed"
    compilations \
        test.0115 oaisim \
        oaisim  $tdir/bin/oaisim.r10.itti.ral \
        $tdir/log/test0115.txt \
	"test 0114:oaisim rel10 itti ral passed" \
        "test 0114:oaisim rel10 itti ral failed" 
    compilations \
        test.0102 nasmesh \
        CMakeFiles/nasmesh/nasmesh.ko $tdir/bin/nasmesh.ko \
        $tdir/log/test0120.txt \
	"test 0120: nasmesh.ko passed" \
        "test 0120: nasmesk.ko failed"
}

make_certs(){
    
    # for certtificate generation
    rm -rf demoCA
    mkdir -m 777 -p demoCA
    echo 01 > demoCA/serial
    touch demoCA/index.txt
    
    echo "creating the certificate"
    
    user=$(whoami)
    HOSTNAME=$(hostname -f)
    
    echo "Creating certificate for user '$HOSTNAME'"
    
    # CA self certificate
    openssl req  -new -batch -x509 -days 3650 -nodes -newkey rsa:1024 -out cacert.pem -keyout cakey.pem -subj /CN=eur/C=FR/ST=PACA/L=Aix/O=Eurecom/OU=CM
    
    # openssl genrsa -out user.key.pem 1024
    openssl genrsa -out hss.key.pem 1024
    #openssl req -new -batch -out user.csr.pem -key user.key.pem -subj /CN=$HOSTNAME.eur/C=FR/ST=PACA/L=Aix/O=Eurecom/OU=CM
    openssl req -new -batch -out hss.csr.pem -key hss.key.pem -subj /CN=hss.eur/C=FR/ST=PACA/L=Aix/O=Eurecom/OU=CM
    openssl ca -cert cacert.pem -keyfile cakey.pem -in hss.csr.pem -out hss.cert.pem -outdir . -batch
    
    if [ ! -d /usr/local/etc/freeDiameter ];  then
        echo "Creating non existing directory: /usr/local/etc/freeDiameter/"
        $SUDO mkdir /usr/local/etc/freeDiameter/
    fi
    
    echo "Copying *.pem to /usr/local/etc/freeDiameter/"
    $SUDO cp *.pem /usr/local/etc/freeDiameter/
    mv *.pem bin/
    
    # openssl genrsa -out ubuntu.key.pem 1024
    # openssl req -new -batch -x509 -out ubuntu.csr.pem -key ubuntu.key.pem -subj /CN=ubuntu.localdomain/C=FR/ST=BdR/L=Aix/O=fD/OU=Tests
    # openssl ca -cert cacert.pem -keyfile cakey.pem -in ubuntu.csr.pem -out ubuntu.cert.pem -outdir . -batch
    
}

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

check_epc_s6a_certificate() {
    if [ -d /usr/local/etc/freeDiameter ]
    then
        if [ -f /usr/local/etc/freeDiameter/user.cert.pem ]
        then
            full_hostname=`cat /usr/local/etc/freeDiameter/user.cert.pem | grep "Subject" | grep "CN" | cut -d '=' -f6`
            if [ a$full_hostname == a`hostname`.${1:-'eur'} ]
            then
                echo_success "EPC S6A: Found valid certificate in /usr/local/etc/freeDiameter"
                return 0
            fi
        fi
    fi
    echo_error "EPC S6A: Did not find valid certificate in /usr/local/etc/freeDiameter"
    echo_warning "EPC S6A: generatting new certificate in /usr/local/etc/freeDiameter..."
    cd $OPENAIRCN_DIR/S6A/freediameter
    ./make_certs.sh ${1:-'eur'}
    if [ $# -lt 2 ] ; then
        check_epc_s6a_certificate ${1:-'eur'}  2
        return $?
    else
	exit 1
    fi
}

check_hss_s6a_certificate() {
    if [ -d /usr/local/etc/freeDiameter ]; then
        if [ -f /usr/local/etc/freeDiameter/hss.cert.pem ];  then
            full_hostname=`cat /usr/local/etc/freeDiameter/hss.cert.pem | grep "Subject" | grep "CN" | cut -d '=' -f6`
            if [ a$full_hostname == a`hostname`.${1:-'eur'} ]
            then
                echo_success "HSS S6A: Found valid certificate in /usr/local/etc/freeDiameter"
                return 0
            else 
                echo_error "Bad hss hostname found in cert file: "$full_hostname " hostname is "`hostname`
            fi
        fi
    fi
    echo_error "S6A: Did not find valid certificate in /usr/local/etc/freeDiameter"
    echo_warning "S6A: generatting new certificate in /usr/local/etc/freeDiameter..."
    cd $OPENAIRCN_DIR/OPENAIRHSS/conf
    ./make_certs.sh ${1:-'eur'}
    if [ $# -lt 2 ] ; then
        check_hss_s6a_certificate ${1:-'eur'} 2
        return $?
    else
	exit 1
    fi
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
    check_install_asn1c
}

check_install_asn1c(){    
    $SUDO $OPENAIR_TARGETS/SCRIPTS/install_asn1c_0.9.24.modified.bash
}

#################################################
# 2. compile 
################################################
compile_hss() {
    cd $OPENAIRCN_DIR/OPENAIRHSS
    
    if [ "$1" -eq 1 ]; then
        echo_info "build a clean HSS"
        rm -rfv obj* m4 .autom4* configure
    fi

    echo_success "Invoking autogen"
    ./autogen.sh || return 1
    mkdir -p objs ; cd objs
    echo_success "Invoking configure"
    ./configure || return 1
    if [ -f Makefile ];  then
        echo_success "Compiling..."
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


compile_epc() {
    cd $OPENAIRCN_DIR
    if [ $1 = 1 ]; then
        echo_info "build a clean EPC"
        bash_exec "rm -rf objs"
    fi
    OBJ_DIR=`find . -maxdepth 1 -type d -iname obj*`
    if [ ! -n "$OBJ_DIR" ]; then
        OBJ_DIR="objs"
        bash_exec "mkdir -m 777 ./$OBJ_DIR"
        echo_success "Created $OBJ_DIR directory"
    else
        OBJ_DIR=`basename $OBJ_DIR`
    fi
    if [ ! -f $OBJ_DIR/Makefile ]; then
        if [ ! -n "m4" ]; then
            mkdir -m 777 m4
        fi
        bash_exec "autoreconf -i -f"
        echo_success "Invoking autogen"
        bash_exec "libtoolize"        
        bash_exec "./autogen.sh"
        cd ./$OBJ_DIR
        echo_success "Invoking configure"
        if [ $DEBUG -ne 0 ]; then 
            ../configure --enable-debug --enable-standalone-epc --enable-gtp1u-in-kernel LDFLAGS=-L/usr/local/lib
        else
            ../configure                --enable-standalone-epc --enable-gtp1u-in-kernel LDFLAGS=-L/usr/local/lib
        fi
    else
        cd ./$OBJ_DIR
    fi

    #    pkill oai_epc
    #    pkill tshark

    if [ -f Makefile ]; then
        echo_success "Compiling..."
        make -j $NUM_CPU
        if [ $? -ne 0 ]; then
            echo_error "Build failed, exiting"
            return 1
        else 
            cp -pf ./OAI_EPC/oai_epc  $OPENAIR_TARGETS/bin
        fi
    else
        echo_error "Configure failed, exiting"
        return 1
    fi
    
    cd $OPENAIRCN_DIR/GTPV1-U/GTPUAH;
    make
    if [ $? -ne 0 ]; then
        echo_error "Build GTPUAH module failed, exiting"
        return 1
    else 
       	$SUDO cp -pfv ./Bin/libxt_*.so /lib/xtables
        $SUDO cp -pfv ./Bin/*.ko $OPENAIR_TARGETS/bin
    fi
    
    cd $OPENAIRCN_DIR/GTPV1-U/GTPURH;
    make
    if [ $? -ne 0 ]; then
        echo_error "Build GTPURH module failed, exiting"
        return 1
    else 
	$SUDO cp -pfv ./Bin/libxt_*.so /lib/xtables
	$SUDO cp -pfv ./Bin/*.ko $OPENAIR_TARGETS/bin
    fi
    return 0
}

compile_nas_tools() {

    export NVRAM_DIR=$OPENAIR_TARGETS/bin
    
    cd $NVRAM_DIR
    
    if [ ! -f /tmp/nas_cleaned ]; then
        echo_success "make --directory=$OPENAIRCN_DIR/NAS/EURECOM-NAS/tools veryveryclean"
        make --directory=$OPENAIRCN_DIR/NAS/EURECOM-NAS/tools veryveryclean
    fi
    echo_success "make --directory=$OPENAIRCN_DIR/NAS/EURECOM-NAS/tools all"
    make --directory=$OPENAIRCN_DIR/NAS/EURECOM-NAS/tools all
    rm .ue.nvram
    rm .usim.nvram
    touch /tmp/nas_cleaned
}


compile_ue_ip_nw_driver() {
    cd $OPENAIR2_DIR && make ue_ip.ko
}

# arg1 is RT
# arg2 is HW 
# arg3 is ENB_S1
install_ltesoftmodem() {
    # RT
    if [ $1 = "RTAI" ]; then 
        if [ ! -f /tmp/init_rt_done.tmp ]; then
            echo_info "  8.1 Insert RTAI modules"
            $SUDO insmod /usr/realtime/modules/rtai_hal.ko     > /dev/null 2>&1
            $SUDO insmod /usr/realtime/modules/rtai_sched.ko   > /dev/null 2>&1
            $SUDO insmod /usr/realtime/modules/rtai_sem.ko     > /dev/null 2>&1
            $SUDO insmod /usr/realtime/modules/rtai_fifos.ko   > /dev/null 2>&1
            $SUDO insmod /usr/realtime/modules/rtai_mbx.ko     > /dev/null 2>&1
            $SUDO touch /tmp/init_rt_done.tmp
            $SUDO chmod 666 /tmp/init_rt_done.tmp
        else
            echo_warning "  8.1 RTAI modules already inserted"
        fi
    fi
    #HW
    if [ $2 = "EXMIMO" ]; then 
	echo_info "  8.2 [EXMIMO] creating RTAI fifos"
	for i in `seq 0 64`; do 
	    have_rtfX=`ls /dev/ |grep -c rtf$i`;
	    if [ "$have_rtfX" -eq 0 ] ; then 
		$SUDO mknod -m 666 /dev/rtf$i c 150 $i; 
	    fi;
	done
	echo_info "  8.3 [EXMIMO] Build lte-softmodemdrivers"
	cd $OPENAIR_TARGETS/ARCH/EXMIMO/DRIVER/eurecom && make clean && make  # || exit 1
	cd $OPENAIR_TARGETS/ARCH/EXMIMO/USERSPACE/OAI_FW_INIT && make clean && make  # || exit 1
	
	echo_info "  8.4 [EXMIMO] Setup RF card"
	cd $OPENAIR_TARGETS/RT/USER
	. ./init_exmimo2.sh
    else 
	if [ $2 = "USRP" ]; then
	    echo_info "  8.2 [USRP] "
	fi

    fi
    
    # ENB_S1
    if [ $3 = 0 ]; then 
        cd $OPENAIR2_DIR && make clean && make nasmesh_netlink.ko  #|| exit 1
        cd $OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL && make clean && make  # || exit 1
    fi
    
}

# arg1 is ENB_S1 'boolean'
install_oaisim() {
    if [ $1 = 0 ]; then 
	cd $OPENAIR2_DIR && make clean && make nasmesh_netlink.ko  #|| exit 1
	cd $OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL && make clean && make  # || exit 1
    else
	compile_ue_ip_nw_driver
	install_nas_tools
    fi 
}


install_nas_tools() {
    cd $OPENAIR_TARGETS/bin
    if [ ! -f .ue.nvram ]; then
        echo_success "generate .ue_emm.nvram .ue.nvram"
        $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/ue_data --gen
    fi

    if [ ! -f .usim.nvram ]; then
        echo_success "generate .usim.nvram"
        $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/usim_data --gen
    fi
    $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/ue_data --print
    $OPENAIRCN_DIR/NAS/EURECOM-NAS/bin/usim_data --print
}

install_nasmesh(){
    echo_success "LOAD NASMESH IP DRIVER FOR UE AND eNB" 
    (cd $OPENAIR2_DIR/NAS/DRIVER/MESH/RB_TOOL && make clean && make)
    (cd $OPENAIR2_DIR && make clean && make nasmesh_netlink_address_fix.ko)
    $SUDO rmmod nasmesh
    $SUDO insmod $OPENAIR2_DIR/NAS/DRIVER/MESH/nasmesh.ko
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
    E_BADARGS=65
    MYSQL=`which mysql`
    rv=0
    if [ $# -ne $EXPECTED_ARGS ]
    then
        echo_fatal "Usage: $0 dbuser dbpass hssuser hsspass databasename"
        rv=1
    fi

    set_openair_env
    
    # removed %
    #Q1="GRANT ALL PRIVILEGES ON *.* TO '$3'@'%' IDENTIFIED BY '$4' WITH GRANT OPTION;"
    Q1="GRANT ALL PRIVILEGES ON *.* TO '$3'@'localhost' IDENTIFIED BY '$4' WITH GRANT OPTION;"
    Q2="FLUSH PRIVILEGES;"
    SQL="${Q1}${Q2}"
    $MYSQL -u $1 --password=$2 -e "$SQL"
    if [ $? -ne 0 ]; then
	echo_error "$3 permissions failed"
	return 1
    else
	echo_success "$3 permissions succeeded"
    fi
    
    
    Q1="CREATE DATABASE IF NOT EXISTS ${BTICK}$5${BTICK};"
    SQL="${Q1}"
    $MYSQL -u $3 --password=$4 -e "$SQL"
    if [ $? -ne 0 ]; then
	echo_error "$5 creation failed"
	return 1
    else
	echo_success "$5 creation succeeded"
    fi
    
    
    # test if tables have been created
    mysql -u $3 --password=$4  -e "desc $5.users" > /dev/null 2>&1
    
    if [ $? -eq 1 ]; then 
        $MYSQL -u $3 --password=$4 $5 < $OPENAIRCN_DIR/OPENAIRHSS/db/oai_db.sql
        if [ $? -ne 0 ]; then
            echo_error "$5 tables creation failed"
            return 1
        else
            echo_success "$5 tables creation succeeded"
        fi
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

########################################
### print help
######################################

print_help(){
    echo_success "Name : build_oai  - install and build OAI"
    echo_success "-c | --clean                            : Enable clean OAI build (default disabled)"
    echo_success "--clean-iptables"
    echo_success "-C | --config-file                      : Set the config file local path"
    echo_success "-g | --run-with-gdb                     : Run the executable built by this script with gdb"
    echo_success "-K | --itti-dump-file                   : Set the execution events trace file"
    echo_success "-l | --build-target                     : Set the LTE build target: ENB,EPC,HSS,NONE (default ENB)"
    echo_success "-m | --build-from-makefile              : Enable build from the makefile (default disabled)"
    echo_success "-r | --3gpp-release                     : Set the release: Rel8, Rel10 (default Rel10)"
    echo_success "-s | --check                            : Enable OAI testing and sanity check (default disabled)"
    echo_success "-V | --vcd                              : Log vcd events (costs performance)"
    echo_success "-w | --hardware                         : Set the hardware platform: EXMIMO, USRP (also installs UHD driver), ETHERNET, NONE, (default EXMIMO)"
    echo_success "-x | --xforms                           : Enable xforms (default disabled)"
}

print_help_perf(){
    echo_success "Name : perf_oai  generate traffic and evaluate the performance "
    echo_success "Usage: perf_oai.bash -l ITG "
    echo_success "-l | --perf-app               : Set Performance evaluation app: ITGS, ITGD,PING, OTG-OAISIM, OTG-CBA, (default PING)"
    echo_success "-m | --owd                    : enable D-ITG one-way-delay meter (default disabled)"
    echo_success "-e | --duration               : set the duration of the experiment (default 60000ms)"
    echo_success "-i | --idt-dist               : set the distribution of the inter-departure time: CONSTANT, UNIFORM,EXPONENTIAL (default CONSTANT)"
    echo_success "-s | --ps-dist                : set the distribution of the inter-departure time (default CONSTANT, available options: UNIFORM,EXPONENTIAL)"
    echo_success "-d | --dst                    : set the destination address (default 127.0.0.1)"
    echo_success "-p | --dst-port               : set the destination address (default NONE)"
    echo_success "-t | --test                   : enable test mode to validate the functionality (default disabled)"
    echo_success "-k | --keep-log-file          : keep the log files (default disabled)"

}
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


cecho()   # Color-echo
# arg1 = message
# arg2 = color
{
    local default_msg="No Message."
    message=${1:-$default_msg}
    color=${2:-$green}
    if [ "$BUILD_FROM_MAKEFILE" = "" ]; then 
	echo -e -n "$color$message$reset_color"
	echo
    else 
	echo "$message"
    fi
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

echo_fatal() {
    local my_string=""
    until [ -z "$1" ]
    do
        my_string="$my_string$1"
        shift
    done
    echo_error "$my_string"
    exit -1
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
echo_info() {
    local my_string=""
    until [ -z "$1" ]
    do
        my_string="$my_string$1"
        shift
    done
    cecho "$my_string" $blue
}

bash_exec() {
    output=$($1 2>&1)
    result=$?
    if [ $result -eq 0 ]; then
        echo_success "$1"
    else
        echo_error "$1: $output"
    fi
}
