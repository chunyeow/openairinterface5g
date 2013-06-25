clear all
close all

filepath_figs = 'figs';
filepath_tex = 'tex';

% dd = dir(filepath);
% for d=3:length(dd)
%     dname=num2str(dd(d).name);
%     dsize=num2str(dd(d).bytes);
%     b=findstr(dname,'.EMOS');
%     if ~isempty(b) % if it is a measuremnt file
%         tmp = sscanf(dname,'data_term%d_idx%d_[...].EMOS');
%         term = tmp(1);
%         idx = tmp(2);
%         filenames{term,idx+1} = fullfile(filepath,dname);
%     end
% end

addpath('/homes/kaltenbe/Devel/openair1/EMOS/MATLAB/import_filter');
addpath('/homes/kaltenbe/Devel/openair1/EMOS/MATLAB');

%% Select Measurements
% % MU_Outdoor campaign 20071126
% cd /extras/kaltenbe/EMOS/data/20071126_MU_Outdoor/
% file_indices = [32 50 51];
% file_indices = [32];

% % MU_Indoor campaign 20080129
% file_indices = [15 16 17 23 25 28 29 30 31];

% % MU_Indoor campaign 20080205
% cd /extras/kaltenbe/EMOS/data/20080205_MU_Indoor_EN_EC
% file_indices = [82:85 87:90 100];
% file_indices = 100;

% MU_Outdoor campaign 20080213
cd /extras/kaltenbe/EMOS/data/20080213_MU_Outdoor
% file_indices = [40:44 46:47 49:57 59];
file_indices = [50];

%cd /extras/kaltenbe/EMOS/data/20080618_WHERE/Walking
%cd /extras/kaltenbe/EMOS/data/20080618_WHERE/Driving
%cd /extras/kaltenbe/EMOS/data/20080617_WHERE/
%file_indices = [5];

%% load data
disp('[eval_data_MU] loading metadata');
load('metadata_MU.mat');

% disp('[eval_data_MU] loading image file');
% if isunix
%     mm = imread('/extras/kaltenbe/EMOS/2004_fd0006_250_c_0485.tif');
% else
%     mm = imread('\\extras\kaltenbe\EMOS\2004_fd0006_250_c_0485.tif');
% end    

%% generate figures and latex files 
NTx = 4;
%NFrames_max = 1000;
NUser = size(filename_emos,1);
% the indices of the .EMOS files start with 0 whereas
% the indices of the .mat files start with 1
% here we use the Matlab convention

NIdx = length(file_indices);
SNRdB = 10;

if ~exist(filepath_figs,'dir')
    mkdir(filepath_figs)
end
if ~exist(filepath_tex,'dir')
    mkdir(filepath_tex)
end

%%
%fdw = fopen(fullfile(filepath_tex,'/results.tex'),'w');
%fdp = fopen('tex/presentation.tex','w');
%fprintf(fdw,'\\section{Measurement Results}\n');

for idx = file_indices
%for idx = 1:length(est)
    if ~isempty(est(1,idx).rx_rssi_dBm)
        disp(sprintf('[eval_data_MU] Processing file %d of %d',idx, NIdx));
        plot_gps_data
    end
    plot_data_MU_CVQ
    plot_data_MU_CVQ3
    %plot_data_corrmat
    %pause
    %EMOS_generate_LaTex_file
    %EMOS_generate_LaTex_presentation
    %EMOS_plot_multiple_measurements
    %fprintf(fdw,'\\input{%s/idx_%d.tex}\n',fullfile(pwd,filepath_tex,''),idx);
end

%fclose(fdw);
%fclose(fdp);
 
% %% compile the latex source  
% cd(filepath_tex)
% %system('latex SFR_4th_report.tex');
% system('latex SFR_4th_report.tex');
% system('dvips SFR_4th_report.dvi');
% system('ps2pdf SFR_4th_report.ps');
% cd ..