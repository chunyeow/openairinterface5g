#!/bin/bash

rm -rf demoCA
mkdir demoCA
echo 01 > demoCA/serial
touch demoCA/index.txt

user=$(whoami)
HOSTNAME=$(hostname -f)

echo "Creating certificate for user '$HOSTNAME'"

# CA self certificate
openssl req  -new -batch -x509 -days 3650 -nodes -newkey rsa:1024 -out cacert.pem -keyout cakey.pem -subj /CN=eur/C=FR/ST=PACA/L=Aix/O=Eurecom/OU=CM

openssl genrsa -out user.key.pem 1024
openssl req -new -batch -out user.csr.pem -key user.key.pem -subj /CN=$HOSTNAME.eur/C=FR/ST=PACA/L=Aix/O=Eurecom/OU=CM
openssl ca -cert cacert.pem -keyfile cakey.pem -in user.csr.pem -out user.cert.pem -outdir . -batch

if [ ! -d /usr/local/etc/freeDiameter ]
then
    echo "Creating non existing directory: /usr/local/etc/freeDiameter/"
    sudo mkdir /usr/local/etc/freeDiameter/
fi

echo "Copying cakey.pem user.key.pem cacert.pem to /usr/local/etc/freeDiameter/"
sudo cp user.key.pem user.cert.pem cacert.pem /usr/local/etc/freeDiameter/

# openssl genrsa -out ubuntu.key.pem 1024
# openssl req -new -batch -x509 -out ubuntu.csr.pem -key ubuntu.key.pem -subj /CN=ubuntu.localdomain/C=FR/ST=BdR/L=Aix/O=fD/OU=Tests
# openssl ca -cert cacert.pem -keyfile cakey.pem -in ubuntu.csr.pem -out ubuntu.cert.pem -outdir . -batch
