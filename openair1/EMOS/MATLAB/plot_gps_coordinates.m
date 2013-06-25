function h = plot_gps_coordinates(longitude, latitude)
% h = plot_gps_coordinates(longitude, latitude)
%
%  This function plots the gps coordinates given by langitude and latutude
%  onto the map of garbejaire. If langitude and latutude are matrices, each
%  row is treated as a seperate user and plotted in a different color.
%  This function uses the Plate carrée projection
%  (http://en.wikipedia.org/wiki/Plate_carr%C3%A9e_projection). We assume
%  the coordinates of 3 reference points are known in lat/lon as well as
%  x/y coordinates.
%  The function returns the handle of the figure

h = figure;
mm=imread('map_garbejaire.jpg');
image(mm);  % plots the image itself
hold on

% GOOGLE MAPS
% reference points in image coordinates 
x = [ 132.5634 581.7200 300.6440 ];
y = [ 133.1389  77.5833 337.3056 ];
% reference points in lat/lon
lat = [43.6236 43.6239 43.6222];
lon = [7.04554 7.04963 7.04707];

% TOMTOM
% reference points in image coordinates (these are taken from tomtom measurements)
x = [ 252.3120 135.5207 555.7223 ];
y = [ 262.5364 159.9868 216.6093 ];
% reference points in lat/lon
lat = [43.62269 43.62339 43.62293];
lon = [7.04670 7.04552 7.04948];

%plot(x,y,'o')


% calculate the scale factor of the projection by averaging 
% scale(1) = sqrt((x(2)-x(1))^2 + (y(2)-y(1))^2)/sqrt((lon(2)-lon(1))^2 + (lat(2)-lat(1))^2);
% scale(2) = sqrt((x(3)-x(1))^2 + (y(3)-y(1))^2)/sqrt((lon(3)-lon(1))^2 + (lat(3)-lat(1))^2);
% scale(3) = sqrt((x(3)-x(2))^2 + (y(3)-y(2))^2)/sqrt((lon(3)-lon(2))^2 + (lat(3)-lat(2))^2);
scalex(1) = (x(2) - x(1))/(lon(2)-lon(1));
scaley(1) = (y(2) - y(1))/(lat(2)-lat(1));
scalex(2) = (x(3) - x(1))/(lon(3)-lon(1));
scaley(2) = (y(3) - y(1))/(lat(3)-lat(1));
scalex(3) = (x(3) - x(2))/(lon(3)-lon(2));
scaley(3) = (y(3) - y(2))/(lat(3)-lat(2));

gps_x = zeros([size(latitude) 2]);
gps_y = zeros([size(latitude) 2]);
gps_y(:,:,1)=(latitude-lat(1))*mean(scaley) + y(1);     % gps_x is the north-south direction
gps_x(:,:,1)=(longitude-lon(1))*mean(scalex) + x(1);     % gps_y is the east-west direction
gps_y(:,:,2)=(latitude-lat(2))*mean(scaley) + y(2);     % gps_x is the north-south direction
gps_x(:,:,2)=(longitude-lon(2))*mean(scalex) + x(2);     % gps_y is the east-west direction
% gps_y(:,:,3)=(latitude-lat(3))*mean(scaley) + y(3);     % gps_x is the north-south direction
% gps_x(:,:,3)=(longitude-lon(3))*mean(scalex) + x(3);     % gps_y is the east-west direction

% plot_style = {'rx','bx','gx','yx'};
% for i=1:size(longitude,2)
%     plot(gps_x(:,i),gps_y(:,i),plot_style{i})
% end

plot(mean(gps_x,3).',mean(gps_y,3).','x')