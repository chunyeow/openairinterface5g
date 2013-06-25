% this script with DPC, MMSE and SU MIMO takes about 10h for one SNR on the
% cluster.

load_channel = 1;

% open all files
for k=1:NUser
    fid(k) = fopen(filename_emos{k,idx},'r');
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
    align_matrix(u,est(u,idx).framestamp_tx - min(framestamp_min(:,idx)) + 1) = true;
end

crc_matrix = false(NUser,NFrames_Tx);
for u = 1:NUser
    crc_matrix(u,est(u,idx).framestamp_tx - min(framestamp_min(:,idx)) + 1) = ~est(u,idx).err_ind;
end

NFrames_pos = sum(all(crc_matrix));

NSNR = length(SNRdB);

% CAP_SU_TDMA_ML = [];
% CAP_SU_TDMA_MMSE = [];
% CAP_SU_TDMA_CL_ML = [];

% CAP_4U_4x2_DPC = zeros(NFreq,NFrames_Tx);
% CAP_4U_4x1_DPC = zeros(NFreq,NFrames_pos,NSNR);
% CAP_4U_ZF = zeros(NFreq,NFrames_Tx);
% CAP_4U_MMSE = zeros(NFreq,NFrames_Tx,NSNR);
% CAP_4U_AS_ZF = zeros(NFreq,NFrames_Tx);
% CAP_4U_AS_MMSE = zeros(NFreq,NFrames_Tx);
CAP_2U_ZF = zeros(NFreq,NFrames_Tx,NSNR);
CAP_2U_MMSE = zeros(NFreq,NFrames_Tx,NSNR);
% CAP_2U_AS_ZF = zeros(NFreq,NFrames_Tx);
% CAP_2U_AS_MMSE = zeros(NFreq,NFrames_Tx);
% CAP_2U_BD_WF = zeros(NFreq,NFrames_Tx);
% CAP_2U_BD_EP = zeros(NFreq,NFrames_Tx);

% Eigval_4U_ZF = zeros(NFreq,NFrames_Tx,NTx);

% gain_4U_ZF = zeros(NFrames_Tx,1);
% gain_4U_MMSE = zeros(NFrames_Tx,1);
% gain_4U_AS_ZF = zeros(NFrames_Tx,1);
% gain_4U_AS_MMSE = zeros(NFrames_Tx,1);
% gain_2U_ZF = zeros(NFrames_Tx,1);
% gain_2U_MMSE = zeros(NFrames_Tx,1);
% gain_2U_AS_ZF = zeros(NFrames_Tx,1);
% gain_2U_AS_MMSE = zeros(NFrames_Tx,1);
% gain_2U_BD_WF = zeros(NFrames_Tx,2);
% gain_2U_BD_EP = zeros(NFrames_Tx,2);


while any(NFrames_read < sum(align_matrix,2).')

    %Tx_frames = (NFrames_Tx_read+1) : min(NFrames_Tx_read + NFrames_max, NFrames_Tx);
    Tx_frames = (NFrames_Tx_read+1) : min(NFrames_Tx_read + NFrames_max,size(align_matrix,2));
    [H, H_fq, dummy_est, dummy_gps, NFrames_read] = load_estimates_MU_lm_new(fid, NTx, length(Tx_frames), NFrames_read, align_matrix(:,Tx_frames), IFversion);
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
        H(:,:,:,:,k) = H(:,:,:,:,k)*sqrt(NTx*NRx)/sqrt(mean(est(k,idx).Hnorm));
        H_fq(:,:,:,:,k) = H_fq(:,:,:,:,k)*sqrt(NTx*NRx)/sqrt(mean(est(k,idx).Hnorm));
    end

    %H_fq_MU = cat(4,H_fq_MU,H_fq);
    %H_MU = cat(4,H_MU,H);

    % %% calculate different capacities
    % CAP_MU_ZF   = cat(1,CAP_MU_ZF,capacity_MU_ZF(H_fq,SNR,0));
    % CAP_4U_SUTS = cat(1,CAP_4U_SUTS,capacity_MU_SUTS(H_fq,SNR,0));
    % CAP_SU_TDMA_CL_ML = cat(1,CAP_SU_TDMA_CL_ML,capacity_SU_TDMA_CL_ML(H_fq(1,:,:,:,:),10.^(SNRdB./10),0));

     for k2=1:size(H_fq,4)
        % [HSf,antenna_index] = antenna_selection(H_fq(:,:,:,k2,:));
        for k1=1:size(H_fq,3)
            %HSUl = conj(permute(squeeze(H_fq(:,:,k1,k2,:)),[2 1 3]));
            HS = squeeze(H_fq(1,:,k1,k2,:)).';
            for k0=1:length(SNRdB)
                
                SNR=10^(SNRdB(k0)/10);
                % MMSE regularization factor
                mu = NTx/SNR;
                % noise variance
                n_p=1/SNR;

                % 4 Users, DPC
                % CAP_4U_4x2_DPC(k1,NFrames_Tx_read + k2) = iterative_waterfill(HSUl,SNR,50);
                % CAP_4U_4x1_DPC(k1,NFrames_Tx_proc + k2,k0) = iterative_waterfill(HSUl(:,1,:),SNR,50);

                % 4 Users, Antenna selection
                % HS = squeeze(HSf(:,:,k1,:,:)).';
                % [CAP_4U_AS_MMSE(k1,NFrames_Tx_read + k2), gain] = MISO_MMSE(HS,n_p,mu);
                % gain_4U_AS_MMSE(NFrames_Tx_read + k2,:) = gain_4U_AS_MMSE(NFrames_Tx_read + k2,:) + gain;
                % [CAP_4U_AS_ZF(k1,NFrames_Tx_read + k2),gain] = MISO_MMSE(HS,n_p,0);
                % gain_4U_AS_ZF(NFrames_Tx_read + k2,:) = gain_4U_AS_ZF(NFrames_Tx_read + k2,:) + gain;

                % 2 Users, Antenna selection
                % HS = squeeze(HSf(:,:,k1,:,sched_users)).';
                % [CAP_2U_AS_MMSE(k1,NFrames_Tx_read + k2), gain] = MISO_MMSE(HS,n_p,mu);
                % gain_2U_AS_MMSE(NFrames_Tx_read + k2,:) = gain_2U_AS_MMSE(NFrames_Tx_read + k2,:) + gain;
                % [CAP_2U_AS_ZF(k1,NFrames_Tx_read + k2), gain] = MISO_MMSE(HS,n_p,0);
                % gain_2U_AS_ZF(NFrames_Tx_read + k2,:) = gain_2U_AS_ZF(NFrames_Tx_read + k2,:) + gain;

                % 4 Users, No Antenna selection
                % HS = squeeze(H_fq(1,:,k1,k2,:)).';
                % [CAP_4U_MMSE(k1,NFrames_Tx_read + k2,k0), gain] = MISO_MMSE(HS,n_p,mu);
                % gain_4U_MMSE(NFrames_Tx_read + k2,:) = gain_4U_MMSE(NFrames_Tx_read + k2,:) + gain;
                % [CAP_4U_ZF(k1,NFrames_Tx_read + k2), gain,eigval] = MISO_MMSE(HS,n_p,0);
                % gain_4U_ZF(NFrames_Tx_read + k2,:) = gain_4U_ZF(NFrames_Tx_read + k2,:) + gain;
                % Eigval_4U_ZF(k1,NFrames_Tx_read + k2,:) = eigval;

                % 2 Users, No Antenna selection
                HS = squeeze(H_fq(1,:,k1,k2,sched_users)).';
                [CAP_2U_MMSE(k1,NFrames_Tx_read + k2, k0), gain] = MISO_MMSE(HS,n_p,mu);
                % gain_2U_MMSE(NFrames_Tx_read + k2,:) = gain_2U_MMSE(NFrames_Tx_read + k2,:) + gain;
                [CAP_2U_ZF(k1,NFrames_Tx_read + k2, k0), gain] = MISO_MMSE(HS,n_p,0);
                % gain_2U_ZF(NFrames_Tx_read + k2,:) = gain_2U_ZF(NFrames_Tx_read + k2,:) + gain;

                % 2 Users, block diagonalization
                % HS = squeeze(H_fq(:,:,k1,k2,sched_users));
                % [CAP_2U_BD_EP(k1,NFrames_Tx_read + k2), gain] = BD(HS,n_p,0);
                % gain_2U_BD_EP(NFrames_Tx_read + k2,:) = gain_2U_BD_EP(NFrames_Tx_read + k2,:) + gain;
                % [CAP_2U_BD_WF(k1,NFrames_Tx_read + k2), gain] = BD(HS,n_p,1);
                % gain_2U_BD_WF(NFrames_Tx_read + k2,:) = gain_2U_BD_EP(NFrames_Tx_read + k2,:) + gain;

            end
        end

        % gain_4U_AS_MMSE(NFrames_Tx_read + k2,:) = gain_4U_AS_MMSE(NFrames_Tx_read + k2,:) / NFreq;
        % gain_4U_AS_ZF(NFrames_Tx_read + k2,:) = gain_4U_AS_ZF(NFrames_Tx_read + k2,:) / NFreq;
        % gain_2U_AS_MMSE(NFrames_Tx_read + k2,:) = gain_2U_AS_MMSE(NFrames_Tx_read + k2,:) / NFreq;
        % gain_2U_AS_ZF(NFrames_Tx_read + k2,:) = gain_2U_AS_ZF(NFrames_Tx_read + k2,:) / NFreq;
        % gain_4U_MMSE(NFrames_Tx_read + k2,:) = gain_4U_MMSE(NFrames_Tx_read + k2,:) / NFreq;
        % gain_4U_ZF(NFrames_Tx_read + k2,:) = gain_4U_ZF(NFrames_Tx_read + k2,:) / NFreq;
        % gain_2U_MMSE(NFrames_Tx_read + k2,:) = gain_2U_MMSE(NFrames_Tx_read + k2,:) / NFreq;
        % gain_2U_ZF(NFrames_Tx_read + k2,:) = gain_2U_ZF(NFrames_Tx_read + k2,:) / NFreq;
        % gain_2U_BD_WF(NFrames_Tx_read + k2,:) = gain_2U_BD_WF(NFrames_Tx_read + k2,:) / NFreq;
        % gain_2U_BD_EP(NFrames_Tx_read + k2,:) = gain_2U_BD_EP(NFrames_Tx_read + k2,:) / NFreq;

    end
    
    NFrames_Tx_proc = NFrames_Tx_proc + size(H_fq,4);
    disp(['[eval_data_MU] Chunk = ' num2str(chunks), ', NFrames_Tx_proc = ' num2str(NFrames_Tx_proc) ]);

    %     %% calculate and plot the PDP and frequency response over time
    %     % The function Dprofile requires that size(H)=[N_t,N_tau,N_Tx,N_Rx]
    %     % but load estimates returns size(H1) = [NRx,NTx,NTau,NFrames]
    %     disp('[eval_data_MU] PDP Calculation');
    %     for k=1:NUser
    %         DProf(k,:) = DProf(k,:) + DProfile(permute(squeeze(H(:,:,:,:,k)),[4 3 2 1]));
    %     end

    %% save calcualted data
    filename_mat = fullfile(filepath,sprintf('results_multiSNR_4x1_idx_%d.mat',idx));
    if strfind(version,'7.3')
        save(filename_mat,'-V7','CAP*','SNRdB');
        %'gain*');
    else
        save(filename_mat,'CAP*','SNRdB');
        %'gain*');
    end

end

% close all files
for k=1:NUser
    fclose(fid(k));
end



