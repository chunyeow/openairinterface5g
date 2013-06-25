if ~exist(pathname,'dir')
    error('Path does not exist!')
end

d = dir(fullfile(pathname, 'data_term3*.EMOS'));
filenames = {d.name};
filedates = {d.date};
[filedates,idx] = sort(filedates);
filenames = filenames(idx);

% NFrames = floor([d.bytes]/CHANNEL_BUFFER_SIZE)*NO_ESTIMATES_DISK;
% for i=1:length(filenames)
%     file_idx(i) = sscanf(d(i).name,'data_term3_idx%d_%s.EMOS',1);
% end

NFrames_max = 100*60*10;

if (exist(fullfile(pathname,'results_UE.mat'),'file'))
    load(fullfile(pathname,'results_UE.mat'));
    start_idx = file_idx + 1
else
    timestamp_cat = [];
    phy_measurements_cat = struct([]);
    rx_rssi_dBm_cat = [];
    gps_lon_cat = [];
    gps_lat_cat = [];
    gps_time_cat = [];
    gps_speed_cat = [];
    frame_tx_cat = [];  
    pbch_fer_cat = [];
    dlsch_fer_cat = [];
    tbs_cat = [];
    mcs_cat = [];
    UE_mode_cat = [];
    NFrames = zeros(1,length(filenames));
    start_time = zeros(1,length(filenames));
    start_idx = 1;
end

for file_idx = start_idx:length(filenames)
    disp(filenames{file_idx});
    
    [path,file,ext,ver] = fileparts(filenames{file_idx});

    if file(10)=='1'
        is_eNb=1;
    else
        is_eNb=0;
    end

    [H, H_fq, estimates, gps_data, NFrames(file_idx)] = load_estimates_lte(fullfile(pathname,filenames{file_idx}),NFrames_max,decimation,is_eNb);
    start_time(file_idx) = datenum(file((end-14):end),'yyyymmddTHHMMSS');

%%
    phy_measurements = repmat(phy_measurements_struct,1,NFrames(file_idx)/decimation);
    for i=1:NFrames(file_idx)/decimation
        phy_measurements(i) = estimates(i).phy_measurements(1);
    end
    
    rx_rssi_dBm = zeros(NFrames(file_idx)/decimation,3);
    pbch_fer = zeros(NFrames(file_idx)/decimation,1);
    dlsch_fer = zeros(NFrames(file_idx)/decimation,1);
    mcs = zeros(NFrames(file_idx)/decimation,1);
    tbs = zeros(NFrames(file_idx)/decimation,1);
    for i=1:NFrames(file_idx)/decimation
        rx_rssi_dBm(i,:) = estimates(i).phy_measurements(1).rx_rssi_dBm(:);
        pbch_fer(i) = estimates(i).pbch_fer(1);
        dlsch_fer(i) = estimates(i).dlsch_fer(1);
        mcs(i) = get_mcs(estimates(i).dci_alloc(7,1).dci_pdu);
        tbs(i) = get_tbs(mcs(i),25);
    end
    
    phy_measurements_cat = [phy_measurements_cat phy_measurements];
    timestamp_cat = [timestamp_cat [estimates.timestamp]];
    frame_tx_cat = [frame_tx_cat [estimates.frame_tx]];
    rx_rssi_dBm_cat = [rx_rssi_dBm_cat; rx_rssi_dBm];
    pbch_fer_cat = [pbch_fer_cat; pbch_fer];
    dlsch_fer_cat = [dlsch_fer_cat; dlsch_fer];
    mcs_cat = [mcs_cat; mcs];
    tbs_cat = [tbs_cat; tbs];
    UE_mode_cat = [UE_mode_cat [estimates.UE_mode]];
    gps_lon_cat = [gps_lon_cat [gps_data.longitude]];
    gps_lat_cat = [gps_lat_cat [gps_data.latitude]];
    gps_time_cat = [gps_time_cat [gps_data.timestamp]];
    gps_speed_cat = [gps_speed_cat [gps_data.speed]];
    
    save(fullfile(pathname,'results_UE.mat'),'timestamp_cat','frame_tx_cat',...
        'rx_rssi_dBm_cat','pbch_fer_cat','dlsch_fer_cat','mcs_cat','tbs_cat',...
        'UE_mode_cat','phy_measurements_cat','gps_lon_cat','gps_lat_cat','gps_time_cat','gps_speed_cat',...
        'file_idx','NFrames','start_time','filenames','filedates');

%     h_fig = figure(2);
%     hold off
%     plot(frame_tx_cat,rx_rssi_dBm_cat)
%     title('RX RSSI [dBm]')
%     xlabel('Frame number')
%     ylabel('RX RSSI [dBm]')
%     saveas(h_fig,fullfile(pathname,'RX_RSSI_dBm.eps'),'epsc2')
% 
%     h_fig = figure(3);
%     hold off
%     plot_gps_coordinates([],gps_lon_cat, gps_lat_cat,rx_rssi_dBm_cat(:,1));
%     title('RX I0 [dBm]')
%     xlabel('Frame number')
%     ylabel('RX I0 [dBm]')
%     saveas(h_fig,fullfile(pathname,'RX_RSSI_dBm_gps.eps'),'epsc2')
end    

