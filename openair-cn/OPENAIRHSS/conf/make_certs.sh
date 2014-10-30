################################################################################
#    OpenAirInterface
#    Copyright(c) 1999 - 2014 Eurecom
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
#   included in this distribution in the file called "COPYING". If not,
#   see <http://www.gnu.org/licenses/>.
#
#  Contact Information
#  OpenAirInterface Admin: openair_admin@eurecom.fr
#  OpenAirInterface Tech : openair_tech@eurecom.fr
#  OpenAirInterface Dev  : openair4g-devel@eurecom.fr
#
#  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France.
#
################################################################################
rm -rf demoCA
mkdir demoCA
echo 01 > demoCA/serial
touch demoCA/index.txt

echo "Creating certificate for HSS"
# 
# # CA self certificate
# openssl req  -new -batch -x509 -days 3650 -nodes -newkey rsa:1024 -out cacert.pem -keyout cakey.pem -subj /CN=test.fr/C=FR/ST=Biot/L=Aix/O=test.fr/OU=mobiles
# 
# openssl genrsa -out hss.key.pem 1024
# openssl req -new -batch -out hss.csr.pem -key hss.key.pem -subj /CN=hss.test.fr/C=FR/ST=Biot/L=Aix/O=test.fr/OU=mobiles
# openssl ca -cert cacert.pem -keyfile cakey.pem -in hss.csr.pem -out hss.cert.pem -outdir . -batch

# Create a Root Certification Authority Certificate
openssl req  -new -batch -x509 -days 3650 -nodes -newkey rsa:1024 -out cacert.pem -keyout cakey.pem -subj /CN=eur/C=FR/ST=PACA/L=Aix/O=Eurecom/OU=CM

# Generate a Private Key
openssl genrsa -out hss.key.pem 1024

# Generate a CSR (Certificate Signing Request) that will be self-signed
openssl req -new -batch -out hss.csr.pem -key hss.key.pem -subj /CN=hss.pft/C=FR/ST=PACA/L=Aix/O=Eurecom/OU=CM

# Certification authority
openssl ca -cert cacert.pem -keyfile cakey.pem -in hss.csr.pem -out hss.cert.pem -outdir . -batch

# openssl genrsa -out $hss.key.pem 1024
# openssl req -new -batch -out $hss.csr.pem -key $hss.key.pem -subj /CN=$hss.test.fr/C=FR/ST=Biot/L=Aix/O=test.fr/OU=mobiles
# openssl ca -cert cacert.pem -keyfile cakey.pem -in $hss.csr.pem -out $hss.cert.pem -outdir . -batch
