clear all
close all

%%
%file_indices = 39:60;
file_indices = [82:85 87:90 100 1 2];
%file_indices = [32 50 51];

load('metadata_MU.mat')

Ecap = zeros(max(file_indices),14);
Mcap = zeros(max(file_indices),14);

for idx = file_indices
    disp(sprintf('[eval_data_MU] Processing file %d of %d',idx, length(file_indices)));

    filename_mat = sprintf('results_v2_idx_%d.mat',idx);

    load(filename_mat)

    align_matrix = repmat(-1,4,max(framestamp_max(:,idx)) - min(framestamp_min(:,idx)) + 1);
    for u = 1:4
        align_matrix(u,est(u,idx).framestamp_tx - min(framestamp_min(:,idx)) + 1) = est(u,idx).err_ind;
    end
    NFrames_Tx_proc = sum(all(align_matrix==0));

    Ecap(idx,1) = mean(CAP_SU1x1_TDMA_ML(:));
    Mcap(idx,1) = median(CAP_SU1x1_TDMA_ML(:));
    Ecap(idx,2) = mean(CAP_SU1x2_TDMA_ML(:));
    Mcap(idx,2) = median(CAP_SU1x2_TDMA_ML(:));
    Ecap(idx,3) = mean(CAP_SU2x2_TDMA_ML(:));
    Mcap(idx,3) = median(CAP_SU2x2_TDMA_ML(:));
    Ecap(idx,4) = mean(CAP_SU_TDMA_DSTTD(:));
    Mcap(idx,4) = median(CAP_SU_TDMA_DSTTD(:));
    Ecap(idx,5) = mean(reshape(CAP_2U_ZF(:,1:NFrames_Tx_proc),1,[]));
    Mcap(idx,5) = median(reshape(CAP_2U_ZF(:,1:NFrames_Tx_proc),1,[]));
    Ecap(idx,6) = mean(reshape(CAP_4U_ZF(:,1:NFrames_Tx_proc),1,[]));
    Mcap(idx,6) = median(reshape(CAP_4U_ZF(:,1:NFrames_Tx_proc),1,[]));
    Ecap(idx,7) = mean(reshape(CAP_2U_AS_ZF(:,1:NFrames_Tx_proc),1,[]));
    Mcap(idx,7) = median(reshape(CAP_2U_AS_ZF(:,1:NFrames_Tx_proc),1,[]));
    Ecap(idx,8) = mean(reshape(CAP_4U_AS_ZF(:,1:NFrames_Tx_proc),1,[]));
    Mcap(idx,8) = median(reshape(CAP_4U_AS_ZF(:,1:NFrames_Tx_proc),1,[]));
    Ecap(idx,9) = mean(reshape(CAP_2U_MMSE(:,1:NFrames_Tx_proc),1,[]));
    Mcap(idx,9) = median(reshape(CAP_2U_MMSE(:,1:NFrames_Tx_proc),1,[]));
    Ecap(idx,10) = mean(reshape(CAP_4U_MMSE(:,1:NFrames_Tx_proc),1,[]));
    Mcap(idx,10) = median(reshape(CAP_4U_MMSE(:,1:NFrames_Tx_proc),1,[]));
    Ecap(idx,11) = mean(reshape(CAP_2U_AS_MMSE(:,1:NFrames_Tx_proc),1,[]));
    Mcap(idx,11) = median(reshape(CAP_2U_AS_MMSE(:,1:NFrames_Tx_proc),1,[]));
    Ecap(idx,12) = mean(reshape(CAP_2U2Tx_MMSE(:,1:NFrames_Tx_proc),1,[])); 
    Mcap(idx,12) = median(reshape(CAP_2U2Tx_MMSE(:,1:NFrames_Tx_proc),1,[])); 
    Ecap(idx,13) = mean(reshape(CAP_4U_AS_MMSE(:,1:NFrames_Tx_proc),1,[])); 
    Mcap(idx,13) = median(reshape(CAP_4U_AS_MMSE(:,1:NFrames_Tx_proc),1,[])); 
    %Ecap(idx,14) = mean(reshape(CAP_2U2Tx_ZF(:,1:NFrames_Tx_proc),1,[]));
    %Mcap(idx,14) = median(reshape(CAP_2U2Tx_ZF(:,1:NFrames_Tx_proc),1,[]));

end

save Ecap Ecap Mcap
textable('Ecap_table.tex',[file_indices.' Ecap(file_indices,:)]);
textable('Mcap_table.tex',[file_indices.' Mcap(file_indices,:)]);