if ~exist(pathname,'dir')
    error('Path does not exist!')
end

d = dir(fullfile(pathname, 'data_term3*.EMOS'));
filenames = {d.name};
filedates = {d.date};
tmp = char(filenames.');
tmp(:,1:17) = [];
tmp2 = cellstr(tmp).';
[tmp3,idx] = sort(tmp2);
filenames = filenames(idx);

% NFrames = floor([d.bytes]/CHANNEL_BUFFER_SIZE)*NO_ESTIMATES_DISK;
% for i=1:length(filenames)
%     file_idx(i) = sscanf(d(i).name,'data_term3_idx%d_%s.EMOS',1);
% end

NFrames_max = 100*60*10;

if (exist(fullfile(pathname,'results_UE_new.mat'),'file'))
    load(fullfile(pathname,'results_UE_new.mat'));
    start_idx = file_idx + 1
else
    start_idx = 1;
    minestimates=[];
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
    time_idx = regexp(file,'\d{8}T\d{6}');
    start_time(file_idx) = datenum(file(time_idx:end),'yyyymmddTHHMMSS');

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
    
    minestimates(file_idx).filename = filenames{file_idx};    
    minestimates(file_idx).filedate = filedates{file_idx};   
    minestimates(file_idx).start_time =  start_time(file_idx);
    minestimates(file_idx).NFrames =  NFrames(file_idx);
    minestimates(file_idx).phy_measurements = phy_measurements;
    minestimates(file_idx).timestamp = [estimates.timestamp];
    minestimates(file_idx).frame_tx = [estimates.frame_tx];
    minestimates(file_idx).mimo_mode= [estimates.mimo_mode];
    minestimates(file_idx).rx_rssi_dBm = rx_rssi_dBm;
    minestimates(file_idx).pbch_fer = pbch_fer;
    minestimates(file_idx).dlsch_fer = dlsch_fer;
    minestimates(file_idx).mcs_cat = mcs;
    minestimates(file_idx).tbs_cat = tbs;
    minestimates(file_idx).UE_mode_cat = [estimates.UE_mode];
    minestimates(file_idx).gps_data = gps_data;
    
    
    save(fullfile(pathname,'results_UE_new.mat'),'minestimates','file_idx');

end    

