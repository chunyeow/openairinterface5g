%% Script for reading in the EMOS input
clear all;
close all;

addpath('E:\Synchro\kaltenbe\My Documents\openair4G\openair1\EMOS\LTE\IMPORT_FILTER')

% outdoor messungen 2013-10-10 fuer ITJ paper
%filepath = 'E:\EMOS\MU-MIMO\ITJ3\';
%filename = 'UE_data_20131030_143345';

% indoor messungen 20140220 fuer SAM paper
filepath = 'E:\EMOS\MU-MIMO\SAM\';

% outdoor messungen for SAM paper
%filename = 'UE_data_pmi_fixed_all';
filename = 'UE_data_20140220_163107';
%filename = 'UE_data_all';

[estimates, NFrames] = load_estimates_lte_new([filepath filename '.EMOS'], inf, 1, 0, 0);

%% frame
frame_rx = double([estimates.frame_rx]);
%throughput = [0 diff(double([estimates.total_TBS]))*100];
throughput = double([estimates.bitrate]);
mcs = double([estimates.mcs]);
use_iu_receiver = ([estimates.use_ia_receiver]==0);
use_ia_receiver = ([estimates.use_ia_receiver]==1);
use_iag_receiver = ([estimates.use_ia_receiver]==2);

%% phy measurements
SNR_dB = zeros(1,NFrames);
INR_dB = zeros(1,NFrames);
for i=2:NFrames
    SNR_dB(i-1) = estimates(i).phy_measurements.precoded_cqi_dB(1,1);
    INR_dB(i-1) = estimates(i).phy_measurements.precoded_cqi_dB(1,2);
end
SIR_dB = SNR_dB - INR_dB;

%% average
avg_tp_ia = zeros(28,1);
avg_tp_iag = zeros(28,1);
avg_tp_iu = zeros(28,1);
avg_SIR_ia = zeros(28,1);
avg_SIR_iag = zeros(28,1);
num_ia = zeros(28,1);
num_iag = zeros(28,1);
num_iu = zeros(28,1);


for imcs=16 %[0:11 13:25]
    %%
	mcs_window = (mcs==imcs);
	throughput_iag_mcs = throughput(use_iag_receiver & mcs_window);
    num_iag(imcs+1) = sum(use_iag_receiver & mcs_window);
	avg_tp_iag(imcs+1) = mean(throughput_iag_mcs);
	throughput_ia_mcs = throughput(use_ia_receiver & mcs_window);
    num_ia(imcs+1) = sum(use_ia_receiver & mcs_window);
	avg_tp_ia(imcs+1) = mean(throughput_ia_mcs);
	throughput_iu_mcs = throughput(use_iu_receiver & mcs_window);
    num_iu(imcs+1) = sum(use_iu_receiver & mcs_window);
	avg_tp_iu(imcs+1) = mean(throughput_iu_mcs);
    
    SIR_iu_mcs = SIR_dB(use_iu_receiver & mcs_window);
    SIR_ia_mcs = SIR_dB(use_ia_receiver & mcs_window);
    SIR_iag_mcs = SIR_dB(use_iag_receiver & mcs_window);
    avg_SIR_ia(imcs+1) = mean(SIR_ia_mcs);
    avg_SIR_iag(imcs+1) = mean(SIR_iag_mcs);
    
    isir=0;
    for sir=min(SIR_dB):max(SIR_dB)
        ia(1,isir+1) = sum(use_ia_receiver & mcs_window & (SIR_dB==sir));
        if ia(1,isir+1)>=5
            IA(1,isir+1) = mean(throughput(use_ia_receiver & mcs_window & (SIR_dB==sir)));
        else
            IA(1,isir+1) = NaN;
        end
        iu(1,isir+1) = sum(use_iu_receiver & mcs_window & (SIR_dB==sir));        
        if iu(1,isir+1)>=5
            IU(1,isir+1) = mean(throughput(use_iu_receiver & mcs_window & (SIR_dB==sir)));
        else
            IU(1,isir+1) = NaN;
        end
        iag(1,isir+1) = sum(use_iag_receiver & mcs_window & (SIR_dB==sir));        
        if iag(1,isir+1)>=5
            IAG(1,isir+1) = mean(throughput(use_iag_receiver & mcs_window & (SIR_dB==sir)));
        else
            IAG(1,isir+1) = NaN;
        end
        isir = isir+1;
    end
end


%% Plot
f = figure(1); 
hold off
colormap hot
bar(0:1:27,[avg_tp_iu avg_tp_ia avg_tp_iag])
grid on
xlim([-1 28])
h = legend('IU Receiver','IA Receiver','IA genie');
set(h,'FontSize',14);
xlabel('MCS','FontSize',14)
ylabel('Throughput [bps]','FontSize',14)

saveas(f, ['tp_vs_mcs' filename '.eps'], 'epsc2')
%save(filename,'avg_tp*', 'mcs*', 'throughput*', 'use*', 'frame*');
%csvwrite([filename '.csv'],[(0:1:27).' avg_tp_iu avg_tp_ia avg_tp_iag])

%% Plot2
plot_style = {'b-*';'r-*';'g-*';'y-*';'k-*';...
    'b-o';'r-o';'g-o';'y-o';'k-o';...
    'b-s';'r-s';'g-s';'y-s';'k-s';...
    'b-d';'r-d';'g-d';'y-d';'k-d';...
    'b-x';'r-x';'g-x';'y-x';'k-x';...
    'b-+';'r-+';'g-+';'y-+';'k-+'};
legend_str = {};
f = figure(2);
hold off
plot(min(SIR_dB):max(SIR_dB),IAG/1000,'r-x','Markersize',10)
hold on
plot(min(SIR_dB):max(SIR_dB),IA/1000,'b--o')
plot(min(SIR_dB):max(SIR_dB),IU/1000,'k:s')
h=legend('NA-IA','IA','IU','Location','NorthWest');
set(h,'FontSize',14);
%for imcs=0:27
    %plot(avg_SIR_ia(imcs+1),avg_tp_ia(imcs+1)-avg_tp_iu(imcs+1),plot_style{imcs+1},'Markersize',10);
    %hold on
    %legend_str{imcs+1} = sprintf('mcs %d',imcs);    
%end
%legend(legend_str)
xlabel('SIR [dB]');
ylabel('Throughput [kbps]')
saveas(f, ['tp_vs_sir' filename '.eps'], 'epsc2')

%% Plot 3
f = figure(3);
hold off
bar(min(SIR_dB):max(SIR_dB),[ia; iu; iag].')
h=legend('NA-IA','IA','IU','Location','NorthWest');
set(h,'FontSize',14);
xlabel('SIR [dB]');
ylabel('Occurrence');
shading flat
% plot([0:4], X(15:2:23,:).')
% legend('mcs 14', 'mcs 16', 'mcs 18', 'mcs 20', 'mcs 22')
% xlabel('SIR [dB]')
% ylabel('delta TP [bps]')
saveas(f,['hist_sir' filename '.eps'], 'epsc2')