function h_fig = pfair(dlsch_throughput,gps_lon_cat,gps_lat_cat,dist,mm,pathname,desc,h_fig)
%% extrapolation to loaded cell
N_samples = floor(length(dlsch_throughput)/4)*4;
%dlsch_throughput(~UE_connected) = 0;
dlsch_throughput4 = reshape(dlsch_throughput(1:N_samples),4,[]);
dlsch_throughput4_pfair = (dlsch_throughput4.^2)./repmat(sum(dlsch_throughput4,1),4,1);
dlsch_throughput4_pfair(isnan(dlsch_throughput4_pfair))=0;
dlsch_throughput_pfair = sum(dlsch_throughput4_pfair,1);

%% over time
h_fig = h_fig+1;
h_fig = figure(h_fig);
hold off
plot(dlsch_throughput4_pfair.','x')
hold on
plot(dlsch_throughput_pfair,'kx','Linewidth',2)
ylim([0 8.64e6]);
ylabel('Throughput [bps]')
xlabel('Time [sec]')
legend('User 1','User 2','User 3','User 4','Sum rate');
saveas(h_fig,fullfile(pathname,['pfair_throughput_4users_',desc,'.eps']),'epsc2');


%%
h_fig = h_fig+1;
h_fig = figure(h_fig);
hold off
colors = {'b','g','r','c','m','y','k','b--','g--','r--','c--','m--','y--','k--'};
for n = 1:4
    [f,x] = ecdf(dlsch_throughput4_pfair(n,:));
    plot(x,f,colors{n});
    hold on
end
[f,x] = ecdf(dlsch_throughput_pfair);
plot(x,f,'k','Linewidth',2);
xlim([0 8.64e6]);
legend('User 1','User 2','User 3','User 4','Sum rate');
title('DL Throughput CDF')
xlabel('Throughput [bps]')
ylabel('P(x<abscissa)')
grid on
saveas(h_fig,fullfile(pathname,['pfair_throughput_cdf_4users_' desc '.eps']),'epsc2');

%% DLSCH throughput over GPS coordinates
h_fig = h_fig+1;
figure(h_fig);
hold off
plot_gps_coordinates(mm,gps_lon_cat(1:N_samples), gps_lat_cat(1:N_samples), dlsch_throughput4_pfair(:), [0 8.64e6]);
title('DLSCH Throughput [bps]')
saveas(h_fig,fullfile(pathname,['pfair_troughput_gps_4users_' desc '.jpg']),'jpg')

%% plot througput as a function of distance
h_fig = h_fig+1;    
h_fig = figure(h_fig);
hold off
[out,n,n2] = plot_in_bins(dist(1:N_samples), dlsch_throughput4_pfair(:),  0:ceil(max(dist)));
ylim([0 8.64e6]);
title('DLSCH Throughput vs Dist');
xlabel('Dist[km]');
ylabel('Throughput[Bits/sec]');
saveas(h_fig,fullfile(pathname,['pfair_throughput_dist_4users_' desc '.eps']),'epsc2');
