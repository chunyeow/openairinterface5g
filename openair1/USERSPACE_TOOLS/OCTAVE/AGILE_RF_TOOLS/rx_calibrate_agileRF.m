%frequency = input('Input freq in MHz: ', 's'); 
% script to setup mxa and sign generator 

size_data=20480;
Fs=7680; %sampling freq in kHz
band = 2500; % noise band in kHz 
gpib_card=0;      % first GPIB PCI card in the computer
sme=28;   % this is configured in the signal generator Utilities->System->GPIB->Address menu (for SME)

openair_init;
freq=2000; % normally loop

% first, calculation of IQ missmatch
[ratio,phase]=IQ_estimation(freq);


gpib_send(gpib_card,sme,'*RST;*CLS');   % reset and configure the signal generator

gpib_send(gpib_card,sme,'POW -100dBm');
%disp('FREQ ',int2str(freq),'MHz');
gpib_send(gpib_card,sme,strcat('FREQ ',int2str(freq),'MHz'));
%disp('FREQ ',int2str(freq),'MHz');
%frequency = input('Input freq in MHz: ', 's'); 

cmd1='/homes/nussbaum/open_freya/arch/openair_CardBus_MIMO1/LEON3/hostpc/user/tools/scripts/set_receiver -M ';
frequency=int2str(freq);


ALL_gain1=0:16:3;
ALL_gain2=0:16:3;

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
cmd=strcat(cmd1,frequency,' -g1 ',int2str(gain1),' -g2 ',int2str(gain2));
system(cmd);
%system('set_receiver -M ' frequency ' -g1 ' int2str(gain1) ' -g2 ' int2str(gain2));
%system('sleep 1');
gpib_send(gpib_card,sme,'OUTP:STAT ON'); %  activate output 
sleep(.1);
table(i,1)=gain1;
table(i,2)=gain2;

s=oarf_get_frame(0);
plot(20*log10(abs(fft(s(:,1)))))
signal=s(:,1);

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
S_in = -100;
SNR_in=S_in - N_in

NF=SNR_in-SNR



i=i+1;
end;
end;
disp(table);

P_num =100; %(warning, to be updated)



% Post processing of the table started here
Pmin = -40; % in dBm
Pmax = 20;  % in dBm



% a Tx calibration file is calculated for a given board and frequency

%fid=fopen(['/tmp/cal_TX' frequency '_' int2str(n) '.dat'],'wb')
%fwrite(fid,signal);
%fclose(fid);
