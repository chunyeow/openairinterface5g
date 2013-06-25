set(0, 'DefaultLineMarkerSize', 10);
set(0, 'Defaultaxesfontsize', 14);
set(0, 'DefaultLineLineWidth', 2);

%%
root_path = '../AWGN_SISO_Winter_School';

plot_style = {'b-*';'r-*';'g-*';'y-*';'k-*';...
    'b-o';'r-o';'g-o';'y-o';'k-o';...
    'b-s';'r-s';'g-s';'y-s';'k-s';...
    'b-d';'r-d';'g-d';'y-d';'k-d';...
    'b-x';'r-x';'g-x';'y-x';'k-x';...
    'b-+';'r-+';'g-+';'y-+';'k-+'};

figure(1)
hold off
legend_str = {};
i=1;
for mcs=0:25
    file = fullfile(root_path,sprintf('awgn_bler_tx1_mcs%d.csv',mcs));
    data = dlmread(file,';',1,0);
    snr = data(:,1);
    bler = data(:,5)./data(:,6); % round 1
    
    snr_tmp = snr(bler<0.1);
    c_rate(i) = data(1,4);
    snr_10p(i) = snr_tmp(1);
    bler_10p(i) = bler(snr==snr_10p(i));
    figure(1)
    semilogy(snr,bler(:,1),plot_style{mcs+1});
    hold on
    legend_str{i} = sprintf('mcs %d',mcs);
    i=i+1;
end

%%

figure;
h_leg = legend(legend_str,'location','eastoutside');
set(h_leg,'FontSize',10);
title('AWGN Performance Curves for LTE 5 MhZ');
ylabel 'BLER'
xlabel 'SNR'
ylim([0.001 1])
grid on

figure;
plot(c_rate(1:10), snr_10p(1:10),'r-x');
hold on
grid on
plot(c_rate(11:17), snr_10p(11:17),'b-x');
plot(c_rate(18:26), snr_10p(18:26),'g-x');
set(gca,'YTick',[-5:16])
set(gca,'XTick',[0.1:0.05:1])
title('SNR (dB) needed to achieve BLER of 10% in AWGN');
ylabel 'SNR'
xlabel 'code rate'

