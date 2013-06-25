%%
load(fullfile(pathname,'results_eNB.mat'));
h_fig = figure(2);
hold off
plot(frame_tx_cat,rx_N0_dBm_cat,'x')
title('RX I0 [dBm]')
xlabel('Frame number')
ylabel('RX I0 [dBm]')
legend('Sector 0','Sector 1','Sector 2');
saveas(h_fig,fullfile(pathname,'RX_I0_dBm.eps'),'epsc2')

%%
load(fullfile(pathname,'results_UE.mat'));
h_fig = figure(3);
hold off
plot_gps_coordinates(mm,gps_lon_cat, gps_lat_cat,rx_N0_dBm_cat);
title('RX I0 [dBm]')
xlabel('Frame number')
ylabel('RX I0 [dBm]')
saveas(h_fig,fullfile(pathname,'RX_RSSI_dBm_gps.jpg'),'jpg')



