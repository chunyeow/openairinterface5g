% Skript: top_eval_data_quick_plot.m:
%
% This script visualizes the meatadata prevoiusly read with
% top_eval_data_quick.
%
%
%----------------------------------------------------------------------%
%                                                                      %
%              Author: Florian Kaltenberger                            %
%             Created: 21/12/2007 by FK                               %
%   Last modification: 21/12/2007 by FK                                %
%        Organization: Institut Eurecom                                %
%                      2229 Route des Cretes                           %
%                      B.P. 193                                        %
%                      06904 Sophia Antipolis Cedex, France            %
%                                                                      %
%----------------------------------------------------------------------%

clear all
close all

addpath('/homes/kaltenbe/Devel/openair1/EMOS/MATLAB/')
%mm = imread('E:\2004_fd0006_250_c_0485.tif');
mm = imread('/extras/kaltenbe/EMOS/bellaterra_north.jpg');
cd('/extras/kaltenbe/EMOS/data/20081023_CHORIST_BARCELONA/Campus2/');

for term = 3 %1:4;

    filename = sprintf('data_term%d.mat',term);
    if ~exist(filename,'file')
        continue
    end
    load(filename);

    lat = [gps_data.latitude];
    lon = [gps_data.longitude];

    %%
    rx_rssi_dBm = cat(3,est.rx_rssi_dBm);
    h_fig = figure;
    plot_gps_coordinates_bellaterra(mm,lon,lat,mean(rx_rssi_dBm(1,:,1:100:end),2));
    title('Rx RSSI (dBm)')
    saveas(h_fig,sprintf('rx_rssi_term%d.jpg',term))

    %%
    rx_snr = cat(3,est.rx_power_dB) - cat(3,est.n0_power_dB);
    h_fig = figure;
    plot_gps_coordinates_bellaterra(mm,lon,lat,mean(rx_snr(1,:,1:100:end),2));
    title('Rx SNR (dB)')
    saveas(h_fig,sprintf('rx_snr_term%d.jpg',term))

    %%
    % err_ind = [0 diff([est.pdu_errors])];
    % err_ind(err_ind<0) = 0;
    % err_ind(err_ind>1) = 1;
    % err_100 = sum(reshape(err_ind,100,[]),1);
    err_ind_all = [est.crc_status];
    err_100 = sum(reshape(err_ind_all,100,[]),1);

    h_fig = figure;
    plot_gps_coordinates_bellaterra(mm,lon,lat,err_100(1:end));
    title('Rx FER (%)')
    saveas(h_fig,sprintf('rx_fer_term%d.jpg',term))
end