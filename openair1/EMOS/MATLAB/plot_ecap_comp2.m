clear all
close all


%% plot iid & measured
h_fig = figure(2);
hold off
load cap_iid_NTx4_NRx1_NUser2.mat

plot(SNRdB,ECap_MU_ZF_iid*0.8,'r--','Linewidth',1.5);
hold on
plot(SNRdB,ECap_SU_TDMA_CL_ML_iid*0.8,'b-','Linewidth',1.5);

load results_multiSNR_4x1_idx_32.mat
ECap_2U_ZF = mean(reshape(CAP_2U_ZF,[],length(SNRdB)),1);
ECap_2U_MMSE = mean(reshape(CAP_2U_MMSE,[],length(SNRdB)),1);

load results_DPC_SNR=-20-50_idx_32
SNRdB = -20:10:30;
ECap_4U_DPC = mean(reshape(CAP_4U_4x1_DPC,[],size(CAP_4U_4x1_DPC,3)),1);
ECap_SU_4x1_TDMA_CL_ML = mean(CAP_SU_4x1_TDMA_CL_ML,1);
ECap_SU_4x2_TDMA_CL_ML = mean(CAP_SU_4x2_TDMA_CL_ML,1);

plot(SNRdB,ECap_2U_ZF(1:length(SNRdB))*0.8,'r--','Linewidth',3);
plot(SNRdB,ECap_SU_4x1_TDMA_CL_ML(1:length(SNRdB))*0.8,'b-','Linewidth',3);

legend('MU-MIMO ZF 4x[1,1] iid','SU-MISO TDMA 4x1 iid',...
    'MU-MIMO ZF 4x[1,1] meas','SU-MISO TDMA 4x1 meas',...
    'Location','NorthWest')
%set(gca,'Fontsize',14)
xlabel('SNR [dB]','Fontsize',12,'Fontweight','bold')
ylabel('Sum rate [bits/sec/Hz]','Fontsize',12,'Fontweight','bold')
grid on
saveas(h_fig,'figs_lte/idx_32_ecap.eps','epsc2');


%%
% ECap_4U_MMSE_iid(8) - ECap_4U_MMSE(8)
% ECap_4U_DPC_iid(8) - ECap_4U_DPC(8)
% ECap_SU_TDMA_CL_ML_iid(8) - ECap_SU_4x1_TDMA_CL_ML(8)
% 
% ECap_4U_DPC(8) - ECap_4U_MMSE(8)
% ECap_4U_DPC(8) - ECap_SU_4x1_TDMA_CL_ML(8)
% ECap_4U_MMSE(8) - ECap_SU_4x1_TDMA_CL_ML(8)
% 
% ECap_4U_DPC(8)./ECap_4U_MMSE(8)
% ECap_4U_DPC(8)./ECap_SU_4x1_TDMA_CL_ML(8)
% ECap_4U_MMSE(8)./ECap_SU_4x1_TDMA_CL_ML(8)
% 
