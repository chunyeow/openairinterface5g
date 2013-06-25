% file: plot_comparison_all_modes

close all
clear all
h_fig = 0;

addpath('~/Devel/matlab/IPDM')
addpath('CapCom')
addpath('maps')

% pathname = '/media/disk/PENNE/';
% mm = 'penne';
pathname = '/media/data/AMBIALET/';
mm = 'ambialet';
%pathname = '/emos/EMOS/';
%mm = 'cordes';

%%
mode1 = load(fullfile(pathname,'Mode1/results/results_UE.mat'));
mode1_ul = load(fullfile(pathname,'Mode1/results/results_eNB.mat'));
mode2 = load(fullfile(pathname,'Mode2/results/results_UE.mat'));
if strcmp(mm,'cordes')
    mode2_update = load(fullfile(pathname,'Mode2_update/results/results_UE.mat'));
    mode2_ul = load(fullfile(pathname,'Mode2_update/results/results_eNB.mat'));
else
    mode2_ul = load(fullfile(pathname,'Mode2/results/results_eNB.mat'));
end
mode6 = load(fullfile(pathname,'Mode6/results/results_UE.mat'));
mode6_ul = load(fullfile(pathname,'Mode6/results/results_eNB.mat'));

mode2_ideal = load(fullfile(pathname,'/Mode2/results/results_cat_UE.mat'));

%%
mode1.UE_synched = (mode1.UE_mode_cat>0);
mode1.UE_connected = (mode1.UE_mode_cat==3);
mode1.throughput = double(100./(100+mode1.dlsch_fer_cat).*mode1.tbs_cat.*6.*100);
mode1.good = (mode1.dlsch_fer_cat<=100 & mode1.dlsch_fer_cat>=0).';

mode2.UE_synched = (mode2.UE_mode_cat>0);
mode2.UE_connected = (mode2.UE_mode_cat==3);
mode2.throughput = double(100./(100+mode2.dlsch_fer_cat).*mode2.tbs_cat.*6.*100);
mode2.good = (mode2.dlsch_fer_cat<=100 & mode2.dlsch_fer_cat>=0).';

if strcmp(mm,'cordes')
    mode2_update.UE_synched = (mode2_update.UE_mode_cat>0);
    mode2_update.UE_connected = (mode2_update.UE_mode_cat==3);
end

mode6.UE_synched = (mode6.UE_mode_cat>0);
mode6.UE_connected = (mode6.UE_mode_cat==3);
mode6.throughput = double(100./(100+mode6.dlsch_fer_cat).*mode6.tbs_cat.*6.*100);
mode6.good = (mode6.dlsch_fer_cat<=100 & mode6.dlsch_fer_cat>=0).';

mode2_ideal.UE_connected = (mode2_ideal.UE_mode_cat(1:100:end)==3);
mode2_ideal.UE_synched = (mode2_ideal.UE_mode_cat(1:100:end)>0);

%%
mode1_ul.ulsch_fer_cat = [100 diff(mode1_ul.ulsch_errors_cat)];
mode1_ul.ulsch_throughput = double(mode1_ul.tbs_cat) .* double(100 - mode1_ul.ulsch_fer_cat) .* 3;
mode1_ul.eNB_connected = ([mode1_ul.eNb_UE_stats_cat(:).UE_mode]==3);
mode1_ul.ulsch_throughput(1,~mode1_ul.eNB_connected) = 0;
mode1_ul.ulsch_throughput_ideal_1Rx = scale_ideal_tp(mode1_ul.Rate_64Qam_1RX_cat*100);
mode1_ul.ulsch_throughput_ideal_2Rx = scale_ideal_tp(mode1_ul.Rate_64Qam_2RX_cat*100);
mode1_ul.good = ~isnan(mode1_ul.ulsch_throughput_ideal_1Rx);
mode1_ul.ulsch_throughput_ideal_1Rx(~mode1_ul.eNB_connected,1) = 0;
mode1_ul.ulsch_throughput_ideal_2Rx(~mode1_ul.eNB_connected,1) = 0;

mode2_ul.ulsch_fer_cat = [100 diff(mode2_ul.ulsch_errors_cat)];
mode2_ul.ulsch_throughput = double(mode2_ul.tbs_cat) .* double(100 - mode2_ul.ulsch_fer_cat) .* 3;
mode2_ul.eNB_connected = ([mode2_ul.eNb_UE_stats_cat(:).UE_mode]==3);
mode2_ul.ulsch_throughput(1,~mode2_ul.eNB_connected) = 0;
mode2_ul.ulsch_throughput_ideal_1Rx = scale_ideal_tp(mode2_ul.Rate_64Qam_1RX_cat*100);
mode2_ul.ulsch_throughput_ideal_2Rx = scale_ideal_tp(mode2_ul.Rate_64Qam_2RX_cat*100);
mode2_ul.good = ~isnan(mode2_ul.ulsch_throughput_ideal_1Rx);
mode2_ul.ulsch_throughput_ideal_1Rx(~mode2_ul.eNB_connected,1) = 0;
mode2_ul.ulsch_throughput_ideal_2Rx(~mode2_ul.eNB_connected,1) = 0;

mode6_ul.ulsch_fer_cat = [100 diff(mode6_ul.ulsch_errors_cat)];
mode6_ul.ulsch_throughput = double(mode6_ul.tbs_cat) .* double(100 - mode6_ul.ulsch_fer_cat) .* 3;
mode6_ul.eNB_connected = ([mode6_ul.eNb_UE_stats_cat(:).UE_mode]==3);
mode6_ul.ulsch_throughput(1,~mode6_ul.eNB_connected) = 0;
mode6_ul.ulsch_throughput_ideal_1Rx = scale_ideal_tp(mode6_ul.Rate_64Qam_1RX_cat*100);
mode6_ul.ulsch_throughput_ideal_2Rx = scale_ideal_tp(mode6_ul.Rate_64Qam_2RX_cat*100);
mode6_ul.good = ~isnan(mode6_ul.ulsch_throughput_ideal_1Rx);
mode6_ul.ulsch_throughput_ideal_1Rx(~mode6_ul.eNB_connected,1) = 0;
mode6_ul.ulsch_throughput_ideal_2Rx(~mode6_ul.eNB_connected,1) = 0;


%% calc distance
[mode1.dist, mode1.dist_travelled] = calc_dist(mode1.gps_lat_cat,mode1.gps_lon_cat,mm);
[mode2.dist, mode2.dist_travelled] = calc_dist(mode2.gps_lat_cat,mode2.gps_lon_cat,mm);
[mode6.dist, mode6.dist_travelled] = calc_dist(mode6.gps_lat_cat,mode6.gps_lon_cat,mm);


%% define parcours 1
NFrames_cum = cumsum(mode2.NFrames);
parcours1 = false(1,length(mode2.timestamp_cat));

switch mm
    case 'cordes'
        idx(1) = 1; %start
        idx(2) = strmatch('data_term3_idx30_20100511T164443.EMOS',char(mode2.filenames)); %end
        idx(3) = strmatch('data_term3_idx22_20100518T145355.EMOS',char(mode2.filenames)); %start
        idx(4) = strmatch('data_term3_idx11_20100520T153238.EMOS',char(mode2.filenames)); %end

        frame_idx = NFrames_cum(idx)/100;
        parcours1(frame_idx(1):frame_idx(2)) = true;
        parcours1(frame_idx(3):frame_idx(4)) = true;
    case 'penne'
        % double check this - do we neglect the coverage roads here?
        idx = strmatch('data_term3_idx72_20100701T151158.EMOS',char(mode2.filenames));
        frame_idx = NFrames_cum(idx)/100;
        parcours1(1:frame_idx) = true;
    case 'ambialet'
        idx(1) = strmatch('data_term3_idx34_20100719T191120.EMOS',char(mode2.filenames)); %last file before parcours 1
        idx(2) = strmatch('data_term3_idx07_20100721T180138.EMOS',char(mode2.filenames)); %last file of parcours 1

        frame_idx = NFrames_cum(idx)/100;
        parcours1(frame_idx(1)+1:frame_idx(2)) = true;
end


%% find a common subset of points
% take the points from mode6 where UE_connected==1 as basis
% then for each of those points find points in the other set that are close
tol = 0.0001;
mode1_gps = [mode1.gps_lat_cat.' mode1.gps_lon_cat.'];
mode2_gps = [mode2.gps_lat_cat.' mode2.gps_lon_cat.'];
mode6_gps = [mode6.gps_lat_cat.' mode6.gps_lon_cat.'];
mode1.ind_common = false(1,length(mode1_gps));
mode2.ind_common = false(1,length(mode2_gps));
mode6.ind_common = false(1,length(mode6_gps));
for i=1:length(mode6_gps)
    mode1_d = ipdm(mode6_gps(i,:),mode1_gps,'Subset','Maximum','Limit',tol);
    mode2_d = ipdm(mode6_gps(i,:),mode2_gps,'Subset','Maximum','Limit',tol);
    mode1.ind_common(mode1_d<=tol) = true;
    mode2.ind_common(mode2_d<=tol) = true;
    if any(mode1_d<=tol) && any(mode2_d<=tol)
        mode6.ind_common(i) = true;
    end
end

%% find routes taken more than once
mode1.ind_duplicate = filter_routes(mode1.gps_lat_cat,mode1.gps_lon_cat,mode1.gps_time_cat);
mode2.ind_duplicate = filter_routes(mode2.gps_lat_cat,mode2.gps_lon_cat,mode2.gps_time_cat);
mode6.ind_duplicate = filter_routes(mode6.gps_lat_cat,mode6.gps_lon_cat,mode6.gps_time_cat);

% sum(parcours1 & ~mode2.ind_duplicate & ~mode2.ind_common)
% sum(~mode1.ind_duplicate & ~mode1.ind_common)

% add mode2.throughput( parcours1 & ~mode2.ind_duplicate &
% ~mode2.ind_common) to the points in mode6.throughput

%% set throughput to 0 when not connected resp. not synched
mode1.throughput(~mode1.UE_connected | ~mode1.good) = 0;
mode2.throughput(~mode2.UE_connected | ~mode2.good) = 0;
mode6.throughput(~mode6.UE_connected | ~mode6.good) = 0;
nn = fieldnames(mode2_ideal);
for n = strmatch('rateps',nn).'
    mode2_ideal.(nn{n})(~mode2_ideal.UE_synched) = 0;
end

% %% select common routes of parcours 1 that have only been taken once
% mode1.throughput(~mode1.ind_common | mode1.ind_duplicate) = nan;
% mode2.throughput(~mode2.ind_common | mode2.ind_duplicate) = nan;
% mode6.throughput(~mode6.ind_common | mode6.ind_duplicate) = nan;
% nn = fieldnames(mode2_ideal);
% for n = strmatch('rateps',nn).'
%     mode2_ideal.(nn{n})(~mode2.ind_common | mode2.ind_duplicate) = nan;
% end

%%
h_fig = h_fig+1;
figure(h_fig);
hold off
colors = {'b','g','r','c','m','y','k','b--','g--','r--','c--','m--','y--','k--'};
legend_str = {};
ni=1;
for n = strmatch('rateps',nn).'
    hold on
    si = strfind(nn{n},'supportedQam');
    if si
        [f,x] = ecdf(scale_ideal_tp(mode2_ideal.(nn{n})(mode2.ind_common & ~mode2.ind_duplicate)));
        plot(x,f,colors{ni},'Linewidth',2);
        legend_tmp = nn{n};
        legend_tmp(si:si+10) = [];
        legend_str{ni} = legend_tmp;
        ni=ni+1;
    end
end
[f,x] = ecdf(mode1.throughput(mode1.ind_common & ~mode1.ind_duplicate));
plot(x,f,colors{ni},'Linewidth',2)
legend_str{ni} = sprintf('TX mode 1 (%d pts)',sum((mode1.ind_common & ~mode1.ind_duplicate)));
ni=ni+1;
[f,x] = ecdf(mode2.throughput(mode2.ind_common & ~mode2.ind_duplicate));
plot(x,f,colors{ni},'Linewidth',2)
legend_str{ni} = sprintf('Tx mode 2 (%d pts)',sum(mode2.ind_common & ~mode2.ind_duplicate));
ni=ni+1;
[f,x] = ecdf(mode6.throughput(mode6.ind_common & ~mode6.ind_duplicate));
plot(x,f,colors{ni},'Linewidth',2)
legend_str{ni} = sprintf('Tx mode 6 (%d pts)',sum(mode6.ind_common & ~mode6.ind_duplicate));
ni=ni+1;

title('DLSCH throughput CDF')
xlabel('Throughput [bps]')
ylabel('P(x<abscissa)')
legend(legend_str,'Interpreter','none','Location','SouthOutside');
grid on
saveas(h_fig,fullfile(pathname,'results','throughput_cdf_comparison.eps'),'epsc2')

%% FDD
h_fig = h_fig+1;
figure(h_fig);
hold off
colors = {'b','g','r','c','m','y','k','b--','g--','r--','c--','m--','y--','k--'};
legend_str = {};
ni=1;
for n = strmatch('rateps',nn).'
    hold on
    si = strfind(nn{n},'supportedQam');
    if si
        [f,x] = ecdf(scale_ideal_tp(mode2_ideal.(nn{n})(mode2.ind_common & ~mode2.ind_duplicate))*10/6);
        plot(x,f,colors{ni},'Linewidth',2);
        legend_tmp = nn{n};
        legend_tmp(si:si+10) = [];
        legend_str{ni} = legend_tmp;
        ni=ni+1;
    end
end
[f,x] = ecdf(mode1.throughput(mode1.ind_common & ~mode1.ind_duplicate)*10/6);
plot(x,f,colors{ni},'Linewidth',2)
        legend_str{ni} = 'TX mode 1';
        ni=ni+1;
[f,x] = ecdf(mode2.throughput(mode2.ind_common & ~mode2.ind_duplicate)*10/6);
plot(x,f,colors{ni},'Linewidth',2)
        legend_str{ni} = 'Tx mode 2';
        ni=ni+1;
[f,x] = ecdf(mode6.throughput(mode6.ind_common & ~mode6.ind_duplicate)*10/6);
plot(x,f,colors{ni},'Linewidth',2)
        legend_str{ni} = 'Tx mode 6';
        ni=ni+1;

title('DLSCH throughput CDF for FDD')
xlabel('Throughput [bps]')
ylabel('P(x<abscissa)')
legend(legend_str,'Interpreter','none','Location','SouthOutside');
grid on
saveas(h_fig,fullfile(pathname,'results','throughput_cdf_comparison_fdd.eps'),'epsc2')


%% Uncoded throughput CDF comparison
h_fig = h_fig+1;
figure(h_fig);
hold off
colors = {'b','g','r','c','m','y','k','b--','g--','r--','c--','m--','y--','k--'};
legend_str = {};
ni=1;
for n = strmatch('rateps',nn).'
    hold on
    si = strfind(nn{n},'supportedQam');
    if si
        [f,x] = ecdf(coded2uncoded(scale_ideal_tp(mode2_ideal.(nn{n})(mode2.ind_common & ~mode2.ind_duplicate)),'DL'));
        plot(x,f,colors{ni},'Linewidth',2);
        legend_tmp = nn{n};
        legend_tmp(si:si+10) = [];
        legend_str{ni} = legend_tmp;
        ni=ni+1;
    end
end
[f,x] = ecdf(coded2uncoded(mode1.throughput(mode1.ind_common & ~mode1.ind_duplicate),'DL'));
plot(x,f,colors{ni},'Linewidth',2)
legend_str{ni} = 'TX mode 1';
ni=ni+1;
[f,x] = ecdf(coded2uncoded(mode2.throughput(mode2.ind_common & ~mode2.ind_duplicate),'DL'));
plot(x,f,colors{ni},'Linewidth',2)
legend_str{ni} = 'Tx mode 2';
ni=ni+1;
[f,x] = ecdf(coded2uncoded(mode6.throughput(mode6.ind_common & ~mode6.ind_duplicate),'DL'));
plot(x,f,colors{ni},'Linewidth',2)
legend_str{ni} = 'Tx mode 6';
ni=ni+1;

title('DLSCH uncoded throughput CDF')
xlabel('Throughput [bps]')
ylabel('P(x<abscissa)')
legend(legend_str,'Interpreter','none','Location','SouthOutside');
grid on
saveas(h_fig,fullfile(pathname,'results','DLSCH_uncoded_throughput_cdf_comparison.eps'),'epsc2')

%% Kfactor vs throughput
h_fig = h_fig + 1;
figure(h_fig)
mode2_ideal.K_fac_cat = reshape(mode2_ideal.K_fac_cat,4,[]);
mode2_ideal.K_fac_cat(:,~mode2_ideal.UE_synched) = nan;
plot_in_bins(10*log10(mean(mode2_ideal.K_fac_cat,1)),mode2.throughput,-20:3:30);
xlabel('Ricean K-Factor [dB]')
ylabel('Throughput [bps]')
saveas(h_fig,fullfile(pathname,'Mode2','results','throughput_vs_Kfactor.eps'),'epsc2')

%% Kfactor vs UL throughput
h_fig = h_fig + 1;
figure(h_fig)
mode2_ul.K_fac_cat = reshape(mode2_ul.K_fac_cat,4,[]);
mode2_ul.K_fac_cat(:,~mode2_ul.eNB_connected) = nan;
plot_in_bins(10*log10(mean(mode2_ul.K_fac_cat,1)),mode2_ul.ulsch_throughput,-20:3:30);
xlabel('Ricean K-Factor [dB]')
ylabel('Throughput [bps]')
saveas(h_fig,fullfile(pathname,'Mode2','results','throughput_vs_Kfactor_ul.eps'),'epsc2')

%% Kfactor vs UL throughput
h_fig = h_fig + 1;
figure(h_fig)
plot_in_bins(10*log10(mean(mode2_ul.K_fac_cat,1)),mode2_ul.ulsch_throughput_ideal_1Rx,-20:3:30);
xlabel('Ricean K-Factor [dB]')
ylabel('Throughput [bps]')
saveas(h_fig,fullfile(pathname,'Mode2','results','throughput_vs_Kfactor_ul_ideal_1Rx.eps'),'epsc2')

%% Kfactor vs UL throughput
h_fig = h_fig + 1;
figure(h_fig)
plot_in_bins(10*log10(mean(mode2_ul.K_fac_cat,1)),mode2_ul.ulsch_throughput_ideal_2Rx,-20:3:30);
xlabel('Ricean K-Factor [dB]')
ylabel('Throughput [bps]')
saveas(h_fig,fullfile(pathname,'Mode2','results','throughput_vs_Kfactor_ul_ideal_2Rx.eps'),'epsc2')


% %% Coded throughput CDF comparison (when connected)
% mode1.throughput(~mode1.UE_connected | ~mode1.good) = nan;
% mode2.throughput(~mode2.UE_connected | ~mode2.good) = nan;
% mode6.throughput(~mode6.UE_connected | ~mode6.good) = nan;
% nn = fieldnames(mode2_ideal);
% for n = strmatch('rateps',nn).'
%     mode2_ideal.(nn{n})(~mode2_ideal.UE_connected) = nan;
% end

%%
h_fig = h_fig+1;
figure(h_fig);
hold off
colors = {'b','g','r','c','m','y','k','b--','g--','r--','c--','m--','y--','k--'};
legend_str = {};
ni=1;
for n = strmatch('rateps',nn).'
    hold on
    si = strfind(nn{n},'supportedQam');
    if si
        [f,x] = ecdf(scale_ideal_tp(mode2_ideal.(nn{n})(mode2.ind_common & ~mode2.ind_duplicate & mode2.UE_connected)));
        plot(x,f,colors{ni},'Linewidth',2);
        legend_tmp = nn{n};
        legend_tmp(si:si+10) = [];
        legend_str{ni} = legend_tmp;
        ni=ni+1;
    end
end
[f,x] = ecdf(mode1.throughput(mode1.ind_common & ~mode1.ind_duplicate & mode1.UE_connected));
plot(x,f,colors{ni},'Linewidth',2)
legend_str{ni} = sprintf('TX mode 1 (%d pts)',sum(mode1.ind_common & ~mode1.ind_duplicate & mode1.UE_connected));
ni=ni+1;
[f,x] = ecdf(mode2.throughput(mode2.ind_common & ~mode2.ind_duplicate & mode2.UE_connected));
plot(x,f,colors{ni},'Linewidth',2)
legend_str{ni} = sprintf('Tx mode 2 (%d pts)',sum(mode2.ind_common & ~mode2.ind_duplicate & mode2.UE_connected));
ni=ni+1;
[f,x] = ecdf(mode6.throughput(mode6.ind_common & ~mode6.ind_duplicate & mode6.UE_connected));
plot(x,f,colors{ni},'Linewidth',2)
legend_str{ni} = sprintf('Tx mode 6 (%d pts)',sum(mode6.ind_common & ~mode6.ind_duplicate & mode6.UE_connected));
ni=ni+1;

title('DLSCH throughput CDF (when connected)')
xlabel('Throughput [bps]')
ylabel('P(x<abscissa)')
legend(legend_str,'Interpreter','none','Location','SouthOutside');
grid on
saveas(h_fig,fullfile(pathname,'results','throughput_connected_cdf_comparison.eps'),'epsc2')

%% Uncoded throughput CDF comparison (when connected)
h_fig = h_fig+1;
figure(h_fig);
hold off
colors = {'b','g','r','c','m','y','k','b--','g--','r--','c--','m--','y--','k--'};
legend_str = {};
ni=1;
for n = strmatch('rateps',nn).'
    hold on
    si = strfind(nn{n},'supportedQam');
    if si
        [f,x] = ecdf(coded2uncoded(scale_ideal_tp(mode2_ideal.(nn{n})(mode2.ind_common & ~mode2.ind_duplicate & mode2.UE_connected)),'DL'));
        plot(x,f,colors{ni},'Linewidth',2);
        legend_tmp = nn{n};
        legend_tmp(si:si+10) = [];
        legend_str{ni} = legend_tmp;
        ni=ni+1;
    end
end
[f,x] = ecdf(coded2uncoded(mode1.throughput(mode1.ind_common & ~mode1.ind_duplicate & mode1.UE_connected),'DL'));
plot(x,f,colors{ni},'Linewidth',2)
legend_str{ni} = 'TX mode 1';
ni=ni+1;
[f,x] = ecdf(coded2uncoded(mode2.throughput(mode2.ind_common & ~mode2.ind_duplicate & mode2.UE_connected),'DL'));
plot(x,f,colors{ni},'Linewidth',2)
legend_str{ni} = 'Tx mode 2';
ni=ni+1;
[f,x] = ecdf(coded2uncoded(mode6.throughput(mode6.ind_common & ~mode6.ind_duplicate & mode6.UE_connected),'DL'));
plot(x,f,colors{ni},'Linewidth',2)
legend_str{ni} = 'Tx mode 6';
ni=ni+1;

title('DLSCH uncoded throughput CDF (when connected)')
xlabel('Throughput [bps]')
ylabel('P(x<abscissa)')
legend(legend_str,'Interpreter','none','Location','SouthOutside');
grid on
saveas(h_fig,fullfile(pathname,'results','DLSCH_uncoded_throughput_connected_cdf_comparison.eps'),'epsc2')

%% UL Coded throughput CDF comparison
h_fig = h_fig+1;
figure(h_fig);
hold off
[f,x] = ecdf([mode1_ul.ulsch_throughput mode2_ul.ulsch_throughput mode6_ul.ulsch_throughput]);
plot(x,f,'b','Linewidth',2)
hold on
[f,x] = ecdf([mode1_ul.ulsch_throughput_ideal_1Rx; mode2_ul.ulsch_throughput_ideal_1Rx; mode6_ul.ulsch_throughput_ideal_1Rx]);
plot(x,f,'g','Linewidth',2)
[f,x] = ecdf([mode1_ul.ulsch_throughput_ideal_2Rx; mode2_ul.ulsch_throughput_ideal_2Rx; mode6_ul.ulsch_throughput_ideal_2Rx]);
plot(x,f,'r','Linewidth',2)
xlim([0 4.86e6])
legend('modem','ideal 1 rx antenna','ideal 2 rx antennas','Location','SouthEast');
title('UL Throughput CDF')
xlabel('UL Throughput [bps]')
ylabel('P(x<abscissa)')
grid on
saveas(h_fig,fullfile(pathname,'results','UL_throughput_cdf_comparison.eps'),'epsc2')

%% UL FDD
h_fig = h_fig+1;
figure(h_fig);
hold off
[f,x] = ecdf([mode1_ul.ulsch_throughput mode2_ul.ulsch_throughput mode6_ul.ulsch_throughput]*10/3);
plot(x,f,'b','Linewidth',2)
hold on
[f,x] = ecdf([mode1_ul.ulsch_throughput_ideal_1Rx; mode2_ul.ulsch_throughput_ideal_1Rx; mode6_ul.ulsch_throughput_ideal_1Rx]*10/3);
plot(x,f,'g','Linewidth',2)
[f,x] = ecdf([mode1_ul.ulsch_throughput_ideal_2Rx; mode2_ul.ulsch_throughput_ideal_2Rx; mode6_ul.ulsch_throughput_ideal_2Rx]*10/3);
plot(x,f,'r','Linewidth',2)
legend('modem','ideal 1 rx antenna','ideal 2 rx antennas','Location','SouthEast');
title('UL Throughput CDF for FDD')
xlabel('UL Throughput [bps]')
ylabel('P(x<abscissa)')
grid on
saveas(h_fig,fullfile(pathname,'results','UL_throughput_cdf_comparison_fdd.eps'),'epsc2')

%% UL Unoded throughput CDF comparison
h_fig = h_fig+1;
figure(h_fig);
hold off
[f,x] = ecdf(coded2uncoded([mode1_ul.ulsch_throughput mode2_ul.ulsch_throughput mode6_ul.ulsch_throughput],'UL'));
plot(x,f,'b','Linewidth',2)
hold on
[f,x] = ecdf(coded2uncoded([mode1_ul.ulsch_throughput_ideal_1Rx; mode2_ul.ulsch_throughput_ideal_1Rx; mode6_ul.ulsch_throughput_ideal_1Rx],'UL'));
plot(x,f,'g','Linewidth',2)
[f,x] = ecdf(coded2uncoded([mode1_ul.ulsch_throughput_ideal_2Rx; mode2_ul.ulsch_throughput_ideal_2Rx; mode6_ul.ulsch_throughput_ideal_2Rx],'UL'));
plot(x,f,'r','Linewidth',2)
xlim([0 4.86e6])
legend('modem','ideal 1 rx antenna','ideal 2 rx antennas','Location','SouthEast');
title('UL Uncoded throughput CDF')
xlabel('UL Throughput [bps]')
ylabel('P(x<abscissa)')
grid on
saveas(h_fig,fullfile(pathname,'results','UL_uncoded_throughput_cdf_comparison.eps'),'epsc2')

%% PBCH
h_fig = h_fig + 1;
figure(h_fig)
if strcmp(mm,'cordes')
    mode2_update.pbch_fer_cat(~mode2_update.UE_synched) = 100;
    mode2_update.pbch_good = (mode2_update.pbch_fer_cat<=100 & mode2_update.pbch_fer_cat>=0).';
    [f,x] = ecdf(double(mode2_update.pbch_fer_cat(mode2_update.pbch_good)));
    n_points = sum(mode2_update.pbch_good);
else
    mode2.pbch_fer_cat(~mode2.UE_synched) = 100;
    mode2.pbch_good = (mode2.pbch_fer_cat<=100 & mode2.pbch_fer_cat>=0).';
    [f,x] = ecdf(double(mode2.pbch_fer_cat(mode2.pbch_good & parcours1)));
    n_points = sum(mode2.pbch_good & parcours1);
end
plot(x,f,'r','Linewidth',2)
hold on
mode1.pbch_fer_cat(~mode1.UE_synched) = 100;
mode1.pbch_good = (mode1.pbch_fer_cat<=100 & mode1.pbch_fer_cat>=0).';
[f,x] = ecdf(double(mode1.pbch_fer_cat(mode1.pbch_good)));
plot(x,f,'b','Linewidth',2)
legend(sprintf('Mode 2/6 (%d pts)',n_points),...
    sprintf('Mode 1 (%d pts)',sum(mode1.pbch_good)),'Location','SouthEast');
title('PBCH FER CDF')
xlabel('PBCH FER [%]')
ylabel('P(x<abscissa)')
grid on
saveas(h_fig,fullfile(pathname,'results','PBCH_FER_cdf_comparison.eps'),'epsc2')

%% fit path loss model for all measurements
dist = [mode1.dist mode2.dist mode6.dist];
dist_ok = ((dist>0.5) & (dist<100)).';
rx_rssi_dBm_cat = [mode1.rx_rssi_dBm_cat; mode2.rx_rssi_dBm_cat; mode6.rx_rssi_dBm_cat];
good = (rx_rssi_dBm_cat<40 & rx_rssi_dBm_cat>-120);

%%
h_fig = h_fig+1;
figure(h_fig);
hold off
semilogx(dist(dist_ok & good(:,1)), double(rx_rssi_dBm_cat(dist_ok & good(:,1),1)), 'rx')
hold on
max_dist = ceil(max(dist(dist_ok & good(:,1))));
PL = double(rx_rssi_dBm_cat(dist_ok & good(:,1),1)) - 43;
d = logspace(-1,log10(max_dist),100);
res = [ones(length(PL),1) log10(dist(dist_ok & good(:,1)).')]\PL;
semilogx(d,res(1)+res(2)*log10(d)+43,'b')
semilogx(d,43-cost231_hata(d),'k')
legend('measured',sprintf('fitted (PL_0=%4.2fdB, R=%4.2f)',res(1)+43,res(2)/10),'COST231-Hata','Location','Best');
title('RX RSSI distance from BS [dBm]')
xlabel('Distance from BS [Km]')
ylabel('RX RSSI [dBm]')
xlim([0.1 max_dist])
ylim([-110 -30]);
saveas(h_fig, fullfile(pathname,'results','RX_RSSI_dBm_dist_with_PL.eps'),'epsc2');



%%
h_fig = h_fig+1;
figure(h_fig);
hold off
plot_in_bins(dist(good(:,1)),double(rx_rssi_dBm_cat(good(:,1),1)),0:max_dist);
xlabel('Distance [km]')
ylabel('RX RSSI [dBm]')
ylim([-110 -30]);
saveas(h_fig,fullfile(pathname,'results','RX_RSSI_dBm_dist_bars.eps'),'epsc2');


%% distance travelled comparison
% to select regions use the gps_link_brush.m tool
close all

switch mm
    case 'cordes'
        % plot_distance_travelled_cordes
    case 'penne'
        for file_id = 1:3
            load(sprintf('penne_zoom%d.mat',file_id))
            select2b = false(size(parcours1));
            select2b(parcours1)=select2;
            index1 = find(select1);
            index2 = find(select2);
            index6 = find(select6);
            index2b = find(select2b);
            plot_distance_travelled(mode1,index1,mode2,mode2_ideal,index2b,mode6,index6,pathname,file_id,mm);
        end

    case 'ambialet'
        for file_id = 1:3
            load(sprintf('ambialet_zoom%d.mat',file_id))
            select2b = false(size(parcours1));
            select2b(parcours1)=select2;
            index1 = find(select1);
            index2 = find(select2);
            index6 = find(select6);
            index2b = find(select2b);
            plot_distance_travelled(mode1,index1,mode2,mode2_ideal,index2b,mode6,index6,pathname,file_id,mm);
        end
        
        eval_village_ambialet
        
end
