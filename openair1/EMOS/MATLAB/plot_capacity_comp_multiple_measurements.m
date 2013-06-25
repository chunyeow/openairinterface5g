close all
clear all

%file_indices = [32 50 51];
%file_indices = [27 28 32 33 50 51 52];

files = {'/extras/kaltenbe/EMOS/data/20071126_MU_Outdoor/results_v2_idx_32.mat', ...
    '/extras/kaltenbe/EMOS/data/20080205_MU_Indoor_EN_EC/results_v2_idx_100.mat',...
    '/extras/kaltenbe/EMOS/data/20080213_MU_Outdoor/results_v2_idx_52.mat'};
files_DPC = {'/extras/kaltenbe/EMOS/data/20071126_MU_Outdoor/results_DPC_idx_32.mat', ...
    '/extras/kaltenbe/EMOS/data/20080205_MU_Indoor_EN_EC/results_DPC_4x1_idx_100.mat',...
    '/extras/kaltenbe/EMOS/data/20080213_MU_Outdoor/results_DPC_4x1_idx_52.mat'};

NIdx = length(files);
SNR = 10;

plot_style_ZF = {'b-','b--','b:'};
plot_style_MMSE = {'r-','r--','r:'};


for idx = 1:NIdx
    disp(sprintf('[eval_data_MU] Processing file %d of %d',idx, NIdx));

    % filename_mat = sprintf('results_idx_%d.mat',idx);

    filepath = 'figs';

    load(files{idx});
    load(files_DPC{idx});

    % post processing
    %[Cap_4U_SUTS_f,Cap_4U_SUTS_x] = ecdf(CAP_4U_SUTS(:));
    [Cap_4U_ZF_f,Cap_4U_ZF_x] = ecdf(CAP_4U_ZF(:));
    [Cap_4U_MMSE_f,Cap_4U_MMSE_x] = ecdf(CAP_4U_MMSE(:));
    %[Cap_2U_ZF_f,Cap_2U_ZF_x] = ecdf(CAP_2U_ZF(:));
    %[Cap_2U_MMSE_f,Cap_2U_MMSE_x] = ecdf(CAP_2U_MMSE(:));
    [Cap_4U_AS_ZF_f,Cap_4U_AS_ZF_x] = ecdf(CAP_4U_AS_ZF(:));
    [Cap_4U_AS_MMSE_f,Cap_4U_AS_MMSE_x] = ecdf(CAP_4U_AS_MMSE(:));
    %[Cap_2U_AS_ZF_f,Cap_2U_AS_ZF_x] = ecdf(CAP_2U_AS_ZF(:));
    %[Cap_2U_AS_MMSE_f,Cap_2U_AS_MMSE_x] = ecdf(CAP_2U_AS_MMSE(:));
    [Cap_4U_4x1_DPC_f,Cap_4U_4x1_DPC_x] = ecdf(CAP_4U_4x1_DPC(:));
    [Cap_SU_TDMA_CL_ML_f,Cap_SU_TDMA_CL_ML_x] = ecdf(max(reshape(CAP_SU_TDMA_CL_ML,4,[]),[],1));

    % use the following color coding
    % ZF = blue, MMSE = red, SUTS = black, BD = cyan, MMSE AS = magenta, 
    % 4U = '-', 2U = '--'
    % measured = 2pt, iid = 1pt

    %%
    h_fig = figure(10);

    plot(Cap_4U_ZF_x,Cap_4U_ZF_f,plot_style_ZF{idx},'Linewidth',2)
    hold on
    plot(Cap_4U_MMSE_x,Cap_4U_MMSE_f,plot_style_MMSE{idx},'Linewidth',2)

    %%
    h_fig = figure(11);

    plot(Cap_4U_AS_ZF_x,Cap_4U_AS_ZF_f,plot_style_ZF{idx},'Linewidth',2)
    hold on
    plot(Cap_4U_AS_MMSE_x,Cap_4U_AS_MMSE_f,plot_style_MMSE{idx},'Linewidth',2)

    %%
    h_fig = figure(12);

    plot(Cap_4U_4x1_DPC_x,Cap_4U_4x1_DPC_f,plot_style_ZF{idx},'Linewidth',2)
    hold on
    plot(Cap_SU_TDMA_CL_ML_x,Cap_SU_TDMA_CL_ML_f,plot_style_MMSE{idx},'Linewidth',2)

%% calculate the Rx correlation matrix between users taking the first Rx antenna
    NUser=4;
    NTx=4;
    n = 0:3;
    m = 0;

    for k1=0:3
        for k2=0:3
            tmp = R(k1+n*NUser+m*NUser*NTx+1,k2+n*NUser+m*NUser*NTx+1);
            % RRx(k1+1,k2+1) = mean(tmp(:));
            RRx(k1+1,k2+1) = sum(diag(tmp));
        end
    end

    figure(13)
    subplot(2,4,idx)
    imagesc(abs(RRx),[0,10])
    set(gca,'xtick',1:4)
    set(gca,'ytick',1:4)
    
    %% calculate the Tx correlation matrix taking the first Rx antenna
    k = 0:3;
    m = 0;

    for n1=0:3
        for n2=0:3
            tmp = R(k+n1*NUser+m*NUser*NTx+1,k+n2*NUser+m*NUser*NTx+1);
            RTx(n1+1,n2+1) = sum(diag(tmp));
        end
    end

    figure(13)
    subplot(2,4,4+idx)
    imagesc(abs(RTx),[0,10])
    set(gca,'xtick',1:4)
    set(gca,'ytick',1:4)

%%
end

% h_fig = figure(10);
% load('Cap_iid_N_Rx=1.mat')
% plot(Cap_4U_ZF_iid_x,Cap_4U_ZF_iid_f,'b--','Linewidth',1)
% hold on
% plot(Cap_4U_MMSE_iid_x,Cap_4U_MMSE_iid_f,'r','Linewidth',1)

%%
figure(13)
cmap = colormap('gray');
cmap = cmap(end:-1:1,:);
colormap(cmap);
hcolorbar = subplot(2,4,[4 8]);
colorbar(hcolorbar);
set(hcolorbar,'YTick',linspace(1,64,6))
set(hcolorbar,'YTicklabel',0:1:10)

%%
subplot(2,4,1)
title('outdoor far')
ylabel('Rx corr')
subplot(2,4,2)
title('indoor')
subplot(2,4,3)
title('outdoor near')
subplot(2,4,5)
ylabel('Tx corr')

drawnow


%%
h_fig = figure(10);
legend('MU-MIMO ZF Measurement 1','MU-MIMO MMSE Measurement 1',...
    'MU-MIMO ZF Measurement 2','MU-MIMO MMSE Measurement 2',...
    'MU-MIMO ZF Measurement 3','MU-MIMO MMSE Measurement 3'...
    ,'Location','SouthEast')
title('Multi-user Capacity for M=4, K=4, N=1, and SNR=10dB','Fontsize',14)
xlabel('bits/channel use','Fontsize',12)
xlim([0 18])
ylabel('CDF','Fontsize',12)
grid on
annotation('ellipse','Position',[0.1339 0.5215 0.1625 0.05952]);
annotation('textbox','String',{'MU-MIMO ZF'}, 'LineStyle','none',...
    'Position',[0.1295 0.5881 0.1972 0.05]);
annotation('ellipse','Position',[0.3589 0.6253 0.1196 0.05952]);
annotation('textbox','String',{'MU-MIMO MMSE'},'LineStyle','none',...
    'Position',[0.4746 0.6062 0.2433 0.07143]);
filename_cap = 'capacity_comp_multiple_measurements_ZF_MMSE.eps';
saveas(h_fig, filename_cap, 'epsc2');
system(['epstopdf ' filename_cap]);


h_fig = figure(11);
legend('M32 ZF 4U','M32 MMSE 4U','M50 ZF 4U','M50 MMSE 4U','M51 ZF 4U','M51 MMSE 4U','Location','SouthEast')
title('Multi-user Capacity for NTx=4 with antenna selection and SNR=10dB')
xlabel('bits/sec/Hz')
xlim([0 18])
ylabel('CDF')
grid on
filename_cap = 'capacity_comp_multiple_measurements_AS.eps';
saveas(h_fig, filename_cap, 'epsc2');
system(['epstopdf ' filename_cap]);

h_fig = figure(12);
legend('MU-MIMO DPC Measurement 1','SU-MISO TDMA Measurement 1',...
    'MU-MIMO DPC Measurement 2','SU-MISO TDMA Measurement 2',...
    'MU-MIMO DPC Measurement 3','SU-MISO TDMA Measurement 3'...
    ,'Location','SouthEast')
title('Multi-user Capacity for M=4, K=4, N=1, and SNR=10dB','Fontsize',14)
xlabel('bits/channel use','Fontsize',12)
xlim([0 18])
ylabel('CDF','Fontsize',12)
grid on
annotation('ellipse','Position',[0.3171 0.6548 0.1026 0.05952]);
annotation('ellipse','Position',[0.5156 0.5586 0.1026 0.05952]);
annotation('textbox','String',{'SU-MISO TDMA'},'LineStyle','none',...
    'Position',[0.1599 0.6976 0.1972 0.05]);
annotation('textbox','String',{'MU-MIMO DPC'},'LineStyle','none',...
    'Position',[0.621 0.5157 0.2018 0.07143]);

filename_cap = 'capacity_comp_multiple_measurements_DPC_SU.eps';
saveas(h_fig, filename_cap, 'epsc2');
system(['epstopdf ' filename_cap]);
