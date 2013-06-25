load_channel = 1;

SNR=10^(SNRdB/10);
% MMSE regularization factor
mu = NTx/SNR;
% noise variance
n_p=1/SNR;

% open all files
for k=1:NUser
    fid(k) = fopen(filename_emos{avail_users(k),idx},'r');
    if (fid(k)==-1)
        error('[eval_data_MU] Error opening file %s', filename_emos{avail_users(k),idx});
    end
end


% read the data in chunks of 1000 Frames
NFrames_max = 1000;
NFrames_read = zeros(1,NUser);
NFrames_Tx = max(framestamp_max(:,idx))-min(framestamp_min(:,idx))+1;
NFreq = 40;

chunks = 0;
NFrames_Tx_read = 0; %Number of Tx frames read from file
NFrames_Tx_proc = 0; %Number of Tx frames processed (with all crc's positive)

align_matrix = false(NUser,NFrames_Tx);
for u = 1:NUser
    align_matrix(u,est(avail_users(u),idx).framestamp_tx - min(framestamp_min(avail_users,idx)) + 1) = true;
end

crc_matrix = false(NUser,NFrames_Tx);
for u = 1:NUser
    crc_matrix(u,est(avail_users(u),idx).framestamp_tx - min(framestamp_min(avail_users,idx)) + 1) = ~est(avail_users(u),idx).err_ind;
end

NFrames_pos = sum(all(crc_matrix));

CAP_SU_TDMA_ML = [];
CAP_SU1x1_TDMA_ML = [];
CAP_SU1x2_TDMA_ML = [];
CAP_SU2x2_TDMA_ML = [];
%CAP_SU_TDMA_DSTTD = [];
%CAP_SU_TDMA_CL_ML = [];

%CAP_4U_DPC = zeros(NFreq,NFrames_pos);

% CAP_4U_ZF = zeros(NFreq,NFrames_pos);
% CAP_4U_MMSE = zeros(NFreq,NFrames_pos);
% CAP_4U_AS_ZF = zeros(NFreq,NFrames_pos);
% CAP_4U_AS_MMSE = zeros(NFreq,NFrames_pos);
CAP_2U_ZF = zeros(NFreq,NFrames_pos);
CAP_2U_MMSE = zeros(NFreq,NFrames_pos);
CAP_2U_AS_ZF = zeros(NFreq,NFrames_pos);
CAP_2U_AS_MMSE = zeros(NFreq,NFrames_pos);
% CAP_2U_BD = zeros(NFreq,NFrames_pos);
% CAP_2U2Tx_ZF = zeros(NFreq,NFrames_pos);
% CAP_2U2Tx_MMSE = zeros(NFreq,NFrames_pos);

% Eigval_4U_ZF = zeros(NFreq,NFrames_pos,NUser);
 
% gain_4U_ZF = zeros(NFrames_pos,1);
% gain_4U_MMSE = zeros(NFrames_pos,1);
% gain_4U_AS_ZF = zeros(NFrames_pos,1);
% gain_4U_AS_MMSE = zeros(NFrames_pos,1);
gain_2U_ZF = zeros(NFrames_pos,1);
gain_2U_MMSE = zeros(NFrames_pos,1);
gain_2U_AS_ZF = zeros(NFrames_pos,1);
gain_2U_AS_MMSE = zeros(NFrames_pos,1);
% gain_2U_BD = zeros(NFrames_pos,2);

DProf = zeros(NUser, 256/NTx);

R = zeros(NUser*NTx*NRx);
R_Tx = zeros(NTx);
R_Tx_AS = zeros(NTx);

%% main loop
%while any(NFrames_read < NFrames(:,idx).')
%fk 30.1.2008: changed the following two line since it produced an endless loop
while any(NFrames_read < sum(align_matrix,2).')

    %Tx_frames = (NFrames_Tx_read+1) : min(NFrames_Tx_read + NFrames_max, NFrames_Tx);
    Tx_frames = (NFrames_Tx_read+1) : min(NFrames_Tx_read + NFrames_max,size(align_matrix,2));
    if IFversion>=2
        [H, H_fq, dummy_est, dummy_gps, NFrames_read] = load_estimates2_MU_lm_new(fid, NTx, length(Tx_frames), NFrames_read, align_matrix(:,Tx_frames), IFversion);
    else
        [H, H_fq, dummy_est, dummy_gps, NFrames_read] = load_estimates_MU_lm_new(fid, NTx, length(Tx_frames), NFrames_read, align_matrix(:,Tx_frames), IFversion);
    end
    if isempty(H_fq)
        warning('[eval_data_MU] No data read! Terminating loop.');
        break
    end
    NFrames_Tx_read = NFrames_Tx_read + size(H_fq,4);

    %use only measurements where all users have positive CRC
    H = H(:,:,:,all(crc_matrix(:,Tx_frames)),:);
    H_fq = H_fq(:,:,:,all(crc_matrix(:,Tx_frames)),:);
    
    chunks = chunks + 1;
    disp(['[eval_data_MU] Chunk = ' num2str(chunks) ', NFrames_read = ' num2str(NFrames_read) ', NFrames_Tx_read = ' num2str(NFrames_Tx_read) ]);
    
    % normalize H per user
    % the mean squared Frobenius norm has been precomputed in est(k,idx).Hnorm
    % in order to get E(|h_{i,j,k}|^2) = 1, we need 
    % size(H_fq) = [NRx,NTx,NFreq,NFrames,Nuser]
    for k=1:NUser
        H(:,:,:,:,k) = H(:,:,:,:,k)*sqrt(NTx*NRx)/sqrt(mean(est(avail_users(k),idx).Hnorm));
        H_fq(:,:,:,:,k) = H_fq(:,:,:,:,k)*sqrt(NTx*NRx)/sqrt(mean(est(avail_users(k),idx).Hnorm));
    end

    %H_fq_MU = cat(4,H_fq_MU,H_fq);
    %H_MU = cat(4,H_MU,H);

    %% calculate different capacities
    disp('[eval_data_MU] Calculating TDMA capacities');
    % CAP_MU_ZF   = cat(1,CAP_MU_ZF,capacity_MU_ZF(H_fq,SNR,0));
    CAP_SU_TDMA_ML = cat(1,CAP_SU_TDMA_ML,capacity_SU_TDMA_ML(H_fq,SNR,0));
    CAP_SU1x1_TDMA_ML = cat(1,CAP_SU1x1_TDMA_ML,capacity_SU_TDMA_ML(H_fq(1,1,:,:,:),SNR,0));
    CAP_SU1x2_TDMA_ML = cat(1,CAP_SU1x2_TDMA_ML,capacity_SU_TDMA_ML(H_fq(:,1,:,:,:),SNR,0));
    CAP_SU2x2_TDMA_ML = cat(1,CAP_SU2x2_TDMA_ML,capacity_SU_TDMA_ML(H_fq(:,1:2,:,:,:),SNR,0));
    % CAP_SU_TDMA_DSTTD = cat(1,CAP_SU_TDMA_DSTTD,capacity_SU_TDMA_DSTTD(H_fq,SNR,0));
    % CAP_SU_TDMA_CL_ML = cat(1,CAP_SU_TDMA_CL_ML,capacity_SU_TDMA_CL_ML(H_fq,SNR,0));


    disp('[eval_data_MU] Calculating SDMA capacities');
    for k2=1:size(H_fq,4)
        [HSf,antenna_index] = antenna_selection(H_fq(:,:,:,k2,:));
        for k1=1:size(H_fq,3)
%             % 4 Users, DPC
%             HS = conj(permute(squeeze(H_fq(:,:,k1,k2,:)),[2 1 3]));
%             CAP_4U_DPC(k1,NFrames_Tx_proc + k2) = iterative_waterfill(HS,SNR,50);
            
%             % 4 Users, Antenna selection
%             HS = squeeze(HSf(:,:,k1,:,:)).';
%             [CAP_4U_AS_MMSE(k1,NFrames_Tx_proc + k2), gain] = MISO_MMSE(HS,n_p,mu);
%             gain_4U_AS_MMSE(NFrames_Tx_proc + k2,:) = gain_4U_AS_MMSE(NFrames_Tx_proc + k2,:) + gain;
%             [CAP_4U_AS_ZF(k1,NFrames_Tx_proc + k2),gain] = MISO_MMSE(HS,n_p,0);
%             gain_4U_AS_ZF(NFrames_Tx_proc + k2,:) = gain_4U_AS_ZF(NFrames_Tx_proc + k2,:) + gain;

            % 2 Users, Antenna selection
            HS = squeeze(HSf(:,:,k1,:,sched_users)).';
            [CAP_2U_AS_MMSE(k1,NFrames_Tx_proc + k2), gain] = MISO_MMSE(HS,n_p,mu);
            gain_2U_AS_MMSE(NFrames_Tx_proc + k2,:) = gain_2U_AS_MMSE(NFrames_Tx_proc + k2,:) + gain;
            [CAP_2U_AS_ZF(k1,NFrames_Tx_proc + k2), gain] = MISO_MMSE(HS,n_p,0);
            gain_2U_AS_ZF(NFrames_Tx_proc + k2,:) = gain_2U_AS_ZF(NFrames_Tx_proc + k2,:) + gain;
            R_Tx_AS = R_Tx+HS'*HS;

%             % 4 Users, No Antenna selection
%             HS = squeeze(H_fq(1,:,k1,k2,:)).';
%             [CAP_4U_MMSE(k1,NFrames_Tx_proc + k2), gain] = MISO_MMSE(HS,n_p,mu);
%             gain_4U_MMSE(NFrames_Tx_proc + k2,:) = gain_4U_MMSE(NFrames_Tx_proc + k2,:) + gain;
%             [CAP_4U_ZF(k1,NFrames_Tx_proc + k2), gain, eigval] = MISO_MMSE(HS,n_p,0);
%             gain_4U_ZF(NFrames_Tx_proc + k2,:) = gain_4U_ZF(NFrames_Tx_proc + k2,:) + gain;
%             Eigval_4U_ZF(k1,k2,:) = eigval;
            
            % 2 Users, No Antenna selection
            HS = squeeze(H_fq(1,:,k1,k2,sched_users)).';
            [CAP_2U_MMSE(k1,NFrames_Tx_proc + k2), gain] = MISO_MMSE(HS,n_p,mu);
            gain_2U_MMSE(NFrames_Tx_proc + k2,:) = gain_2U_MMSE(NFrames_Tx_proc + k2,:) + gain;
            [CAP_2U_ZF(k1,NFrames_Tx_proc + k2), gain] = MISO_MMSE(HS,n_p,0);
            gain_2U_ZF(NFrames_Tx_proc + k2,:) = gain_2U_ZF(NFrames_Tx_proc + k2,:) + gain;
            R_Tx = R_Tx+HS'*HS;

%             % 2 Users, 2 Antennas, block diagonalization
%             HS = squeeze(H_fq(:,:,k1,k2,sched_users));
%             [CAP_2U_BD(k1,NFrames_Tx_proc + k2), gain] = BD(HS,n_p);
%             gain_2U_BD(NFrames_Tx_proc + k2,:) = gain_2U_BD(NFrames_Tx_proc + k2,:) + gain;

%             % 2 Users, 2 Transmit Antennas (cross polarized), Antenna selection
%             %HS = reshape(permute(H_fq(:,:,k1,k2,sched_users),[2 1 3 4 5]),NTx,NRx*length(sched_users));
%             HS = squeeze(HSf(:,1:2,k1,:,sched_users));
%             [CAP_2U2Tx_ZF(k1,NFrames_Tx_proc + k2), gain] = MISO_MMSE(HS,n_p,0);
%             [CAP_2U2Tx_MMSE(k1,NFrames_Tx_proc + k2), gain] = MISO_MMSE(HS,n_p,mu);
    
            % calculate the full correlation matrix
            % R(k+n*NUser+m*NUser*NTx,k'+n'*NUser+m'*NUser*NTx)
            % k .. User, n .. tx antenna, m .. Rx antenna
            HS = reshape(permute(H_fq(:,:,k1,k2,:),[5 2 3 4 1]),NTx*NRx*NUser,1);
            R = R+HS*HS';
        end

%         gain_4U_AS_MMSE(NFrames_Tx_proc + k2,:) = gain_4U_AS_MMSE(NFrames_Tx_proc + k2,:) / NFreq;
%         gain_4U_AS_ZF(NFrames_Tx_proc + k2,:) = gain_4U_AS_ZF(NFrames_Tx_proc + k2,:) / NFreq;
        gain_2U_AS_MMSE(NFrames_Tx_proc + k2,:) = gain_2U_AS_MMSE(NFrames_Tx_proc + k2,:) / NFreq;
        gain_2U_AS_ZF(NFrames_Tx_proc + k2,:) = gain_2U_AS_ZF(NFrames_Tx_proc + k2,:) / NFreq;
%         gain_4U_MMSE(NFrames_Tx_proc + k2,:) = gain_4U_MMSE(NFrames_Tx_proc + k2,:) / NFreq;
%         gain_4U_ZF(NFrames_Tx_proc + k2,:) = gain_4U_ZF(NFrames_Tx_proc + k2,:) / NFreq;
        gain_2U_MMSE(NFrames_Tx_proc + k2,:) = gain_2U_MMSE(NFrames_Tx_proc + k2,:) / NFreq;
        gain_2U_ZF(NFrames_Tx_proc + k2,:) = gain_2U_ZF(NFrames_Tx_proc + k2,:) / NFreq;
%         gain_2U_BD(NFrames_Tx_proc + k2,:) = gain_2U_BD(NFrames_Tx_proc + k2,:) / NFreq;
        
    end
    

    %% calculate and plot the PDP and frequency response over time
    % The function Dprofile requires that size(H)=[N_t,N_tau,N_Tx,N_Rx]
    % but load estimates returns size(H1) = [NRx,NTx,NTau,NFrames]
    disp('[eval_data_MU] PDP Calculation');
    for k=1:NUser
        DProf(k,:) = DProf(k,:) + DProfile(permute(squeeze(H(:,:,:,:,k)),[4 3 2 1]));
    end
%     if any(any(isnan(DProf)))
%         keyboard
%     end
    
    NFrames_Tx_proc = NFrames_Tx_proc + size(H_fq,4);
    disp(['[eval_data_MU] Chunk = ' num2str(chunks), ', NFrames_Tx_proc = ' num2str(NFrames_Tx_proc) ]);

end

% close all files
for k=1:NUser
    fclose(fid(k));
end

% if NFrames_Tx_read < NFrames_Tx
%     CAP_4U_ZF(:,NFrames_Tx_read+1:end) = [];
%     CAP_4U_MMSE(:,NFrames_Tx_read+1:end) = [];
%     CAP_2U_ZF(:,NFrames_Tx_read+1:end) = [];
%     CAP_2U_MMSE(:,NFrames_Tx_read+1:end) = [];    
% end

% % post processing
% [Cap_4U_SUTS_f,Cap_4U_SUTS_x] = ecdf(CAP_MU_SUTS(:));
% [Cap_4U_ZF_f,Cap_4U_ZF_x] = ecdf(CAP_4U_ZF(:));
% [Cap_4U_MMSE_f,Cap_4U_MMSE_x] = ecdf(CAP_4U_MMSE(:));
% [Cap_2U_ZF_f,Cap_2U_ZF_x] = ecdf(CAP_2U_ZF(:));
% [Cap_2U_MMSE_f,Cap_2U_MMSE_x] = ecdf(CAP_2U_MMSE(:));

DProf = DProf ./ chunks;
R = R ./ (NFrames_Tx_proc*NFreq);
R_Tx = R_Tx ./ (NFrames_Tx_proc*NFreq);
R_Tx_AS = R_Tx_AS ./ (NFrames_Tx_proc*NFreq);

%% save calcualted data
filename_mat = fullfile(filepath,sprintf('results_v2_idx_%d.mat',idx));
if strfind(version,'7.3')
    save(filename_mat,'-V7','CAP*',...
        'gain*',...
        'DProf','R*');
else
    save(filename_mat,'CAP*',...
        'gain*',...
        'DProf','R*');
end


