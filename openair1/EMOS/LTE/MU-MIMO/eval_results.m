%% Script for reading in the EMOS input
clear all;
%close all;

addpath('E:\Synchro\kaltenbe\My Documents\openair4G\openair1\EMOS\LTE\IMPORT_FILTER')

% 2rx
% [estimates,NFrames]=load_estimates_lte_new('E:\Synchro\kaltenbe\My Documents\openair4G\openair1\EMOS\DUMP\UE_data_20130315_142130.EMOS',inf,1,0);
% 1rx
% estimates = load_estimates_lte_new('E:\EMOS\globecom\UE_data_20130315_155139.EMOS', inf, 1, 0);

% 2rx long test
% estimates = load_estimates_lte_new('/home/wagner/GC/UE_data_20130315_142130.EMOS', 250000, 1, 0);
% 1rx short test
% estimates = load_estimates_lte_new('/home/wagner/GC/UE_data_20130315_153101.EMOS', inf, 1, 0);

% ITJ 
%Nrx = 1;
%estimates = load_estimates_lte_new('E:\EMOS\ITJ (globecom bis)\UE_data_20130916_175331.EMOS', inf, 1, 0, 0);
Nrx = 2;
%estimates = load_estimates_lte_new('E:\EMOS\ITJ (globecom bis)\UE_data_20130916_174719.EMOS', inf, 1, 0, 0);
%estimates = load_estimates_lte_new('E:\EMOS\ITJ (globecom bis)\UE_data_20130924_143804.EMOS', inf, 1, 0, 0);
%estimates = load_estimates_lte_new('E:\EMOS\ITJ (globecom bis)\UE_data_20130924_143250.EMOS', inf, 1, 0, 0);

% outdoor messungen 2013-10-10
filepath = 'E:\EMOS\MU-MIMO\ITJ3\';
filename = 'UE_data_20131030_143345';
estimates = load_estimates_lte_new([filepath filename '.EMOS'], inf, 1, 0, 0);

%% frame
frame_rx = double([estimates.frame_rx]);
%throughput = [0 diff(double([estimates.total_TBS]))*100];
throughput = double([estimates.bitrate]);
mcs = double([estimates.mcs]);
use_iu_receiver = ([estimates.use_ia_receiver]==0);
use_ia_receiver = ([estimates.use_ia_receiver]==1);
use_iag_receiver = ([estimates.use_ia_receiver]==2);


%% average
avg_tp_ia = zeros(28,1);
avg_tp_iag = zeros(28,1);
avg_tp_iu = zeros(28,1);
num_ia = zeros(28,1);
num_iag = zeros(28,1);
num_iu = zeros(28,1);

for imcs=0:1:27
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
end


%% Plot
figure(2) 
hold off
colormap hot
bar(0:1:27,[avg_tp_iu avg_tp_ia avg_tp_iag])
grid on
xlim([-1 28])
h = legend('IU Receiver','IA Receiver','IA genie');
set(h,'FontSize',14);
xlabel('MCS','FontSize',14)
ylabel('Throughput [bps]','FontSize',14)

%saveas(gcf, filename, 'eps')
%save(filename,'avg_tp*', 'mcs*', 'throughput*', 'use*', 'frame*');
%csvwrite([filename '.csv'],[(0:1:27).' avg_tp_iu avg_tp_ia avg_tp_iag])