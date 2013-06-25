% file: plot_results_UE_quick.m
% author: florian.kaltenberger@eurecom.fr
% purpose: plots the results from the UE after post_processing_ue_quick
% requires pathname to be set to the right directory

load(fullfile(pathname,'results_UE.mat'));
if exist(fullfile(pathname,'nomadic','results_UE.mat'),'dir')
    nomadic = load(fullfile(pathname,'nomadic','results_UE.mat'));
    nomadic_flag = true;
else
    nomadic_flag = false;
end

plot_results_UE_quick_common


