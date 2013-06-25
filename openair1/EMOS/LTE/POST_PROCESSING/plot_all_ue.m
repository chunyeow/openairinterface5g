% file: plot_all.m
% author: florian.kaltenberger@eurecom.fr
% purpose: plots the results from the UE for several directories
% requires pathname to be set to the right directory

close all
clear all

% in the following two lines set the root path and select the subfolders
% you want to plot
root_path = '/emos/AMBIALET/Mode2';  
d = dir(fullfile(root_path, '*MODE2*'));
dir_names = {d.name};

% set to the output directory
pathname = '/emos/AMBIALET/Mode2/results';

mm='ambialet';
decimation = 100;
dim = zeros(1,length(dir_names));

for i=1:length(dir_names)
    if exist(fullfile(root_path,dir_names{i},'results_UE.mat'),'file')
        temp = load(fullfile(root_path,dir_names{i},'results_UE.mat'));
        if (i==1)
            nn = fieldnames(temp);
            for n = 1:length(nn)
                eval([nn{n} '= [];']);
                [dummy, dim(n)] = eval(['max(size(temp.' nn{n} '))']); 
            end
        end
        for n = 1:length(nn)
            eval([nn{n} '   = cat(' num2str(dim(n)) ', ' nn{n} ', temp.' nn{n} ');']);
        end
            
        %results_UE = cat(1,results_UE,temp);
        fprintf('%d/%d\n',i,length(dir_names));
    end
%    if exist(fullfile(root_path,dir_names{i},'nomadic','results_UE_new.mat'),'file')
%        nomadic = load(fullfile(root_path,dir_names{i},'nomadic','results_UE_new.mat'));
%        results_UE_nomadic = cat(2,results_UE_nomadic,nomadic.minestimates);
%         %phy_measurements_cat = [phy_measurements_cat temp.phy_measurements_cat];
%         timestamp_cat = [timestamp_cat temp.timestamp_cat];
%         frame_tx_cat = [frame_tx_cat temp.frame_tx_cat];
%         rx_rssi_dBm_cat = [rx_rssi_dBm_cat; temp.rx_rssi_dBm_cat];
%         pbch_fer_cat = [pbch_fer_cat; temp.pbch_fer_cat];
%         dlsch_fer_cat = [dlsch_fer_cat; temp.dlsch_fer_cat];
%         mcs_cat = [mcs_cat; temp.mcs_cat];
%         %tbs_cat = [tbs_cat; temp.tbs_cat];
%         UE_mode_cat = [UE_mode_cat temp.UE_mode_cat];
%         gps_lon_cat = [gps_lon_cat temp.gps_lon_cat];
%         gps_lat_cat = [gps_lat_cat temp.gps_lat_cat];
%         %gps_time_cat = [gps_time_cat [gps_data.timestamp]];
        
%    end
end

% %%
% table = [[results_UE.gps_lon_cat]; [results_UE.gps_lat_cat]; double([results_UE.UE_mode_cat])].';
% dlmwrite('UE_mode_all.csv',table,'delimiter',';','precision','%10.8f');
% dlmwrite('UE_mode_2and3.csv',table([results_UE.UE_mode_cat]==3 | [results_UE.UE_mode_cat]==2,:),'delimiter',';','precision','%10.8f');

save(fullfile(pathname,'results_UE.mat'),nn{:});

nomadic_flag = false;
h_fig = 0;

plot_results_UE_quick_common

