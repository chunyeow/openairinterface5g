%load 'bler_awgn.mat';

set(0, 'DefaultLineMarkerSize', 10);
set(0, 'Defaultaxesfontsize', 14);
set(0, 'DefaultLineLineWidth', 2);
set(0, 'DefaultAxesFontName', 'Courier');
set(0, 'DefaultTextFontName', 'Courier');

h_fig = figure(2);
hold off
plot_style = {'b-o','g-x','r-+','c-*','m-s','y-d','k-^',...
    'b--o','g--x','r--+','c--*','m--s','y--d','k-^',...
    'b-.o','g-.x','r-.+','c-.*','m-.s','y-.d','k-.^',...
    'b:o','g:x','r:+','c:*','m:s','y:d','k:^'};
for i=0:24
    eval(sprintf('bler_%d;',i));
    semilogy(bler(:,1), bler(:,2),plot_style{i+1})
    hold on
end
grid on
h = legend('mcs0(QPSK)','mcs1(QPSK)','mcs2(QPSK)','mcs3(QPSK)','mcs4(QPSK)',...
       'mcs5(QPSK)','mcs6(QPSK)','mcs7(QPSK)','mcs8(QPSK)','mcs9(QPSK)',...
       'mcs10(16QAM)','mcs11(16QAM)','mcs12(16QAM)','mcs13(16QAM)',...
       'mcs14(16QAM)','mcs15(16QAM)','mcs16(16QAM)','mcs17(64QAM)',...
       'mcs18(64QAM)','mcs19(64QAM)','mcs20(64QAM)','mcs21(64QAM)',...
       'mcs22(64QAM)','mcs23(64QAM)','mcs24(64QAM)',...
       'location','eastoutside');
set(h,'FontSize',10);
title 'SISO, AWGN, perfect channel estimation'
ylabel 'BLER'
xlabel 'SNR'
saveas(h_fig,'siso_awgn_perfectCE.eps','epsc2');


%% 
load SISO.mat

scale_bps = 2400*6*100;
c_siso = [c_siso_4Qam; c_siso_16Qam; c_siso_64Qam];
bps_siso = c_siso.*scale_bps;

snr_all = -10:0.2:20;
bler_all = zeros(length(snr_all),22);
for i=0:25
    eval(sprintf('bler = bler%d;',i));
    bler_all(:,i+1) = interp1(bler(:,1), (1-bler(:,2))*get_tbs(i,25)*6*100,snr_all,'nearest','extrap');
end

figure(1);
hold off
plot(snr_all,max(bler_all,[],2))
hold on
plot(SNR,max(bps_siso),'r--');


