clear all
close all

%%
% pathname = '/emos/EMOS/';
% mm = 'cordes';
pathname = '/emos/AMBIALET/';
mm = 'ambialet';
mode2_ideal = load(fullfile(pathname,'/Mode2/results/results_cat_UE.mat'));
mode2 = load(fullfile(pathname,'/Mode2/results/results_UE.mat'));

%%
figure(1)
hold off
time = [mode2_ideal.gps_data_cat.timestamp];
select = zeros(size(time));
plot(time,select,'x')
xlabel('Time')
ylabel('Select')
linkdata on

%%
figure(2)
hold off
lat = [mode2_ideal.gps_data_cat.latitude];
lon = [mode2_ideal.gps_data_cat.longitude];
[x,y] = plot_gps_coordinates(mm,lon,lat,[],[],'','blue','Marker','x','Line','none');
linkdata on
set(findobj('XDataSource','x'),'ZDataSource','select');


