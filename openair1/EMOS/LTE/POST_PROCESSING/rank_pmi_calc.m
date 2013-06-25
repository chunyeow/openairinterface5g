
pathname = '/homes/latif/Mode2_UE_matFiles/';
d = dir(fullfile(pathname, '*_results_UE.mat'));
filenames = {d.name};

for f = 1:length(filenames)

    try
    s = fullfile(pathname, filenames{f});
    
    if (exist(regexprep(s, '_results_UE.mat', '_results_PMI.mat')))
        continue;
    end
    
    load(s);

    pmi1 = [];
    pmi2 = [];
    lat = [];
    lon = [];
    count = 0;
    rank_Ind=[];

    for b = 1:100:length(minestimates)
    
        count = count+1;
        
        q1 = double(minestimates(b).phy_measurements.subband_pmi_re(1,:,1))+1j*double(minestimates(b).phy_measurements.subband_pmi_im(1,:,1));
        q2 = double(minestimates(b).phy_measurements.subband_pmi_re(2,:,1))+1j*double(minestimates(b).phy_measurements.subband_pmi_im(2,:,1));
        
        for i = 1:1:7
            
            qq1(i) = quantize_q(q1(i));
            qq1(i) = map_q(qq1(i));
            qq2(i) = quantize_q(q2(i));
            qq2(i) = map_q(qq2(i));
            
            if ((qq1(i)==0 && qq2(i)==3) || (qq1(i)==3 && qq2(i)==0) || (qq1(i)==2 && qq2(i)==1) || (qq1(i)==1 && qq2(i)==2))
                ran(i) = 1;
            else
                ran(i) = 0;
            end
            
            
        end
        
       if sum(ran==1)>=4
            
            rank_Ind = [rank_Ind 1];
        else
            rank_Ind = [rank_Ind 0];
        end
        
        pmi1 = [pmi1 qq1];
        pmi2 = [pmi2 qq2];
  
    
    end
    lat = [gps_data.latitude];
    lon = [gps_data.longitude];
    pmi1 = reshape(pmi1,7,[]);
    pmi2 = reshape(pmi2,7,[]);
    UE_synched = ([minestimates(1:100:end).UE_mode] > 0);

    save(regexprep(s, '_results_UE.mat', '_results_PMI.mat'), 'lat', 'lon', 'pmi1', 'pmi2', 'UE_synched', 'rank_Ind');
    catch me
        continue;
    end
    
    %save(fullfile(s, 'results_UE.mat'), '*_cat', 'pmi1','pmi2','rank_Ind');
end


%%
%CONCATENATION SCRIPT

lat_cat= [];
lon_cat=[];
pmi1_cat=[];
pmi2_cat=[];
UE_synched_cat=[];
rank_Ind_cat=[];

pathname = '/homes/latif/Mode2_UE_matFiles/';
d = dir(fullfile(pathname, '*_results_PMI.mat'));
filenames = {d.name};

for f = 1:length(filenames)
    s = fullfile(pathname, filenames{f});
    load(s);
    
    lat_cat = [lat_cat lat];
    lon_cat = [lon_cat lon];
    pmi1_cat=[pmi1_cat pmi1];

    pmi2_cat=[pmi2_cat pmi2];
    UE_synched_cat=[UE_synched_cat UE_synched];
    rank_Ind_cat=[rank_Ind_cat rank_Ind];

    
end

save(fullfile(pathname, 'Mode2_PMI_Cat.mat'), 'lat_cat', 'rank_Ind_cat', 'lon_cat','pmi1_cat', 'pmi2_cat','UE_synched_cat');
%%
%PLOTTING SCRIPT

mm='cordes';
in = 0;
in = in + 1;
h_fig = figure(in);
hold off
plot_gps_coordinates(mm,lon_cat(UE_synched_cat),lat_cat(UE_synched_cat),pmi1_cat(1,UE_synched_cat));
title('PMI')
saveas(h_fig,fullfile(pathname,'PMI_1stRX_subband1.jpg'),'jpg')

in = in + 1;
h_fig = figure(in);
hold off
plot_gps_coordinates(mm, lon_cat(UE_synched_cat),lat_cat(UE_synched_cat),pmi1_cat(2,UE_synched_cat));
title('PMI')
saveas(h_fig,fullfile(pathname,'PMI_1stRX_subband2.jpg'),'jpg')

in = in + 1;
h_fig = figure(in);
hold off
plot_gps_coordinates(mm, lon_cat(UE_synched_cat),lat_cat(UE_synched_cat),pmi2_cat(1,UE_synched_cat));
title('PMI')
saveas(h_fig,fullfile(pathname,'PMI_2ndRX_subband1.jpg'),'jpg')

in = in + 1;
h_fig = figure(in);
hold off
plot_gps_coordinates(mm, lon_cat(UE_synched_cat),lat_cat(UE_synched_cat),pmi2_cat(2,UE_synched_cat));
title('PMI')
saveas(h_fig,fullfile(pathname,'PMI_2ndRX_subband2.jpg'),'jpg')
