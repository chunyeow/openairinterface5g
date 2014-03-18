close all
clear all

%% read data
fid = fopen('E:\EMOS\corridor\eNB_data_20140108_185919.EMOS','r');
[v,c]=fread(fid, 76800*4*100, 'int16',0,'ieee-le');
v2 = double(v(1:2:end))+1j*double(v(2:2:end));
fclose(fid);

figure(1)
plot(abs(fftshift(fft(v2))))

%% sync
addpath('../../../openair1/PHY/LTE_REFSIG');
primary_synch; %loads the primary sync signal

pss_t = upsample(primary_synch0_time,4*4);
[corr,lag] = xcorr(v2,pss_t);
figure(2);
plot(lag,abs(corr));

%% frame start detection
%load('../LOCALIZATION/ofdm_pilots_sync_2048.mat');
load('E:\EMOS\corridor\ofdm_pilots_sync_2048_v7.mat');
ofdm_symbol_length = num_carriers + prefix_length;
frame_length = ofdm_symbol_length*num_symbols_frame;
useful_carriers = num_carriers-num_zeros-1;

% TODO: do a proper peak detection. for now just take the max
[m,i]=max(abs(corr));
frame_start = lag(i) - prefix_length;

%% ofdm receiver
received = v2(frame_start:frame_start+frame_length);
received_f = zeros(num_symbols_frame,useful_carriers);
for i=0:num_symbols_frame-1;
    ifblock=received(i*ofdm_symbol_length+[1:ofdm_symbol_length]);
    ifblock(1:prefix_length)=[];
    fblock=fft(ifblock);
    received_f(i+1,:) = [fblock(2:useful_carriers/2+1); fblock(end-useful_carriers/2+1:end)];
end

%% channel estimation
H=conj(f).*received_f;
Ht = ifft(H,[],2);
PDP = mean(abs(Ht(2:end,:).^2),1);
figure(3)
surf(20*log10(abs(Ht)))
xlabel('time [OFDM symbol]')
ylabel('delay time [samples]')
zlabel('power [dB]')
shading interp
figure(4)
plot(10*log10(PDP))
xlabel('delay time [samples]')
ylabel('power [dB]')


