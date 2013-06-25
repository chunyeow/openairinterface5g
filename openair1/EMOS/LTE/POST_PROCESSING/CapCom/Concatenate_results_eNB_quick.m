d = dir(fullfile(pathname, 'data_term1*.mat'));
filenames = {d.name};

gps_data_cat = [];

%minestimates_cat = [];

%K_fac_cat = [];

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
    
    %K_fac_cat = cat(2,K_fac_cat,K_fac);
    
    save(fullfile(pathname,'results_cat_eNB.mat'),'*_cat');

end

