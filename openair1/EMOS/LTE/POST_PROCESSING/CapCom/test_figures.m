% file: test_figures
% this file reads the data from exaclty one measurement and procudes all
% the figures requested by Mr. Scot. 
clear all
close all

decimation = 1;
NFrames_max = 100*60*10;
pathname = '/extras/kaltenbe/CNES/emos_raw_data/20100512_mode2_parcours2_part2/';

%%
filename_UE = 'data_term3_idx04_20100512T103328.EMOS';
is_eNb = 0;
version = 1;

[H, H_fq, gps_data, NFrames, minestimates, throughput, SNR, K_fac] = load_estimates_lte_1(fullfile(pathname,filename_UE),NFrames_max,decimation,is_eNb,version);             

save(regexprep((fullfile(pathname,filename_UE,'results_UE.mat')), '.EMOS/results_UE.mat', '_results_UE.mat'), 'gps_data', 'NFrames', 'SNR', 'throughput', 'minestimates', 'K_fac');

%%
Concatenate_results_UE_quick

plot_results_cat

%%
filename_eNB = 'data_term1_idx04_20100512T103520.EMOS';
is_eNb = 1;
version = 0;

[H, H_fq, gps_data, NFrames, minestimates, throughput, SNR, K_fac] = load_estimates_lte_1(fullfile(pathname,filename_eNB),NFrames_max,decimation,is_eNb,version);             

save(regexprep((fullfile(pathname,filename_eNB,'results_eNB.mat')), '.EMOS/results_eNB.mat', '_results_eNB.mat'), 'gps_data', 'NFrames', 'SNR', 'throughput', 'minestimates', 'K_fac');

%%
Concatenate_results_eNB_quick

plot_results_eNb_cat