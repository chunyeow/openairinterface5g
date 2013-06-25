%clear all
close all

% %% load the files
% disp('[eval_data_MU] loading image file');
% if isunix
%     mm = imread('/extras/kaltenbe/EMOS/2004_fd0006_250_c_0485.tif');
% else
%     mm = imread('\\extras\kaltenbe\EMOS\2004_fd0006_250_c_0485.tif');
% end    
% 
% %%
% load('metadata_MU.mat');
% 
% Nuser = size(est,1);

%% plot the rx_rssi over time
scrsz = get(0,'ScreenSize');
h_fig = figure('Position',[1 scrsz(4)/2 scrsz(3) scrsz(4)/2]);
%h_fig = figure(1);
%plot_style = {'rx','bx','gx','kx'};
plot_style = {'c.','m.','b.','g.','k.'};
for u=3:Nuser
    rx_rssi_dBm = cat(3,est(u,:).rx_rssi_dBm);
    %timestamp_tx = [est(u,:).timestamp_tx];
    %timestamp_tx = timestamp_tx - 4.476e5;
    framestamp_tx = [est(u,:).framestamp_tx];
    err_ind = logical([est(u,:).crc_status]);

    plot(framestamp_tx,mean(squeeze(rx_rssi_dBm(1,:,:)),1),plot_style{u});
    hold on
    % plot(framestamp_tx(err_ind),mean(rx_rssi_dBm(:,err_ind),1),'rx');

end
hold off
xlabel('File Index')
ylabel('RSSI [dBm]')
%xlim([min(framestamp_is),max(framestamp_is)])
%xlim([framestamp_is(max_start),framestamp_is(max_start+max_length)]);
xtick = floor(min(min(framestamp_min))/22500)*22500:22500:floor(max(max(framestamp_max))/22500)*22500;
%xlim([7312500 7987500]) %file index 25 to 55
set(gca,'xtick',xtick)
set(gca,'xticklabel',mod(floor(xtick/22500),100))
[legend_h,object_h,plot_h,text_strings] = legend('User 1','User 2','User 3','User 4','orientation','horizontal','location','south');
h_legend_line = findobj(object_h,'type','line');
set(h_legend_line,'LineStyle','-')
set(h_legend_line,'LineWidth',3)
grid on
saveas(h_fig, 'rx_rssi_time.eps', 'psc2');

%% plot the FER over time
h_fig = figure(2);
%plot_style = {'rx','bx','gx','kx'};
plot_style = {'c.','m.','b.','g.','k.'};
for u=3:Nuser
    %timestamp_tx = [est(u,:).timestamp_tx];
    %timestamp_tx = timestamp_tx - 4.476e5;
    framestamp_tx = [est(u,:).framestamp_tx];
    err_ind = [est(u,:).crc_status];
    fer = sum(reshape(err_ind,100,[]),1);

    plot(framestamp_tx(1:100:end),fer,plot_style{u});
    hold on
    % plot(framestamp_tx(err_ind),mean(rx_rssi_dBm(:,err_ind),1),'rx');

end
hold off
xlabel('Frame')
ylabel('FER')
%xlim([min(framestamp_is),max(framestamp_is)])
%xlim([framestamp_is(max_start),framestamp_is(max_start+max_length)]);
legend('User 1','User 2','User 3','User 4')

saveas(h_fig, 'fer_time.eps', 'psc2');

%% plot the maps
for u = 3:Nuser;

    lat = [gps_data(u,:).latitude];
    lon = [gps_data(u,:).longitude];

    %%
    rx_rssi_dBm = cat(3,est(u,:).rx_rssi_dBm);
    h_fig = figure;
    plot_gps_coordinates_sophia(mm,lon,lat,mean(squeeze(rx_rssi_dBm(1,:,1:100:end)),1));
    title('Rx RSSI (dBm)')
    saveas(h_fig,sprintf('rx_rssi_term%d.jpg',u))

    %%
    rx_snr = cat(3,est(u,:).rx_power) - cat(3,est(u,:).n0_power);
    h_fig = figure;
    plot_gps_coordinates_sophia(mm,lon,lat,mean(squeeze(rx_snr(1,:,1:100:end)),1));
    title('Rx SNR (dB)')
    saveas(h_fig,sprintf('rx_snr_term%d.jpg',u))

    %%
    % err_ind = [0 diff([est.pdu_errors])];
    % err_ind(err_ind<0) = 0;
    % err_ind(err_ind>1) = 1;
    % err_100 = sum(reshape(err_ind,100,[]),1);
    err_ind_all = [est(u,:).crc_status];
    err_100 = sum(reshape(err_ind_all,100,[]),1);

    h_fig = figure;
    plot_gps_coordinates_sophia(mm,lon,lat,err_100(1:end));
    title('Rx FER (%)')
    saveas(h_fig,sprintf('rx_fer_term%d.jpg',u))
end