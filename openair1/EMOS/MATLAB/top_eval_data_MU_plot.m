clear all
close all

filepath = '';

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

%% load data
disp('[eval_data_MU] loading metadata');
load('metadata_MU.mat');
disp('[eval_data_MU] loading image file');
mm = imread('E:\2004_fd0006_250_c_0485.tif');

%% generate figures and latex files 
NTx = 4;
%NFrames_max = 1000;
NUser = size(filename_emos,1);
%file_indices = [12 13 14 15 27 28 29 30];
file_indices = [27 28 32 33 50 51 52];
NIdx = length(file_indices);
SNR = 10;

if ~exist('figs','dir')
    mkdir('figs')
end
if ~exist('tex','dir')
    mkdir('tex')
end

fdw = fopen('tex/results.tex','w');

for idx = file_indices
    disp(sprintf('[eval_data_MU] Processing file %d of %d',idx, NIdx));
    %eval_data_MU_plot
    %EMOS_generate_LaTex_file
    fprintf(fdw,'\\include{idx_%d}\n',idx);
end

fclose(fdw);

%% compile the latex source  
cd tex
system('latex SFR_4th_report.tex');
system('dvips SFR_4th_report.dvi');
system('ps2pfd SFR_4th_report.ps');