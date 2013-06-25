function [gps_x, gps_y] = plot_gps_coordinates_sophia(mm, longitude, latitude, rx_rssi, label, color)
% h = plot_gps_coordinates(longitude, latitude)
%
%  This function plots the gps coordinates given by langitude and latutude
%  onto the map of garbejaire. If langitude and latutude are matrices, each
%  row is treated as a seperate user and plotted in a different color.
%  This function uses the Plate carr�e projection
%  (http://en.wikipedia.org/wiki/Plate_carr%C3%A9e_projection). We assume
%  the coordinates of 3 reference points are known in lat/lon as well as
%  x/y coordinates. This projection method is only fairly accurate. Also
%  there will be inaccuracies in the reference points. It is therefore
%  necessary the check the result and include a correction factor if
%  neccesary.
%  The function returns the handle of the figure

if (nargin == 4 && ~isempty(rx_rssi))
    m = colormap;
    cmin = min(rx_rssi);
    cmax = max(rx_rssi);
    s = (cmax-cmin)/(length(m)-1);
    cidx = ceil((rx_rssi-cmin)/s+1);
end

if nargin <= 5
    color = 'blue';
end

% % GOOGLE MAPS
% % reference points in image coordinates 
% x = [319.8624 417.6346 682.9017];
% y = [457.7185 306.1954 439.7053];
% % reference points in lat/lon
% lat = [43.6166 43.6263 43.6172]; 
% lon = [7.03778 7.04703 7.07062];

% % GOOGLE MAPS with BS
% % reference points in image coordinates 
% x = [319.8624 391.9894 682.9017];
% y = [457.7185 37.0563 439.7053];
% % reference points in lat/lon
% lat = [43.6166 43.6436 43.6172]; 
% lon = [7.03778 7.04523 7.07062];

% TOMTOM
% reference points in image coordinates (these are taken from tomtom measurements)
x = [ 4055  ...
    %3939 4359 
    4066 ...
    %4128 
    4968];
y = [ 3576 ...
    %3474 3530 
    2999 ...
    %2937 
    4396];
% reference points in lat/lon
lat = [43.62269 ...
    %43.62339 43.62293 
    43.62650 ...
    %43.62698 
    43.61708];
lon = [ 7.04670 ...
    %7.04552  7.04948  
    7.04689 ... 
    %7.04746  
    7.05468];

%plot(x,y,'o')

% calculate the scale factor of the projection by averaging
pairs = nchoosek(1:length(x),2);
npairs = nchoosek(length(x),2);
for i = 1:npairs
    scalex(i) = (x(pairs(i,1)) - x(pairs(i,2)))/(lon(pairs(i,1))-lon(pairs(i,2)));
    scaley(i) = (y(pairs(i,1)) - y(pairs(i,2)))/(lat(pairs(i,1))-lat(pairs(i,2)));
end

% apply the correction factor, which was estimated manually by inspection
scaley = mean(scaley);
scalex = mean(scalex)*1.15;

gps_y = (latitude-lat(1))*mean(scaley) + y(1);     % gps_x is the north-south direction
gps_x = (longitude-lon(1))*mean(scalex) + x(1);     % gps_y is the east-west direction

if ~isempty(mm)
    image(mm);  % plots the image itself
    hold on
end
if (nargin == 4 && ~isempty(rx_rssi))
    for i=1:length(gps_x)
        plot(gps_x(i),gps_y(i),'x','color',m(cidx(i),:))
    end
    yt = round(linspace(min(rx_rssi),max(rx_rssi),8));
    hcb = colorbar;
    set(hcb,'YTickMode','manual');
    set(hcb,'YTick',8:8:length(m));
    set(hcb,'YTickLabel',yt);
else
    plot(gps_x,gps_y,'x','Color',color);
end
if nargin >= 5
    h = text(mean(gps_x),mean(gps_y),label);
    set(h,'Color',color,'FontWeight','bold');
end
% if ~isempty(mm)
%     hold off
% end
if isempty(gps_x) || isempty(gps_y) || any(latitude==0) || any(longitude==0)
    axis([3264    5819    2610    4556]);
else
    axis([min(gps_x)-100, max(gps_x)+100, min(gps_y)-100, max(gps_y)+100]);
end
axis off
