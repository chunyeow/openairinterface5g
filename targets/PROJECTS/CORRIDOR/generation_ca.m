%% this script generates the signals for the CORRIDOR channel sounding campaing

addpath('../../../openair1/SIMULATION/LTE_PHY/')
%addpath('../../../openair1/PHY/LTE_ESTIMATION/')
addpath('../../../openair1/PHY/LTE_REFSIG/')
%addpath('../../../targets/ARCH/EXMIMO/USERSPACE/OCTAVE')

rand('seed',42); %make sure seed random numbers are alwyas the same

% load the LTE sync sequence
primary_synch;
nant = 4;

%% this generates one LTE frame (10ms) full of OFDM modulated random QPSK symbols
%% 20MHz carrier
nb_rb = 100; %this can be 25, 50, or 100
num_carriers = 2048/100*nb_rb;
num_zeros = num_carriers-(12*nb_rb+1);
prefix_length = num_carriers/4; %this is extended CP
num_symbols_frame = 120;
preamble_length = 120;

[s1,f1] = OFDM_TX_FRAME_MIMO(num_carriers,num_zeros,prefix_length,num_symbols_frame,preamble_length,nant);
% scale to conserve energy (Matlabs IFFT does not scale)
s1=s1*sqrt(num_carriers);

% upsample PSS to the right frequency and insert it in the first symbol of the frame

pss0_up = interp(primary_synch0_time,num_carriers/128);
pss0_up_cp = [pss0_up(num_carriers-prefix_length+1:end) pss0_up];

s1(:,1:num_carriers+prefix_length) = repmat(pss0_up_cp,nant,1);

%% 10MHz carrier
nb_rb = 50; %this can be 25, 50, or 100
num_carriers = 2048/100*nb_rb;
num_zeros = num_carriers-(12*nb_rb+1);
prefix_length = num_carriers/4; %this is extended CP

[s2,f2] = OFDM_TX_FRAME_MIMO(num_carriers,num_zeros,prefix_length,num_symbols_frame,preamble_length,nant);
% scale to conserve energy (Matlabs IFFT does not scale)
s2=s2*sqrt(num_carriers);

% upsample PSS to the right frequency and insert it in the first symbol of the frame

pss0_up = interp(primary_synch0_time,num_carriers/128);
pss0_up_cp = [pss0_up(num_carriers-prefix_length+1:end) pss0_up];

s2(:,1:num_carriers+prefix_length) = repmat(pss0_up_cp,nant,1);

%% 5MHz carrier
nb_rb = 25; %this can be 25, 50, or 100
num_carriers = 2048/100*nb_rb;
num_zeros = num_carriers-(12*nb_rb+1);
prefix_length = num_carriers/4; %this is extended CP

[s3,f3] = OFDM_TX_FRAME_MIMO(num_carriers,num_zeros,prefix_length,num_symbols_frame,preamble_length,nant);
% scale to conserve energy (Matlabs IFFT does not scale)
s3=s3*sqrt(num_carriers);

% upsample PSS to the right frequency and insert it in the first symbol of the frame

pss0_up = interp(primary_synch0_time,num_carriers/128);
pss0_up_cp = [pss0_up(num_carriers-prefix_length+1:end) pss0_up];

s3(:,1:num_carriers+prefix_length) = repmat(pss0_up_cp,nant,1);

%% combine the 10 and 20 MHz carriers
f1_shift = -5e6;
f2_shift = 10e6;
sample_rate = 30.72e6*2;
s = zeros(nant,sample_rate/100);
for a=1:nant
    s1_up = interp(s1(a,:),2);
    s1_shift = s1_up .* exp(2*1i*pi*f1_shift*(0:length(s1_up)-1)/sample_rate);
    s2_up = interp(s2(a,:),4);
    s2_shift = s2_up .* exp(2*1i*pi*f2_shift*(0:length(s2_up)-1)/sample_rate);
    s(a,:) = s1_shift + s2_shift/sqrt(2);
end

%%
figure(1)
hold off
plot(linspace(-sample_rate/2,sample_rate/2,length(s)),20*log10(abs(fftshift(fft(s,[],2)))))

%% save for later use (channel estimation and transmission with the SMBV)
save('ofdm_pilots_sync_30MHz.mat','-v7','s1','s2','s3','f1','f2','f3','num_carriers','num_zeros','prefix_length','num_symbols_frame','preamble_length');

s_all = sum(s,1);
s_all(1:5120) = s(1,1:5120);
mat2wv(s_all, 'ofdm_pilots_sync_30MHz.wv', sample_rate, 1);

s_all = sum(s3,1);
s_all(1:640) = s3(1,1:640);
mat2wv(s_all, 'ofdm_pilots_sync_5MHz.wv', 7.68e6, 1);

