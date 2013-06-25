bit_vec = [4 6 12]

% filename_codebook = 'Codebook.mat'; %random codebook
% load(fullfile('~/Devel/openair1/EMOS/MATLAB/lim_feedback',filename_codebook));
% filestr = 'RVQ'

filename_codebook = sprintf('Codebook_idx_new%d.mat',idx); %random correlated codebook
if ~exist(filename_codebook,'file')
    disp('[eval_data_MU] Generating Codebook')
    generate_corr_codebook
else
    load(filename_codebook);
end
filestr = 'RVQ_corr'

% % grassmanian codebook
% filename_codebook = sprintf('codebook_%d_%dvec.mat',NTx,pow2(bit_vec)); 
% load(fullfile('~/Devel/openair1/EMOS/MATLAB/lim_feedback',filename_codebook));
% Codebook = repmat(eval(sprintf('codebook_%d_%dvec',NTx,pow2(bit_vec))),[1,1,4]);
% filestr = 'Grassmanian'

timestamp = datestr(now);
timestamp = strrep(timestamp,' ','_');

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

%DProf = zeros(NUser, NTx, 64);

%CAP_4U_ZF_CVQ = zeros(NFreq,NFrames_pos,length(bit_vec));
%CAP_4U_MMSE_CVQ = zeros(NFreq,NFrames_pos,length(bit_vec));
CAP_4U_ZF_RVQ = zeros(NFreq,NFrames_pos,length(bit_vec));
CAP_4U_MMSE_RVQ = zeros(NFreq,NFrames_pos,length(bit_vec));
CAP_SU_CVQ = zeros(NFreq,NFrames_pos,length(bit_vec),NUser);

Codebook_ind = zeros(NFreq,NFrames_pos,length(bit_vec),NUser);

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
        H(:,:,:,:,k) = H(:,:,:,:,k)*sqrt(NTx*NRx)/sqrt(mean(est(sched_users(k),idx).Hnorm));
        H_fq(:,:,:,:,k) = H_fq(:,:,:,:,k)*sqrt(NTx*NRx)/sqrt(mean(est(sched_users(k),idx).Hnorm));
    end

    disp('[eval_data_MU] Calculating SDMA capacities');
    for k2=1:size(H_fq,4)
        %[HSf,antenna_index] = antenna_selection(H_fq(:,:,:,k2,:));
        for k1=1:size(H_fq,3)
            HS = squeeze(H_fq(1,:,k1,k2,:));
            Codebook_ind(k1,NFrames_Tx_proc + k2,:,:) = get_codebook_idx(HS,Codebook(:,1:pow2(max(bit_vec)),1:NUser),bit_vec);
            for b = 1:length(bit_vec)

%                 [Hq, Codebook_ind(k1,NFrames_Tx_proc + k2,b,:)] = quantize_H(HS,bit_vec(b));
% 
%                 [CAP_4U_MMSE_CVQ(k1,NFrames_Tx_proc + k2,b)] = MISO_CVQ(HS.',Hq.',n_p,mu);
%                 [CAP_4U_ZF_CVQ(k1,NFrames_Tx_proc + k2,b)] = MISO_CVQ(HS.',Hq.',n_p,0);

                Hq = zeros(NTx,NUser);
                for k=1:NUser
                    Hq(:,k) = Codebook(:,Codebook_ind(k1,NFrames_Tx_proc + k2,b,k),k);
                    % Hq has to have the same amplitude as H
                    Hq(:,k) = Hq(:,k).*norm(HS(:,k));
                    CAP_SU_CVQ(k1,NFrames_Tx_proc + k2,b,k) = log2(1+SNR^2*abs(Hq(:,k)'*HS(:,k))^2);
                end

                [CAP_4U_MMSE_RVQ(k1,NFrames_Tx_proc + k2,b)] = MISO_RVQ(HS.',Hq.',n_p,mu);
                [CAP_4U_ZF_RVQ(k1,NFrames_Tx_proc + k2,b)] = MISO_RVQ(HS.',Hq.',n_p,0);
            end
        end
    end

%     %% calculate and plot the PDP and frequency response over time
%     % The function Dprofile requires that size(H)=[N_t,N_tau,N_Tx,N_Rx]
%     % but load estimates returns size(H1) = [NRx,NTx,NTau,NFrames]
%     disp('[eval_data_MU] PDP Calculation');
%     for k=1:NUser
%         for l=1:NTx
%             DProf(k,l,:) = DProf(k,l,:) + reshape(DProfile(squeeze(permute(H(:,l,:,:,k),[4 3 2 1 5]))),1,1,[]);
%         end
%     end
    

    NFrames_Tx_proc = NFrames_Tx_proc + size(H_fq,4);
    disp(['[eval_data_MU] Chunk = ' num2str(chunks), ', NFrames_Tx_proc = ' num2str(NFrames_Tx_proc) ]);
%    sendmail('kaltenbe@eurecom.fr',['[eval_data_MU] Chunk = ' num2str(chunks), ', NFrames_Tx_proc = ' num2str(NFrames_Tx_proc) ],'');

    %% save calcualted data
    filename_mat = fullfile(filepath,sprintf('results_%s_%dbit_SNR_%d_idx_%d_%s.mat',filestr,bit_vec(1),SNRdB,idx,timestamp));
    if strfind(version,'7.3')
        save(filename_mat,'-V7','CAP*','bit_vec','Codebook_ind');
    else
        save(filename_mat,'CAP*','bit_vec','Codebook_ind');
    end
    
end

% close all files
for k=1:NUser
    fclose(fid(k));
end

%DProf = DProf ./ chunks;

% %% save calcualted data
% filename_mat = fullfile(filepath,sprintf('results_RVQ_corr_SNR_%d_idx_%d.mat',SNRdB,idx));
% if strfind(version,'7.3')
%     save(filename_mat,'-V7','CAP*','bit_vec','Codebook_ind');
% else
%     save(filename_mat,'CAP*','bit_vec','Codebook_ind');
% end