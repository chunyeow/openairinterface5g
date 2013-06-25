%pathname = '/extras/kaltenbe/CNES/emos_postprocessed_data/20100510_mode2_parcours1_part1/';

d = dir(fullfile(pathname, 'data_term3*.mat'));
d1 = dir(fullfile(pathname, 'data_term3*.EMOS'));
filenames = {d.name};
filedates = {d1.date};
[filedates,idx] = sort(filedates);
filenames = filenames(idx);

gps_data_cat = [];

%minestimates_cat = [];
UE_mode_cat = [];

K_fac_cat = [];

%%
for f = 1:length(filenames)
    s = fullfile(pathname, filenames{f});
    load(s);
    
    nn = fieldnames(throughput);
    if (f==1)
        for n = 1:length(nn)
            eval([nn{n} '_cat = [];']);
        end
    end   
    for n = 1:length(nn)
        eval([nn{n} '_cat = [' nn{n} '_cat throughput.' nn{n} '];']);
    end

    nn = fieldnames(SNR);
    if (f==1)
        for n = 1:length(nn)
            eval([nn{n} '_cat = [];']);
        end
    end   
    for n = 1:length(nn)
        eval([nn{n} '_cat = [' nn{n} '_cat SNR.' nn{n} '];']);
    end

    gps_data_cat = cat(2,gps_data_cat,gps_data);
    
    %minestimates_cat = cat(2,minestimates_cat,minestimates);  
    
    UE_mode_cat = cat(2,UE_mode_cat,[minestimates.UE_mode]);
    
    K_fac_cat = cat(2,K_fac_cat,K_fac);
    
    save(fullfile(pathname,'results_cat_UE.mat'),'*_cat');
    
end

 
