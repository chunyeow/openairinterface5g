%plot_results_eNb_cat

%clear all;
close all;
%clc;

%%
mm='cordes';
in = 0;

%% set throughput to 0 when UE was not connected
UE_connected = all(reshape([minestimates.UE_mode]==3,100,[]),1);
nn = fieldnames(throughput);
for n = 1:length(nn)
    eval([nn{n} '_cat(~UE_connected) = 0;']);
end

% %% get real throughput from modem
% ulsch_error = double([1 diff([minestimates_cat.ulsch_errors])]);
% rateps_modem = (1-ulsch_error) .* double([minestimates_cat.tbs]);
% rateps_modem([minestimates.UE_mode]~=3) = 0;
% rateps_modem = sum(reshape(rateps_modem,100,[]),1)*3;
% 
% ulsch_mcs = [minestimates_cat.mcs];
% mod_order = zeros(size(ulsch_mcs));
% mod_order(ulsch_mcs<10) = 2;
% mod_order(ulsch_mcs>=10 & ulsch_mcs<17) = 4;
% mod_order(ulsch_mcs>=17) = 6;
% rateps_uncoded_modem = 2700*mod_order;
% rateps_uncoded_modem([minestimates.UE_mode]~=3) = 0;
% rateps_uncoded_modem = sum(reshape(rateps_uncoded_modem,100,[]),1)*3;



%% plot coded throughput as CDFs
in = in+1;    
h_fig = figure(in);
colors = {'b','g','r','c','m','y','k','b--','g--','r--','c--','m--','y--','k--'};
legend_str = {};
ni=1;
for n = 1:length(nn)
    hold on
    si = strfind(nn{n},'64Qam');
    if si
        eval(['[f,x] = ecdf(' nn{n} '_cat);']);
        plot(x,f,colors{ni},'Linewidth',2);
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
xlabel('Throughput [bps]')
ylabel('P(x<abscissa)')
grid on
saveas(h_fig,fullfile(pathname,'UL_coded_throughput_cdf_comparison.eps'),'epsc2');

%% plot uncoded throughput as CDFs
in = in+1;    
h_fig = figure(in);
colors = {'b','g','r','c','m','y','k','b--','g--','r--','c--','m--','y--','k--'};
legend_str = {};
ni=1;
for n = 1:length(nn)
    hold on
    si = strfind(nn{n},'64Qam');
    if si
        eval(['[f,x] = ecdf(coded2uncoded(' nn{n} '_cat,''UL''));']);
        plot(x,f,colors{ni},'Linewidth',2);
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
xlabel('Uncoded Throughput [bps]')
ylabel('P(x<abscissa)')
grid on
saveas(h_fig,fullfile(pathname,'UL_uncoded_throughput_cdf_comparison.eps'),'epsc2');

if 0
%% plot as a function of the speed of the UE (histogram)

in = in+1;    
h_fig = figure(in);
title('Effective Throughputs for 1stRX');
xlabel('Speed[Meters/Second]');
ylabel('Throughput[Bits/sec]');
hold on;
plot([gps_data_cat.speed], rateps_SISO_64Qam_eNB1_1stRx_cat,  'bx');
plot([gps_data_cat.speed], rateps_alamouti_64Qam_eNB1_1stRx_cat,'go');
plot([gps_data_cat.speed], rateps_beamforming_64Qam_eNB1_1Rx_maxq_cat,'rd');
plot([gps_data_cat.speed], rateps_beamforming_64Qam_eNB1_1Rx_feedbackq_cat,'c*');
legend('SISO','Alamouti','Optimal Beamforming', 'Beamforming with feedback')

in = in+1;    
h_fig = figure(in);
title('Effective Throughputs for 2ndRX');
xlabel('Speed[Meters/Second]');
ylabel('Throughput[Bits/sec]');
hold on;
plot([gps_data_cat.speed], rateps_SISO_64Qam_eNB1_2ndRx_cat,  'bx');
plot([gps_data_cat.speed], rateps_alamouti_64Qam_eNB1_2ndRx_cat,'go');
plot([gps_data_cat.speed], rateps_beamforming_64Qam_eNB1_1Rx_maxq_cat,'rd');
plot([gps_data_cat.speed], rateps_beamforming_64Qam_eNB1_1Rx_feedbackq_cat,'c*');
legend('SISO','Alamouti','Optimal Beamforming', 'Beamforming with feedback')


in = in+1;    
h_fig = figure(in);
title('Effective Throughputs for 2RX');
xlabel('Speed[Meters/Second]');
ylabel('Throughput[Bits/sec]');
hold on;
plot([gps_data_cat.speed], rateps_SISO_64Qam_eNB1_2Rx_cat,  'bx');
plot([gps_data_cat.speed], rateps_alamouti_64Qam_eNB1_2Rx_cat,'go');
plot([gps_data_cat.speed], rateps_beamforming_64Qam_eNB1_2Rx_maxq_cat,'rd');
plot([gps_data_cat.speed], rateps_beamforming_64Qam_eNB1_2Rx_feedbackq_cat,'c*');
legend('SISO','Alamouti','Optimal Beamforming', 'Beamforming with feedback')


%% plot on map

in = in+1;    
h_fig = figure(in);
[gps_x, gps_y] = plot_gps_coordinates(mm, [gps_data_cat.longitude], [gps_data_cat.latitude], rateps_SISO_64Qam_eNB1_2Rx_cat);
title('SISO Throughput (2Rx)');
in = in+1;    
h_fig = figure(in);
[gps_x, gps_y] = plot_gps_coordinates(mm, [gps_data_cat.longitude], [gps_data_cat.latitude], rateps_alamouti_64Qam_eNB1_2Rx_cat);
title('Alamouti Througput (2Rx)');
in = in+1;    
h_fig = figure(in);
[gps_x, gps_y] = plot_gps_coordinates(mm, [gps_data_cat.longitude], [gps_data_cat.latitude], rateps_beamforming_64Qam_eNB1_2Rx_maxq_cat);
title('optimal Beamforming (2Rx)');
in = in+1;    
h_fig = figure(in);
[gps_x, gps_y] = plot_gps_coordinates(mm, [gps_data_cat.longitude], [gps_data_cat.latitude], rateps_beamforming_64Qam_eNB1_2Rx_feedbackq_cat);
title('Beamforming using feedback (2Rx)');
end

%% plot as a function of time
in = in+1;    
h_fig = figure(in);
title('Effective Throughputs for 1stRX');
xlabel('Time[Seconds]');
ylabel('Throughput[Bits/sec]');
hold on;
plot(rateps_SISO_64Qam_eNB1_1Rx_cat, 'bx');
plot(rateps_SISO_64Qam_eNB1_2Rx_cat, 'go');
%plot(rateps_modem,'rd')
legend('SISO 1Rx','SISO 2Rx')
saveas(h_fig,fullfile(pathname,'UL_coded_throughput_time.eps'),'epsc2');

in = in+1;    
h_fig = figure(in);
title('Uncoded Throughputs for 1stRX');
xlabel('Time[Seconds]');
ylabel('Throughput[Bits/sec]');
hold on;
plot(coded2uncoded(rateps_SISO_64Qam_eNB1_1Rx_cat,'UL'), 'bx');
plot(coded2uncoded(rateps_SISO_64Qam_eNB1_2Rx_cat,'UL'), 'go');
%plot(rateps_uncoded_modem,'rd')
legend('SISO 1Rx','SISO 2Rx')
saveas(h_fig,fullfile(pathname,'UL_uncoded_throughput_time.eps'),'epsc2');


if 0
%% plot as a function of distance from BS (histogram)

in = in+1;
h_fig = figure(in);
title('Effective Throughputs for 1stRX');
xlabel('Distance from BS [Km]');
ylabel('Throughput [Bits/sec]');
hold on;
plot(dist,rateps_SISO_64Qam_eNB1_1stRx_cat,  'bx');
plot(dist,rateps_alamouti_64Qam_eNB1_1stRx_cat,'go');
plot(dist,rateps_beamforming_64Qam_eNB1_1Rx_maxq_cat,'rd');
plot(dist,rateps_beamforming_64Qam_eNB1_1Rx_feedbackq_cat,'c*');
legend('SISO','Alamouti','Optimal Beamforming', 'Beamforming with feedback')

in = in+1;    
h_fig = figure(in);
title('Effective Throughputs for 2ndRX');
xlabel('Distance from BS [Km]');
ylabel('Throughput[Bits/sec]');
hold on;
plot(dist,rateps_SISO_64Qam_eNB1_2ndRx_cat,  'bx');
plot(dist,rateps_alamouti_64Qam_eNB1_2ndRx_cat,'go');
plot(dist,rateps_beamforming_64Qam_eNB1_1Rx_maxq_cat,'rd');
plot(dist,rateps_beamforming_64Qam_eNB1_1Rx_feedbackq_cat,'c*');
legend('SISO','Alamouti','Optimal Beamforming', 'Beamforming with feedback')


in = in+1;    
h_fig = figure(in);
title('Effective Throughputs for 2RX');
xlabel('Distance from BS [Km]');
ylabel('Throughput[Bits/sec]');
hold on;
plot(dist,rateps_SISO_64Qam_eNB1_2Rx_cat,  'bx');
plot(dist,rateps_alamouti_64Qam_eNB1_2Rx_cat,'go');
plot(dist,rateps_beamforming_64Qam_eNB1_2Rx_maxq_cat,'rd');
plot(dist,rateps_beamforming_64Qam_eNB1_2Rx_feedbackq_cat,'c*');
legend('SISO','Alamouti','Optimal Beamforming', 'Beamforming with feedback')
end


