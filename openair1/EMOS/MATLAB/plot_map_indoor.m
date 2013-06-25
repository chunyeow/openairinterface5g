x = 1.0e+03 * [4.0668  4.0812  4.1054  4.0628  4.0910  4.0471  3.9160  4.0917];
y = 1.0e+03 * [2.9938  2.9262  2.9112  2.8928  2.8777  2.9328  2.9321  2.9505];

ax =    1.0e+03 * [3.8589    4.2404    2.8111    3.1121];

mm = imread('/extras/kaltenbe/EMOS/2004_fd0006_250_c_0485.tif');

h_fig = image(mm);
axis(ax)
axis off

plot(x(1),y(1),'gx','Markersize',10)
text(x(1),y(1),'Basestation','color','g','Fontweight','bold')

plot(x(2),y(2),'bx','Markersize',10)
text(x(2),y(2),'User 3','color','b','Fontweight','bold')

plot(x(3),y(3),'mx','Markersize',10)
text(x(3),y(3),'User 2','color','m','Fontweight','bold')

plot(x(4),y(4),'rx','Markersize',10)
text(x(4),y(4),'User 1,4','color','r','Fontweight','bold')

% plot(x(5),y(5),'wx','Markersize',10)
% text(x(5),y(5),'User 4','color','w','Fontweight','bold')

plot(x(6),y(6),'wx','Markersize',10)
text(x(6),y(6),'User 1,2,3,4','color','w','Fontweight','bold')

filepath = 'figs';
filename_gps = fullfile(filepath,sprintf('idx_%d_map_rx_rssi.eps',100));
saveas(h_fig, filename_gps, 'epsc2');
