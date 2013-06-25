%% load data
estimates_eNB = load(fullfile(pathname,'results_eNB.mat'));

h_fig = 0;

% calculate the indices where the ue and the eNB were connected (they might
% not have set the flag in the same frame)
eNB_connected = ([estimates_eNB.eNb_UE_stats_cat(:).UE_mode]==3);
%timebase = estimates_eNB.gps_time_cat - min(estimates_eNB.gps_time_cat);
timebase = 1:length(estimates_eNB.gps_time_cat);
[dist, dist_traveled] = calc_dist(estimates_eNB.gps_lat_cat,estimates_eNB.gps_lon_cat,mm);
max_dist = ceil(max(dist));

%% N0 over time
h_fig = h_fig+1;
figure(h_fig);
hold off
plot(timebase,estimates_eNB.rx_N0_dBm_cat,'x')
ylim([-110 -90]); 
title('UL I0 [dBm]')
xlabel('Time [sec]')
ylabel('UL I0 [dBm]')
legend('Sector 0','Sector 1','Sector 2')
saveas(h_fig,fullfile(pathname,'UL_I0_dBm.eps'),'epsc2')

%% UL RSSI over time
UL_rssi_cat = zeros(length(estimates_eNB.phy_measurements_cat),3);
for i=1:length(estimates_eNB.phy_measurements_cat)
    UL_rssi_cat(i,:) = double([estimates_eNB.phy_measurements_cat(i,:).rx_rssi_dBm]);
end
UL_rssi_cat(~eNB_connected,:) = -120;
h_fig = h_fig+1;
figure(h_fig);
hold off
plot(timebase,UL_rssi_cat,'x');
title('UL RSSI [dBm]')
xlabel('Time [sec]')
ylabel('UL RSSI [dBm]')
legend('Sector 0','Sector 1','Sector 2')
ylim([-110 -30]);
saveas(h_fig,fullfile(pathname,'UL_RSSI_dBm.eps'),'epsc2')

%% UL RSSI on map
UL_UE_rssi_cat = zeros(length(estimates_eNB.eNb_UE_stats_cat),1);
sector = [estimates_eNB.eNb_UE_stats_cat.sector];
for i=1:length(estimates_eNB.eNb_UE_stats_cat)
    UL_UE_rssi_cat(i) = UL_rssi_cat(i,sector(i)+1);
end

h_fig = h_fig+1;
figure(h_fig);
hold off
plot_gps_coordinates(mm,estimates_eNB.gps_lon_cat, estimates_eNB.gps_lat_cat, ...
        UL_UE_rssi_cat,[-110 -30]);
title('UL RSSI [dBm]')
saveas(h_fig,fullfile(pathname,'UL_RSSI_dBm_gps.jpg'),'jpg')


%% ULSCH throughput over time
% if (estimates_eNB.start_time(1) >= datenum('20100610T000000','yyyymmddTHHMMSS'));
%     version = 1;
%     good = true(size(estimates_eNB.Rate_64Qam_1RX_cat));
% else 
%     version = 0;
%     good = ([estimates_eNB.eNb_UE_stats_cat.sector].'==0);
% end

estimates_eNB.ulsch_fer_cat = [100 diff(estimates_eNB.ulsch_errors_cat)];
ulsch_throughput = double(estimates_eNB.tbs_cat) .* double(100 - estimates_eNB.ulsch_fer_cat) .* 3;
ulsch_throughput(1,~eNB_connected) = 0;
ulsch_throughput_ideal_1Rx = scale_ideal_tp(estimates_eNB.Rate_64Qam_1RX_cat*100);
ulsch_throughput_ideal_2Rx = scale_ideal_tp(estimates_eNB.Rate_64Qam_2RX_cat*100);

% we need to filter out the NaN measurements, that are due to the recording
% bug in the early versions. 
good = ~isnan(ulsch_throughput_ideal_1Rx);

ulsch_throughput_ideal_1Rx(~eNB_connected,1) = 0;
%ulsch_throughput_ideal_1Rx(~good) = nan;
ulsch_throughput_ideal_2Rx(~eNB_connected,1) = 0;
%ulsch_throughput_ideal_2Rx(~good) = nan;

h_fig = h_fig+1;
figure(h_fig);
hold off
plot(timebase,ulsch_throughput,'x');
hold on
plot(timebase,ulsch_throughput_ideal_1Rx,'rx');
hold on
plot(timebase,ulsch_throughput_ideal_2Rx,'gx');
ylim([0 4.86e6])
legend('modem','ideal 1 rx antenna','ideal 2 rx antennas');
title('UL Throughput [bps]')
xlabel('Time [sec]')
ylabel('UL Throughput [bps]')
saveas(h_fig,fullfile(pathname,'UL_throughput.eps'),'epsc2')

%% Throughputs on maps
h_fig = h_fig+1;
figure(h_fig);
hold off
plot_gps_coordinates(mm,estimates_eNB.gps_lon_cat(good), estimates_eNB.gps_lat_cat(good), ...
        ulsch_throughput(good),[0 4.86e6]);
title('UL Throughput (modem) [bps]')
saveas(h_fig,fullfile(pathname,'UL_throughput_gps.jpg'),'jpg')

%%
h_fig = h_fig+1;
figure(h_fig);
hold off
plot_gps_coordinates(mm,estimates_eNB.gps_lon_cat(good), estimates_eNB.gps_lat_cat(good), ...
        ulsch_throughput_ideal_1Rx(good),[0 4.86e6]);
title('UL Throughput (ideal, 1Rx) [bps]')
saveas(h_fig,fullfile(pathname,'UL_throughput_ideal_1Rx_gps.jpg'),'jpg')

%%
h_fig = h_fig+1;
figure(h_fig);
hold off
plot_gps_coordinates(mm,estimates_eNB.gps_lon_cat(good), estimates_eNB.gps_lat_cat(good), ...
        ulsch_throughput_ideal_2Rx(good),[0 4.86e6]);
title('UL Throughput (ideal, 2Rx) [bps]')
saveas(h_fig,fullfile(pathname,'UL_throughput_ideal_2Rx_gps.jpg'),'jpg')


%% Coded throughput CDF comparison
h_fig = h_fig+1;
figure(h_fig);
hold off
[f,x] = ecdf(ulsch_throughput(good));
plot(x,f,'b','Linewidth',2)
hold on
[f,x] = ecdf(ulsch_throughput_ideal_1Rx(good));
plot(x,f,'g','Linewidth',2)
[f,x] = ecdf(ulsch_throughput_ideal_2Rx(good));
plot(x,f,'r','Linewidth',2)
xlim([0 4.86e6])
legend('modem','ideal 1 rx antenna','ideal 2 rx antennas','Location','SouthEast');
title('UL Throughput CDF')
xlabel('UL Throughput [bps]')
ylabel('P(x<abscissa)')
grid on
saveas(h_fig,fullfile(pathname,'UL_throughput_cdf_comparison.eps'),'epsc2')

%% FDD
h_fig = h_fig+1;
figure(h_fig);
hold off
[f,x] = ecdf(ulsch_throughput(good)*10/3);
plot(x,f,'b','Linewidth',2)
hold on
[f,x] = ecdf(ulsch_throughput_ideal_1Rx(good)*10/3);
plot(x,f,'g','Linewidth',2)
[f,x] = ecdf(ulsch_throughput_ideal_2Rx(good)*10/3);
plot(x,f,'r','Linewidth',2)
legend('modem','ideal 1 rx antenna','ideal 2 rx antennas','Location','SouthEast');
title('UL Throughput CDF for FDD')
xlabel('UL Throughput [bps]')
ylabel('P(x<abscissa)')
grid on
saveas(h_fig,fullfile(pathname,'UL_throughput_cdf_comparison_fdd.eps'),'epsc2')


%% Unoded throughput CDF comparison
h_fig = h_fig+1;
figure(h_fig);
hold off
[f,x] = ecdf(coded2uncoded(ulsch_throughput(good),'UL'));
plot(x,f,'b','Linewidth',2)
hold on
[f,x] = ecdf(coded2uncoded(ulsch_throughput_ideal_1Rx(good),'UL'));
plot(x,f,'g','Linewidth',2)
[f,x] = ecdf(coded2uncoded(ulsch_throughput_ideal_2Rx(good),'UL'));
plot(x,f,'r','Linewidth',2)
xlim([0 4.86e6])
legend('modem','ideal 1 rx antenna','ideal 2 rx antennas','Location','SouthEast');
title('UL Uncoded throughput CDF')
xlabel('UL Throughput [bps]')
ylabel('P(x<abscissa)')
grid on
saveas(h_fig,fullfile(pathname,'UL_uncoded_throughput_cdf_comparison.eps'),'epsc2')



%% plot througput as a function of speed

ulsch_throughput(1,~eNB_connected) = nan;
ulsch_throughput_ideal_1Rx(~eNB_connected,1) = nan;
ulsch_throughput_ideal_2Rx(~eNB_connected,1) = nan;
estimates_eNB.gps_speed_cat(~eNB_connected) = nan;
dist(~eNB_connected) = nan;

h_fig = h_fig+1;    
h_fig = figure(h_fig);
hold off
plot_in_bins(estimates_eNB.gps_speed_cat(good), ulsch_throughput(good),  0:5:40);
ylim([0 4.86e6])
title('UL Throughput vs Speed');
xlabel('Speed[Meters/Second]');
ylabel('Throughput[Bits/sec]');
saveas(h_fig,fullfile(pathname,'UL_throughput_speed.eps'),'epsc2');

h_fig = h_fig+1;    
h_fig = figure(h_fig);
hold off
plot_in_bins(estimates_eNB.gps_speed_cat(good), ulsch_throughput_ideal_1Rx(good),  0:5:40);
ylim([0 4.86e6])
title('UL Throughput vs Speed, ideal, 1Rx');
xlabel('Speed[Meters/Second]');
ylabel('Throughput[Bits/sec]');
saveas(h_fig,fullfile(pathname,'UL_throughput_ideal_1Rx_speed.eps'),'epsc2');

h_fig = h_fig+1;    
h_fig = figure(h_fig);
hold off
plot_in_bins(estimates_eNB.gps_speed_cat(good), ulsch_throughput_ideal_2Rx(good),  0:5:40);
ylim([0 4.86e6])
title('UL Throughput vs Speed, ideal, 2Rx');
xlabel('Speed[Meters/Second]');
ylabel('Throughput[Bits/sec]');
saveas(h_fig,fullfile(pathname,'UL_throughput_ideal_2Rx_speed.eps'),'epsc2');

%% plot througput as a function of distance
h_fig = h_fig+1;    
h_fig = figure(h_fig);
hold off
plot_in_bins(dist(good), ulsch_throughput(good),  0:max_dist);
ylim([0 4.86e6])
title('UL Throughput vs Dist');
xlabel('Dist[km]');
ylabel('Throughput[Bits/sec]');
saveas(h_fig,fullfile(pathname,'UL_throughput_dist.eps'),'epsc2');

h_fig = h_fig+1;    
h_fig = figure(h_fig);
hold off
plot_in_bins(dist(good), ulsch_throughput_ideal_1Rx(good),  0:max_dist);
ylim([0 4.86e6])
title('UL Throughput vs Dist, ideal, 1Rx');
xlabel('Dist[km]');
ylabel('Throughput[Bits/sec]');
saveas(h_fig,fullfile(pathname,'UL_throughput_ideal_1Rx_dist.eps'),'epsc2');

h_fig = h_fig+1;    
h_fig = figure(h_fig);
hold off
plot_in_bins(dist(good), ulsch_throughput_ideal_2Rx(good),  0:max_dist);
ylim([0 4.86e6])
title('UL Throughput vs Dist, ideal, 2Rx');
xlabel('Dist[km]');
ylabel('Throughput[Bits/sec]');
saveas(h_fig,fullfile(pathname,'UL_throughput_ideal_2Rx_dist.eps'),'epsc2');




