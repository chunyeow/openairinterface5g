clear all
close all

filepath = '.';

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

%setpref('Internet','SMTP_Server','mailhost.eurecom.fr');

addpath('/homes/kaltenbe/Devel/openair_trunk/openair1/EMOS/MATLAB/')
addpath('/homes/kaltenbe/Devel/openair_trunk/openair1/EMOS/MATLAB/lim_feedback/')
addpath('/homes/kaltenbe/Devel/openair_trunk/openair1/EMOS/MATLAB/import_filter/')
addpath('/homes/kaltenbe/Devel/openair_trunk/openair1/EMOS/MATLAB/MAC_to_BC/')


%%
% Measurement 1
cd /extras/kaltenbe/EMOS/data/20071126_MU_Outdoor/
% file_indices = [27 28 32 33 50 51 52];
% file_indices = [32 50 51];
% file_indices = [34 37 38 40];
file_indices = 32;
% SNRdB = 0:5:15;
SNRdB = 10;
IFversion = 0.3;
sched_users = 1:4;
avail_users = 1:4;
NTx = 4;
NRx = 2;
NUser = 4;

% % Measurement 2
% cd /extras/kaltenbe/EMOS/data/20080205_MU_Indoor_EN_EC
% % file_indices = [82:85 87:90 100 1 2];
% file_indices = 100;
% IFversion = 0.3;
% SNRdB = 10;
% sched_users = [1:4];
% avail_users = [1:4];
% NUser = 4;
% NTx = 4;
% NRx = 2;

% % Measurement 3
% cd /extras/kaltenbe/EMOS/data/20080213_MU_Outdoor
% % file_indices = 45:55;
% % file_indices = 52;
% file_indices = 50;
% IFversion = 0.4;
% SNRdB = 10; %0:5:15;
% sched_users = [1:4];
% avail_users = [1:4];
% NTx = 4;
% NRx = 2;
% NUser = 4;

% Measurement 4
%cd /extras/kaltenbe/EMOS/data/20080617_WHERE
% % file_indices = 45:55;
% file_indices = 62:70;
% IFversion = 0.4;
% SNRdB = 10;
% avail_users = [1:2];
% sched_users = [1:2];
% NUser = 2;

% % Measurement 20081110 
% cd /extras/kaltenbe/EMOS/data/20081110_MU_Outdoor/
% file_indices = 25:29;
% % SNRdB = 0:5:15;
% SNRdB = 10;
% IFversion = 2.0;
% sched_users = 1:2;
% avail_users = 3:4;
% NTx = 2;
% NRx = 2;
% NUser = 2;

load('metadata_MU.mat');

%%
NIdx = length(file_indices);

warning('off','MATLAB:nearlySingularMatrix')
warning('off','MATLAB:SingularMatrix')

for idx = file_indices
    disp(sprintf('[eval_data_MU] Processing file %d of %d',idx, NIdx));
    
    if IFversion>=2
        for k=1:size(est,1)
            est(k,idx).err_ind = -est(k,idx).crc_status;
        end
    end

    %eval_corr_coef
    eval_data_MU
    %eval_data_MU_DPC
    %eval_data_MU_RVQ
    %eval_data_MU_CVQ
    %eval_data_MU_RVQ_multiSNR
end

warning('on','MATLAB:nearlySingularMatrix')
warning('on','MATLAB:SingularMatrix')
