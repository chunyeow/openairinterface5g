% eval_village_ambialet.m

% The village of vallence d'albigeois is enclosed in the following
% coordinates
lat_min = 44.017015135704256;
lon_min = 2.3998260498046875;
lat_max = 44.02226141991229;
lon_max = 2.4095678329467773;
% next to the village is an open crossing that we use for comparison
lat_min2 = 44.01099676748375;
lon_min2 = 2.3856210708618164;
lat_max2 = lat_min;
lon_max2 = lon_min;


mode1_village = mode1.gps_lat_cat<=lat_max & mode1.gps_lat_cat >= lat_min & mode1.gps_lon_cat<=lon_max & mode1.gps_lon_cat>=lon_min;
mode2_village = mode2.gps_lat_cat<=lat_max & mode2.gps_lat_cat >= lat_min & mode2.gps_lon_cat<=lon_max & mode2.gps_lon_cat>=lon_min;
mode6_village = mode6.gps_lat_cat<=lat_max & mode6.gps_lat_cat >= lat_min & mode6.gps_lon_cat<=lon_max & mode6.gps_lon_cat>=lon_min;

mode1_xing = mode1.gps_lat_cat<=lat_max2 & mode1.gps_lat_cat >= lat_min2 & mode1.gps_lon_cat<=lon_max2 & mode1.gps_lon_cat>=lon_min2;
mode2_xing = mode2.gps_lat_cat<=lat_max2 & mode2.gps_lat_cat >= lat_min2 & mode2.gps_lon_cat<=lon_max2 & mode2.gps_lon_cat>=lon_min2;
mode6_xing = mode6.gps_lat_cat<=lat_max2 & mode6.gps_lat_cat >= lat_min2 & mode6.gps_lon_cat<=lon_max2 & mode6.gps_lon_cat>=lon_min2;

%%
h_fig = figure(1);
hold off
plot_gps_coordinates(mm,mode2.gps_lon_cat(mode2_village),mode2.gps_lat_cat(mode2_village),[],[],'Village','blue');
hold on
plot_gps_coordinates('',mode2.gps_lon_cat(mode2_xing),mode2.gps_lat_cat(mode2_xing),[],[],'Xing','red');
saveas(h_fig,fullfile(pathname,'results','village_comparison_map.jpg'),'jpg')

%%
h_fig = figure(2);
hold off
colors = {'b','g','r','c','m','y','k','b--','g--','r--','c--','m--','y--','k--'};
legend_str = {};
ni=1;
for n = strmatch('rateps',nn).'
    si = strfind(nn{n},'supportedQam_eNB1_2Rx');
    if si
        [f,x] = ecdf(scale_ideal_tp(mode2_ideal.(nn{n})(mode2_village)));
        plot(x,f,colors{ni},'Linewidth',2);
        hold on
        [f,x] = ecdf(scale_ideal_tp(mode2_ideal.(nn{n})(mode2_xing)));
        plot(x,f,colors{7+ni},'Linewidth',2);
        ni=ni+1;
    end
end
[f,x] = ecdf(mode2.throughput(mode2_village));
plot(x,f,colors{ni},'Linewidth',2)
hold on
[f,x] = ecdf(mode2.throughput(mode2_xing));
plot(x,f,colors{ni+7},'Linewidth',2)
ni=ni+1;

legend_str = {'Tx mode 1 (extrap), Village','Tx mode 1 (extrap) Xing',...
    'Tx mode 2 (extrap), Village','Tx mode 2 (extrap) Xing',...
    'Tx mode 6 (extrap, feedback), Village','Tx mode 6 (extrap, feedback) Xing',...
    'Tx mode 6 (extrap, optim), Village','Tx mode 6 (extrap, optim) Xing',...
    'Tx mode 2 (real), Village','Tx mode 2 (real) Xing'};

title('DLSCH throughput CDF')
xlabel('Throughput [bps]')
ylabel('P(x<abscissa)')
legend(legend_str,'Interpreter','none','Location','SouthOutside','Fontsize',10);
grid on
saveas(h_fig,fullfile(pathname,'results','throughput_cdf_comparison_village.eps'),'epsc2')

