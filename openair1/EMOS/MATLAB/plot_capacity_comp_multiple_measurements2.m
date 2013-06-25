close all
clear all

%file_indices = [32 50 51];
%file_indices = [27 28 32 33 50 51 52];

files = {'/extras/kaltenbe/EMOS/data/20071126_MU_Outdoor/results_v2_idx_32.mat', ...
    '/extras/kaltenbe/EMOS/data/20080205_MU_Indoor_EN_EC/results_v2_idx_100.mat',...
    '/extras/kaltenbe/EMOS/data/20080213_MU_Outdoor/results_v2_idx_52.mat'};
files_SU = {'/extras/kaltenbe/EMOS/data/20071126_MU_Outdoor/results_SU_TDMA_SNR_10_idx_32.mat', ...
    '/extras/kaltenbe/EMOS/data/20080205_MU_Indoor_EN_EC/results_SU_TDMA_SNR_10_idx_100.mat',...
    '/extras/kaltenbe/EMOS/data/20080213_MU_Outdoor/results_SU_TDMA_SNR_10_idx_52.mat'};
files_DPC = {'/extras/kaltenbe/EMOS/data/20071126_MU_Outdoor/results_DPC_idx_32.mat', ...
    '/extras/kaltenbe/EMOS/data/20080205_MU_Indoor_EN_EC/results_DPC_4x1_idx_100.mat',...
    '/extras/kaltenbe/EMOS/data/20080213_MU_Outdoor/results_DPC_4x1_idx_52.mat'};
files_RVQ_corr_4bit = {'/extras/kaltenbe/EMOS/data/20071126_MU_Outdoor/results_RVQ_corr_4bit_SNR_10_idx_32_06-Nov-2008_11:45:07.mat', ...
    '/extras/kaltenbe/EMOS/data/20080205_MU_Indoor_EN_EC/results_RVQ_corr_4bit_SNR_10_idx_100_06-Nov-2008_11:44:35.mat',...
    '/extras/kaltenbe/EMOS/data/20080213_MU_Outdoor/results_RVQ_corr_4bit_SNR_10_idx_52_06-Nov-2008_11:43:07.mat'};
files_Grassmanian_4bit = {'/extras/kaltenbe/EMOS/data/20071126_MU_Outdoor/results_Grassmanian_4bit_SNR_10_idx_32_05-Nov-2008_17:57:10.mat', ...
    '/extras/kaltenbe/EMOS/data/20080205_MU_Indoor_EN_EC/results_Grassmanian_4bit_SNR_10_idx_100_05-Nov-2008_17:40:46.mat',...
    '/extras/kaltenbe/EMOS/data/20080213_MU_Outdoor/results_Grassmanian_4bit_SNR_10_idx_52_05-Nov-2008_17:38:36.mat'};
files_RVQ_corr_6bit = {'/extras/kaltenbe/EMOS/data/20071126_MU_Outdoor/results_RVQ_corr_6bit_SNR_10_idx_32_06-Nov-2008_11:45:26.mat', ...
    '/extras/kaltenbe/EMOS/data/20080205_MU_Indoor_EN_EC/results_RVQ_corr_6bit_SNR_10_idx_100_06-Nov-2008_11:44:20.mat',...
    '/extras/kaltenbe/EMOS/data/20080213_MU_Outdoor/results_RVQ_corr_6bit_SNR_10_idx_52_06-Nov-2008_11:43:52.mat'};
files_Grassmanian_6bit = {'/extras/kaltenbe/EMOS/data/20071126_MU_Outdoor/results_Grassmanian_6bit_SNR_10_idx_32_05-Nov-2008_17:56:34.mat', ...
    '/extras/kaltenbe/EMOS/data/20080205_MU_Indoor_EN_EC/results_Grassmanian_6bit_SNR_10_idx_100_05-Nov-2008_17:41:19.mat',...
    '/extras/kaltenbe/EMOS/data/20080213_MU_Outdoor/results_Grassmanian_6bit_SNR_10_idx_52_05-Nov-2008_17:36:10.mat'};
files_RVQ_corr_12bit = {'/extras/kaltenbe/EMOS/data/20071126_MU_Outdoor/results_RVQ_corr_12bit_SNR_10_idx_32_18-Nov-2008_14:33:36.mat',...
    '/extras/kaltenbe/EMOS/data/20080205_MU_Indoor_EN_EC/results_RVQ_corr_12bit_SNR_10_idx_100_19-Nov-2008_10:47:08.mat',...
    '/extras/kaltenbe/EMOS/data/20080213_MU_Outdoor/results_RVQ_corr_12bit_SNR_10_idx_52_19-Nov-2008_10:46:59.mat'};
files_CVQ = {'/extras/kaltenbe/EMOS/data/20071126_MU_Outdoor/results_CVQ_SNR_10_idx_32_13-Mar-2008_21:55:09.mat', ...
    '/extras/kaltenbe/EMOS/data/20080205_MU_Indoor_EN_EC/results_CVQ_SNR_10_idx_100_13-Mar-2008_09:33:57.mat',...
    '/extras/kaltenbe/EMOS/data/20080213_MU_Outdoor/results_CVQ_SNR_10_idx_52_07-Nov-2008_16:56:21.mat'};
files_RVQ = {'/extras/kaltenbe/EMOS/data/20071126_MU_Outdoor/results_RVQ_4bit_SNR_10_idx_32_21-Nov-2008_17:25:43.mat', ...
    '/extras/kaltenbe/EMOS/data/20080205_MU_Indoor_EN_EC/results_RVQ_4bit_SNR_10_idx_100_21-Nov-2008_17:24:58.mat',...
    '/extras/kaltenbe/EMOS/data/20080213_MU_Outdoor/results_RVQ_corr_4bit_SNR_10_idx_50_21-Nov-2008_17:37:43.mat'};


NIdx = length(files);
SNR = 10;

plot_style_ZF = {'b-','b--','b:'};
plot_style_MMSE = {'r-','r--','r:','r-.'};


for idx = 1 %:NIdx
    disp(sprintf('[eval_data_MU] Processing file %d of %d',idx, NIdx));

    % filename_mat = sprintf('results_idx_%d.mat',idx);

    filepath = 'figs';

    load(files{idx});
    load(files_SU{idx});
    load(files_DPC{idx});
    load(files_CVQ{idx});
    Grass_4bit = load(files_Grassmanian_4bit{idx});
    Grass_6bit = load(files_Grassmanian_6bit{idx});
    RVQ_corr_4bit = load(files_RVQ_corr_4bit{idx});
    RVQ_corr_6bit = load(files_RVQ_corr_6bit{idx});
    RVQ_corr_12bit = load(files_RVQ_corr_12bit{idx});
    RVQ = load(files_RVQ{idx});
    

    % post processing
    [Cap_4U_ZF_f,Cap_4U_ZF_x] = ecdf(CAP_4U_ZF(:));
    [Cap_4U_MMSE_f,Cap_4U_MMSE_x] = ecdf(CAP_4U_MMSE(:));
    %[Cap_2U_ZF_f,Cap_2U_ZF_x] = ecdf(CAP_2U_ZF(:));
    %[Cap_2U_MMSE_f,Cap_2U_MMSE_x] = ecdf(CAP_2U_MMSE(:));
    [Cap_4U_AS_ZF_f,Cap_4U_AS_ZF_x] = ecdf(CAP_4U_AS_ZF(:));
    [Cap_4U_AS_MMSE_f,Cap_4U_AS_MMSE_x] = ecdf(CAP_4U_AS_MMSE(:));
    %[Cap_2U_AS_ZF_f,Cap_2U_AS_ZF_x] = ecdf(CAP_2U_AS_ZF(:));
    %[Cap_2U_AS_MMSE_f,Cap_2U_AS_MMSE_x] = ecdf(CAP_2U_AS_MMSE(:));
    [Cap_4U_4x1_DPC_f,Cap_4U_4x1_DPC_x] = ecdf(CAP_4U_4x1_DPC(:));

    [Cap_SU_TDMA_ML_f,Cap_SU_TDMA_ML_x] = ecdf(CAP_SU4x1_TDMA_ML(:));
    [Cap_SU_TDMA_CL_ML_f,Cap_SU_TDMA_CL_ML_x] = ecdf(CAP_SU4x1_TDMA_CL_ML(:));

    [Cap_4U_Fourier_4bit_f,Cap_4U_Fourier_4bit_x] = ecdf(reshape(CAP_4U_MMSE_CVQ(:,:,1),1,[]));
    [Cap_4U_Fourier_6bit_f,Cap_4U_Fourier_6bit_x] = ecdf(reshape(CAP_4U_MMSE_CVQ(:,:,2),1,[]));
    [Cap_4U_Fourier_12bit_f,Cap_4U_Fourier_12bit_x] = ecdf(reshape(CAP_4U_MMSE_CVQ(:,:,3),1,[]));
    [Cap_4U_Grass_4bit_f,Cap_4U_Grass_4bit_x] = ecdf(Grass_4bit.CAP_4U_MMSE_RVQ(:));
    [Cap_4U_Grass_6bit_f,Cap_4U_Grass_6bit_x] = ecdf(Grass_6bit.CAP_4U_MMSE_RVQ(:));
    [Cap_4U_RVQ_corr_4bit_f,Cap_4U_RVQ_corr_4bit_x] = ecdf(RVQ_corr_4bit.CAP_4U_MMSE_RVQ(:));
    [Cap_4U_RVQ_corr_6bit_f,Cap_4U_RVQ_corr_6bit_x] = ecdf(RVQ_corr_6bit.CAP_4U_MMSE_RVQ(:));
    [Cap_4U_RVQ_corr_12bit_f,Cap_4U_RVQ_corr_12bit_x] = ecdf(RVQ_corr_12bit.CAP_4U_MMSE_RVQ(:));
    [Cap_4U_RVQ_4bit_f,Cap_4U_RVQ_4bit_x] = ecdf(reshape(RVQ.CAP_4U_MMSE_RVQ(:,:,1),1,[]));
    [Cap_4U_RVQ_6bit_f,Cap_4U_RVQ_6bit_x] = ecdf(reshape(RVQ.CAP_4U_MMSE_RVQ(:,:,2),1,[]));
    [Cap_4U_RVQ_12bit_f,Cap_4U_RVQ_12bit_x] = ecdf(reshape(RVQ.CAP_4U_MMSE_RVQ(:,:,3),1,[]));

    % use the following color coding
    % ZF = blue, MMSE = red, SUTS = black, BD = cyan, MMSE AS = magenta, 
    % 4U = '-', 2U = '--'
    % measured = 2pt, iid = 1pt

%%
    h_fig = figure(10+idx-1);
    hold off
    %plot(Cap_4U_4x1_DPC_x,Cap_4U_4x1_DPC_f,plot_style_ZF{1},'Linewidth',2)
    plot(Cap_4U_MMSE_x,Cap_4U_MMSE_f,plot_style_ZF{1},'Linewidth',2)
    hold on
    plot(Cap_SU_TDMA_ML_x,Cap_SU_TDMA_ML_f,plot_style_ZF{2},'Linewidth',2)
    %plot(Cap_SU_TDMA_CL_ML_x,Cap_SU_TDMA_CL_ML_f,plot_style_ZF{3},'Linewidth',2)
    plot(Cap_4U_Fourier_6bit_x,Cap_4U_Fourier_6bit_f,plot_style_MMSE{1},'Linewidth',1)
    plot(Cap_4U_Grass_6bit_x,Cap_4U_Grass_6bit_f,plot_style_MMSE{2},'Linewidth',1)
    plot(Cap_4U_RVQ_6bit_x,Cap_4U_RVQ_6bit_f,plot_style_MMSE{3},'Linewidth',1)
    plot(Cap_4U_RVQ_corr_6bit_x,Cap_4U_RVQ_corr_6bit_f,plot_style_MMSE{4},'Linewidth',1)
    legend('MU-MIMO MMSE perfect feedback','SU-MISO TDMA (no feedback)', ...
        'MU-MIMO Fourier MMSE 6 bit','MU-MIMO Grassmannian MMSE 6 bit', ...
        'MU-MIMO RVQ MMSE 6 bit','MU-MIMO RVQ corr MMSE 6 bit', ...
        'Location','SouthEast')
    title(sprintf('Multi-user Capacity for Measurement %d (M=4, K=4, N=1, SNR=10dB)',idx),'Fontsize',14)
    xlabel('bits/channel use','Fontsize', 12)
    xlim([0 18])
    ylabel('CDF','Fontsize', 12)
    grid on
    % Create textarrow
    annotation(h_fig,'textarrow',[0.2643 0.2875],[0.7585 0.7286],...
    'TextEdgeColor','none',...
    'String',{'no feedback'});
    annotation(h_fig,'textarrow',[0.5304 0.4661],[0.5823 0.5762],...
    'TextEdgeColor','none',...
    'String',{'perfect feedback'});
    annotation(h_fig,'ellipse','Position',[0.2054 0.2119 0.08314 0.05]);
    annotation(h_fig,'textbox','String',{'limited','feedback'},...
        'HorizontalAlignment','center',...
        'FitBoxToText','on',...
        'LineStyle','none',...
        'Position',[0.29 0.237 0.074 0.07381]);
    filename_cap = sprintf('capacity_comp_measurement_%d_6bits.eps',idx);
    saveas(h_fig, filename_cap, 'epsc2');
    system(['epstopdf ' filename_cap]);

%%
    h_fig = figure(20+idx-1);
    hold off
    %plot(Cap_4U_4x1_DPC_x,Cap_4U_4x1_DPC_f,plot_style_ZF{1},'Linewidth',2)
    plot(Cap_4U_MMSE_x,Cap_4U_MMSE_f,plot_style_ZF{1},'Linewidth',2)
    hold on
    plot(Cap_SU_TDMA_ML_x,Cap_SU_TDMA_ML_f,plot_style_ZF{2},'Linewidth',2)
    %plot(Cap_SU_TDMA_CL_ML_x,Cap_SU_TDMA_CL_ML_f,plot_style_ZF{3},'Linewidth',2)
    plot(Cap_4U_Fourier_12bit_x,Cap_4U_Fourier_12bit_f,plot_style_MMSE{1},'Linewidth',1)
    %plot(Cap_4U_Grass_12bit_x,Cap_4U_Grass_12bit_f,plot_style_MMSE{2},'Linewidth',2)
    plot(Cap_4U_RVQ_12bit_x,Cap_4U_RVQ_12bit_f,plot_style_MMSE{3},'Linewidth',1)
    plot(Cap_4U_RVQ_corr_12bit_x,Cap_4U_RVQ_corr_12bit_f,plot_style_MMSE{4},'Linewidth',1)
    legend('MU-MIMO MMSE perfect feedback','SU-MISO TDMA (no feedback)', ...
        'MU-MIMO Fourier MMSE 12 bit', ...
        'MU-MIMO RVQ MMSE 12 bit','MU-MIMO RVQ corr MMSE 12 bit', ...
        'Location','SouthEast')
    title(sprintf('Multi-user Capacity for Measurement %d (M=4, K=4, N=1, SNR=10dB)',idx),'Fontsize',14)
    xlabel('bits/channel use','Fontsize', 12)
    xlim([0 18])
    ylabel('CDF','Fontsize', 12)
    grid on
    % Create textarrow
    annotation(h_fig,'textarrow',[0.2643 0.2875],[0.7585 0.7286],...
    'TextEdgeColor','none',...
    'String',{'no feedback'});
    annotation(h_fig,'textarrow',[0.5304 0.4661],[0.5823 0.5762],...
    'TextEdgeColor','none',...
    'String',{'perfect feedback'});    
    annotation(h_fig,'ellipse','Position',[0.2196 0.1976 0.125 0.07619]);
    annotation(h_fig,'textbox','String',{'limited','feedback'},...
        'HorizontalAlignment','center',...
        'FitBoxToText','on',...
        'LineStyle','none',...
        'Position',[0.2525 0.2751 0.074 0.07381]);
    filename_cap = sprintf('capacity_comp_measurement_%d_12bits.eps',idx);
    saveas(h_fig, filename_cap, 'epsc2');
    system(['epstopdf ' filename_cap]);

    
%%
%     h_fig = figure(10)
%     
%     plot(Cap_4U_ZF_x,Cap_4U_ZF_f,plot_style_ZF{idx},'Linewidth',2)
%     hold on
%     plot(Cap_4U_MMSE_x,Cap_4U_MMSE_f,plot_style_MMSE{idx},'Linewidth',2)

      %%
%     h_fig = figure(11);
% 
%     plot(Cap_4U_AS_ZF_x,Cap_4U_AS_ZF_f,plot_style_ZF{idx},'Linewidth',2)
%     hold on
%     plot(Cap_4U_AS_MMSE_x,Cap_4U_AS_MMSE_f,plot_style_MMSE{idx},'Linewidth',2)

    %%
%     h_fig = figure(12);
% 
%     plot(Cap_4U_4x1_DPC_x,Cap_4U_4x1_DPC_f,plot_style_ZF{idx},'Linewidth',2)
%     hold on
%     plot(Cap_SU_TDMA_CL_ML_x,Cap_SU_TDMA_CL_ML_f,plot_style_MMSE{idx},'Linewidth',2)

    %keyboard
    
    %%
%     h_fig = figure(13);
% 
%     plot(Cap_4U_Grass_4bit_x,Cap_4U_Grass_4bit_f,plot_style_ZF{idx},'Linewidth',2)
%     hold on
%     plot(Cap_4U_RVQ_corr_4bit_x,Cap_4U_RVQ_corr_4bit_f,plot_style_MMSE{idx},'Linewidth',2)

    %%
%     h_fig = figure(14);
% 
%     plot(Cap_4U_Grass_6bit_x,Cap_4U_Grass_6bit_f,plot_style_ZF{idx},'Linewidth',2)
%     hold on
%     plot(Cap_4U_RVQ_corr_6bit_x,Cap_4U_RVQ_corr_6bit_f,plot_style_MMSE{idx},'Linewidth',2)


end

% h_fig = figure(10);
% load('Cap_iid_N_Rx=1.mat')
% plot(Cap_4U_ZF_iid_x,Cap_4U_ZF_iid_f,'b--','Linewidth',1)
% hold on
% plot(Cap_4U_MMSE_iid_x,Cap_4U_MMSE_iid_f,'r','Linewidth',1)


%%
% h_fig = figure(10);
% legend('MU-MIMO ZF 4U outdoor far','MU-MIMO MMSE 4U outdoor far',...
%     'MU-MIMO ZF 4U indoor','MU-MIMO MMSE 4U indoor',...
%     'MU-MIMO ZF 4U outdoor near','MU-MIMO MMSE 4U outdoor near'...
%     ,'Location','SouthEast')
% title('Multiuser Capacity for M=4, N=1, and SNR=10dB')
% xlabel('bits/sec/Hz')
% xlim([0 18])
% ylabel('CDF')
% grid on
% annotation('ellipse','Position',[0.1339 0.5215 0.1625 0.05952]);
% annotation('textbox','String',{'MU-MIMO ZF'}, 'LineStyle','none',...
%     'Position',[0.1295 0.5881 0.1972 0.05]);
% annotation('ellipse','Position',[0.3589 0.6253 0.1196 0.05952]);
% annotation('textbox','String',{'MU-MIMO MMSE'},'LineStyle','none',...
%     'Position',[0.4746 0.6062 0.2433 0.07143]);
% filename_cap = 'capacity_comp_multiple_measurements_ZF_MMSE.eps';
% %saveas(h_fig, filename_cap, 'epsc2');
% %system(['epstopdf ' filename_cap]);
% 
% 
% h_fig = figure(11);
% legend('M32 ZF 4U','M32 MMSE 4U','M50 ZF 4U','M50 MMSE 4U','M51 ZF 4U','M51 MMSE 4U','Location','SouthEast')
% title('Multiuser Capacity for NTx=4 with antenna selection and SNR=10dB')
% xlabel('bits/sec/Hz')
% xlim([0 18])
% ylabel('CDF')
% grid on
% filename_cap = 'capacity_comp_multiple_measurements_AS.eps';
% %saveas(h_fig, filename_cap, 'epsc2');
% %system(['epstopdf ' filename_cap]);
% 
% h_fig = figure(12);
% legend('MU-MIMO DPC 4U outdoor far','SU-MISO TDMA 4U outdoor far',...
%     'MU-MIMO DPC 4U indoor','SU-MISO TDMA 4U indoor',...
%     'MU-MIMO DPC 4U outdoor near','SU-MISO TDMA 4U outdoor near'...
%     ,'Location','SouthEast')
% title('Multiuser Capacity for M=4, N=1, and SNR=10dB')
% xlabel('bits/sec/Hz')
% xlim([0 18])
% ylabel('CDF')
% grid on
% annotation('ellipse','Position',[0.3171 0.6548 0.1026 0.05952]);
% annotation('ellipse','Position',[0.5156 0.5586 0.1026 0.05952]);
% annotation('textbox','String',{'SU-MIMO TDMA'},'LineStyle','none',...
%     'Position',[0.1599 0.6976 0.1972 0.05]);
% annotation('textbox','String',{'MU-MIMO DPC'},'LineStyle','none',...
%     'Position',[0.621 0.5157 0.2018 0.07143]);
% 
% filename_cap = 'capacity_comp_multiple_measurements_DPC_SU.eps';
% %saveas(h_fig, filename_cap, 'epsc2');
% %system(['epstopdf ' filename_cap]);
% 
% %%
% h_fig = figure(13);
% legend('MU-MIMO Grassmanian MMSE 4U outdoor far','MU-MIMO RVQ corr MMSE 4U outdoor far',...
%     'MU-MIMO Grassmanian MMSE 4U indoor','MU-MIMO RVQ corr MMSE 4U indoor',...
%     'MU-MIMO Grassmanian MMSE 4U outdoor near','MU-MIMO RVQ corr 4U outdoor near'...
%     ,'Location','SouthEast')
% title('Multiuser Capacity for M=4, N=1, 4bit, and SNR=10dB')
% xlabel('bits/sec/Hz')
% xlim([0 18])
% ylabel('CDF')
% grid on
% filename_cap = 'capacity_comp_multiple_measurements_RVQ_4bit.eps';
% saveas(h_fig, filename_cap, 'epsc2');
% %system(['epstopdf ' filename_cap]);
% 
% 
% h_fig = figure(14);
% legend('MU-MIMO Grassmanian MMSE 4U outdoor far','MU-MIMO RVQ corr MMSE 4U outdoor far',...
%     'MU-MIMO Grassmanian MMSE 4U indoor','MU-MIMO RVQ corr MMSE 4U indoor',...
%     'MU-MIMO Grassmanian MMSE 4U outdoor near','MU-MIMO RVQ corr 4U outdoor near'...
%     ,'Location','SouthEast')
% title('Multiuser Capacity for M=4, N=1, 6bit, and SNR=10dB')
% xlabel('bits/sec/Hz')
% xlim([0 18])
% ylabel('CDF')
% grid on
% filename_cap = 'capacity_comp_multiple_measurements_RVQ_6bit.eps';
% saveas(h_fig, filename_cap, 'epsc2');
% %system(['epstopdf ' filename_cap]);
% 
% 

