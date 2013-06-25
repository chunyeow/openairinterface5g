close all
clear all

addpath('../IMPORT_FILTER')
addpath('maps')
addpath('CapCom')
decimation = 100;
struct_template;

%root_path = '/emos/EMOS/Nomadic/';
%root_path = '/emos/AMBIALET/Nomadic/';
root_path = '/media/disk/PENNE/Nomadic/';

%mm='cordes';
%mm='ambialet';
mm='penne';

d = dir(fullfile(root_path, '*nomadic*'));
dir_names = {d.name};
id=[d.isdir];
dir_names(~id) = [];

%% post processing for nomadic points

for i=1:length(dir_names)
    pathname = fullfile(root_path,dir_names{i});
    post_processing_UE_nomadic_quick
    post_processing_eNB_nomadic_quick
    eval_nomadic_points
end




 
