function plot_distance_travelled(mode1,index1,mode2,mode2_ideal,index2b,mode6,index6,pathname,file_id, mm)

[dist1, dist_travelled1] = calc_dist(mode1.gps_lat_cat(index1), mode1.gps_lon_cat(index1), mm);
[dist2b, dist_travelled2b] = calc_dist(mode2.gps_lat_cat(index2b), mode2.gps_lon_cat(index2b), mm);
[dist6, dist_travelled6] = calc_dist(mode6.gps_lat_cat(index6), mode6.gps_lon_cat(index6), mm);

%%
h_fig = figure(3);
hold off
plot(dist_travelled1, mode1.rx_rssi_dBm_cat(index1,1))
hold on
plot(dist_travelled2b, mode2.rx_rssi_dBm_cat(index2b,1),'r')
plot(dist_travelled6, mode6.rx_rssi_dBm_cat(index6,1),'g')
legend('Mode1','Mode2','Mode6')
xlabel('Distance travelled [km]')
ylabel('RX RSSI [dBm]')
ylim([-110 -30]);
saveas(h_fig,fullfile(pathname,'results',sprintf('all_modes_comparison%d_rssi_dBm.eps',file_id)),'epsc2');

%%
h_fig = figure(4);
hold off
plot_gps_coordinates(mm,mode1.gps_lon_cat(index1), ...
    mode1.gps_lat_cat(index1),...
    double(mode1.rx_rssi_dBm_cat(index1,1)),[-110 -30]);
saveas(h_fig,fullfile(pathname,'results',sprintf('all_modes_comparison%d_rssi_dBm.jpg',file_id)),'jpg');

%%
h_fig = figure(5);
hold off
plot(dist_travelled1, mode1.throughput(index1,1));
hold on
plot(dist_travelled2b, mode2.throughput(index2b,1),'r');
plot(dist_travelled6, mode6.throughput(index6,1),'g')
ylim([0 8.64e6]);
legend('Mode1','Mode2','Mode6')
xlabel('Distance travelled [km]')
ylabel('Throughput [bps]')
saveas(h_fig,fullfile(pathname,'results',sprintf('all_modes_comparison%d_troughput_distance_travelled.eps',file_id)),'epsc2');

%% cqi
h_fig = figure(6);
temp = cat(3,mode6.phy_measurements_cat(:).precoded_cqi_dB);
mode6.precoded_cqi_dB = squeeze(temp(1,1,:));
temp = cat(1,mode6.phy_measurements_cat(:).wideband_cqi_tot);
mode6.wideband_cqi_dB = temp(:,1);

hold off
plot(dist_travelled6, mode6.precoded_cqi_dB(index6,1),'r')
hold on
plot(dist_travelled6, mode6.wideband_cqi_dB(index6,1),'b')

legend('Precoded CQI','Normal CQI')
xlabel('Distance travelled [km]')
ylabel('CQI')
saveas(h_fig,fullfile(pathname,'results',sprintf('all_modes_comparison%d_cqi_distance_travelled.eps',file_id)),'epsc2');

%% ideal curves
h_fig = figure(7);
hold off
plot(dist_travelled2b, mode2_ideal.rateps_SISO_supportedQam_eNB1_2Rx_cat(index2b),'r');
hold on
plot(dist_travelled2b, mode2_ideal.rateps_alamouti_supportedQam_eNB1_2Rx_cat(index2b),'g');
plot(dist_travelled2b, mode2_ideal.rateps_beamforming_supportedQam_eNB1_2Rx_maxq_cat(index2b),'b');
plot(dist_travelled2b, mode2_ideal.rateps_beamforming_supportedQam_eNB1_2Rx_feedbackq_cat(index2b),'k');
ylim([0 8.64e6]);
legend('Mode1','Mode2','Mode6 opt','Mode6 feedback')
xlabel('Distance travelled [km]')
ylabel('Throughput [bps]')
saveas(h_fig,fullfile(pathname,'results',sprintf('all_modes_comparison%d_throughput_ideal_distance_travelled.eps',file_id)),'epsc2');


%% PBCH
h_fig = figure(8);
hold off
mode1.pbch_good = (mode1.pbch_fer_cat<=100 & mode1.pbch_fer_cat>=0).';
mode2.pbch_good = (mode2.pbch_fer_cat<=100 & mode2.pbch_fer_cat>=0).';
mode1.pbch_fer_cat(~mode1.UE_synched | ~mode1.pbch_good) = nan;
mode2.pbch_fer_cat(~mode2.UE_synched | ~mode2.pbch_good) = nan;
plot(dist_travelled1, mode1.pbch_fer_cat(index1,1));
hold on
plot(dist_travelled2b, mode2.pbch_fer_cat(index2b,1),'r');
%plot(dist_travelled6, mode6.throughput(index6,1),'g')


legend('Mode1','Mode2/6')
xlabel('Distance travelled [km]')
ylabel('PBCH FER [bps]')
saveas(h_fig,fullfile(pathname,'results',sprintf('all_modes_comparison%d_pbch_fer_distance_travelled.eps',file_id)),'epsc2');





