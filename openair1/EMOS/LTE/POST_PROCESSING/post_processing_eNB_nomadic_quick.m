if ~exist(pathname,'dir')
    error('Path does not exist!')
end

d = dir(fullfile(pathname, 'data_term1*.EMOS'));
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

if (exist(fullfile(pathname,'results_eNB_new.mat'),'file'))
    load(fullfile(pathname,'results_eNB_new.mat'));
    start_idx = file_idx + 1
else
    start_idx = 1;
    minestimates=[];
end


for file_idx = start_idx:length(filenames)
    disp(filenames{file_idx});
    
    [path,file,ext,ver] = fileparts(filenames{file_idx});
    time_idx = regexp(file,'\d{8}T\d{6}');
    start_time(file_idx) = datenum(file(time_idx:end),'yyyymmddTHHMMSS');

    if file(10)=='1'
        is_eNb=1;
    else
        is_eNb=0;
    end
    
    if (start_time(file_idx) >= datenum('20100610T000000','yyyymmddTHHMMSS'));
        version = 1;
    else 
        version = 0;
    end
    

    [H, H_fq, estimates, gps_data, NFrames(file_idx)] = load_estimates_lte(fullfile(pathname,filenames{file_idx}),NFrames_max,decimation,is_eNb,version);

%%
    phy_measurements = repmat(phy_measurements_eNb_struct,NFrames(file_idx)/decimation,3);
    eNb_UE_stats = repmat(eNb_UE_stats_struct,1,NFrames(file_idx)/decimation);
    mcs = zeros(1,NFrames(file_idx)/decimation);
    tbs = zeros(1,NFrames(file_idx)/decimation);
    %rx_N0_dBm = zeros(NFrames(file_idx)/decimation,3);
    %rx_N0_subband_dBm = zeros(NFrames(file_idx)/decimation,3,25);
    for i=1:NFrames(file_idx)/decimation
        phy_measurements(i,:) = estimates(i).phy_measurements_eNb;
        if (version<1)
            eNb_UE_stats(i) = estimates(i).eNb_UE_stats(3,1);
        else
            eNb_UE_stats(i) = estimates(i).eNb_UE_stats(1,1);
        end
        mcs(i) = get_mcs(estimates(i).dci_alloc(10,1).dci_pdu,'format0');
        tbs(i) = get_tbs(mcs(i),25);
        %for j=1:3
           %rx_N0_dBm(i,j) = estimates(i).phy_measurements_eNb(j).n0_power_tot_dBm;
           %rx_N0_subband_dBm(i,j,:) = estimates(i).phy_measurements_eNb(j).n0_subband_power_tot_dBm;
        %end
    end
    
   
    minestimates(file_idx).filename = filenames{file_idx};    
    minestimates(file_idx).filedate = filedates{file_idx};   
    minestimates(file_idx).start_time =  start_time(file_idx);
    minestimates(file_idx).NFrames =  NFrames(file_idx);
    minestimates(file_idx).phy_measurements = phy_measurements;
    minestimates(file_idx).eNb_UE_stats = eNb_UE_stats;
    minestimates(file_idx).timestamp = [estimates.timestamp];
    minestimates(file_idx).frame_tx = [estimates.frame_tx];
    minestimates(file_idx).mimo_mode= [estimates.mimo_mode];
    minestimates(file_idx).ulsch_errors = [estimates.ulsch_errors];
    minestimates(file_idx).mcs_cat = mcs;
    minestimates(file_idx).tbs_cat = tbs;
    minestimates(file_idx).gps_data = gps_data;
    
    
    save(fullfile(pathname,'results_eNB_new.mat'),'minestimates','file_idx');

end    

