% file: plot_figures_top.m
% this file reads the data post processed measurement data and procudes all
% the figures requested by Mr. Scot. The comparison between different modes
% is done with the script plot_comparison_all_modes.m and
% mode1_mode2_comparison.m
clear all
close all

%decimation = 1;
%NFrames_max = 100*60*10;
mm='cordes';

%% results from raw channel estimates (ideal throughput)
%pathname = '/emos/EMOS/Mode2/20100520_mode2_parcours1_part4_part5/';
pathname = '/extras/kaltenbe/CNES/emos_postprocessed_data/Mode2/results';
%Concatenate_results_UE_quick
plot_results_cat

%% results from quick post processing (modem throughput)
pathname = '/extras/kaltenbe/CNES/emos_postprocessed_data/Mode1/results';
plot_results_ue_quick

%%
pathname = '/extras/kaltenbe/CNES/emos_postprocessed_data/Mode2/results';
plot_results_ue_quick

%%
pathname = '/extras/kaltenbe/CNES/emos_postprocessed_data/Mode2_update/results';
plot_results_ue_quick

%%
pathname = '/extras/kaltenbe/CNES/emos_postprocessed_data/Mode6/results';
plot_results_ue_quick
plot_results_eNB_quick


