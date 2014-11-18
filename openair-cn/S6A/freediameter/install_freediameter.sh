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

# L.GAUTHIER: GNUTLS 3.1.23 compiles on 14.04 x64 with nettle comming with ubuntu
# see http://www.bauer-power.net/2014/06/how-to-install-gnutls-3123-from-source.html#.VD6LI4VxOPI
# if we have ubuntu 14.10 the default packages are ok

[ -f /etc/os-release ] && source /etc/os-release
if  echo $NAME $VERSION_ID | awk '{version=$2+0; if (version>14 && $1=="Ubuntu") exit 0} {exit 1}'
 then 
   apt-get install -y gnutls-bin nettle-bin nettle-dev libssl-dev 
else 
  echo "Downloading nettle archive"

  rm -rf nettle-2.5.tar.gz nettle-2.5.tar nettle-2.5 2> /dev/null
  wget ftp://ftp.lysator.liu.se/pub/security/lsh/nettle-2.5.tar.gz > install_log.txt
  echo "Uncompressing nettle archive"
  tar xf nettle-2.5.tar.gz
  cd nettle-2.5/
  ./configure --disable-openssl --enable-shared --prefix=/usr > install_log.txt || exit -1
  echo "Compiling nettle"
  make -j4 > install_log.txt 2>&1
  make check > install_log.txt
  sudo make install > install_log.txt
  cd ../

  echo "Downloading gnutls archive"
  rm -rf gnutls-3.1.0.tar.xz gnutls-3.1.0 2> /dev/null
  wget ftp://ftp.gnutls.org/gcrypt/gnutls/stable/gnutls-3.1.0.tar.xz > install_log.txt
  echo "Uncompressing gnutls archive"
  tar xf gnutls-3.1.0.tar.xz
  cd gnutls-3.1.0/
  ./configure --prefix=/usr || exit -1
  echo "Compiling gnutls"
  make -j2 > install_log.txt 2>&1
  sudo make install > install_log.txt
  cd ../
fi

echo "Downloading freeDiameter archive"
apt-get install -y mercurial
rm -rf latest.tar.gz freeDiameter
wget http://www.freediameter.net/latest.tar.gz
echo "Uncompressing freeDiameter archive"
tar -xzf latest.tar.gz
cd freeDiameter
hg update 1.1.5
patch -p1 < $OPENAIRCN_DIR/S6A/freediameter/freediameter-1.1.5.patch > install_log.txt
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ../ > install_log.txt || exit -1
echo "Compiling freeDiameter"
make -j2 > install_log.txt 2>&1
#make help
make test > install_log.txt
sudo make install > install_log.txt

cd $CURRENT_PATH
./make_certs.sh
