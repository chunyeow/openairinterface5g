filename_mat = sprintf('results_v2_idx_%d.mat',idx);
%filename_CVQ_mat = sprintf('results_CVQ_idx_%d.mat',idx);
filename_CVQ_mat = sprintf('results_CVQ_SNR_%d_idx_%d_31-Mar-2008_09:22:50.mat',SNRdB,idx);

%filename_RVQ_mat = sprintf('results_RVQ_SNR_%d_idx_%d_28-Mar-2008_16:43:58.mat',SNRdB,idx);
%filename_RVQ_corr_mat = sprintf('results_RVQ_corr_SNR_%d_idx_%d_28-Mar-2008_16:43:16',SNRdB,idx);
filename_RVQ_mat = sprintf('results_RVQ_SNR_%d_idx_%d_22-Jul-2008_15:56:48.mat',SNRdB,idx);
filename_RVQ_corr_mat = sprintf('results_RVQ_corr_SNR_%d_idx_%d_22-Jul-2008_15:35:38.mat',SNRdB,idx);

filename_SU_TDMA_mat = sprintf('results_SU_TDMA_SNR_%d_idx_%d.mat',SNRdB,idx);

%%
% d = dir(filename_CVQ_mat);
% [m,i] = max([d.datenum]);
% filename_CVQ_mat = d(i).name;
% d = dir(filename_RVQ_mat);
% [m,i] = max([d.datenum]);
% filename_RVQ_mat = d(i).name;
% d = dir(filename_RVQ_corr_mat);
% [m,i] = max([d.datenum]);
% filename_RVQ_corr_mat = d(i).name;

logscale = 0;

%%
    h_fig = figure(3);
    align_matrix = repmat(-1,4,max(framestamp_max(:,idx)) - min(framestamp_min(:,idx)) + 1);
    for u = 1:4
        align_matrix(u,est(u,idx).framestamp_tx - min(framestamp_min(:,idx)) + 1) = est(u,idx).err_ind;
    end

    map = [1 1 1; ...
           0 1 0; ...
           1 0 0];

    image(align_matrix+2); % image values have to be in range 1:m
    colormap(map);
    set(gca,'YTickLabel',{'','User 1','','User 2','','User 3','','User 4'},...
        'YTick',[0.5 1 1.5 2 2.5 3 3.5 4],...
        'XTick',zeros(1,0));
    %pos = get(h_fig,'Position');
    set(h_fig,'Position',[360 504 560 104])
    %pos = get(h_fig,'PaperPosition');
    set(h_fig,'PaperPosition',[0.25 2.5 8 1.5])
    filename_align = fullfile(filepath_figs,sprintf('idx_%d_align.eps',idx));
    saveas(h_fig, filename_align, 'epsc2');

%%    
if exist(filename_mat,'file')
%%
    load(filename_mat)
    load(filename_CVQ_mat)
    load(filename_RVQ_mat,'CAP_4U_ZF_RVQ','CAP_4U_MMSE_RVQ');
    RVQ_corr = load(filename_RVQ_corr_mat,'CAP_4U_ZF_RVQ','CAP_4U_MMSE_RVQ');
    load(filename_SU_TDMA_mat,'CAP_SU4x1_TDMA_ML','CAP_SU_TDMA_DSTTD','CAP_SU4x1_TDMA_CL_ML')

%%
    % post processing
    NFrames_Tx_proc = sum(all(align_matrix==0));
    [Cap_SU4x1_TDMA_ML_f,Cap_SU4x1_TDMA_ML_x] = ecdf(CAP_SU4x1_TDMA_ML(:));
    [Cap_SU4x1_TDMA_CL_ML_f,Cap_SU4x1_TDMA_CL_ML_x] = ecdf(CAP_SU4x1_TDMA_CL_ML(:));
    [Cap_SU_TDMA_DSTTD_f,Cap_SU_TDMA_DSTTD_x] = ecdf(CAP_SU_TDMA_DSTTD(:));
    [Cap_4U_ZF_f,Cap_4U_ZF_x] = ecdf(reshape(CAP_4U_ZF(:,1:NFrames_Tx_proc),1,[]));
    [Cap_4U_MMSE_f,Cap_4U_MMSE_x] = ecdf(reshape(CAP_4U_MMSE(:,1:NFrames_Tx_proc),1,[]));
    [Cap_4U_ZF_CVQ_4bits_f,Cap_4U_ZF_CVQ_4bits_x] = ecdf(reshape(CAP_4U_ZF_CVQ(:,1:NFrames_Tx_proc,1),1,[]));
    [Cap_4U_MMSE_CVQ_4bits_f,Cap_4U_MMSE_CVQ_4bits_x] = ecdf(reshape(CAP_4U_MMSE_CVQ(:,1:NFrames_Tx_proc,1),1,[]));
    [Cap_4U_ZF_CVQ_12bits_f,Cap_4U_ZF_CVQ_12bits_x] = ecdf(reshape(CAP_4U_ZF_CVQ(:,1:NFrames_Tx_proc,3),1,[]));
    [Cap_4U_MMSE_CVQ_12bits_f,Cap_4U_MMSE_CVQ_12bits_x] = ecdf(reshape(CAP_4U_MMSE_CVQ(:,1:NFrames_Tx_proc,3),1,[]));
    [Cap_4U_ZF_RVQ_12bits_f,Cap_4U_ZF_RVQ_12bits_x] = ecdf(reshape(CAP_4U_ZF_RVQ(:,1:NFrames_Tx_proc,3),1,[]));
    [Cap_4U_MMSE_RVQ_12bits_f,Cap_4U_MMSE_RVQ_12bits_x] = ecdf(reshape(CAP_4U_MMSE_RVQ(:,1:NFrames_Tx_proc,3),1,[]));
    [Cap_4U_ZF_RVQ_corr_12bits_f,Cap_4U_ZF_RVQ_corr_12bits_x] = ecdf(reshape(RVQ_corr.CAP_4U_ZF_RVQ(:,1:NFrames_Tx_proc,3),1,[]));
    [Cap_4U_MMSE_RVQ_corr_12bits_f,Cap_4U_MMSE_RVQ_corr_12bits_x] = ecdf(reshape(RVQ_corr.CAP_4U_MMSE_RVQ(:,1:NFrames_Tx_proc,3),1,[]));

    % use the following color coding
    % ZF = blue, MMSE = red, SUTS = black, BD = cyan, MMSE AS = magenta, 
    % 4U = '-', 2U = '--'
    % measured = 2pt, iid = 1pt

    
%% SU-MIMO TDMA vs 4U MU-MIMO MMSE (measured only)
    h_fig = figure(5);
    %load('cap_iid.mat')

    subplot(1,2,2)
    hold off
    plot(Cap_SU4x1_TDMA_CL_ML_x,Cap_SU4x1_TDMA_CL_ML_f,'k','Linewidth',1)
    hold on
    plot(Cap_SU4x1_TDMA_ML_x,Cap_SU4x1_TDMA_ML_f,'k--','Linewidth',1)
    plot(Cap_4U_MMSE_CVQ_12bits_x,Cap_4U_MMSE_CVQ_12bits_f,'r--','Linewidth',2)
    plot(Cap_4U_MMSE_RVQ_corr_12bits_x,Cap_4U_MMSE_RVQ_corr_12bits_f,'r:','Linewidth',2)

    [legend_h,object_h,plot_h,text_strings] = legend('SU-MISO perfect feedback',...
        'SU-MISO no feedback',...
        'MU-MIMO 12 bit DFT codebook',...
        'MU-MIMO 12 bit random corr codebook',...
        'Location','SouthOutside');
    set(legend_h,'FontSize',8);

    title('(b)')
    xlabel('bits/sec/Hz')
    xlim([0 12])
    set(gca,'xtick',0:2:12);
    ylabel('Prob(capacity < x-axis)')
    if logscale
        set(gca,'yscale','log')
        ylim([1e-2 1])
    end
    grid on
   
    subplot(1,2,1)
    hold off
%    plot(Cap_4U_ZF_x,Cap_4U_ZF_f,'b','Linewidth',2)
    plot(Cap_4U_MMSE_x,Cap_4U_MMSE_f,'r','Linewidth',2)
    hold on

%     plot(Cap_4U_ZF_CVQ_4bits_x,Cap_4U_ZF_CVQ_4bits_f,'b:','Linewidth',2)
%     plot(Cap_4U_MMSE_CVQ_4bits_x,Cap_4U_MMSE_CVQ_4bits_f,'r:','Linewidth',2)

%    plot(Cap_4U_ZF_CVQ_12bits_x,Cap_4U_ZF_CVQ_12bits_f,'b--','Linewidth',2)
    plot(Cap_4U_MMSE_CVQ_12bits_x,Cap_4U_MMSE_CVQ_12bits_f,'r--','Linewidth',2)

%    plot(Cap_4U_ZF_RVQ_12bits_x,Cap_4U_ZF_RVQ_12bits_f,'b-.','Linewidth',2)
    plot(Cap_4U_MMSE_RVQ_12bits_x,Cap_4U_MMSE_RVQ_12bits_f,'r-.','Linewidth',2)

%    plot(Cap_4U_ZF_RVQ_corr_12bits_x,Cap_4U_ZF_RVQ_corr_12bits_f,'b:','Linewidth',2)
    plot(Cap_4U_MMSE_RVQ_corr_12bits_x,Cap_4U_MMSE_RVQ_corr_12bits_f,'r:','Linewidth',2)
    
    [legend_h,object_h,plot_h,text_strings] = legend('MU-MIMO perfect feedback',...
        'MU-MIMO 12 bit DFT codebook',...
        'MU-MIMO 12 bit random codebook',...
        'MU-MIMO 12 bit random corr codebook',...
        'Location','SouthOutside');
    set(legend_h,'Fontsize',8);
    
    title('(a)')
    xlabel('bits/sec/Hz')
    xlim([0 12])
    set(gca,'xtick',0:2:12);
    ylabel('Prob(capacity < x-axis)')
    if logscale
        set(gca,'yscale','log')
        ylim([1e-2 1])
    end
    grid on
    filename_cap = fullfile(filepath_figs,sprintf('idx_%d_SNR_%d_capacity_CVQ_MMSE_4U.eps',idx,SNRdB));
    saveas(h_fig, filename_cap, 'epsc2');
    filename_cap_pdf = fullfile(filepath_figs,sprintf('idx_%d_SNR_%d_capacity_CVQ_MMSE_4U.pdf',idx,SNRdB));
    system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));

%% SU-MIMO TDMA vs 4U MU-MIMO ZF  (measured only)
    h_fig = figure(6);
    %load('cap_iid.mat')
    hold off

    plot(Cap_SU4x1_TDMA_CL_ML_x,Cap_SU4x1_TDMA_CL_ML_f,'k','Linewidth',1)
    hold on
    plot(Cap_SU4x1_TDMA_ML_x,Cap_SU4x1_TDMA_ML_f,'k--','Linewidth',1)

    plot(Cap_4U_ZF_x,Cap_4U_ZF_f,'b','Linewidth',2)
%    plot(Cap_4U_MMSE_x,Cap_4U_MMSE_f,'r','Linewidth',2)

%     plot(Cap_4U_ZF_CVQ_4bits_x,Cap_4U_ZF_CVQ_4bits_f,'b:','Linewidth',2)
%     plot(Cap_4U_MMSE_CVQ_4bits_x,Cap_4U_MMSE_CVQ_4bits_f,'r:','Linewidth',2)

    plot(Cap_4U_ZF_CVQ_12bits_x,Cap_4U_ZF_CVQ_12bits_f,'b--','Linewidth',2)
%    plot(Cap_4U_MMSE_CVQ_12bits_x,Cap_4U_MMSE_CVQ_12bits_f,'r--','Linewidth',2)

    plot(Cap_4U_ZF_RVQ_12bits_x,Cap_4U_ZF_RVQ_12bits_f,'b-.','Linewidth',2)
%    plot(Cap_4U_MMSE_RVQ_12bits_x,Cap_4U_MMSE_RVQ_12bits_f,'r-.','Linewidth',2)

    plot(Cap_4U_ZF_RVQ_corr_12bits_x,Cap_4U_ZF_RVQ_corr_12bits_f,'b:','Linewidth',2)
%    plot(Cap_4U_MMSE_RVQ_corr_12bits_x,Cap_4U_MMSE_RVQ_corr_12bits_f,'r:','Linewidth',2)
    
    legend('SU-MISO TDMA perfect feedback','SU-MISO TDMA no feedback',...
        'MU-MIMO ZF perfect feedback',...
        'MU-MIMO ZF 12 bit DFT codebook',...
        'MU-MIMO ZF 12 bit random codebook',...
        'MU-MIMO ZF 12 bit random corr codebook',...
        'Location','SouthEast')
    %title(sprintf('SU-MIMO vs. MU-MIMO, 4 Users, SNR = %d',SNRdB),'Fontsize',14,'Fontweight','bold')
    xlabel('bits/sec/Hz')
    xlim([0 16])
    ylabel('Prob(capacity < x-axis)')
    if logscale
        set(gca,'yscale','log')
        ylim([1e-2 1])
    end
    grid on
    filename_cap = fullfile(filepath_figs,sprintf('idx_%d_SNR_%d_capacity_CVQ_ZF_4U.eps',idx,SNRdB));
    saveas(h_fig, filename_cap, 'epsc2');
    filename_cap_pdf = fullfile(filepath_figs,sprintf('idx_%d_SNR_%d_capacity_CVQ_ZF_4U.pdf',idx,SNRdB));
    system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));
    
%%
end