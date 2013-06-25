clear all
close all

addpath('../IMPORT_FILTER');

% [estimates_eNB,NFrames_eNB]=load_estimates_lte_new('E:\EMOS\recip_exmimo2\eNB_data_20130327_163635.EMOS',inf,1,1);
% [estimates_UE,NFrames_UE]=load_estimates_lte_new('E:\EMOS\recip_exmimo2\UE_data_20130327_163721.EMOS',inf,1,0);

[estimates_eNB,NFrames_eNB]=load_estimates_lte_new('E:\EMOS\recip_exmimo2\eNB_data_20130328_114006.EMOS',inf,1,1);
[estimates_UE,NFrames_UE]=load_estimates_lte_new('E:\EMOS\recip_exmimo2\UE_data_20130328_115128.EMOS',inf,1,0);

%load eNB_estimates2
%load UE_estimates2

% use r3531 for measuremnts taken march 2013
struct_template_new

%%
NFrames = 1000;
decimation = 1;
NFrames_eNB = size(estimates_eNB,2);
NFrames_UE = size(estimates_UE,2);

H_eNB = complex(zeros(floor(NFrames/decimation),size(fifo_dump_emos_struct_eNb.channel,1)/2,size(fifo_dump_emos_struct_eNb.channel,2)));
H_UE = complex(zeros(floor(NFrames/decimation),size(fifo_dump_emos_struct_UE.channel,1)/2,size(fifo_dump_emos_struct_UE.channel,2),size(fifo_dump_emos_struct_UE.channel,3)));

% find the indices to align the eNB and the UE data
frames_eNB = [estimates_eNB.frame_tx]; %frame counter at eNB
frames_UE = [estimates_UE.frame_rx]; %frame counter at UE
%frame0 = frames_UE(3);
%idx0_eNB = find(frames_eNB==frame0);
%idx0_UE = find(frames_UE==frame0);
idx0_eNB = 1;
idx0_UE = 4354;

%%
for k=1:NFrames %-max(idx0_eNB,idx0_UE)
    %disp([frame0+k-1 frames_eNB(idx0_eNB+k-1) frames_UE(idx0_UE+k-1)]);
    H_eNB(k,:,:) = double(estimates_eNB(idx0_eNB+k-1).channel(1:2:end,:)) + 1j*double(estimates_eNB(idx0_eNB+k-1).channel(2:2:end,:));
    H_UE(k,:,:,:) = double(estimates_UE(idx0_UE+k-1).channel(1:2:end,:,:)) + 1j*double(estimates_UE(idx0_UE+k-1).channel(2:2:end,:,:));
end

% size(H_eNB) = [NFrames,N_RB_UL_EMOS*12*N_PILOTS_UL_EMOS*N_SUBFRAMES_UL_EMOS,NB_ANTENNAS_RX_EMOS]
% where N_RB_UL_EMOS*12 = 25*12 = 300 is the number of usefull subcarriers
%       N_PILOTS_UL_EMOS = 2 is the number of OFDM symbols per subframe that have pilots
%       N_SUBFRAMES_UL_EMOS = 3 is the number of UL subframes per frame* 

% size(H_UE) =
% [NFrames,NUMBER_OF_OFDM_CARRIERS_EMOS*N_PILOTS_DL_EMOS*N_SUBFRAMES_DL_EMOS,NB_ANTENNAS_TX_EMOS]
% where NUMBER_OF_OFDM_CARRIERS_EMOS = 512 is the number of OFDM carrier
%       (out of which only the first 300 are non-zero)
%       N_PILOTS_DL_EMOS = 4 is the number of OFDM carriers that have pilots 
%       (linear interpolation in the frequency domain has already been
%       performed)
%       N_SUBFRAMES_DL_EMOS = 5 is the number of DL subframes per frame*

%       NB_ANTENNAS_TX_EMOS = NB_ANTENNAS_RX =2 is the number of antennas at the eNB
%       (the UE has one antenna for TX/RX)

% *here we are using TDD configuration 3 which looks like this (S = special subframe)
% [DL | S | UL | UL | UL | DL | DL | DL | DL | DL]


%%
figure(1)
hold off
%plot(idx0_eNB:idx0_eNB+NFrames_eNB-1,[estimates_eNB.frame_tx]-1024*4)
%hold on
plot(idx0_UE:idx0_UE+NFrames_UE-1,[estimates_UE.frame_rx],'r')

%% 
txant = 1;
rxant = 1;
frames = 201:210;

%H_UE2 = reshape(permute(H_UE(frames,2049:2560,:),[2 1 3]),512,[],2);
%H_UE2 = reshape(permute(H_UE(:,2561:3072,:),[2 1 3]),512,[],2);
H_UE2 = reshape(permute(H_UE(:,1:512,:),[2 1 3]),512,NFrames,2,2);
H_UE3 = complex(zeros(size(H_UE2)));
H_UE3(363:512,:,:,:) = H_UE2(6:155,:,:,:); 
H_UE3(1:151,:,:,:) = H_UE2(156:306,:,:,:);
H_UE_t = ifft(conj(H_UE3),512,1); %the channel estimates are stored in conjugated format
H_UE_t = circshift(H_UE_t,[-15,0,0]); %shift the peak to the desired position
figure(2)
hold off
%waterfall(squeeze(20*log10(abs(H_UE_t))).')
plot(squeeze(20*log10(abs(H_UE_t(:,frames,txant,rxant)))))
title('PDP DL channel')

