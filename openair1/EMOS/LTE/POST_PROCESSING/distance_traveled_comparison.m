clear all
close all

pathname='/emos/EMOS/results/';

mode1 = load('/emos/EMOS/Mode1/results/results_UE.mat');
mode2 = load('/emos/EMOS/Mode2/results/results_UE.mat');
mode2_update = load('/emos/EMOS/Mode2_update/results/results_UE.mat');
mode6 = load('/emos/EMOS/Mode6/results/results_UE.mat');
mm='cordes';


%%
[mode1.dist, mode1.dist_travelled] = calc_dist(mode1.gps_lat_cat,mode1.gps_lon_cat);
[mode2.dist, mode2.dist_travelled] = calc_dist(mode2.gps_lat_cat,mode2.gps_lon_cat);
[mode2_update.dist, mode2_update.dist_travelled] = calc_dist(mode2_update.gps_lat_cat,mode2_update.gps_lon_cat);
[mode6.dist, mode6.dist_travelled] = calc_dist(mode6.gps_lat_cat,mode6.gps_lon_cat);

mode1.UE_connected = (mode1.UE_mode_cat==3);
%mode1.good = (mode1.dlsch_fer_cat<=100 & mode1.dlsch_fer_cat>=0).';
mode1.throughput = (100-mode1.dlsch_fer_cat).*mode1.tbs_cat*6;
mode1.throughput(~mode1.UE_connected) = 0;
mode2.UE_connected = (mode2.UE_mode_cat==3);
%mode2.good = (mode2.dlsch_fer_cat<=100 & mode2.dlsch_fer_cat>=0).';
mode2.throughput = (100-mode2.dlsch_fer_cat).*mode2.tbs_cat*6;
mode2.throughput(~mode2.UE_connected) = 0;
mode2_update.UE_connected = (mode2_update.UE_mode_cat==3);
%mode2_update.good = (mode2_update.dlsch_fer_cat<=100 & mode2_update.dlsch_fer_cat>=0).';
mode2_update.throughput = (100-mode2_update.dlsch_fer_cat).*mode2_update.tbs_cat*6;
mode2_update.throughput(~mode2_update.UE_connected) = 0;
mode6.UE_connected = (mode6.UE_mode_cat==3);
%mode6.good = (mode6.dlsch_fer_cat<=100 & mode6.dlsch_fer_cat>=0).';
mode6.throughput = (100-mode6.dlsch_fer_cat).*mode6.tbs_cat*6;
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
saveas(h_fig,fullfile(pathname,'all_modes_comparison_rssi_dBm.eps'),'epsc2');


%%
h_fig = figure(4);
plot_gps_coordinates(mm,mode1.gps_lon_cat(start_mode1:start_mode1+length_mode1), ...
    mode1.gps_lat_cat(start_mode1:start_mode1+length_mode1),...
    double(mode1.rx_rssi_dBm_cat(start_mode1:start_mode1+length_mode1,1)));
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


legend('Mode1','Mode2','Mode6')
xlabel('Distance travelled [km]')
ylabel('Throughput [bps]')
saveas(h_fig,fullfile(pathname,'all_modes_comparison_troughput_distance_travelled.eps'),'epsc2');



%% Zoom2
load cordes_zoom2.mat
select1 = find(select1);
select2 = find(select2);
select6 = find(select6);
select2b = false(size(parcours1));
select2b(parcours1)=select2;
select2b = find(select2b);

h_fig = figure(3);
hold off
plot(mode1.dist_travelled(select1)-mode1.dist_travelled(select1(1)), ...
    mode1.rx_rssi_dBm_cat(select1,1))
hold on
plot(mode2.dist_travelled(select2b)-mode2.dist_travelled(select2b(1)), ...
    mode2.rx_rssi_dBm_cat(select2b,1),'r')
plot(mode6.dist_travelled(select6)-mode6.dist_travelled(select6(1)), ...
    mode6.rx_rssi_dBm_cat(select6,1),'g')
legend('Mode1','Mode2','Mode6')
xlabel('Distance travelled [km]')
ylabel('RX RSSI [dBm]')
%saveas(h_fig,fullfile(pathname,'all_modes_comparison2_rssi_dBm.eps'),'epsc2');


%%
h_fig = figure(4);
plot_gps_coordinates(mm,mode1.gps_lon_cat(select1), ...
    mode1.gps_lat_cat(select1),...
    double(mode1.rx_rssi_dBm_cat(select1,1)));
%saveas(h_fig,fullfile(pathname,'all_modes_comparison2_rssi_dBm.jpg'),'jpg');

%%
h_fig = figure(5);
hold off
plot(mode1.dist_travelled(select1)-mode1.dist_travelled(select1(1)),...
    mode1.throughput(select1,1));
hold on
plot(mode2.dist_travelled(select2)-mode2.dist_travelled(select2(1)),...
    mode2.throughput(select2,1),'r');
plot(mode6.dist_travelled(select6)-mode6.dist_travelled(select6(1)), ...
    mode6.throughput(select6,1),'g')


legend('Mode1','Mode2','Mode6')
xlabel('Distance travelled [km]')
ylabel('Throughput [bps]')
%saveas(h_fig,fullfile(pathname,'all_modes_comparison2_troughput_distance_travelled.eps'),'epsc2');


