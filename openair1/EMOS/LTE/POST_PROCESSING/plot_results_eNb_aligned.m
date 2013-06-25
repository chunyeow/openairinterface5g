%% load data
estimates_UE = load(fullfile(pathname,'results_UE.mat'));
estimates_eNB = load(fullfile(pathname,'results_eNB.mat'));
mm='cordes';

%% align the gps measurements of the UE with the eNB
% calculate the align matrix
framestamp_min = floor(min(estimates_UE.frame_tx_cat(1),estimates_eNB.frame_tx_cat(1))/decimation);
framestamp_max = floor(max(estimates_UE.frame_tx_cat(end),estimates_eNB.frame_tx_cat(end))/decimation);
NFrames = framestamp_max-framestamp_min+1;

align_matrix = false(2,NFrames);
align_matrix(1,floor(estimates_eNB.frame_tx_cat/decimation) - framestamp_min + 1) = true;
% the UE might contain frames with the same frame number or corrupt frame numbers. 
% In this loop we find them and save their indices. They are deemed corrupt
% if the current is smaller than the previous or if it is bigger or smaller
% than framestamp_min or framestamp_max respectively
framestamp_last = -1;
UE_duplicates = false(1,length(estimates_UE.frame_tx_cat));
for i=1:length(estimates_UE.frame_tx_cat)
    framestamp = floor(estimates_UE.frame_tx_cat(i)/decimation);
    if ((framestamp<=framestamp_last) || (framestamp>framestamp_max) || (framestamp<framestamp_min)) 
        UE_duplicates(i) = true;
    else
        align_matrix(2, framestamp - framestamp_min + 1) = true; 
    end
    framestamp_last = framestamp;
end
eNB_aligned = align_matrix(1,:);
UE_aligned  = align_matrix(2,:);

% calculate the indices where the ue and the eNB were connected (they might
% not have set the flag in the same frame)
UE_connected = (estimates_UE.UE_mode_cat);
eNB_connected = ([estimates_eNB.eNb_UE_stats_cat(:).UE_mode]);
connected_matrix = false(2,NFrames);
connected_matrix(1,floor(estimates_eNB.frame_tx_cat/decimation) - framestamp_min + 1) = eNB_connected;
connected_matrix(2,floor(estimates_UE.frame_tx_cat/decimation) - framestamp_min + 1) = UE_connected;
all_connected = all(connected_matrix>0,1);

% get the aligned gps data, removing duplicates
gps_lat_aligned = zeros(1,NFrames);
gps_lon_aligned = zeros(1,NFrames);
gps_lat_aligned(1,UE_aligned) = estimates_UE.gps_lat_cat(~UE_duplicates);
gps_lon_aligned(1,UE_aligned) = estimates_UE.gps_lon_cat(~UE_duplicates);

%%
h_fig = figure(99);
hold off
plot(estimates_eNB.frame_tx_cat)
hold on
plot(estimates_UE.frame_tx_cat(~UE_duplicates),'r')
legend('eNB','UE')

% frame_aligned = zeros(NFrames,2);
% frame_aligned(eNB_aligned,1) = estimates_eNB.frame_tx_cat;
% frame_aligned(UE_aligned,2) = estimates_UE.frame_tx_cat(~UE_duplicates);
h_fig = figure(98);
hold off
plot(estimates_eNB.frame_tx_cat,eNB_connected,'x');
hold on
plot(estimates_UE.frame_tx_cat(~UE_duplicates),UE_connected(~UE_duplicates),'rx')
legend('eNb','UE')

%%
UL_rssi_cat = zeros(length(estimates_eNB.eNb_UE_stats_cat),2);
for i=1:length(estimates_eNB.eNb_UE_stats_cat)
    UL_rssi_cat(i,:) = double(estimates_eNB.eNb_UE_stats_cat(i).UL_rssi);
end
UL_rssi_aligned = zeros(NFrames,2);
UL_rssi_aligned(eNB_aligned,:) = UL_rssi_cat;

h_fig = figure(15);
hold off
plot_gps_coordinates(mm,gps_lon_aligned(all_connected), ...
        gps_lat_aligned(all_connected), ...
        mean(UL_rssi_aligned(all_connected,:),2));
title('UL RSSI [dBm]')
saveas(h_fig,fullfile(pathname,'UL_RSSI_dBm_gps.jpg'),'jpg')

%%
ulsch_throughput_aligned = zeros(NFrames,1);
ulsch_throughput_aligned(eNB_aligned,1) = ulsch_throughput;
h_fig = figure(16);
hold off
plot_gps_coordinates(mm,gps_lon_aligned(all_connected), ...
        gps_lat_aligned(all_connected), ...
        ulsch_throughput_aligned(all_connected,1));
title('UL Throughput (modem) [bps]')
saveas(h_fig,fullfile(pathname,'UL_throughput_gps.jpg'),'jpg')

%%
ulsch_throughput_ideal_1Rx_aligned = zeros(NFrames,1);
ulsch_throughput_ideal_1Rx_aligned(eNB_aligned,1) = ulsch_throughput_ideal_1Rx;
h_fig = figure(17);
hold off
plot_gps_coordinates(mm,gps_lon_aligned(all_connected), ...
        gps_lat_aligned(all_connected), ...
        ulsch_throughput_ideal_1Rx_aligned(all_connected,1));
title('UL Throughput (ideal, 1Rx) [bps]')
saveas(h_fig,fullfile(pathname,'UL_throughput_ideal_1Rx_gps.jpg'),'jpg')

%%
ulsch_throughput_ideal_2Rx_aligned = zeros(NFrames,1);
ulsch_throughput_ideal_2Rx_aligned(eNB_aligned,1) = ulsch_throughput_ideal_2Rx;
h_fig = figure(18);
hold off
plot_gps_coordinates(mm,gps_lon_aligned(all_connected), ...
        gps_lat_aligned(all_connected), ...
        ulsch_throughput_ideal_2Rx_aligned(all_connected,1));
title('UL Throughput (ideal, 2Rx) [bps]')
saveas(h_fig,fullfile(pathname,'UL_throughput_ideal_2Rx_gps.jpg'),'jpg')


