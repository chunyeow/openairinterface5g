function out=rx_post(nom,ratio,phase)

%load data.dat signal
%fid=fopen('/tmp/store1.dat',)
fid=fopen(nom,'rb')
signal=fread(fid,20480*2,'float');
disp(signal(1:10));
signal=signal(1:2:length(signal))+j*signal(2:2:length(signal));

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
disp('Warning')
end;


% calculation of the signal power
Pu=sum(f(max_pos-20:max_pos+20))
ns_noise =round((band/Fs)*(size_data/2))
Pnoise=sum(f(max_pos-ns_noise:max_pos+ns_noise))-Pu
plot(10*log10(f))

I=real(signal);
Q=imag(signal);
Qcor=(sin(phase)*I+ratio*Q)/cos(phase);
comp=I+j*Qcor;
hold on
plot(20*log10(abs(fft(comp))),'g')

comp=comp(1:10000).*exp(-j*(pi/2+0.1)*(1:10000))';
figure
plot(real(comp(1:500)));
hold on
plot(imag(comp(1:500)),'g');
figure
plot(20*log10(abs(fft(comp))),'g')

%end;