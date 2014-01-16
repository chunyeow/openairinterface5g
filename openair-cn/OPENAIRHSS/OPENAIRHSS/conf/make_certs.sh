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

# CA self certificate
openssl req  -new -batch -x509 -days 3650 -nodes -newkey rsa:1024 -out cacert.pem -keyout cakey.pem -subj /CN=eur/C=FR/ST=PACA/L=Aix/O=Eurecom/OU=CM

openssl genrsa -out hss.key.pem 1024
openssl req -new -batch -out hss.csr.pem -key hss.key.pem -subj /CN=hss.eur/C=FR/ST=PACA/L=Aix/O=Eurecom/OU=CM
openssl ca -cert cacert.pem -keyfile cakey.pem -in hss.csr.pem -out hss.cert.pem -outdir . -batch

# openssl genrsa -out $hss.key.pem 1024
# openssl req -new -batch -out $hss.csr.pem -key $hss.key.pem -subj /CN=$hss.test.fr/C=FR/ST=Biot/L=Aix/O=test.fr/OU=mobiles
# openssl ca -cert cacert.pem -keyfile cakey.pem -in $hss.csr.pem -out $hss.cert.pem -outdir . -batch
