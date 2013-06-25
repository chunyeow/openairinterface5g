load_channel = 1;

SNR=10^(SNRdB/10);
% MMSE regularization factor
mu = NTx/SNR;
% noise variance
n_p=1/SNR;

% open all files
for k=1:NUser
    fid(k) = fopen(filename_emos{k,idx},'r');
    if (fid(k)==-1)
        error('[eval_data_MU] Error opening file %s', filename_emos{k,idx});
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
    align_matrix(u,est(u,idx).framestamp_tx - min(framestamp_min(:,idx)) + 1) = true;
end

crc_matrix = false(NUser,NFrames_Tx);
for u = 1:NUser
    crc_matrix(u,est(u,idx).framestamp_tx - min(framestamp_min(:,idx)) + 1) = ~est(u,idx).err_ind;
end

NFrames_pos = sum(all(crc_matrix));

CAP_SU4x1_TDMA_ML = [];
CAP_SU_TDMA_DSTTD = [];
CAP_SU4x1_TDMA_CL_ML = [];

%while any(NFrames_read < NFrames(:,idx).')
%fk 30.1.2008: changed the following two line since it produced an endless loop
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

    %% calculate different capacities
    disp('[eval_data_MU] Calculating TDMA capacities');
    % CAP_MU_ZF   = cat(1,CAP_MU_ZF,capacity_MU_ZF(H_fq,SNR,0));
    CAP_SU4x1_TDMA_ML = cat(1,CAP_SU4x1_TDMA_ML,capacity_SU_TDMA_ML(H_fq(1,:,:,:,:),SNR,0));
    CAP_SU_TDMA_DSTTD = cat(1,CAP_SU_TDMA_DSTTD,capacity_SU_TDMA_DSTTD(H_fq,SNR,0));
    CAP_SU4x1_TDMA_CL_ML = cat(1,CAP_SU4x1_TDMA_CL_ML,capacity_SU_TDMA_CL_ML(H_fq(1,:,:,:,:),SNR,0));


    NFrames_Tx_proc = NFrames_Tx_proc + size(H_fq,4);
    disp(['[eval_data_MU] Chunk = ' num2str(chunks), ', NFrames_Tx_proc = ' num2str(NFrames_Tx_proc) ]);

end

% close all files
for k=1:NUser
    fclose(fid(k));
end

%% save calcualted data
filename_mat = fullfile(filepath,sprintf('results_SU_TDMA_SNR_%d_idx_%d.mat',SNRdB,idx));
if strfind(version,'7.3')
    save(filename_mat,'-V7','CAP*')
else
    save(filename_mat,'CAP*')
end


