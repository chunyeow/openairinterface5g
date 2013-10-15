Now we are using USRP N200/210 and working with OAI v4164.

** Get source code of UHD
git clone git://github.com/EttusResearch/uhd.git

** Install prerequisites
sudo apt-get install libboost-all-dev libusb-1.0-0-dev python-cheetah doxygen python-docutils

** Install UHD
cd <uhd-repo-path>/host
mkdir build
cd build
cmake ../
make
make test
sudo make install
sudo ldconfig

** Check the speed of USRP
The results should have almost no overflows and underflows, otherwise the OAI cannot work in realtime. 
./benchmark_rate --tx_rate 6.25e6 --rx_rate 6.25e6 --duration 30

** Build lte-softmodem-usrp
cd targets/RTAI/USER
./make_for_usrp.sh

** Run lte-softmodem-usrp
./lte-softmodem-usrp
