%clear all
%close all
global mumimo_count beamforming_count
pathname     = '/extras/kaltenbe/CNES/ressults_MUMIMO/';
UE1_pathname = '/media/data/AMBIALET/Mode2/20100720_MODE2_S12345678/';
UE2_pathname = '/media/data/AMBIALET/Mode2/20100721_MODE2_S8910111213141516/';

load select_mu-mimo_ambialet.mat

mumimo_on=1;
decimation = 1;
NFrames_max = 100*60*10;


for file_idx = 1:min(length(UE1_files),length(UE2_files))
      mumimo_count = 0;
      beamforming_count = 0;
    disp(UE1_files{file_idx});
    disp(UE2_files{file_idx});

    is_eNb=0;
    
    %try
    [H, H_fq, gps_data, NFrames, minestimates, throughput, SNR] = ...
        load_estimates_lte_2({fullfile(UE1_pathname,UE1_files{file_idx}), fullfile(UE2_pathname,UE2_files{file_idx})}, ...
        NFrames_max,decimation,is_eNb,mumimo_on);
   % catch exception
   % disp(exception.getReport)
   % fprintf('Detected error in reading file %d, Skipping it',file_idx);
   %     continue
   % end
    
if (mumimo_on)
       save(fullfile(pathname,sprintf('results_MU-MIMO_idx%d.mat',file_idx)),...
        'gps_data', 'NFrames', 'SNR', 'throughput', 'minestimates', 'file_idx', 'mumimo_count', 'beamforming_count');
else
        save(fullfile(pathname,sprintf('results_beamforming_idx%d.mat',file_idx)),...
        'gps_data', 'NFrames', 'SNR', 'throughput', 'minestimates', 'file_idx');
end

end


