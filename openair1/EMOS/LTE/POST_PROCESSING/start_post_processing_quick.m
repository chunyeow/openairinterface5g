close all
clear all

addpath('../IMPORT_FILTER')
addpath('maps')
addpath('CapCom')
decimation = 100;
struct_template;

root_path = '/emos/AMBIALET/Mode2/';
%root_path = '/media/Iomega_HDD/EMOS/data/20100702/';
%root_path = '/extras/kaltenbe/CNES/emos_preprocessed_data_penne/';
%root_path = '/media/Iomega_HDD-1/';
%root_path = '/media/Iomega_HDD/EMOS/data/';

mm='ambialet';

d = dir(fullfile(root_path, '*extended*'));
dir_names = {d.name};
%dir_names = {'20100616_Coverage_Runs'};

%% post processing for nomadic points
% 
% for i=1:length(dir_names)
%     pathname = fullfile(root_path,dir_names{i},'nomadic');
%     if exist(pathname,'dir')
%         post_processing_UE_nomadic_quick
%     end
% end
% 
% %eval_nomadic_points
% 

 
%% post processing for vehicular measurements
for i=1:length(dir_names)
    pathname = fullfile(root_path,dir_names{i});
    post_processing_UE_quick
    post_processing_eNb_quick
    get_gps_coordinates_eNB
end

%% plot
for i=1:length(dir_names)
    pathname = fullfile(root_path,dir_names{i});
    disp(pathname);
    h_fig = 0;
    plot_results_UE_quick
    plot_results_eNb_quick
    %plot_results_eNb_aligned
end
