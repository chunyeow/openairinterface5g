% file: plot_ber3
% author: florian.kaltenberger@eurecom.fr
% purpose: this script plots the figures for the PIMRC 2009 paper

clear all 
close all

figpath = '.';

%%
% size(est2) = [RX_type, CH2_power, CH1_power]
% RX_type: 1=SINLGE, 2=MMSE, 3=ML
% CHx_power: 1=-20, 2=-10, 3=0
load '/extras/kaltenbe/EMOS/data/20090723_DualStream_3Rooms/metadata_MU.mat';
est2 = reshape(est(3,[7:9 4:6 1:3 13:15 16:27]),3,4,2);

%load '/extras/kaltenbe/EMOS/data/20090506_DualStream_Lab/metadata_MU.mat';
%est2 = reshape(est(3,1:45),3,5,3);

%%
for c1=1:size(est2,3)
    for c2=1:size(est2,2)
        for r=1:size(est2,1)
            FER1(r,c2,c1)=sum(est2(r,c2,c1).crc_status(1,:)==-1)./length(est2(r,c2,c1).crc_status);        
            FER2(r,c2,c1)=sum(est2(r,c2,c1).crc_status(2,:)==-1)./length(est2(r,c2,c1).crc_status);
            SNR1(r,c2,c1)=10*log10(mean(mean(est2(r,c2,c1).rx_power(2,:,:)./est2(r,c2,c1).n0_power(2,:,:))));        
            SNR2(r,c2,c1)=10*log10(mean(mean(est2(r,c2,c1).rx_power(3,:,:)./est2(r,c2,c1).n0_power(3,:,:))));        
            P1(r,c2,c1)=mean(est2(r,c2,c1).rx_avg_power_dB(2,:));        
            P2(r,c2,c1)=mean(est2(r,c2,c1).rx_avg_power_dB(3,:));        
        end
    end
end

%%
h_fig = figure(1);
hold off
plot_style={'r-x','g-o','b-s','k-'; ...
            'r--x','g--o','b--s','k--'; ...
            'r:x','g:o','b:s','k:'};
clear legend_str
CH_power = -10:10:0;        
SINR = -5:5:10;
for c1=1:2
for r=1:2
    semilogy(SINR,squeeze(FER1(r,:,c1)),plot_style{r,c1},'Linewidth',2,'Markersize',10)
    if (r==2)
        legend_str{r,c1} = sprintf('MMSE, Power CH1 = %d dBm',CH_power(c1));
    elseif (r==1)
        legend_str{r,c1} = sprintf('max-log MAP, Power CH1 = %d dBm',CH_power(c1));
    end        
    hold on
end
end
%legend_str=legend_str.';
legend(legend_str{:},'Location','SouthEast')
xlabel('Power CH2 [dBm]','Fontsize',14)
ylabel('FER Stream 1','Fontsize',14)
grid on
saveas(h_fig,fullfile(figpath,sprintf('FER_comparison1.eps')),'epsc2');



%%
h_fig = figure(2);
hold off
plot_style={'r--x','g--o','b--s','k--'; ...
            'r-x','g-o','b-s','k-'; ...
            'r:x','g:o','b:s','k:'};
clear legend_str
%CH_power = -20:10:0;        
SINR = -10:5:5;
k=1;
for r=2:-1:1
for c1=1:2
    semilogy(SINR,squeeze(FER2(r,:,c1)),plot_style{r,c1},'Linewidth',2,'Markersize',10)
    if (r==2)
        legend_str{k} = sprintf('MMSE, Power CH1 = %d dBm',CH_power(c1));
    elseif (r==1)
        legend_str{k} = sprintf('max-log MAP, Power CH1 = %d dBm',CH_power(c1));
    end        
    k=k+1;
    hold on
end
end
%legend_str=legend_str.';
legend(legend_str{:},'Location','SouthEast')
xlabel('SIR [dB]','Fontsize',14)
ylabel('FER Stream 2','Fontsize',14)
grid on
saveas(h_fig,fullfile(figpath,sprintf('FER_comparison2.eps')),'epsc2');



