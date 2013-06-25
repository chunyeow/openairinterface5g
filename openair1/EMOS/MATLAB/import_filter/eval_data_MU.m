%% parameters and initialization
close all
clear all

addpath('import_filter')

% uncomment this for measurement 1
% filenames = {'meas1/data_term1_idx31_20071126_153610.EMOS',...
%     'meas1/data_term2_idx31_20071126_153327.EMOS',...
%     'meas1/data_term3_idx31_20071126_153548.EMOS',...
%     'meas1/data_term4_idx31_20071126_153535.EMOS'};
% IFversion = 0.3;
% NUser = 4;
% NTx = 4;
% NRx = 2;

% uncomment this for measurement 2
filenames = {'meas2/data_term1_idx51_20080213_153009.EMOS',...
    'meas2/data_term2_idx51_20080213_105524.EMOS',...
    'meas2/data_term3_idx51_20080213_155013.EMOS',...
    'meas2/data_term4_idx51_20080213_105607.EMOS'};
IFversion = 0.4;
NUser = 4;
NTx = 4;
NRx = 2;

% uncomment this for measurement 4
% filenames = {'meas4/data_term1_idx4_20080618_150244.EMOS',...
%     'meas4/data_term2_idx4_20080618_145746.EMOS'};
% IFversion = 0.4;
% NUser = 2;
% NTx = 2;
% NRx = 2;

NFrames_read = zeros(1,NUser);
NFrames_Tx_read = 0; %Number of Tx frames read from file
NFrames_Tx_proc = 0; %Number of Tx frames processed (with all crc's positive)
NFrames_max = 200;   %Size of a chunk
chunks = 0;

% load the metadata
[estimates, gps_data, NFrames, align_matrix, crc_matrix] = load_estimates_MU_no_channel(filenames, NTx);

NFrames_pos = sum(all(crc_matrix));
NChunks = ceil(NFrames_pos/NFrames_max);
DProf = zeros(NChunks, NRx, NTx, NUser, 256/NTx);

R = zeros(NChunks, NUser*NTx*NRx, NUser*NTx*NRx);
Rice = zeros(NChunks,NUser);


% open all files
for k=1:NUser
    fid(k) = fopen(filenames{k},'r');
    if (fid(k)==-1)
        error('[eval_data_MU] Error opening file %s', filenames{k});
    end
end

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
        H(:,:,:,:,k) = H(:,:,:,:,k)*sqrt(NTx*NRx)/sqrt(mean(estimates(k).Hnorm));
        H_fq(:,:,:,:,k) = H_fq(:,:,:,:,k)*sqrt(NTx*NRx)/sqrt(mean(estimates(k).Hnorm));
    end

    % do whatever you like with H or H_fq
    keyboard

    NFrames_Tx_proc = NFrames_Tx_proc + size(H_fq,4);
    disp(['[eval_data_MU] Chunk = ' num2str(chunks), ', NFrames_Tx_proc = ' num2str(NFrames_Tx_proc) ]);
end

% close all files
for k=1:NUser
    fclose(fid(k));
end

