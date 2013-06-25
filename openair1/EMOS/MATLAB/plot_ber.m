% file: plot_ber 
% author: florian.kaltenberger@eurecom.fr
% purpose: this script plots the figures for the PIMRC 2009 paper

close all
clear all

addpath('import_filter');

%%
path = '/extras/kaltenbe/EMOS/data/20090505_DualStream_Lab/';
figpath = '/extras/kaltenbe/EMOS/data/20090505_DualStream_Lab/figs';
d = dir(path);
idx_vec = [1 2 4];

for i=1:length(idx_vec)
%%    
idx = idx_vec(i);
[estimates, gps_data, NFrames] = load_estimates2_no_channel(fullfile(path,d(idx+3).name));
disp(idx);

SNR_edges = [-Inf 0:3:45 Inf];
SNR_edges1 = [-Inf 27:6:45 Inf];

%%
SNR = 10*log10(estimates.rx_power./estimates.n0_power);
Nerrors = zeros(2,length(SNR_edges));
BER = zeros(2,length(SNR_edges));

for c=1:2
    [n,bin] = histc(squeeze(mean(SNR(c+1,:,:),2)),SNR_edges);
    figure(c)
    hold off
    bar(SNR_edges(2:end-1),n(2:end-1),'histc');
    xlabel('SNR')
    ylabel('Count')
    title(sprintf('SNR Histogram for Stream %d',c));
    for b=1:length(SNR_edges)
        Nerrors(c,b) = sum(estimates.crc_status(c,bin==b)==-1);
        BER(c,b) = Nerrors(c,b)/sum(bin==b);
    end
end

%%
h_fig = figure(3);
hold off
semilogy(SNR_edges-1.5,BER.');
grid on
xlabel('SNR');
ylabel('FER');
legend('Stream 1','Stream 2')
title(sprintf('RX mode = %d',mode(estimates.rx_mode)));
saveas(h_fig,fullfile(figpath,sprintf('FER1+2_idx%d.eps',idx)),'epsc2');


%%
figure(4)
hold off
[n2,bin2] = my_hist3(squeeze(mean(SNR(2:3,:,:),2)).','Edges',{SNR_edges1, SNR_edges});
% size(n2) = [SNR_CH1, SNR_CH2]
%hist3(squeeze(mean(SNR(2:3,:,:),2)).','Edges',{SNR_edges, SNR_edges});
xlabel('SNR stream 1')
ylabel('SNR stream 2')
zlabel('Count')
Nerrors1 = accumarray(bin2,estimates.crc_status(1,:)==-1,size(n2));
Nerrors2 = accumarray(bin2,estimates.crc_status(2,:)==-1,size(n2));
FER1 = Nerrors1./n2;
FER2 = Nerrors2./n2;

%%
h_fig = figure(5);
plot_style={'r-','g-','b-','k-'; ...
            'r--','g--','b--','k--'; ...
            'r:','g:','b:','k:'};
k=1;
for b=2:3
    semilogy(SNR_edges,FER1(b,:),plot_style{i,k},'Linewidth',2)
    legend_str{i,k} = sprintf('idx %d, SNR Stream 1 = %ddB',idx,SNR_edges1(b));
    k=k+1;
    hold on
end
legend_str2=legend_str.';
legend(legend_str2{:},'Location','SouthWest')
xlabel('SNR Stream 2','Fontsize',14)
ylabel('FER Stream 2','Fontsize',14)
grid on
saveas(h_fig,fullfile(figpath,sprintf('FER1_idx%d.eps',idx)),'epsc2');

if 0
%%
h_fig = figure(6);
hold off
surf(SNR_edges(2:end-1),SNR_edges1(2:end-1),FER1(2:end-1,2:end-1));
set(gca,'zscale','log')
xlabel('SNR stream 2')
ylabel('SNR stream 1')
zlabel('FER1');
saveas(h_fig,fullfile(figpath,sprintf('FER1_3D_idx%d.eps',idx)),'epsc2');

%%
h_fig = figure(7);
hold off
surf(SNR_edges(2:end-1),SNR_edges1(2:end-1),FER2(2:end-1,2:end-1));
set(gca,'zscale','log')
xlabel('SNR stream 2')
ylabel('SNR stream 1')
zlabel('FER2');
saveas(h_fig,fullfile(figpath,sprintf('FER2_3D_idx%d.eps',idx)),'epsc2');

%%
h_fig = figure(8);
hold off
fer2 = sum(reshape(estimates.crc_status(2,:),100,[])==-1,1)./100;
fer1 = sum(reshape(estimates.crc_status(1,:),100,[])==-1,1)./100;
plot(fer1,'r.')
hold on
plot(fer2,'b.')
grid on
xlabel('Distance')
ylabel('FER')
legend('Stream 1','Stream 2')
saveas(h_fig,fullfile(figpath,sprintf('FER1+2_dist_idx%d.eps',idx)),'epsc2');
end

%%
end