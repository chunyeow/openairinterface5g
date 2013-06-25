clc;
pathname = '/media/Expansion_Drive/New Folder/20100512_mode2_parcours2_part7/';
%pathname = '/me/media/Expansion_Drive/New Folder/20100512_mode2_parcours2_part7/dia/Iomega_HDD/EMOS/data/20100506 coverage run part 1/';
%pathname = '/extras/kaltenbe/EMOS/lte_cnes_data/20100511_mode2_parcours2_part1/';
       
d = dir([pathname 'data_term3*.EMOS']);
filenames = {d.name};
%filename = '/extras/kaltenbe/EMOS/lte_cnes_data/20100511_mode2_parcours2_part2/data_term3_idx00_20100512T104109.EMOS';
decimation = 1;
NFrames_max = 100*60*10;

for file_idx = 1:length(filenames)
   % n = round(length(filenames)/2);
    disp(filenames{file_idx});

    [path,file,ext,ver] = fileparts(filenames{file_idx});

    if file(10)=='1'
        is_eNb=1;
    else
        is_eNb=0;
    end
    
%[H, H_fq, gps_data, NFrames, minestimates, throughput, SNR] = load_estimates_lte_1(filename,NFrames_max,decimation,is_eNb);

[H, H_fq, gps_data, NFrames, minestimates, throughput, SNR] = load_estimates_lte_1(fullfile(pathname,filenames{file_idx}),NFrames_max,decimation,is_eNb);

save(regexprep((fullfile(filenames{file_idx},'results_UE.mat')), '/', '_'), 'gps_data', 'NFrames', 'SNR', 'throughput', 'minestimates', 'file_idx');

end
    

