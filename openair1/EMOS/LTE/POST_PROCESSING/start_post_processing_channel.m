close all
clear all

pathname = '/emos/AMBIALET/Mode2/';
dd = dir(fullfile(pathname,'*extended*'));

% pathname = '/emos/EMOS/';
% dd(1).name = '/Mode6/20100610_VTP_MODE6_ZONES_PUSCH_UPDATE.2/';
% dd(1).isdir = 1;
% % dd(1).name = 'Mode6/20100526_mode1_parcours1_part4_part5';
% % dd(1).isdir = 1;
% % dd(2).name = 'Mode2/20100520_mode2_parcours1_part4_part5';
% % dd(2).isdir = 1;


for dir_idx=1:1:length(dd)
    % if dd(dir_idx).isdir == 1
    
    % if length(dd(dir_idx).name) > 10
    
    fpath = fullfile(pathname,dd(dir_idx).name);
    
    d = dir(fullfile(fpath, 'data_term3*.EMOS'));
    filenames = {d.name};
    
    decimation = 1;
    NFrames_max = 100*60*10;
    
    for file_idx = 1:length(filenames)
        
        disp(filenames{file_idx});
        [path,file,ext,ver] = fileparts(filenames{file_idx});
        
        time_idx = regexp(file,'\d{8}T\d{6}');
        start_time = datenum(file(time_idx:end),'yyyymmddTHHMMSS');
        
        if (start_time >= datenum('20100610T000000','yyyymmddTHHMMSS'));
            version = 1;
        else
            version = 0;
        end
        
        if file(10)=='1'
            is_eNb=1;
            mat_file = [file '_results_eNB.mat'];
        else
            is_eNb=0;
            mat_file = [file '_results_UE.mat'];
        end
        
        if (exist(fullfile(fpath,mat_file),'file'))
            continue
        end
        
        [H, H_fq, gps_data, NFrames, minestimates, throughput, SNR, K_fac] = load_estimates_lte_1(fullfile(fpath,filenames{file_idx}),NFrames_max,decimation,is_eNb,version);
            
        if is_eNb
            save(regexprep((fullfile(fpath,filenames{file_idx},'results_eNB.mat')), '.EMOS/results_eNB.mat', '_results_eNB.mat'), 'gps_data', 'NFrames', 'SNR', 'throughput', 'minestimates', 'file_idx');
        else
            save(regexprep((fullfile(fpath,filenames{file_idx},'results_UE.mat')), '.EMOS/results_UE.mat', '_results_UE.mat'), 'gps_data', 'NFrames', 'SNR', 'throughput', 'minestimates', 'file_idx', 'K_fac');
            %save(regexprep((fullfile(fpath,filenames{file_idx},'K_factor.mat')), '.EMOS/K_factor.mat', '_K_factor.mat'), 'gps_data', 'K_fac');
        end
            
    end
    
    % We need to call the concatenate script here
    Concatenate_results_UE_quick
    Concatenate_results_eNB_quick
        
end

