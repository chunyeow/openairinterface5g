idx = 100;
filename_mat = sprintf('results_v2_idx_%d.mat',idx);
filename_dpc_mat = sprintf('results_DPC_4x1_idx_%d.mat',idx);

filepath = 'figs_pimrc';
if ~exist(filepath,'dir')
    mkdir(filepath)
end
logscale = 1;

load(filename_mat)
load(filename_dpc_mat)
%%
% post processing
[Cap_SU_TDMA_CL_ML_f,Cap_SU_TDMA_CL_ML_x] = ecdf(CAP_SU_TDMA_CL_ML(:));
%[Cap_SU_4x1_TDMA_CL_ML_f,Cap_SU_4x1_TDMA_CL_ML_x] = ecdf(CAP_SU_4x1_TDMA_CL_ML);
[Cap_SU_TDMA_ML_f,Cap_SU_TDMA_ML_x] = ecdf(CAP_SU_TDMA_ML(:));
%[Cap_SU_TDMA_MMSE_f,Cap_SU_TDMA_MMSE_x] = ecdf(CAP_SU_TDMA_MMSE(:));
[Cap_4U_ZF_f,Cap_4U_ZF_x] = ecdf(CAP_4U_ZF(:));
[Cap_4U_MMSE_f,Cap_4U_MMSE_x] = ecdf(CAP_4U_MMSE(:));
[Cap_2U_ZF_f,Cap_2U_ZF_x] = ecdf(CAP_2U_ZF(:));
[Cap_2U_MMSE_f,Cap_2U_MMSE_x] = ecdf(CAP_2U_MMSE(:));
[Cap_4U_AS_ZF_f,Cap_4U_AS_ZF_x] = ecdf(CAP_4U_AS_ZF(:));
[Cap_4U_AS_MMSE_f,Cap_4U_AS_MMSE_x] = ecdf(CAP_4U_AS_MMSE(:));
[Cap_2U_AS_ZF_f,Cap_2U_AS_ZF_x] = ecdf(CAP_2U_AS_ZF(:));
[Cap_2U_AS_MMSE_f,Cap_2U_AS_MMSE_x] = ecdf(CAP_2U_AS_MMSE(:));
%[Cap_2U_BD_WF_f,Cap_2U_BD_WF_x] = ecdf(CAP_2U_BD_WF(:));
%[Cap_2U_BD_EP_f,Cap_2U_BD_EP_x] = ecdf(CAP_2U_BD_EP(:));
[Cap_2U2Tx_ZF_f,Cap_2U2Tx_ZF_x] = ecdf(CAP_2U2Tx_ZF(:));
[Cap_2U2Tx_MMSE_f,Cap_2U2Tx_MMSE_x] = ecdf(CAP_2U2Tx_MMSE(:));

%[Cap_4U_4x2_DPC_f,Cap_4U_4x2_DPC_x] = ecdf(CAP_4U_4x2_DPC(:));
[Cap_4U_4x1_DPC_f,Cap_4U_4x1_DPC_x] = ecdf(CAP_4U_4x1_DPC(:));

% use the following color coding
% ZF = blue, MMSE = red, SUTS = black, BD = cyan, MMSE AS = magenta, 
% 4U = '-', 2U = '--'
% measured = 2pt, iid = 1pt

%% SU-MIMO TDMA vs 4U MU-MIMO DPC (measured+iid)
h_fig = figure(6);
load('cap_iid.mat')
hold off
plot(Cap_SU1x4_TDMA_CL_ML_iid_x,Cap_SU1x4_TDMA_CL_ML_iid_f,'k-','Linewidth',1)
hold on

load('Cap_iid_N_Rx=1.mat')
%plot(Cap_4U_ZF_iid_x,Cap_4U_ZF_iid_f,'b--','Linewidth',1)
%plot(Cap_4U_MMSE_iid_x,Cap_4U_MMSE_iid_f,'r--','Linewidth',1)

%plot(Cap_2U_ZF_iid_x,Cap_2U_ZF_iid_f,'b--','Linewidth',1)
%plot(Cap_2U_MMSE_iid_x,Cap_2U_MMSE_iid_f,'r--','Linewidth',1)

plot(Cap_4U_DPC_iid_x,Cap_4U_DPC_iid_f,'b-.','Linewidth',1);

% AS = load('Cap_iid_N_Rx=2.mat');
% plot(AS.Cap_4U_DPC_iid_x,AS.Cap_4U_DPC_iid_f,'c-.','Linewidth',1);

plot(Cap_SU_TDMA_CL_ML_x,Cap_SU_TDMA_CL_ML_f,'k-','Linewidth',2)

%plot(Cap_4U_ZF_x,Cap_4U_ZF_f,'b--','Linewidth',2)
%plot(Cap_4U_MMSE_x,Cap_4U_MMSE_f,'r--','Linewidth',2)

%plot(Cap_2U_ZF_x,Cap_2U_ZF_f,'b--','Linewidth',2)
%plot(Cap_2U_MMSE_x,Cap_2U_MMSE_f,'r--','Linewidth',2)

plot(Cap_4U_4x1_DPC_x,Cap_4U_4x1_DPC_f,'b-.','Linewidth',2);

% plot(Cap_4U_4x2_DPC_x,Cap_4U_4x2_DPC_f,'c-.','Linewidth',2);

%     legend('iid SUTS 4U','iid ZF 4U','iid MMSE 4U','iid ZF 2U','iid MMSE 2U',...
%         'measured SUTS 4U',...
%         'measured ZF 4U','measured MMSE 4U','measured ZF 2U','measured MMSE 2U',...
%         'Location','SouthEast')
legend('SU-MIMO TDMA 4x1 iid',...
    'MU-MIMO DPC 4x[1,1,1,1] iid',...
    'SU-MIMO TDMA 4x1 measured',...
    'MU-MIMO DPC 4x[1,1,1,1] measured',...
    'Location','SouthEast')
% title('Theoretical ideal models vs. measured channels, SNR = 10dB','Fontsize',14,'Fontweight','bold')
xlabel('bits/sec/Hz','Fontsize',14,'Fontweight','bold')
xlim([0 18])
ylabel('CDF','Fontsize',14,'Fontweight','bold')
if logscale
    set(gca,'yscale','log')
    ylim([1e-2 1])
end
grid on
filename_cap = fullfile(filepath,sprintf('idx_%d_capacity_DPC_NRx=1.eps',idx));
saveas(h_fig, filename_cap, 'epsc2');
filename_cap_pdf = fullfile(filepath,sprintf('idx_%d_capacity_DPC_NRx=1.pdf',idx));
system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));

%% SU-MIMO TDMA vs 4U MU-MIMO ZF, 4U MU-MIMO MMSE and 4U MU-MIMO DPC (measured only)
h_fig = figure(7);
load('cap_iid.mat')
hold off
%plot(Cap_SU1x4_TDMA_CL_ML_iid_x,Cap_SU1x4_TDMA_CL_ML_iid_f,'k-','Linewidth',1)
%hold on

load('Cap_iid_N_Rx=1.mat')
%plot(Cap_4U_ZF_iid_x,Cap_4U_ZF_iid_f,'b--','Linewidth',1)
%plot(Cap_4U_MMSE_iid_x,Cap_4U_MMSE_iid_f,'r--','Linewidth',1)

%plot(Cap_2U_ZF_iid_x,Cap_2U_ZF_iid_f,'b--','Linewidth',1)
%plot(Cap_2U_MMSE_iid_x,Cap_2U_MMSE_iid_f,'r--','Linewidth',1)

%plot(Cap_4U_DPC_iid_x,Cap_4U_DPC_iid_f,'b-.','Linewidth',1);

% AS = load('Cap_iid_N_Rx=2.mat');
% plot(AS.Cap_4U_DPC_iid_x,AS.Cap_4U_DPC_iid_f,'c-.','Linewidth',1);

plot(Cap_SU_TDMA_CL_ML_x,Cap_SU_TDMA_CL_ML_f,'k-','Linewidth',2)
hold on

plot(Cap_4U_ZF_x,Cap_4U_ZF_f,'c--','Linewidth',2)
plot(Cap_4U_MMSE_x,Cap_4U_MMSE_f,'r--','Linewidth',2)

%plot(Cap_2U_ZF_x,Cap_2U_ZF_f,'b--','Linewidth',2)
%plot(Cap_2U_MMSE_x,Cap_2U_MMSE_f,'r--','Linewidth',2)

plot(Cap_4U_4x1_DPC_x,Cap_4U_4x1_DPC_f,'b-.','Linewidth',2);

% plot(Cap_4U_4x2_DPC_x,Cap_4U_4x2_DPC_f,'c-.','Linewidth',2);

%     legend('iid SUTS 4U','iid ZF 4U','iid MMSE 4U','iid ZF 2U','iid MMSE 2U',...
%         'measured SUTS 4U',...
%         'measured ZF 4U','measured MMSE 4U','measured ZF 2U','measured MMSE 2U',...
%         'Location','SouthEast')
legend('SU-MIMO TDMA 4x1 measured',...
    'MU-MIMO ZF 4x[1,1,1,1] measured',...
    'MU-MIMO MMSE 4x[1,1,1,1] measured',...
    'MU-MIMO DPC 4x[1,1,1,1] measured',...
    'Location','SouthEast')
% title('Theoretical ideal models vs. measured channels, SNR = 10dB','Fontsize',14,'Fontweight','bold')
xlabel('bits/sec/Hz','Fontsize',14,'Fontweight','bold')
xlim([0 18])
ylabel('CDF','Fontsize',14,'Fontweight','bold')
if logscale
    set(gca,'yscale','log')
    ylim([1e-2 1])
end
grid on
filename_cap = fullfile(filepath,sprintf('idx_%d_capacity_ZF_MMSE_DPC_NRx=1.eps',idx));
saveas(h_fig, filename_cap, 'epsc2');
filename_cap_pdf = fullfile(filepath,sprintf('idx_%d_capacity_ZF_MMSE_DPC_NRx=1.pdf',idx));
system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));

%% SU-MIMO vs. DPC with 2 Rx antennas (meas+iid)
h_fig = figure(8);

hold off
plot(Cap_SU2x4_TDMA_CL_ML_iid_x,Cap_SU2x4_TDMA_CL_ML_iid_f,'k-','Linewidth',1)
hold on

AS = load('Cap_iid_N_Rx=2.mat');
%plot(AS.Cap_2U_BD_WF_iid_x,AS.Cap_2U_BD_WF_iid_f,'r--','Linewidth',1)

%plot(AS.Cap_4U_MMSE_iid_x,AS.Cap_4U_MMSE_iid_f,'r--','Linewidth',1)
%plot(AS.Cap_2U_MMSE_iid_x,AS.Cap_2U_MMSE_iid_f,'m--','Linewidth',1)

plot(AS.Cap_4U_DPC_iid_x,AS.Cap_4U_DPC_iid_f,'b-.','Linewidth',1);

plot(Cap_SU_TDMA_CL_ML_x,Cap_SU_TDMA_CL_ML_f,'k-','Linewidth',2)

%plot(Cap_2U_BD_WF_x,Cap_2U_BD_WF_f,'r--','Linewidth',2)

%plot(Cap_4U_AS_MMSE_x,Cap_4U_AS_MMSE_f,'r--','Linewidth',2)
%plot(Cap_2U_AS_MMSE_x,Cap_2U_AS_MMSE_f,'m--','Linewidth',2)

%plot(Cap_2U2_MMSE_x,Cap_2U2_MMSE_f,'r--','Linewidth',2);
%plot(Cap_2U2_ZF_x,Cap_2U2_ZF_f,'b--','Linewidth',2);

plot(Cap_4U_4x2_DPC_x,Cap_4U_4x2_DPC_f,'b-.','Linewidth',2);

legend('SU-MIMO TDMA 4x2 iid',...
    'MU-MIMO DPC 4x[2,2,2,2] iid', ...
    'SU-MIMO TDMA 4x2 iid ',...
    'MU-MIMO DPC 4x[2,2,2,2] measured',...        
    'Location','SouthEast')
%title('MMSE vs. BD vs. DPC with maximum multiplexing gain','Fontsize',14,'Fontweight','bold')
xlabel('bits/sec/Hz','Fontsize',14,'Fontweight','bold')
xlim([0 18])
ylabel('CDF','Fontsize',14,'Fontweight','bold')
if logscale
    set(gca,'yscale','log')
    ylim([1e-2 1])
end
grid on
filename_cap = fullfile(filepath,sprintf('idx_%d_capacity_DPC_NRx=2.eps',idx));
saveas(h_fig, filename_cap, 'epsc2');
filename_cap_pdf = fullfile(filepath,sprintf('idx_%d_capacity_DPC_NRx=2.pdf',idx));
system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));

%% SU-MIMO vs. MMSE and DPC with 2 Rx antennas (meas only)
h_fig = figure(9);

hold off
%plot(Cap_SU2x4_TDMA_CL_ML_iid_x,Cap_SU2x4_TDMA_CL_ML_iid_f,'k-','Linewidth',1)
%hold on

%AS = load('Cap_iid_N_Rx=2.mat');
%plot(AS.Cap_2U_BD_WF_iid_x,AS.Cap_2U_BD_WF_iid_f,'r--','Linewidth',1)

%plot(AS.Cap_4U_MMSE_iid_x,AS.Cap_4U_MMSE_iid_f,'r--','Linewidth',1)
%plot(AS.Cap_2U_MMSE_iid_x,AS.Cap_2U_MMSE_iid_f,'m--','Linewidth',1)

%plot(AS.Cap_4U_DPC_iid_x,AS.Cap_4U_DPC_iid_f,'b-.','Linewidth',1);

plot(Cap_SU_TDMA_CL_ML_x,Cap_SU_TDMA_CL_ML_f,'k-','Linewidth',2)
hold on

%plot(Cap_2U_BD_WF_x,Cap_2U_BD_WF_f,'r--','Linewidth',2)

plot(Cap_4U_AS_MMSE_x,Cap_4U_AS_MMSE_f,'r--','Linewidth',2)
%plot(Cap_2U_AS_MMSE_x,Cap_2U_AS_MMSE_f,'m--','Linewidth',2)

%plot(Cap_2U2_MMSE_x,Cap_2U2_MMSE_f,'r--','Linewidth',2);
%plot(Cap_2U2_ZF_x,Cap_2U2_ZF_f,'b--','Linewidth',2);

plot(Cap_4U_4x2_DPC_x,Cap_4U_4x2_DPC_f,'b-.','Linewidth',2);

legend('SU-MIMO TDMA 4x2 measured',...
    'MU-MIMO MMSE 4x[2,2,2,2] measured ',...
    'MU-MIMO DPC 4x[2,2,2,2] measured',...        
    'Location','SouthEast')
%title('MMSE vs. BD vs. DPC with maximum multiplexing gain','Fontsize',14,'Fontweight','bold')
xlabel('bits/sec/Hz','Fontsize',14,'Fontweight','bold')
xlim([0 18])
ylabel('CDF','Fontsize',14,'Fontweight','bold')
if logscale
    set(gca,'yscale','log')
    ylim([1e-2 1])
end
grid on
filename_cap = fullfile(filepath,sprintf('idx_%d_capacity_MMSE_DPC_NRx=2.eps',idx));
saveas(h_fig, filename_cap, 'epsc2');
filename_cap_pdf = fullfile(filepath,sprintf('idx_%d_capacity_MMSE_DPC_NRx=2.pdf',idx));
system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));

