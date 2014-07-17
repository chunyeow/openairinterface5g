# !/bin/sh
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

CURRENT_PATH=`pwd`

sudo apt-get install autoconf automake gawk cmake make gcc flex bison libsctp1 libsctp-dev libidn2-0-dev \
libidn11-dev libmysqlclient-dev libxml2-dev swig python-dev cmake-curses-gui \
valgrind guile-2.0-dev libgmp-dev libgcrypt11-dev gdb unzip libtasn1-3-dev g++ \
linux-headers-`uname -r` build-essential -y

source ../../SCRIPTS/utils.bash

if [ -f install_log.txt ]
    then
    rm -f install_log.txt
fi

if [ ! -d /usr/local/src/ ]
    then
    echo "/usr/local/src/ doesn't exist please create one"
    exit -1
fi

if [ ! -w /usr/local/src/ ]
    then
    echo "You don't have permissions to write to /usr/local/src/"
    exit -1
fi

cd /usr/local/src/
echo "Downloading nettle archive"

if [ -f nettle-2.5.tar.gz ]
    then
    rm -f nettle-2.5.tar.gz
fi
if [ -f nettle-2.5.tar ]
    then
    rm -f nettle-2.5.tar
fi
if [ -d nettle-2.5 ]
    then
    rm -rf nettle-2.5/
fi

wget ftp://ftp.lysator.liu.se/pub/security/lsh/nettle-2.5.tar.gz > install_log.txt
gunzip nettle-2.5.tar.gz > install_log.txt
echo "Uncompressing nettle archive"
tar -xf nettle-2.5.tar
cd nettle-2.5/
./configure --disable-openssl --enable-shared --prefix=/usr > install_log.txt
if [ $? -ne 0 ]
then
    exit -1
fi
echo "Compiling nettle"
make -j2 > install_log.txt 2>&1
make check > install_log.txt
sudo make install > install_log.txt
cd ../

echo "Downloading gnutls archive"

if [ -f gnutls-3.1.0.tar.xz ]
    then
    rm -f gnutls-3.1.0.tar.xz
fi
if [ -d gnutls-3.1.0/ ]
    then
    rm -rf gnutls-3.1.0/
fi

wget ftp://ftp.gnutls.org/gcrypt/gnutls/v3.1/gnutls-3.1.0.tar.xz > install_log.txt
tar -xf gnutls-3.1.0.tar.xz
echo "Uncompressing gnutls archive"
cd gnutls-3.1.0/
./configure --prefix=/usr
if [ $? -ne 0 ]
then
    exit -1
fi
echo "Compiling gnutls"
make -j2 > install_log.txt 2>&1
sudo make install > install_log.txt
cd ../

echo "Downloading freeDiameter archive"

if [ -f 1.1.5.tar.gz ]
    then
    rm -f 1.1.5.tar.gz
fi
if [ -d freeDiameter-1.1.5/ ]
    then
    rm -rf freeDiameter-1.1.5/
fi

wget http://www.freediameter.net/hg/freeDiameter/archive/1.1.5.tar.gz > install_log.txt
tar -xzf 1.1.5.tar.gz > install_log.txt
echo "Uncompressing freeDiameter archive"
cd freeDiameter-1.1.5
patch -p1 < $CURRENT_PATH/freediameter-1.1.5.patch > install_log.txt
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ../ > install_log.txt
if [ $? -ne 0 ]
then
    exit -1
fi
echo "Compiling freeDiameter"
make -j2 > install_log.txt 2>&1
#make help
make test > install_log.txt
sudo make install > install_log.txt

cd $CURRENT_PATH
./make_certs.sh
