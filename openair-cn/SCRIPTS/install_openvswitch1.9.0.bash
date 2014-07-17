#!/bin/bash
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
#  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.
#
################################################################################
# Tested on ubuntu 12.04 with updates on 05 april 2013, 07 november 2013, 20 january 2014

# Make sure only root can run our script
if [[ $EUID -ne 0 ]]; then
   echo "This script must be run as root" 1>&2
   exit 1
fi

apt-get install iproute pkg-config python-twisted-conch python-anyjson \
python-pyside python3-pyside python-zope python-qt4 libssl-dev uml-utilities    \
linux-headers-`uname -r` vlan -y

rmmod bridge
cd /usr/local/src/
if [ ! -f /usr/local/src/openvswitch-1.9.0.tar.gz ]; then
    wget http://openvswitch.org/releases/openvswitch-1.9.0.tar.gz
fi
tar -xzf openvswitch-1.9.0.tar.gz
cd openvswitch-1.9.0
./boot.sh
./configure --with-linux=/lib/modules/`uname -r`/build  --disable-ssl
make
make install
make modules_install

if [ -f /lib/modules/`uname -r`/kernel/net/openvswitch/openvswitch.ko ] ; then
    insmod /lib/modules/`uname -r`/kernel/net/openvswitch/openvswitch.ko
else
    if  [ -f /lib/modules//`uname -r`/extra/openvswitch.ko ] ; then
        insmod /lib/modules//`uname -r`/extra/openvswitch.ko
	else
       echo_error "Could not find openvswitch.ko, exiting"
	   exit 1
    fi
fi
# Initialize the configuration database using ovsdb-tool, e.g.:

mkdir -p /usr/local/etc/openvswitch
ovsdb-tool create /usr/local/etc/openvswitch/conf.db vswitchd/vswitch.ovsschema

#=======
# Startup
#=======
# Before starting ovs-vswitchd itself, you need to start its
# configuration database, ovsdb-server.  Each machine on which Open
# vSwitch is installed should run its own copy of ovsdb-server.
# Configure it to use the database you created during step 7 of
# installation, above, to listen on a Unix domain socket, to connect to
# any managers specified in the database itself, and to use the SSL
# configuration in the database:

ovsdb-server --remote=punix:/usr/local/var/run/openvswitch/db.sock --remote=db:Open_vSwitch,manager_options --pidfile --detach

# Then initialize the database using ovs-vsctl.  This is only
# necessary the first time after you create the database with
# ovsdb-tool (but running it at any time is harmless):

ovs-vsctl --no-wait init

# Then start the main Open vSwitch daemon, telling it to
# connect to the same Unix domain socket:

ovs-vswitchd --pidfile --detach

