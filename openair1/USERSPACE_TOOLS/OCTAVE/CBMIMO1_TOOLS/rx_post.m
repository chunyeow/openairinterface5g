function out=rx_post(nom)

%load data.dat signal
%fid=fopen('/tmp/store1.dat',)
fid=fopen(nom,'rb')
signal=fread(fid,20480);

size_data=20480;
Fs=7680; %in kHz
band = 2000; %in kHz 
max_pos=0;
max=0;
f=abs(fft(signal)).^2;
for n=2:size_data
  if(f(n) > max)
    max=f(n);
    max_pos=n;
  end;
end;

disp(max_pos)
freq_max=Fs/size_data*max_pos - Fs/2   % corresponding frequency
if(abs(freq_max-2000)>100)
disp("Warning")
end;


% calculation of the signal power
Pu=sum(f(max_pos-20:max_pos+20))
ns_noise =round((band/Fs)*(size_data/2))
Pnoise=sum(f(max_pos-ns_noise:max_pos+ns_noise))-Pu
plot(10*log10(f))

% plot(10*log10(f(max_pos-ns_noise:max_pos+ns_noise)))

SNR=10*log10(Pu/Pnoise)
N_in = -174 +10*log10(1000*band)
S_in = -110;
SNR_in=S_in - N_in

NF=SNR_in-SNR
end;