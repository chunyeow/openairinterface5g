frequency = input('Input freq in MHz: ', 's'); 
% script to setup mxa and sign generator 
freq=str2num(frequency);
size_data=20480;
Fs=7680; %sampling freq in kHz
band = 1500; % noise band in kHz 
gpib_card=0;      % first GPIB PCI card in the computer
sme=28;   % this is configured in the signal generator Utilities->System->GPIB->Address menu (for SME)
cd ../CBMIMO1_TOOLS;
openair_init;
cd ../AGILE_RF_TOOLS;
%freq=1200; % normally loop
S_in=-100;
loss_cable=0.5 + freq/2000;
% first, calculation of IQ missmatch
[ratio,phi_est]=IQ_estimation(freq);

gpib_send(gpib_card,sme,'*RST;*CLS');   % reset and configure the signal generator
gpib_send(gpib_card,sme,strcat('POW ',int2str(S_in),'dBm'));
%disp('FREQ ',int2str(freq),'MHz');
gpib_send(gpib_card,sme,strcat('FREQ ',int2str(freq),'MHz'));
%disp('FREQ ',int2str(freq),'MHz');
%frequency = input('Input freq in MHz: ', 's'); 

cmd1='/homes/nussbaum/openair1/ARCH/LEON3/hostpc/user/tools/scripts/set_receiver -M ';
frequency=int2str(freq);
ALL_gain1=63;
ALL_gain2=63;

i=0;
for gain1=ALL_gain1
for gain2= ALL_gain2

i=i+1;
end;
end;
table=zeros(i,4);
i=1;

for gain1=ALL_gain1
for gain2=ALL_gain2
S_in_debug=-73 + floor((gain1+gain2)/2);
gpib_send(gpib_card,sme,strcat('POW ',int2str(S_in_debug),'dBm')); % this is for debug without the boards

cmd=strcat(cmd1,frequency,' -g1 ',int2str(gain1),' -g2 ',int2str(gain2));
system(cmd);
%system('set_receiver -M ' frequency ' -g1 ' int2str(gain1) ' -g2 ' int2str(gain2));
%system('sleep 1');
gpib_send(gpib_card,sme,'OUTP:STAT ON'); %  activate output 
sleep(.1);
table(i,1)=gain1;
table(i,2)=gain2;

s=oarf_get_frame(0);
%figure
%plot(20*log10(abs(fft(s(:,1)))),'g')
signal=s(:,1);
I=real(signal);
Q=imag(signal);
disp(phi_est);
Qcor=sin(phi_est)*I;

Qcor=(sin(phi_est)*I+ratio*Q)/cos(phi_est);
figure
plot(I(1:100))
hold on
plot(Qcor(1:100),'g');
comp=I+j*Qcor;
hold off
figure
plot(20*log10(abs(fft(comp))));
max_pos=0;
max=0;
f=abs(fft(signal)).^2;
for n=size_data/2:size_data
  if(f(n) > max)
    max=f(n);
    max_pos=n;
  end;
end;
disp("max_pos")
disp(max_pos)
freq_max=Fs/size_data*max_pos - Fs/2   % corresponding frequency
disp("freq_max")
disp(freq_max)
if(abs(freq_max-2000)>400) % probably and error occurs
disp("Warning")
Pu=0.1;
Pnoise=10000;
else
% calculation of the signal power
Pu=sum(f(max_pos-20:max_pos+20))
ns_noise =round((band/Fs)*(size_data/2))
Pnoise=sum(f(max_pos-ns_noise:max_pos+ns_noise))-Pu
end;


% plot(10*log10(f))

% plot(10*log10(f(max_pos-ns_noise:max_pos+ns_noise)))

SNR=10*log10(Pu/Pnoise)
N_in = -174 +10*log10(1000*band)
S_in = S_in-loss_cable;
SNR_in=S_in - N_in

NF=SNR_in-SNR
disp("gain_total")
gain_total=10*log10(Pu) - S_in

end;
end;
