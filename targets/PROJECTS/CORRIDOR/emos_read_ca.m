close all
clear all
nant1 = 1;
nant2 = 2;
samples_slot1 = 7680*2;
samples_slot2 = 7680;
samples_slot_agg = nant1*samples_slot1 + nant2*samples_slot2;
nframes = 10;
slots_per_frame = 20;

%% read data
fid = fopen('E:\EMOS\corridor\eNB_data_20140311_173205.EMOS','r');
[v,c]=fread(fid, 2*samples_slot_agg*slots_per_frame*nframes, 'int16',0,'ieee-le'); 
fclose(fid);
v0 = double(v(1:2:end,:))+1j*double(v(2:2:end,:));

%%
v1 = zeros(samples_slot1*slots_per_frame*nframes,nant1);
v2 = zeros(samples_slot1*slots_per_frame*nframes,nant2);
for slot=1:slots_per_frame*nframes
    v1((slot-1)*samples_slot1+1:slot*samples_slot1,1) = v0((slot-1)*samples_slot_agg+1:slot*samples_slot_agg-samples_slot2*nant2,1);
    v2((slot-1)*samples_slot2+1:slot*samples_slot2,1) = v0((slot-1)*samples_slot_agg+samples_slot1+1:slot*samples_slot_agg-samples_slot2,1);
    v2((slot-1)*samples_slot2+1:slot*samples_slot2,2) = v0((slot-1)*samples_slot_agg+samples_slot1+samples_slot2+1:slot*samples_slot_agg,1);
end

%%
figure(1)
plot(20*log10(abs(fftshift(fft(v1)))))
figure(2)
plot(20*log10(abs(fftshift(fft(v2)))))


%% sync
addpath('../../../openair1/PHY/LTE_REFSIG');
primary_synch; %loads the primary sync signal

pss_t = upsample(primary_synch0_time,4*2);
[corr,lag] = xcorr(v2(:,1),pss_t);
figure(3);
plot(lag,abs(corr));

%% frame start detection
load('E:\EMOS\corridor\ofdm_pilots_sync_30MHz.mat');
ofdm_symbol_length = num_carriers + prefix_length;
frame_length = ofdm_symbol_length*num_symbols_frame;
useful_carriers = num_carriers-num_zeros-1;

% TODO: do a proper peak detection. for now just take the max
[m,i]=max(abs(corr));
frame_start = lag(i) - prefix_length;

%% ofdm receiver
received = v2(frame_start:frame_start+frame_length,1);
received_f = zeros(num_symbols_frame,useful_carriers);
for i=0:num_symbols_frame-1;
    ifblock=received(i*ofdm_symbol_length+[1:ofdm_symbol_length]);
    ifblock(1:prefix_length)=[];
    fblock=fft(ifblock);
    received_f(i+1,:) = [fblock(2:useful_carriers/2+1); fblock(end-useful_carriers/2+1:end)];
end

%% channel estimation
H=conj(f2).*received_f;
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


