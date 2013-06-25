clear all
close all

%% plot iid results
figure(1)
hold off
load cap_iid_dpc_NTx4_NRx1_NUser4.mat
plot(SNRdB(1:s),ECap_4U_MMSE_iid(1:s),'r');
hold on
plot(SNRdB(1:s),ECap_4U_ZF_iid(1:s),'g');
plot(SNRdB(1:s),ECap_4U_DPC_iid(1:s),'k');

load cap_iid_bd_ep_NTx4_NRx2_NUser2.mat
plot(SNRdB(1:s),ECap_MU_BD_WF_iid(1:s),'m');
load cap_iid_bd_wf_NTx4_NRx2_NUser2.mat
plot(SNRdB(1:s),ECap_MU_BD_WF_iid(1:s),'c');
plot(SNRdB(1:s),ECap_SU_TDMA_CL_ML_iid(1:s),'b');

legend('MMSE 4x[1,1,1,1]','ZF 4x[1,1,1,1]','DPC 4x[1,1,1,1]','BD EP 4x[2,2]','BD WF 4x[2,2]','SU TDMA 4x2','Location','NorthWest')
xlabel('SNR [dB]')
ylabel('Sum rate')
grid on

%% plot iid & measured
h_fig = figure(2);
hold off
load cap_iid_dpc_NTx4_NRx1_NUser4.mat ECap_SU_TDMA_CL_ML_iid ECap_4U_MMSE_iid ECap_4U_DPC_iid SNRdB
plot(SNRdB(1:8),ECap_4U_MMSE_iid(1:8),'r-.','Linewidth',1);
hold on
plot(SNRdB(1:8),ECap_4U_DPC_iid(1:8),'k--','Linewidth',1);
% load cap_iid_bd_wf_NTx4_NRx2_NUser2.mat ECap_SU_TDMA_CL_ML_iid SNRdB
plot(SNRdB(1:8),ECap_SU_TDMA_CL_ML_iid(1:8),'b-','Linewidth',1);

% load results_DPC_SNR=-20-50_idx_32.mat
SNRdB = -20:10:50;
ECap_4U_MMSE = mean(reshape(CAP_4U_MMSE,[],length(SNRdB)),1);
ECap_4U_DPC = mean(reshape(CAP_4U_4x1_DPC,[],length(SNRdB)),1);
ECap_SU_4x1_TDMA_CL_ML = mean(CAP_SU_4x1_TDMA_CL_ML,1);
ECap_SU_4x2_TDMA_CL_ML = mean(CAP_SU_4x2_TDMA_CL_ML,1);

plot(SNRdB,ECap_4U_MMSE,'r-.','Linewidth',2);
plot(SNRdB,ECap_4U_DPC,'k--','Linewidth',2);
plot(SNRdB,ECap_SU_4x1_TDMA_CL_ML,'b-','Linewidth',2);
% plot(SNRdB,ECap_SU_4x2_TDMA_CL_ML,'c:','Linewidth',2);

legend('MU-MIMO MMSE 4x[1,1,1,1] iid','MU-MIMO DPC 4x[1,1,1,1] iid','SU-MIMO TDMA 4x1 iid',...
    'MU-MIMO MMSE 4x[1,1,1,1] meas','MU-MIMO DPC 4x[1,1,1,1] meas','SU-MIMO TDMA 4x1 meas',...
    'Location','NorthWest')
%set(gca,'Fontsize',14)
xlabel('SNR [dB]','Fontsize',14,'Fontweight','bold')
ylabel('Sum rate','Fontsize',14,'Fontweight','bold')
grid on
saveas(h_fig,'figs_cost/idx_32_ecap.eps','epsc2');


%%
ECap_4U_MMSE_iid(8) - ECap_4U_MMSE(8)
ECap_4U_DPC_iid(8) - ECap_4U_DPC(8)
ECap_SU_TDMA_CL_ML_iid(8) - ECap_SU_4x1_TDMA_CL_ML(8)

ECap_4U_DPC(8) - ECap_4U_MMSE(8)
ECap_4U_DPC(8) - ECap_SU_4x1_TDMA_CL_ML(8)
ECap_4U_MMSE(8) - ECap_SU_4x1_TDMA_CL_ML(8)

ECap_4U_DPC(8)./ECap_4U_MMSE(8)
ECap_4U_DPC(8)./ECap_SU_4x1_TDMA_CL_ML(8)
ECap_4U_MMSE(8)./ECap_SU_4x1_TDMA_CL_ML(8)

