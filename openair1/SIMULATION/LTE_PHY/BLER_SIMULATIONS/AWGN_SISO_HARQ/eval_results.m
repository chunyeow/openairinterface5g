set(0, 'DefaultLineMarkerSize', 10);
set(0, 'Defaultaxesfontsize', 14);
set(0, 'DefaultLineLineWidth', 2);

%%
load SISO.mat

scale_bps = 2400*6*100;
c_siso = [c_siso_4Qam; c_siso_16Qam; c_siso_64Qam];
bps_siso = c_siso.*scale_bps;

snr_all = -20:0.2:40;
throughput_all = zeros(length(snr_all),22);

plot_style = {'b-*';'r-*';'g-*';'y-*';'k-*';...
    'b-o';'r-o';'g-o';'y-o';'k-o';...
    'b-s';'r-s';'g-s';'y-s';'k-s';...
    'b-d';'r-d';'g-d';'y-d';'k-d';...
    'b-x';'r-x';'g-x';'y-x';'k-x';...
    'b-+';'r-+';'g-+';'y-+';'k-+'};


%%
figure(1)
hold off
for mcs=0:23
    data = dlmread(sprintf('bler_%d.csv',mcs),';',1,0);
    snr = data(:,1);
    bler = data(:,5)./data(:,6); % round 1
    bler4 = data(:,11)./data(:,6); % round 4
    harq_adjust = data(:,6)./sum(data(:,6:2:12),2);
    throughput_all(:,mcs+1) = interp1(snr, (1-bler4).*harq_adjust.*get_tbs(mcs,25)*6*100,snr_all,'nearest','extrap');
    throughput_all(1:find(snr_all==snr(1)),mcs+1) = 0;
    semilogy(snr,bler(:,1),plot_style{mcs+1});
    hold on
end
legend('mcs0(QPSK)','mcs1(QPSK)','mcs2(QPSK)','mcs3(QPSK)','mcs4(QPSK)',...
       'mcs5(QPSK)','mcs6(QPSK)','mcs7(QPSK)','mcs8(QPSK)','mcs9(QPSK)',...
       'mcs10(16QAM)','mcs11(16QAM)','mcs12(16QAM)','mcs13(16QAM)',...
       'mcs14(16QAM)','mcs15(16QAM)','mcs16(16QAM)','mcs17(64QAM)',...
       'mcs18(64QAM)','mcs19(64QAM)','mcs20(64QAM)','mcs21(64QAM)',...
       'mcs22(64QAM)','mcs23(64QAM)','mcs24(64QAM)','mcs25(64QAM)',...
       'mcs26(64QAM)','mcs27(64QAM)','location','westoutside');
title 'BLER Vs. SNR for SISO LTE with respect to MCS'
ylabel 'BLER'
xlabel 'SNR'
grid on

%%
h_fig = figure(2);
hold off
plot(snr_all,smooth(max(throughput_all,[],2),5))
%plot(snr_all,throughput_all)
hold on
plot(SNR,max(bps_siso),'r--');
legend('simulation','mutual information','location','northwest');
xlabel('SNR [dB]')
ylabel('Throughput [bps]')
grid on

saveas(h_fig,'implementation_loss.eps','epsc2');

%%
tp_sim = max(throughput_all,[],2);
tp_mi = max(bps_siso);
snr_sim = snr_all;
snr_mi = SNR;
save SISO_scaling.mat tp_sim tp_mi snr_sim snr_mi
