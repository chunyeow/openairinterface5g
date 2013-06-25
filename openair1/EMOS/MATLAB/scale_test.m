close all
figure

%%
hold off
image(mm)
hold on
gps_y = (latitude-lat(1))*(scaley*1) + y(1);     % gps_x is the north-south direction
gps_x = (longitude-lon(1))*(scalex*1.15) + x(1);     % gps_y is the east-west direction
plot(gps_x,gps_y,'x');
axis([min(gps_x)-100, max(gps_x)+100, min(gps_y)-100, max(gps_y)+100]);
