%% Concatenating Script
lat_cat= [];
lon_cat=[];
K_fac_cat=[];

root_path = '/media/Expansion_Drive/Mode2/';

d = dir(fullfile(root_path, '*mode2_parcours*'));
dir_names = {d.name};

% post processing for vehicular measurements
for i=1:length(dir_names)
  
    pathname = fullfile(root_path,dir_names{i});
%pathname = '/homes/latif/Mode2_UE_matFiles/';

d = dir(fullfile(pathname, '*_K_factor.mat'));
filenames = {d.name};

for f = 1:length(filenames)
    s = fullfile(pathname, filenames{f});
    load(s);
    
    lat_cat = [lat_cat [gps_data.latitude]];
    lon_cat = [lon_cat [gps_data.longitude]];
    
    K_fac_cat=[K_fac_cat K_fac];

    
end

end
save(fullfile(root_path, 'mode2_K_factor.mat'), 'lat_cat', 'lon_cat','K_fac_cat');

mm='cordes';

%% Plotting Script
in = 0;
in = in + 1;
h_fig = figure(in);
hold off
plot_gps_coordinates(mm,lon_cat,lat_cat,10*log10(K_fac_cat(1:4:end)));
title('Ricean Factor for First Antenna Pair(H11)')
saveas(h_fig,fullfile(root_path,'K_factor_h11.jpg'),'jpg')

in = in + 1;
h_fig = figure(in);
hold off
plot_gps_coordinates(mm,lon_cat,lat_cat,10*log10(K_fac_cat(2:4:end)));
title('Ricean Factor for Second Antenna Pair(H12)')
saveas(h_fig,fullfile(pathname,'K_factor_h12.jpg'),'jpg')

in = in + 1;
h_fig = figure(in);
hold off
plot_gps_coordinates(mm,lon_cat,lat_cat,10*log10(K_fac_cat(3:4:end)));
title('Ricean Factor for third Antenna Pair(H21)')
saveas(h_fig,fullfile(pathname,'K_factor_h21.jpg'),'jpg')

in = in + 1;
h_fig = figure(in);
hold off
plot_gps_coordinates(mm,lon_cat,lat_cat,10*log10(K_fac_cat(4:4:end)));
title('Ricean Factor for Fourth Antenna Pair(H22)')
saveas(h_fig,fullfile(pathname,'K_factor_h22.jpg'),'jpg')

