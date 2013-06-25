%% This file concatenates all the matlab results files from different folders and then saves the resulting file in root_path folder. 
clear all;
close all;

start_idx = 1;
root_path = '/emos/AMBIALET/Mode2/';

d = dir(fullfile(root_path, '*MODE2*'));
dir_names = {d.name};

%%
for d_idx=1:length(dir_names)
    
    pathname = fullfile(root_path,dir_names{d_idx})
    try
        temp = load(fullfile(pathname, 'results_cat_UE.mat'));
    catch exception
        disp(exception.getReport)
        disp(sprintf('Detected error in folder %s, skipping it',pathname));
        continue
    end
%     vars = whos('*_cat');
%     nn = {vars.name};
   
    nn = fieldnames(temp);
    if (start_idx==1)
        for n = 1:length(nn)
            eval([nn{n} ' = [];']);
        end
    end
    for n = 1:length(nn)
        eval([nn{n} ' = [' nn{n} ' ' 'temp.' nn{n} '];']);
    end
    
    save(fullfile(root_path,'results_cat_UE.mat'), '*_cat');
    start_idx = start_idx + 1;
    
end

