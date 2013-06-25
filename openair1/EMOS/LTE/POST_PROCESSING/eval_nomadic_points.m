% file: eval_nomadic_points.m
% author: florian.kaltenberger@eurecom.fr
% purpose: plots the nomadic results from the UE for several directories

% in the following two lines set the root path and select the subfolders
% you want to plot
% root_path = '~/EMOS/data/';  
% d = dir(fullfile(root_path, '*mode1_parcours1'));
% dir_names = {d.name};

%%
% decimation = 100;
% 
% results_UE = [];
% results_UE_nomadic = [];
% 
% for i=1:length(dir_names)
%     if exist(fullfile(root_path,dir_names{i},'results_UE_new.mat'),'file')
%         nomadic = load(fullfile(root_path,dir_names{i},'results_UE_new.mat'));
%         results_UE_nomadic = cat(2,results_UE_nomadic,nomadic.minestimates);
%     end
% end

%pathname='/emos/EMOS/Nomadic/20100611_NOMADIC_ALL_MODES/';
%pathname='/emos/EMOS/Nomadic/20100527_nomadic_all_modes/';
load(fullfile(pathname,'results_UE_new.mat'));
results_UE_nomadic=minestimates;
load(fullfile(pathname,'results_eNB_new.mat'));
results_eNB_nomadic=minestimates;

%%
h_fig = figure(1);
hold off
%plot_gps_coordinates(mm,[results_UE.gps_lon_cat], [results_UE.gps_lat_cat], double([results_UE.UE_mode_cat]));
%hold on
for i=1:length(results_UE_nomadic)
    if i==1 
        mmi = mm;
    else
        mmi = '';
    end
    plot_gps_coordinates(mmi,[results_UE_nomadic(i).gps_data.longitude], [results_UE_nomadic(i).gps_data.latitude], ...
        double(results_UE_nomadic(i).UE_mode_cat),[0 3], sprintf('N%d',i),'black','Marker','.','Markersize',20);
end
saveas(h_fig,fullfile(pathname,'UE_mode_nomadic.jpg'),'jpg');

%%
h_fig = figure(2);
hold off
%rx_rssi_dBm_cat = cat(1,results_UE.rx_rssi_dBm_cat);
%plot_gps_coordinates(mm,[results_UE.gps_lon_cat], [results_UE.gps_lat_cat], double(rx_rssi_dBm_cat(:,1)));
%hold on
for i=1:length(results_UE_nomadic)
    if i==1 
        mmi = mm;
    else
        mmi = '';
    end
    plot_gps_coordinates(mmi,[results_UE_nomadic(i).gps_data.longitude], [results_UE_nomadic(i).gps_data.latitude], ...
        double(results_UE_nomadic(i).rx_rssi_dBm(:,1)),[-110 -30], sprintf('N%d',i),'black','Marker','.','Markersize',20);
end
saveas(h_fig,fullfile(pathname,'RX_RSSI_nomadic.jpg'),'jpg');

%%
fid = fopen(fullfile(pathname,'nomadic_results.csv'),'w');
for i=1:length(results_UE_nomadic)
    rx_rssi_dBm_mean(i) = mean(results_UE_nomadic(i).rx_rssi_dBm(:,1));

    ul_rssi_all_sectors = zeros(size(results_eNB_nomadic(i).phy_measurements));
    for j=1:length(results_eNB_nomadic(i).phy_measurements)
        ul_rssi_all_sectors(j,:) = double([results_eNB_nomadic(i).phy_measurements(j,:).rx_rssi_dBm]);
    end
    ul_rssi_dBm = zeros(length(results_eNB_nomadic(i).eNb_UE_stats),1);
    sector = [results_eNB_nomadic(i).eNb_UE_stats.sector];
    for j=1:length(results_eNB_nomadic(i).eNb_UE_stats)
        ul_rssi_dBm(j) = ul_rssi_all_sectors(j,sector(j)+1);
    end
    ul_rssi_dBm_mean(i) = mean(ul_rssi_dBm);
    

    UE_synched = (results_UE_nomadic(i).UE_mode_cat>0);
    UE_connected = (results_UE_nomadic(i).UE_mode_cat==3);
    UE_mimo_mode = (results_UE_nomadic(i).mimo_mode);
    
    eNB_synched = ([results_eNB_nomadic(i).eNb_UE_stats.UE_mode]>0);
    eNB_connected = ([results_eNB_nomadic(i).eNb_UE_stats.UE_mode]==3);
    eNB_mimo_mode = (results_eNB_nomadic(i).mimo_mode);
    
    good = (results_UE_nomadic(i).dlsch_fer<=100 & results_UE_nomadic(i).dlsch_fer>=0).';
    dlsch_throughput = double(100-results_UE_nomadic(i).dlsch_fer) .* double(results_UE_nomadic(i).tbs_cat) .* 6;
    
    results_eNB_nomadic(i).ulsch_fer = [100 diff([results_eNB_nomadic(i).ulsch_errors])];
    ulsch_throughput = double([results_eNB_nomadic(i).tbs_cat]) .* double(100 - results_eNB_nomadic(i).ulsch_fer) .* 3;
       
    lat = [results_UE_nomadic(i).gps_data.latitude];
    lon = [results_UE_nomadic(i).gps_data.longitude];
    if (i==1)
        fprintf(fid,'index; DL name; UL name; DL RSSI; UL RSSI; Mode1 TP; Mode2 TP; Mode6 TP; UL TP; Mode1 points; Mode2 points; Mode6 points; UL points; Lat; Lon\n');
    end
    fprintf(fid,'%d; %s; %s; %f; %f; %f; %f; %f; %f; %d; %d; %d; %d; %f; %f\n',i, ...
        results_UE_nomadic(i).filename, results_eNB_nomadic(i).filename, ...
        rx_rssi_dBm_mean(i), ul_rssi_dBm_mean(i), ...
        mean(dlsch_throughput(UE_connected & good & UE_mimo_mode==1)), ...
        mean(dlsch_throughput(UE_connected & good & UE_mimo_mode==2)), ...
        mean(dlsch_throughput(UE_connected & good & UE_mimo_mode==6)), ...
        mean(ulsch_throughput(eNB_connected)), ...
        sum(UE_connected & good & UE_mimo_mode==1), ...
        sum(UE_connected & good & UE_mimo_mode==2), ...
        sum(UE_connected & good & UE_mimo_mode==6), ...
        sum(eNB_connected), ...
        mean(lat(lat~=0)),mean(lon(lon~=0)));
end
fclose(fid);


%%
figure(3)
hold off
%plot([results_UE.gps_time_cat],[results_UE.UE_mode_cat],'x')
%hold on
gps_data_cat = [results_UE_nomadic.gps_data];
plot([gps_data_cat.timestamp],[results_UE_nomadic.UE_mode_cat],'rx');

%%
figure(4)
hold off
%plot([results_UE.gps_time_cat],rx_rssi_dBm_cat(:,1),'x');
%hold on
rx_rssi_dBm_cat_nomadic = cat(1,results_UE_nomadic.rx_rssi_dBm);
plot([gps_data_cat.timestamp],rx_rssi_dBm_cat_nomadic(:,1),'rx');

%%
figure(5)
hold off
% UE_connected = ([results_UE.UE_mode_cat]==3);
% dlsch_fer_cat = cat(1,results_UE.dlsch_fer_cat).';
% tbs_cat = cat(1,results_UE.tbs_cat).';
% good = (dlsch_fer_cat<=100 & dlsch_fer_cat>=0);
% timebase = [results_UE.gps_time_cat];
% plot(timebase(UE_connected & good),(100-dlsch_fer_cat(UE_connected & good)).*tbs_cat(UE_connected & good)*7*4/3,'x');
% hold on
UE_connected = ([results_UE_nomadic.UE_mode_cat]==3);
dlsch_fer_cat = cat(1,results_UE_nomadic.dlsch_fer).';
tbs_cat = cat(1,results_UE_nomadic.tbs_cat).';
good = (dlsch_fer_cat<=100 & dlsch_fer_cat>=0);
gps_data = [results_UE_nomadic.gps_data];
timebase = [gps_data.timestamp];
plot(timebase(UE_connected & good),(100-dlsch_fer_cat(UE_connected & good)).*tbs_cat(UE_connected & good)*6,'rx');

