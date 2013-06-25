clear all
system("source /opt/intel/cc/10.0.023/bin/iccvars.sh")

system("./main_sensing");

fd = fopen("rxsig0.dat","r");

temp = fread(fd,20480*2,"int16");
fclose(fd);

rxs = temp(1:2:length(temp)) + sqrt(-1)*temp(2:2:length(temp));

figure(2)
plot(20*log10(abs(fftshift(fft(rxs)))))

