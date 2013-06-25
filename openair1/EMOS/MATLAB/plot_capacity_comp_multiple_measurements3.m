close all
clear all

%file_indices = [32 50 51];
%file_indices = [27 28 32 33 50 51 52];

filepath = 'figs';
addpath('/homes/kaltenbe/Devel/matlab/matrix_distances')
addpath('/homes/kaltenbe/Devel/matlab/spectral_divergence')

cd /extras/kaltenbe/EMOS/data/20081110_MU_Outdoor

%%
load Dist.mat
load CorrCoef.mat
R=[];

%     for idx=25:29
%             a1 = mean(gps_data(3,idx).latitude)/360*2*pi;
%             a2 = mean(gps_data(4,idx).latitude)/360*2*pi;
%             b1 = mean(gps_data(3,idx).longitude)/360*2*pi;
%             b2 = mean(gps_data(4,idx).longitude)/360*2*pi;
%             r = 6378100; %radius of earth in meters
%             Dist(idx) = acos(cos(a1).*cos(b1).*cos(a2).*cos(b2) + cos(a1).*sin(b1).*cos(a2).*sin(b2) + sin(a1).*sin(a2)) * r;
%     end


%%
% d=dir('results_v2_idx_*.mat');
% files = {d.name};
% 
% d=dir('results_DPC_2x1_idx_*.mat');
% files_DPC = {d.name};

% files_DPC = {'/extras/kaltenbe/EMOS/data/20071126_MU_Outdoor/results_DPC_idx_32.mat', ...
%     '/extras/kaltenbe/EMOS/data/20080205_MU_Indoor_EN_EC/results_DPC_4x1_idx_100.mat',...
%     '/extras/kaltenbe/EMOS/data/20080213_MU_Outdoor/results_DPC_4x1_idx_52.mat'};

file_indices = 25:29;
NIdx = length(file_indices);
SNR = 10;

plot_style_ZF = {'b-','b--','b:','b-.','c'};
plot_style_MMSE = {'r-','r--','r:','r-.','m'};


for idx = 1:NIdx
    disp(sprintf('[eval_data_MU] Processing file %d of %d',idx, NIdx));

    load(sprintf('results_v2_idx_%d.mat',file_indices(idx)));
    load(sprintf('results_DPC_2x1_idx_%d.mat',file_indices(idx)));


    %load(files{idx});
    %load(files_DPC{idx});

    % post processing
    %[Cap_4U_SUTS_f,Cap_4U_SUTS_x] = ecdf(CAP_4U_SUTS(:));
    %[Cap_4U_ZF_f,Cap_4U_ZF_x] = ecdf(CAP_4U_ZF(:));
    %[Cap_4U_MMSE_f,Cap_4U_MMSE_x] = ecdf(CAP_4U_MMSE(:));
    [Cap_2U_ZF_f,Cap_2U_ZF_x] = ecdf(CAP_2U_ZF(:));
    [Cap_2U_MMSE_f,Cap_2U_MMSE_x] = ecdf(CAP_2U_MMSE(:));
    %[Cap_4U_AS_ZF_f,Cap_4U_AS_ZF_x] = ecdf(CAP_4U_AS_ZF(:));
    %[Cap_4U_AS_MMSE_f,Cap_4U_AS_MMSE_x] = ecdf(CAP_4U_AS_MMSE(:));
    [Cap_2U_AS_ZF_f,Cap_2U_AS_ZF_x] = ecdf(CAP_2U_AS_ZF(:));
    [Cap_2U_AS_MMSE_f,Cap_2U_AS_MMSE_x] = ecdf(CAP_2U_AS_MMSE(:));
    [Cap_4U_4x1_DPC_f,Cap_4U_4x1_DPC_x] = ecdf(CAP_4U_4x1_DPC(:));
    [Cap_SU_TDMA_CL_ML_f,Cap_SU_TDMA_CL_ML_x] = ecdf(CAP_SU_TDMA_CL_ML(:));
    
    ECap_2U_ZF(idx) = mean(CAP_2U_ZF(~isnan(CAP_2U_ZF)));
    ECap_2U_MMSE(idx) = mean(CAP_2U_MMSE(:));
    ECap_2U_ZF_AS(idx) = mean(CAP_2U_AS_ZF(~isnan(CAP_2U_AS_ZF)));
    ECap_2U_MMSE_AS(idx) = mean(CAP_2U_AS_MMSE(:));
    ECap_2U_DPC(idx) = mean(CAP_4U_4x1_DPC(:));
    ECap_2U_SU_TDMA(idx) = mean(max(reshape(CAP_SU_TDMA_CL_ML,2,[]),[],1));
    
    temp = SD(DProf.');
    sd(idx) = temp(1,2);
    Dgeod(idx) = dgeod(R(1:2:end,1:2:end),R(2:2:end,2:2:end));
    Dcolin(idx) = dcolin(R(1:2:end,1:2:end),R(2:2:end,2:2:end));
    
    % use the following color coding
    % ZF = blue, MMSE = red, SUTS = black, BD = cyan, MMSE AS = magenta, 
    % 4U = '-', 2U = '--'
    % measured = 2pt, iid = 1pt

%     %%
%     h_fig = figure(10);
% 
%     plot(Cap_2U_ZF_x,Cap_2U_ZF_f,plot_style_ZF{idx},'Linewidth',2)
%     hold on
%     plot(Cap_2U_MMSE_x,Cap_2U_MMSE_f,plot_style_MMSE{idx},'Linewidth',2)
% 
%     %%
%     h_fig = figure(11);
% 
%     plot(Cap_2U_AS_ZF_x,Cap_2U_AS_ZF_f,plot_style_ZF{idx},'Linewidth',2)
%     hold on
%     plot(Cap_2U_AS_MMSE_x,Cap_2U_AS_MMSE_f,plot_style_MMSE{idx},'Linewidth',2)
% 
%     %%
%     h_fig = figure(12);
% 
%     plot(Cap_4U_4x1_DPC_x,Cap_4U_4x1_DPC_f,plot_style_ZF{idx},'Linewidth',2)
%     hold on
%     plot(Cap_SU_TDMA_CL_ML_x,Cap_SU_TDMA_CL_ML_f,plot_style_MMSE{idx},'Linewidth',2)

%     %% calculate the Rx correlation matrix between users taking the first Rx antenna
%     NUser=2;
%     NTx=2;
%     n = 0:NTx-1;
%     m = 0;
% 
%     for k1=0:NUser-1
%         for k2=0:NUser-1
%             tmp = R(k1+n*NUser+m*NUser*NTx+1,k2+n*NUser+m*NUser*NTx+1);
%             % RRx(k1+1,k2+1) = mean(tmp(:));
%             RRx(k1+1,k2+1) = sum(tmp(:));
%         end
%     end
% 
%     %% calculate the Tx correlation matrix taking the first Rx antenna
%     k = 0:NUser-1;
%     m = 0;
% 
%     for n1=0:NTx-1
%         for n2=0:NTx-1
%             disp([k+n1*NUser+m*NUser*NTx+1,k+n2*NUser+m*NUser*NTx+1]);
%             tmp = R(k+n1*NUser+m*NUser*NTx+1,k+n2*NUser+m*NUser*NTx+1);
%             RTx(n1+1,n2+1) = sum(tmp(:));
%         end
%     end


    figure(15)
    subplot(2,NIdx+1,idx)
    imagesc(abs(R)./norm(R), [0,1])
    %set(gca,'xtick',1:NUser)
    %set(gca,'ytick',1:NUser)
    
    figure(15)
    subplot(2,NIdx+1,NIdx+1+idx)
    imagesc(abs(R_Tx./norm(R_Tx)),[0,1])
%     set(gca,'xtick',1:NTx)
%     set(gca,'ytick',1:NTx)
    drawnow

%%

end
%keyboard

%%
h_fig = figure(15)
cmap = colormap('gray');
cmap = cmap(end:-1:1,:);
colormap(cmap);
hcolorbar = subplot(2,NIdx+1,[NIdx+1 2*(NIdx+1)]);
colorbar(hcolorbar);
set(hcolorbar,'YTick',linspace(1,64,11))
set(hcolorbar,'YTicklabel',linspace(0,1,11))

% subplot(2,4,1)
% title('outdoor far')
% ylabel('Rx corr')
% subplot(2,4,2)
% title('indoor')
% subplot(2,4,3)
% title('outdoor near')
% subplot(2,4,5)
% ylabel('Tx corr')


% %%
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
% saveas(h_fig, filename_cap, 'epsc2');
% system(['epstopdf ' filename_cap]);
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
% saveas(h_fig, filename_cap, 'epsc2');
% system(['epstopdf ' filename_cap]);
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
% saveas(h_fig, filename_cap, 'epsc2');
% system(['epstopdf ' filename_cap]);

%%
h_fig = figure(13)
%bar(Dist(25:29),[ECap_2U_ZF' ECap_2U_MMSE' ECap_2U_ZF_AS' ECap_2U_MMSE_AS']);
h_bar = bar(Dist(25:29),[ECap_2U_ZF' ECap_2U_MMSE' ECap_2U_DPC' ECap_2U_SU_TDMA']);
set(h_bar(1),'FaceColor',[1 1 1]);
set(h_bar(2),'FaceColor',[0.83 0.82 0.78]);
set(h_bar(3),'FaceColor',[0.5 0.5 0.5]);
set(h_bar(4),'FaceColor',[0 0 0]);
legend('MU-MIMO ZF', 'MU-MIMO MMSE', 'MU-MIMO DPC', 'SU-MISO TDMA','Location','NorthWest');
xlabel('Distance [m]','Fontsize',12)
ylabel('Ergodic Capacity [bits/channel use]','Fontsize',12)
title('Multi-user Capacity for M=2, N=1, K=2 and SNR=10dB','Fontsize',14)
grid on
saveas(h_fig,'capacity_comp_ergodic_distance.eps','epsc2');

%%
h_fig = figure(14)
bar(Dist(25:29),[sd' Dgeod' cc']);
legend('Spectral Divergence','Geodesic Distance','Correlation Coefficient','Location','NorthWest');
xlabel('Distance [m]','Fontsize',12)
%ylabel('Ergodic Capacity','Fontsize',12)
grid on
saveas(h_fig,'capacity_comp_divers.eps','epsc2');