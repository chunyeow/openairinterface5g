fig_path = 'dist_travelled';
file_id = 1;

%%
[mode2_ideal.dist, mode2_ideal.dist_travelled] = calc_dist(lat,lon,mm);

index = find(select==1);

%%
h_fig = figure(3);
hold off
plot(mode2_ideal.dist_travelled(index)-mode2_ideal.dist_travelled(index(1)), ...
    mode2.rx_rssi_dBm_cat(index,1))
xlabel('Distance travelled [km]')
ylabel('RX RSSI [dBm]')
ylim([-110 -30]);
saveas(h_fig,fullfile(pathname,fig_path,sprintf('all_modes_comparison%d_rssi_dBm.eps',file_id)),'epsc2');


%%
h_fig = figure(4);
hold off
plot_gps_coordinates(mm,lon(index), lat(index),...
    double(mode2.rx_rssi_dBm_cat(index,1)),[-110 -30]);
saveas(h_fig,fullfile(pathname,fig_path,sprintf('all_modes_comparison%d_rssi_dBm.jpg',file_id)),'jpg');

%%
h_fig = figure(5);
hold off
plot(mode2_ideal.dist_travelled(index)-mode2_ideal.dist_travelled(index(1)),...
    mode2_ideal.rateps_SISO_supportedQam_eNB1_2Rx_cat(index));
hold on
plot(mode2_ideal.dist_travelled(index)-mode2_ideal.dist_travelled(index(1)),...
    mode2_ideal.rateps_alamouti_supportedQam_eNB1_2Rx_cat(index),'r');
plot(mode2_ideal.dist_travelled(index)-mode2_ideal.dist_travelled(index(1)), ...
    mode2_ideal.rateps_beamforming_supportedQam_eNB1_2Rx_feedbackq_cat(index),'g')
ylim([0 8.64e6]);
legend('Mode1','Mode2','Mode6')
xlabel('Distance travelled [km]')
ylabel('Throughput [bps]')
saveas(h_fig,fullfile(pathname,fig_path,sprintf('all_modes_comparison%d_troughput_distance_travelled.eps',file_id)),'epsc2');

%% cqi
temp = cat(1,mode2.phy_measurements_cat(:).wideband_cqi_tot);
mode2.wideband_cqi_dB = temp(:,1);

h_fig = figure(6);
hold off
plot(mode2_ideal.dist_travelled(index)-mode2_ideal.dist_travelled(index(1)), ...
    mode2.wideband_cqi_dB(index,1),'b')
xlabel('Distance travelled [km]')
ylabel('CQI')
saveas(h_fig,fullfile(pathname,fig_path,sprintf('all_modes_comparison%d_cqi_distance_travelled.eps',file_id)),'epsc2');

% %% ideal curves
% h_fig = figure(7);
% hold off
% plot(mode2.dist_travelled(index2)-mode2.dist_travelled(index2(1)),...
%     mode2_ideal.rateps_SISO_supportedQam_eNB1_2Rx_cat(index2),'r');
% hold on
% plot(mode2.dist_travelled(index2)-mode2.dist_travelled(index2(1)),...
%     mode2_ideal.rateps_alamouti_supportedQam_eNB1_2Rx_cat(index2),'g');
% plot(mode2.dist_travelled(index2)-mode2.dist_travelled(index2(1)),...
%     mode2_ideal.rateps_beamforming_supportedQam_eNB1_2Rx_maxq_cat(index2),'b');
% plot(mode2.dist_travelled(index2)-mode2.dist_travelled(index2(1)),...
%     mode2_ideal.rateps_beamforming_supportedQam_eNB1_2Rx_feedbackq_cat(index2),'k');
% ylim([0 8.64e6]);
% legend('Mode1','Mode2','Mode6 opt','Mode6 feedback')
% xlabel('Distance travelled [km]')
% ylabel('Throughput [bps]')
% saveas(h_fig,fullfile(pathname,fig_path,sprintf('all_modes_comparison%d_throughput_ideal_distance_travelled.eps',file_id)),'epsc2');


%% PBCH
h_fig = figure(8);
hold off
mode2.UE_synched = (mode2.UE_mode_cat>0);
mode2.UE_connected = (mode2.UE_mode_cat==3);
mode2.pbch_good = (mode2.pbch_fer_cat<=100 & mode2.pbch_fer_cat>=0).';
mode2.pbch_fer_cat(~mode2.UE_synched | ~mode2.pbch_good) = nan;
plot(mode2_ideal.dist_travelled(index)-mode2_ideal.dist_travelled(index(1)),...
    mode2.pbch_fer_cat(index,1));
xlabel('Distance travelled [km]')
ylabel('PBCH FER [bps]')
saveas(h_fig,fullfile(pathname,fig_path,sprintf('all_modes_comparison%d_pbch_fer_distance_travelled.eps',file_id)),'epsc2');





