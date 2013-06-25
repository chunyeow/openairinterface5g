clear all
close all

pathname='/emos/EMOS/results/';

coverage     = load('/emos/EMOS/Coverage/results/results_UE.mat');
mode1        = load('/emos/EMOS/Mode1/results/results_UE.mat');
mode2        = load('/emos/EMOS/Mode2/results_wo_coverage/results_UE.mat');
mode2_update = load('/emos/EMOS/Mode2_update/results/results_UE.mat');
mode2_ideal  = load('/emos/EMOS/Mode2/results_wo_coverage/results_cat_UE.mat');
mode6        = load('/emos/EMOS/Mode6/results/results_UE.mat');

mm='cordes';


%%
[coverage.dist, coverage.dist_travelled] = calc_dist(coverage.gps_lat_cat,coverage.gps_lon_cat);
[mode1.dist, mode1.dist_travelled] = calc_dist(mode1.gps_lat_cat,mode1.gps_lon_cat);
[mode2.dist, mode2.dist_travelled] = calc_dist(mode2.gps_lat_cat,mode2.gps_lon_cat);
[mode2_update.dist, mode2_update.dist_travelled] = calc_dist(mode2_update.gps_lat_cat,mode2_update.gps_lon_cat);
[mode6.dist, mode6.dist_travelled] = calc_dist(mode6.gps_lat_cat,mode6.gps_lon_cat);

%%
mode1.UE_connected = (mode1.UE_mode_cat==3);
mode1.UE_synched = (mode1.UE_mode_cat>0);
mode1.good = (mode1.dlsch_fer_cat<=100 & mode1.dlsch_fer_cat>=0).';
mode1.throughput = double(100./(100+mode1.dlsch_fer_cat).*mode1.tbs_cat.*6.*100);
%mode1.throughput = (100-mode1.dlsch_fer_cat).*mode1.tbs_cat*6;
mode1.throughput(~mode1.UE_connected) = 0;

mode2.UE_connected = (mode2.UE_mode_cat==3);
mode2.UE_synched = (mode2.UE_mode_cat>0);
mode2.good = (mode2.dlsch_fer_cat<=100 & mode2.dlsch_fer_cat>=0).';
mode2.throughput = double(100./(100+mode2.dlsch_fer_cat).*mode2.tbs_cat.*6.*100);
%mode2.throughput = (100-mode2.dlsch_fer_cat).*mode2.tbs_cat*6;
mode2.throughput(~mode2.UE_connected) = 0;

mode2_update.UE_connected = (mode2_update.UE_mode_cat==3);
mode2_update.UE_synched = (mode2_update.UE_mode_cat>0);
mode2_update.good = (mode2_update.dlsch_fer_cat<=100 & mode2_update.dlsch_fer_cat>=0).';
mode2_update.throughput = double(100./(100+mode2_update.dlsch_fer_cat).*mode2_update.tbs_cat.*6.*100);
%mode2_update.throughput = (100-mode2_update.dlsch_fer_cat).*mode2_update.tbs_cat*6;
mode2_update.throughput(~mode2_update.UE_connected) = 0;

mode6.UE_connected = (mode6.UE_mode_cat==3);
mode6.UE_synched = (mode6.UE_mode_cat>0);
mode6.good = (mode6.dlsch_fer_cat<=100 & mode6.dlsch_fer_cat>=0).';
mode6.throughput = double(100./(100+mode6.dlsch_fer_cat).*mode6.tbs_cat.*6.*100);
%mode6.throughput = (100-mode6.dlsch_fer_cat).*mode6.tbs_cat*6;
mode6.throughput(~mode6.UE_connected) = 0;


%% Zoom 1
start_mode1 = 746;
length_mode1 = 300;
%start_mode2 = 330;
start_mode2 = 22873+330;
length_mode2 = 380;
start_mode6 = 16599;
end_mode6 = 16276;
%length_mode6 = 

%%
h_fig = figure(3);
hold off
plot(mode1.dist_travelled(start_mode1:start_mode1+length_mode1)-mode1.dist_travelled(start_mode1), ...
    mode1.rx_rssi_dBm_cat(start_mode1:start_mode1+length_mode1,1))
hold on
plot(mode2.dist_travelled(start_mode2:start_mode2+length_mode2)-mode2.dist_travelled(start_mode2), ...
    mode2.rx_rssi_dBm_cat(start_mode2:start_mode2+length_mode2,1),'r')
plot(-mode6.dist_travelled(end_mode6)+mode6.dist_travelled(end_mode6:start_mode6), ...
    mode6.rx_rssi_dBm_cat(end_mode6:start_mode6,1),'g')
legend('Mode1','Mode2','Mode6')
xlabel('Distance travelled [km]')
ylabel('RX RSSI [dBm]')
ylim([-110, -30]);
saveas(h_fig,fullfile(pathname,'all_modes_comparison_rssi_dBm.eps'),'epsc2');


%%
h_fig = figure(4);
hold off
plot_gps_coordinates(mm,mode1.gps_lon_cat(start_mode1:start_mode1+length_mode1), ...
    mode1.gps_lat_cat(start_mode1:start_mode1+length_mode1),...
    double(mode1.rx_rssi_dBm_cat(start_mode1:start_mode1+length_mode1,1)),[-110,-30]);
saveas(h_fig,fullfile(pathname,'all_modes_comparison_rssi_dBm.jpg'),'jpg');

%%
h_fig = figure(5);
hold off
plot(mode1.dist_travelled(start_mode1:start_mode1+length_mode1)-mode1.dist_travelled(start_mode1),...
    mode1.throughput(start_mode1:start_mode1+length_mode1));
hold on
plot(mode2.dist_travelled(start_mode2:start_mode2+length_mode2)-mode2.dist_travelled(start_mode2),...
    mode2.throughput(start_mode2:start_mode2+length_mode2),'r');
plot(-mode6.dist_travelled(end_mode6)+mode6.dist_travelled(end_mode6:start_mode6), ...
    mode6.throughput(end_mode6:start_mode6,1),'g')
ylim([0 8.64e6]);
legend('Mode1','Mode2','Mode6')
xlabel('Distance travelled [km]')
ylabel('Throughput [bps]')
saveas(h_fig,fullfile(pathname,'all_modes_comparison_troughput_distance_travelled.eps'),'epsc2');

%% cqi
h_fig = figure(6);
temp = cat(3,mode6.phy_measurements_cat(:).precoded_cqi_dB);
mode6.precoded_cqi_dB = squeeze(temp(1,1,:));
temp = cat(1,mode6.phy_measurements_cat(:).wideband_cqi_tot);
mode6.wideband_cqi_dB = temp(:,1);

hold off
plot(-mode6.dist_travelled(end_mode6)+mode6.dist_travelled(end_mode6:start_mode6), ...
    mode6.precoded_cqi_dB(end_mode6:start_mode6,1),'r')
hold on
plot(-mode6.dist_travelled(end_mode6)+mode6.dist_travelled(end_mode6:start_mode6), ...
    mode6.wideband_cqi_dB(end_mode6:start_mode6,1),'b')

legend('Precoded CQI','Normal CQI')
xlabel('Distance travelled [km]')
ylabel('CQI')
saveas(h_fig,fullfile(pathname,'all_modes_comparison_cqi_distance_travelled.eps'),'epsc2');

%% ideal curves
h_fig = figure(7);
hold off
plot(mode2.dist_travelled(start_mode2:start_mode2+length_mode2)-mode2.dist_travelled(start_mode2),...
    scale_ideal_tp(mode2_ideal.rateps_SISO_supportedQam_eNB1_2Rx_cat(start_mode2:start_mode2+length_mode2)),'r');
hold on
plot(mode2.dist_travelled(start_mode2:start_mode2+length_mode2)-mode2.dist_travelled(start_mode2),...
    scale_ideal_tp(mode2_ideal.rateps_alamouti_supportedQam_eNB1_2Rx_cat(start_mode2:start_mode2+length_mode2)),'g');
plot(mode2.dist_travelled(start_mode2:start_mode2+length_mode2)-mode2.dist_travelled(start_mode2),...
    scale_ideal_tp(mode2_ideal.rateps_beamforming_supportedQam_eNB1_2Rx_maxq_cat(start_mode2:start_mode2+length_mode2)),'b');
plot(mode2.dist_travelled(start_mode2:start_mode2+length_mode2)-mode2.dist_travelled(start_mode2),...
    scale_ideal_tp(mode2_ideal.rateps_beamforming_supportedQam_eNB1_2Rx_feedbackq_cat(start_mode2:start_mode2+length_mode2)),'k');
ylim([0 8.64e6]);
legend('Mode1','Mode2','Mode6 opt','Mode6 feedback')
xlabel('Distance travelled [km]')
ylabel('Throughput [bps]')
saveas(h_fig,fullfile(pathname,'all_modes_comparison_throughput_ideal_distance_travelled.eps'),'epsc2');


%% Zoom2
load cordes_zoom2.mat
select2b = false(size(parcours1));
select2b(parcours1)=select2;
select1 = find(select1);
select2 = find(select2);
select6 = find(select6);
select2b = find(select2b);

%%
h_fig = figure(3);
hold off
plot(mode1.dist_travelled(select1)-mode1.dist_travelled(select1(1)), ...
    mode1.rx_rssi_dBm_cat(select1,1))
hold on
plot(mode2.dist_travelled(select2b)-mode2.dist_travelled(select2b(1)), ...
    mode2.rx_rssi_dBm_cat(select2b,1),'r')
plot(mode6.dist_travelled(select6)-mode6.dist_travelled(select6(1)), ...
    mode6.rx_rssi_dBm_cat(select6,1),'g')
ylim([-110 -30]);
legend('Mode1','Mode2','Mode6')
xlabel('Distance travelled [km]')
ylabel('RX RSSI [dBm]')
saveas(h_fig,fullfile(pathname,'all_modes_comparison2_rssi_dBm.eps'),'epsc2');


%%
h_fig = figure(4);
hold off
plot_gps_coordinates(mm,mode1.gps_lon_cat(select1), ...
    mode1.gps_lat_cat(select1),...
    double(mode1.rx_rssi_dBm_cat(select1,1)),[-110,-30]);
saveas(h_fig,fullfile(pathname,'all_modes_comparison2_rssi_dBm.jpg'),'jpg');

%%
h_fig = figure(5);
hold off
plot(mode1.dist_travelled(select1)-mode1.dist_travelled(select1(1)),...
    mode1.throughput(select1,1));
hold on
plot(mode2.dist_travelled(select2b)-mode2.dist_travelled(select2b(1)),...
    mode2.throughput(select2b,1),'r');
plot(mode6.dist_travelled(select6)-mode6.dist_travelled(select6(1)), ...
    mode6.throughput(select6,1),'g')
ylim([0 8.64e6]);
legend('Mode1','Mode2','Mode6')
xlabel('Distance travelled [km]')
ylabel('Throughput [bps]')
saveas(h_fig,fullfile(pathname,'all_modes_comparison2_troughput_distance_travelled.eps'),'epsc2');

%% cqi
h_fig = figure(6);
temp = cat(3,mode6.phy_measurements_cat(:).precoded_cqi_dB);
mode6.precoded_cqi_dB = squeeze(temp(1,1,:));
temp = cat(1,mode6.phy_measurements_cat(:).wideband_cqi_tot);
mode6.wideband_cqi_dB = temp(:,1);

hold off
plot(mode6.dist_travelled(select6)-mode6.dist_travelled(select6(1)), ...
    mode6.precoded_cqi_dB(select6,1),'r')
hold on
plot(mode6.dist_travelled(select6)-mode6.dist_travelled(select6(1)), ...
    mode6.wideband_cqi_dB(select6,1),'b')

legend('Precoded CQI','Normal CQI')
xlabel('Distance travelled [km]')
ylabel('CQI')
saveas(h_fig,fullfile(pathname,'all_modes_comparison2_cqi_distance_travelled.eps'),'epsc2');

%% ideal curves
h_fig = figure(7);
hold off
plot(mode2.dist_travelled(select2b)-mode2.dist_travelled(select2b(1)),...
    scale_ideal_tp(mode2_ideal.rateps_SISO_supportedQam_eNB1_2Rx_cat(select2b)),'r');
hold on
plot(mode2.dist_travelled(select2b)-mode2.dist_travelled(select2b(1)),...
    scale_ideal_tp(mode2_ideal.rateps_alamouti_supportedQam_eNB1_2Rx_cat(select2b)),'g');
plot(mode2.dist_travelled(select2b)-mode2.dist_travelled(select2b(1)),...
    scale_ideal_tp(mode2_ideal.rateps_beamforming_supportedQam_eNB1_2Rx_maxq_cat(select2b)),'b');
plot(mode2.dist_travelled(select2b)-mode2.dist_travelled(select2b(1)),...
    scale_ideal_tp(mode2_ideal.rateps_beamforming_supportedQam_eNB1_2Rx_feedbackq_cat(select2b)),'k');
ylim([0 8.64e6]);
legend('Mode1','Mode2','Mode6 opt','Mode6 feedback')
xlabel('Distance travelled [km]')
ylabel('Throughput [bps]')
saveas(h_fig,fullfile(pathname,'all_modes_comparison2_throughput_ideal_distance_travelled.eps'),'epsc2');


%% Zoom3
load cordes_zoom3.mat
select2b = false(size(parcours1));
select2b(parcours1)=select2;
select1 = find(select1);
select2 = find(select2);
select2_update = find(select2_update);
select6 = find(select6);
select2b = find(select2b);

%%
h_fig = figure(3);
hold off
plot(mode1.dist_travelled(select1)-mode1.dist_travelled(select1(1)), ...
    mode1.rx_rssi_dBm_cat(select1,1))
hold on
plot(mode2_update.dist_travelled(select2_update)-mode2_update.dist_travelled(select2_update(1)), ...
    mode2_update.rx_rssi_dBm_cat(select2_update,1),'r')
%plot(mode6.dist_travelled(select6)-mode6.dist_travelled(select6(1)), ...
%    mode6.rx_rssi_dBm_cat(select6,1),'g')
ylim([-110 -30]);
legend('Mode1','Mode2')
xlabel('Distance travelled [km]')
ylabel('RX RSSI [dBm]')
saveas(h_fig,fullfile(pathname,'all_modes_comparison3_rssi_dBm.eps'),'epsc2');


%%
h_fig = figure(4);
hold off
plot_gps_coordinates(mm,mode1.gps_lon_cat(select1), ...
    mode1.gps_lat_cat(select1),...
    double(mode1.rx_rssi_dBm_cat(select1,1)),[-110 -30]);
saveas(h_fig,fullfile(pathname,'all_modes_comparison3_rssi_dBm.jpg'),'jpg');

%%
h_fig = figure(5);
hold off
plot(mode1.dist_travelled(select1)-mode1.dist_travelled(select1(1)),...
    mode1.throughput(select1,1));
hold on
plot(mode2_update.dist_travelled(select2_update)-mode2_update.dist_travelled(select2_update(1)),...
    mode2_update.throughput(select2_update,1),'r');
%plot(mode6.dist_travelled(select6)-mode6.dist_travelled(select6(1)), ...
%    mode6.throughput(select6,1),'g')
ylim([0 8.64e6]);
legend('Mode1','Mode2')
xlabel('Distance travelled [km]')
ylabel('Throughput [bps]')
saveas(h_fig,fullfile(pathname,'all_modes_comparison3_troughput_distance_travelled.eps'),'epsc2');

%%
h_fig = figure(6);
hold off
mode1.pbch_good = (mode1.pbch_fer_cat<=100 & mode1.pbch_fer_cat>=0).';
mode2_update.pbch_good = (mode2_update.pbch_fer_cat<=100 & mode2_update.pbch_fer_cat>=0).';
mode1.pbch_fer_cat(~mode1.UE_synched | ~mode1.pbch_good) = nan;
mode2_update.pbch_fer_cat(~mode2_update.UE_synched | ~mode2_update.pbch_good) = nan;
plot(mode1.dist_travelled(select1)-mode1.dist_travelled(select1(1)),...
    mode1.pbch_fer_cat(select1,1));
hold on
plot(mode2_update.dist_travelled(select2_update)-mode2_update.dist_travelled(select2_update(1)),...
    mode2_update.pbch_fer_cat(select2_update,1),'r');
%plot(mode6.dist_travelled(select6)-mode6.dist_travelled(select6(1)), ...
%    mode6.throughput(select6,1),'g')
legend('Mode1','Mode2')
xlabel('Distance travelled [km]')
ylabel('PBCH FER [bps]')
saveas(h_fig,fullfile(pathname,'all_modes_comparison3_pbch_fer_distance_travelled.eps'),'epsc2');


