% file: plot_figures_top.m
% this file reads the data post processed measurement data and procudes all
% the figures requested by Mr. Scot. The comparison between different modes
% is done with the script plot_comparison_all_modes.m and
% distance_travelled_comparison.m
clear all
close all

%decimation = 1;
%NFrames_max = 100*60*10;
mm='penne';

h_fig = 0;

%% results from quick post processing (modem throughput)
%pathname = '/media/disk/PENNE/Interference/results';
%plot_results_UE_quick
%plot_results_eNb_quick

%% results from raw channel estimates (ideal throughput)
pathname = '/media/disk/PENNE/Mode2/results/';
%Concatenate_results_UE_quick
plot_results_cat

%% results from quick post processing (modem throughput)
pathname = '/media/disk/PENNE/Mode1/results';
plot_results_UE_quick
plot_results_eNb_quick

%%
pathname = '/media/disk/PENNE/Mode2/results';
plot_results_UE_quick
plot_results_eNb_quick

%%
pathname = '/media/disk/PENNE/Mode2_OFDMA/results/';
plot_results_UE_quick
plot_results_eNb_quick

%%
pathname = '/media/disk/PENNE/Mode6/20100702_VTP_Mode6/';
plot_results_UE_quick
plot_results_eNb_quick


