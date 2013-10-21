Pre-requisities:
Cmake libgnutls-3.1.0

The best way to install freeDiameter is to use to script provided in this folder.

download tarball here: http://www.freediameter.net/hg/freeDiameter/archive/1.1.5.tar.gz
extract it somewhere.
cd freeDiameter-1.1.5
patch -p1 < freediameter-1.1.5.patch
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX:PATH=/usr ../
sudo make install

if you choose to install libraries in default path (/usr/local/lib), you will have
    to tell configure to use LDFLAGS='-L/usr/local/lib'