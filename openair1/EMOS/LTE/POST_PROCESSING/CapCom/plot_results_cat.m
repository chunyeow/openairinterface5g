%plot_results_cat
load(fullfile(pathname,'results_cat_UE.mat'));

in = 0;

%plotwrtdistanceCalc;
[dist, dist_travelled] = calc_dist([gps_data_cat.latitude],[gps_data_cat.longitude],mm);
max_dist = ceil(max(dist));
speed = [gps_data_cat.speed];
time = [gps_data_cat.timestamp];
timebase = 1:length(time);
timebase2 = time-time(find(~isnan(time),1));
timebase_cuts = find(diff(timebase2)>60*60*8);


%% set throughput to 0 when UE was not synched
UE_connected = all(reshape(UE_mode_cat,100,[])==3,1);
UE_synched = all(reshape(UE_mode_cat,100,[])>0,1);
vars = whos('rateps*');
nn = {vars.name};
for n = 1:length(nn)
    eval([nn{n} '(~UE_synched) = 0;']);
end
K_fac_cat = reshape(K_fac_cat,4,[]);
K_fac_cat(:,~UE_synched) = nan;

% %% get real throughput from modem
% dlsch_error = double([1 diff([minestimates_cat.dlsch_errors])]);
% rateps_modem = (1-dlsch_error) .* double([minestimates_cat.tbs]);
% rateps_modem([minestimates.UE_mode]~=3) = 0;
% rateps_modem = sum(reshape(rateps_modem,100,[]),1)*6;
% 
% dlsch_mcs = [minestimates_cat.mcs];
% mod_order = zeros(size(dlsch_mcs));
% mod_order(dlsch_mcs<10) = 2;
% mod_order(dlsch_mcs>=10 & dlsch_mcs<17) = 4;
% mod_order(dlsch_mcs>=17) = 6;
% rateps_uncoded_modem = 2400*mod_order;
% rateps_uncoded_modem([minestimates.UE_mode]~=3) = 0;
% rateps_uncoded_modem = sum(reshape(rateps_uncoded_modem,100,[]),1)*6;

%% plot K-factor
in=in+1;
h_fig = figure(in);
hold off
plot_gps_coordinates(mm, [gps_data_cat.longitude], [gps_data_cat.latitude], 10*log10(mean(K_fac_cat,1)));
title('K_factor [dB]')
saveas(h_fig,fullfile(pathname,'K_factor_gps.jpg'),'jpg');

in=in+1;
h_fig = figure(in);
hold off
plot(10*log10(mean(K_fac_cat,1)));
xlabel('Time [sec]')
ylabel('K_factor [dB]')
plot(timebase_cuts,0,'k^','Markersize',10,'Linewidth',2)
saveas(h_fig,fullfile(pathname,'K_factor_time.eps'),'epsc2');


%% plot coded throughput as CDFs
in = in+1;    
h_fig = figure(in);
hold off
colors = {'b','g','r','c','m','y','k','b--','g--','r--','c--','m--','y--','k--'};
legend_str = {};
ni=1;
for n = 1:length(nn)
    si = strfind(nn{n},'supportedQam');
    if si
        eval(['[f,x] = ecdf(scale_ideal_tp(' nn{n} '));']);
        plot(x,f,colors{ni},'Linewidth',2);
        hold on
        legend_tmp = nn{n};
        legend_tmp(si:si+10) = [];
        legend_str{ni} = legend_tmp;
        ni=ni+1;
    end
end
% [f,x] = ecdf(rateps_modem);
% plot(x,f,colors{ni},'Linewidth',2);
% legend_str{ni} = 'rateps_modem';
% ni=ni+1;

legend(legend_str,'Interpreter','none','Location','SouthOutside');
title('DL Throughput CDF')
xlabel('Throughput [bps]')
ylabel('P(x<abscissa)')
grid on
saveas(h_fig,fullfile(pathname,'coded_throughput_cdf_comparison.eps'),'epsc2');


%% plot uncoded throughput as CDFs
in = in+1;    
h_fig = figure(in);
hold off
colors = {'b','g','r','c','m','y','k','b--','g--','r--','c--','m--','y--','k--'};
legend_str = {};
ni=1;
for n = 1:length(nn)
    si = strfind(nn{n},'supportedQam');
    if si
        eval(['[f,x] = ecdf(coded2uncoded(scale_ideal_tp(' nn{n} '),''DL''));']);
        plot(x,f,colors{ni},'Linewidth',2);
        hold on
        legend_tmp = nn{n};
        legend_tmp(si:si+10) = [];
        legend_str{ni} = legend_tmp;
        ni=ni+1;
    end
end
% [f,x] = ecdf(rateps_uncoded_modem);
% plot(x,f,colors{ni},'Linewidth',2);
% legend_str{ni} = 'rateps_modem';
% ni=ni+1;


legend(legend_str,'Interpreter','none','Location','SouthOutside');
xlabel('Uncoded throughput [bps]')
ylabel('P(x<abscissa)')
grid on
saveas(h_fig,fullfile(pathname,'uncoded_throughput_cdf_comparison.eps'),'epsc2');

% %% for all other comparisons set throughput to 0 when UE was not connected
% for n = 1:length(nn)
%     eval([nn{n} '(~UE_connected) = 0;']);
% end


%% plot on map
in = in+1;    
h_fig = figure(in);
hold off
[gps_x, gps_y] = plot_gps_coordinates(mm, [gps_data_cat.longitude], [gps_data_cat.latitude], scale_ideal_tp(rateps_SISO_supportedQam_eNB1_2Rx_cat), [0 8.64e6]);
title('Ideal Throughput (TX mode 1, 2 Rx)');
saveas(h_fig,fullfile(pathname,'coded_throughput_SISO_gps_2Rx.jpg'),'jpg');
in = in+1;    
h_fig = figure(in);
hold off
[gps_x, gps_y] = plot_gps_coordinates(mm, [gps_data_cat.longitude], [gps_data_cat.latitude], scale_ideal_tp(rateps_alamouti_supportedQam_eNB1_2Rx_cat), [0 8.64e6]);
title('Ideal Througput (TX mode 2, 2 Rx)');
saveas(h_fig,fullfile(pathname,'coded_throughput_Alamouti_gps_2Rx.jpg'),'jpg');
in = in+1;    
h_fig = figure(in);
hold off
[gps_x, gps_y] = plot_gps_coordinates(mm, [gps_data_cat.longitude], [gps_data_cat.latitude], scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_2Rx_maxq_cat), [0 8.64e6]);
title('Ideal Throughput (TX mode 6, optimal feedback, 2 Rx)');
saveas(h_fig,fullfile(pathname,'coded_throughput_optBeamforming_gps_2Rx.jpg'),'jpg');
in = in+1;    
h_fig = figure(in);
hold off
[gps_x, gps_y] = plot_gps_coordinates(mm, [gps_data_cat.longitude], [gps_data_cat.latitude], scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_2Rx_feedbackq_cat), [0 8.64e6]);
title('Ideal Throughput (TX mode 6, real feedback, 2Rx)');
saveas(h_fig,fullfile(pathname,'coded_throughput_feedbackBeamforming_gps_2Rx.jpg'),'jpg');



%% plot as a function of time
in = in+1;    
h_fig = figure(in);
hold off
title('Ideal Throughputs for 1stRX');
xlabel('Time[Seconds]');
ylabel('Throughput[Bits/sec]');
hold on;
plot(scale_ideal_tp(rateps_SISO_supportedQam_eNB1_1stRx_cat),  'bx');
plot(scale_ideal_tp(rateps_alamouti_supportedQam_eNB1_1stRx_cat),'go');
plot(scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_1Rx_maxq_cat),'rd');
plot(scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_1Rx_feedbackq_cat),'c*');
legend('TX mode 1','TX mode 2','TX mode 6, optimal feedback', 'TX mode 6, real feedback')
ylim([0 8.64e6]);
plot(timebase_cuts,0,'k^','Markersize',10,'Linewidth',2)
saveas(h_fig,fullfile(pathname,'coded_throughput_time_1stRx.eps'),'epsc2');

in = in+1;    
h_fig = figure(in);
hold off
title('Ideal Throughputs for 2ndRX');
xlabel('Time[Seconds]');
ylabel('Throughput[Bits/sec]');
hold on;
plot(scale_ideal_tp(rateps_SISO_supportedQam_eNB1_2ndRx_cat),  'bx');
plot(scale_ideal_tp(rateps_alamouti_supportedQam_eNB1_2ndRx_cat),'go');
plot(scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_1Rx_maxq_cat),'rd');
plot(scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_1Rx_feedbackq_cat),'c*');
legend('TX mode 1','TX mode 2','TX mode 6, optimal feedback', 'TX mode 6, real feedback')
ylim([0 8.64e6]);
plot(timebase_cuts,0,'k^','Markersize',10,'Linewidth',2)
saveas(h_fig,fullfile(pathname,'coded_throughput_time_2ndRx.eps'),'epsc2');


in = in+1;    
h_fig = figure(in);
hold off
title('Ideal Throughputs for 2RX');
xlabel('Time[Seconds]');
ylabel('Throughput[Bits/sec]');
hold on;
plot(scale_ideal_tp(rateps_SISO_supportedQam_eNB1_2Rx_cat),  'bx');
plot(scale_ideal_tp(rateps_alamouti_supportedQam_eNB1_2Rx_cat),'go');
plot(scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_2Rx_maxq_cat),'rd');
plot(scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_2Rx_feedbackq_cat),'c*');
legend('TX mode 1','TX mode 2','TX mode 6, optimal feedback', 'TX mode 6, real feedback')
ylim([0 8.64e6]);
plot(timebase_cuts,0,'k^','Markersize',10,'Linewidth',2)
saveas(h_fig,fullfile(pathname,'coded_throughput_time_2Rx.eps'),'epsc2');


%% comparison coded and uncoded
in = in+1;    
h_fig = figure(in);
hold off
title('Uncoded Ideal Throughputs for 2RX');
xlabel('Time[Seconds]');
ylabel('Throughput[Bits/sec]');
hold on;
%plot(rateps_SISO_supportedQam_eNB1_2Rx_cat,  'bx');
%plot(rateps_alamouti_supportedQam_eNB1_2Rx_cat,'go');
%plot(rateps_beamforming_supportedQam_eNB1_2Rx_maxq_cat,'rd');
%plot(rateps_beamforming_supportedQam_eNB1_2Rx_feedbackq_cat,'c*');
plot(coded2uncoded(scale_ideal_tp(rateps_SISO_supportedQam_eNB1_2Rx_cat),'DL'),  'bx');
plot(coded2uncoded(scale_ideal_tp(rateps_alamouti_supportedQam_eNB1_2Rx_cat),'DL'),'go');
plot(coded2uncoded(scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_2Rx_maxq_cat),'DL'),'rd');
plot(coded2uncoded(scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_2Rx_feedbackq_cat),'DL'),'c*');
legend('TX mode 1','TX mode 2','TX mode 6, optimal feedback', 'TX mode 6, real feedback')
ylim([0 8.64e6]);
plot(timebase_cuts,0,'k^','Markersize',10,'Linewidth',2)
saveas(h_fig,fullfile(pathname,'ideal_uncoded_throughput_time_2Rx.eps'),'epsc2');

%% extrapolation to loaded cell
in = pfair(scale_ideal_tp(rateps_SISO_supportedQam_eNB1_2Rx_cat),[gps_data_cat.longitude], [gps_data_cat.latitude],dist,mm,pathname,'mode1_2Rx',in);
in = pfair(scale_ideal_tp(rateps_alamouti_supportedQam_eNB1_2Rx_cat),[gps_data_cat.longitude], [gps_data_cat.latitude],dist,mm,pathname,'mode2_2Rx',in);
in = pfair(scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_2Rx_maxq_cat),[gps_data_cat.longitude], [gps_data_cat.latitude],dist,mm,pathname,'mode6_maxq_2Rx',in);
in = pfair(scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_2Rx_feedbackq_cat),[gps_data_cat.longitude], [gps_data_cat.latitude],dist,mm,pathname,'mode6_feedbackq_2Rx',in);


%% plot as a function of distance from BS (histogram)

% 1st Rx antenna
in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(dist, scale_ideal_tp(rateps_SISO_supportedQam_eNB1_1stRx_cat),  0:max_dist, 1);
title('Ideal Throughput vs Dist for Mode1, 1stRX');
xlabel('Dist[km]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_dist_mode1_1stRx.eps'),'epsc2');

in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(dist, scale_ideal_tp(rateps_alamouti_supportedQam_eNB1_1stRx_cat),  0:max_dist, 1);
title('Ideal Throughput vs Dist for Mode2, 1stRX');
xlabel('Dist[km]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_dist_mode2_1stRx.eps'),'epsc2');

in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(dist, scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_1Rx_maxq_cat),  0:max_dist, 1);
title('Ideal Throughput vs Dist for Mode6, ideal feedback, 1stRX');
xlabel('Dist[km]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_dist_mode6_maxq_1stRx.eps'),'epsc2');

in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(dist, scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_1Rx_feedbackq_cat),  0:max_dist, 1);
title('Ideal Throughput vs Dist for Mode6, real feedback, 1stRX');
xlabel('Dist[km]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_dist_mode6_feedbackq_1stRx.eps'),'epsc2');

% 2nd Rx antenna
in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(dist, scale_ideal_tp(rateps_SISO_supportedQam_eNB1_2ndRx_cat),  0:max_dist, 1);
title('Ideal Throughput vs Dist for Mode1, 2ndRX');
xlabel('Dist[km]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_dist_mode1_2ndRx.eps'),'epsc2');

in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(dist, scale_ideal_tp(rateps_alamouti_supportedQam_eNB1_2ndRx_cat),  0:max_dist, 1);
title('Ideal Throughput vs Dist for Mode2, 2ndRX');
xlabel('Dist[km]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_dist_mode2_2ndRx.eps'),'epsc2');

% both Rx antennas
in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(dist, scale_ideal_tp(rateps_SISO_supportedQam_eNB1_2Rx_cat),  0:max_dist, 1);
title('Ideal Throughput vs Dist for Mode1, 2RX');
xlabel('Dist[km]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_dist_mode1_2Rx.eps'),'epsc2');

in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(dist, scale_ideal_tp(rateps_alamouti_supportedQam_eNB1_2Rx_cat),  0:max_dist, 1);
title('Ideal Throughput vs Dist for Mode2, 2RX');
xlabel('Dist[km]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_dist_mode2_2Rx.eps'),'epsc2');

in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(dist, scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_2Rx_maxq_cat),  0:max_dist, 1);
title('Ideal Throughput vs Dist for Mode6, ideal feedback, 2RX');
xlabel('Dist[km]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_dist_mode6_maxq_2Rx.eps'),'epsc2');

in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(dist, scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_2Rx_feedbackq_cat),  0:max_dist, 1);
title('Ideal Throughput vs Dist for Mode6, real feedback, 2RX');
xlabel('Dist[km]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_dist_mode6_feedbackq_2Rx.eps'),'epsc2');

%% plot as a function of the speed of the UE (histogram)

% 1st Rx antenna
in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(speed, scale_ideal_tp(rateps_SISO_supportedQam_eNB1_1stRx_cat),  0:5:40);
title('Ideal Throughput vs Speed for Mode1, 1stRX');
xlabel('Speed[Meters/Second]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_speed_mode1_1stRx.eps'),'epsc2');

in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(speed, scale_ideal_tp(rateps_alamouti_supportedQam_eNB1_1stRx_cat),  0:5:40);
title('Ideal Throughput vs Speed for Mode2, 1stRX');
xlabel('Speed[Meters/Second]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_speed_mode2_1stRx.eps'),'epsc2');

in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(speed, scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_1Rx_maxq_cat),  0:5:40);
title('Ideal Throughput vs Speed for Mode6, ideal feedback, 1stRX');
xlabel('Speed[Meters/Second]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_speed_mode6_maxq_1stRx.eps'),'epsc2');

in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(speed, scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_1Rx_feedbackq_cat),  0:5:40);
title('Ideal Throughput vs Speed for Mode6, real feedback, 1stRX');
xlabel('Speed[Meters/Second]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_speed_mode6_feedbackq_1stRx.eps'),'epsc2');

% 2nd Rx antenna
in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(speed, scale_ideal_tp(rateps_SISO_supportedQam_eNB1_2ndRx_cat),  0:5:40);
title('Ideal Throughput vs Speed for Mode1, 2ndRX');
xlabel('Speed[Meters/Second]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_speed_mode1_2ndRx.eps'),'epsc2');

in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(speed, scale_ideal_tp(rateps_alamouti_supportedQam_eNB1_2ndRx_cat),  0:5:40);
title('Ideal Throughput vs Speed for Mode2, 2ndRX');
xlabel('Speed[Meters/Second]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_speed_mode2_2ndRx.eps'),'epsc2');

% both Rx antennas
in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(speed, scale_ideal_tp(rateps_SISO_supportedQam_eNB1_2Rx_cat),  0:5:40);
title('Ideal Throughput vs Speed for Mode1, 2RX');
xlabel('Speed[Meters/Second]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_speed_mode1_2Rx.eps'),'epsc2');

in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(speed, scale_ideal_tp(rateps_alamouti_supportedQam_eNB1_2Rx_cat),  0:5:40);
title('Ideal Throughput vs Speed for Mode2, 2RX');
xlabel('Speed[Meters/Second]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_speed_mode2_2Rx.eps'),'epsc2');

in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(speed, scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_2Rx_maxq_cat),  0:5:40);
title('Ideal Throughput vs Speed for Mode6, ideal feedback, 2RX');
xlabel('Speed[Meters/Second]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_speed_mode6_maxq_2Rx.eps'),'epsc2');

in = in+1;    
h_fig = figure(in);
hold off
plot_in_bins(speed, scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_2Rx_feedbackq_cat),  0:5:40);
title('Ideal Throughput vs Speed for Mode6, real feedback, 2RX');
xlabel('Speed[Meters/Second]');
ylabel('Throughput[Bits/sec]');
ylim([0 8.64e6]);
saveas(h_fig,fullfile(pathname,'ideal_throughput_speed_mode6_feedbackq_2Rx.eps'),'epsc2');

%% plot as function of K_factor
in = in+1;
h_fig = figure(in);
plot_in_bins(10*log10(mean(K_fac_cat,1)),scale_ideal_tp(rateps_SISO_supportedQam_eNB1_2Rx_cat),-20:3:30);
xlabel('Ricean K-Factor [dB]')
ylabel('Throughput [bps]')
saveas(h_fig,fullfile(pathname,'throughput_vs_Kfactor_mode1_2Rx.eps'),'epsc2')

in = in+1;
h_fig = figure(in);
plot_in_bins(10*log10(mean(K_fac_cat,1)),scale_ideal_tp(rateps_alamouti_supportedQam_eNB1_2Rx_cat),-20:3:30);
xlabel('Ricean K-Factor [dB]')
ylabel('Throughput [bps]')
saveas(h_fig,fullfile(pathname,'throughput_vs_Kfactor_mode2_2Rx.eps'),'epsc2')

in = in+1;
h_fig = figure(in);
plot_in_bins(10*log10(mean(K_fac_cat,1)),scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_2Rx_maxq_cat),-20:3:30);
xlabel('Ricean K-Factor [dB]')
ylabel('Throughput [bps]')
saveas(h_fig,fullfile(pathname,'throughput_vs_Kfactor_mode6_maxq_2Rx.eps'),'epsc2')

in = in+1;
h_fig = figure(in);
plot_in_bins(10*log10(mean(K_fac_cat,1)),scale_ideal_tp(rateps_beamforming_supportedQam_eNB1_2Rx_feedbackq_cat),-20:3:30);
xlabel('Ricean K-Factor [dB]')
ylabel('Throughput [bps]')
saveas(h_fig,fullfile(pathname,'throughput_vs_Kfactor_mode6_feedbackq_2Rx.eps'),'epsc2')

