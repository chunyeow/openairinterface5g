function [gps_x, gps_y] = plot_gps_coordinates(map, longitude, latitude, rx_rssi, limits, label, color, varargin)
% h = plot_gps_coordinates(mm, longitude, latitude, rx_rssi, limits, label, color, style)
%
%  This function plots the gps coordinates given by langitude and latutude
%  onto the map of garbejaire. If langitude and latutude are matrices, each
%  row is treated as a seperate user and plotted in a different color.
%  This function uses the Plate carr???e projection
%  (http://en.wikipedia.org/wiki/Plate_carr%C3%A9e_projection). We assume
%  the coordinates of 3 reference points are known in lat/lon as well as
%  x/y coordinates. This projection method is only fairly accurate. Also
%  there will be inaccuracies in the reference points. It is therefore
%  necessary the check the result and include a correction factor if
%  neccesary.
%
%  The function takes the following parameters
%   map ... 'cordes','penne','ambialet'
%   longitude, latitude  ... from gps data
%   rx_rssi ... data to be plotted
%   label (optional) ... text label for data
%   color (optional) ... plot data in color instead of using data in rx_rssi

persistent mm image_points gps_points_num;

plot_image = 1;
switch map
    case 'cordes'
        if isempty(mm)
            mm=imread('cordes_merged_routes_wp.png');
            load('gps_calib_cordes_new.mat')
        end
    case 'penne'
        if  isempty(mm)
            mm=imread('penne3.png');
            load('gps_calib_penne_new.mat')
        end
     case 'ambialet'
        if  isempty(mm)
            mm=imread('ambialet2.png');
            load('gps_calib_ambialet_new.mat')
        end
     otherwise
        plot_image = 0;
end

if nargin<=3
    rx_rssi = [];
end

if nargin<=4
    if (~isempty(rx_rssi))
        m = colormap;
        cmin = min(rx_rssi(isfinite(rx_rssi)));
        cmax = max(rx_rssi(isfinite(rx_rssi)));
        cidx = fix((rx_rssi-cmin)/(cmax-cmin)*(length(m)-1))+1;
%         s = (cmax-cmin)/(length(m)-1);
%         if (s==0)
%             cidx = ones(size(rx_rssi));
%         else
%             cidx = ceil((rx_rssi-cmin)/s+1);
%         end
    end
else
%    if isempty(rx_rssi)
%        error('rx_rssi cannot be empty if limits is provided');
%    end
    if (~isempty(limits))
        m = colormap;
        cmin = limits(1);
        cmax = limits(2);
        rx_rssi(rx_rssi<cmin | rx_rssi>cmax) = nan;
        cidx = fix((rx_rssi-cmin)/(cmax-cmin)*(length(m)-1))+1;
%         s = (cmax-cmin)/(length(m)-1);
%         if (s==0)
%             cidx = ones(size(rx_rssi));
%         else
%             cidx = ceil((rx_rssi-cmin)/s+1);
%         end
    end
end
    
if nargin <= 6
    color = 'blue';
end
if nargin <= 7
    style = {'Marker','x','Line','none'};
else
    style = varargin;
end

x = image_points(:,1);
y = image_points(:,2);
lat = gps_points_num(:,1);
lon = gps_points_num(:,2);

%plot(x,y,'o')
%keyboard;

% calculate the scale factor of the projection by averaging
pairs = nchoosek(1:length(x),2);
npairs = nchoosek(length(x),2);
for i = 1:npairs
    scalex(i) = (x(pairs(i,1)) - x(pairs(i,2)))/(lon(pairs(i,1))-lon(pairs(i,2)));
    scaley(i) = (y(pairs(i,1)) - y(pairs(i,2)))/(lat(pairs(i,1))-lat(pairs(i,2)));
end

% apply the correction factor, which was estimated manually by inspection
scaley = mean(scaley);
scalex = mean(scalex);

gps_y = (latitude-lat(1))*mean(scaley) + y(1);     % gps_x is the north-south direction
gps_x = (longitude-lon(1))*mean(scalex) + x(1);     % gps_y is the east-west direction

if plot_image
    image(mm);  % plots the image itself
    hold on
end
    
if (~isempty(rx_rssi))
    for i=1:length(gps_x)
        if (latitude(i)~=0 && longitude(i)~=0 && isfinite(rx_rssi(i)))
            plot(gps_x(i),gps_y(i),'color',m(cidx(i),:),style{:})
            hold on
        end
    end
    hcb = colorbar;
    set(hcb,'YTickMode','manual');
    set(hcb,'YTick',1:8:length(m));
    yt = round(linspace(cmin,cmax,64));
    yt = yt(1:8:length(m));
    clabel = cell(size(yt));
    for j=1:length(yt)
        clabel{j} = sprintf('%4.3g', yt(j));
    end
    set(hcb,'YTickLabel',clabel);
else
    plot(gps_x,gps_y,'color',color,style{:});
end
if nargin >= 6
    h = text(mean(gps_x),mean(gps_y),label);
    set(h,'Color',color,'FontWeight','bold');
end
% if ~isempty(mm)
%     hold off
% end
%if isempty(gps_x) || isempty(gps_y) || any(latitude==0) || any(longitude==0)
%    axis([3264    5819    2610    4556]);
%else
%    axis([min(gps_x)-100, max(gps_x)+100, min(gps_y)-100, max(gps_y)+100]);
%end
axis image
axis off
