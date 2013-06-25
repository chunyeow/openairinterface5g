% file: plot_ber2
% author: florian.kaltenberger@eurecom.fr
% purpose: this script plots the figures for the PIMRC 2009 paper

clear all 
close all

load '/extras/kaltenbe/EMOS/data/20090506_DualStream_Lab/metadata_MU.mat';
figpath = '.';

%%
% size(est2) = [RX_type, CH2_power, CH1_power]
% RX_type: 1=SINLGE, 2=MMSE, 3=ML
% CHx_power: 1=-20, 2=-10, 3=0
est2 = reshape(est(3,1:45),3,5,3);

%%
for c1=1:3
    for c2=1:3
        for r=1:3
            FER1(r,c2,c1)=sum(est2(r,c2,c1).crc_status(1,:)==-1)./length(est2(r,c2,c1).crc_status);        
            FER2(r,c2,c1)=sum(est2(r,c2,c1).crc_status(2,:)==-1)./length(est2(r,c2,c1).crc_status);
            SNR1(r,c2,c1)=10*log10(mean(mean(est2(r,c2,c1).rx_power(2,:,:)./est2(r,c2,c1).n0_power(2,:,:))));        
            SNR2(r,c2,c1)=10*log10(mean(mean(est2(r,c2,c1).rx_power(3,:,:)./est2(r,c2,c1).n0_power(3,:,:))));        
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
CH_power = -20:10:0;        
for c1=1:3
for r=2:3
    semilogy(CH_power,squeeze(FER1(r,:,c1)),plot_style{r-1,c1},'Linewidth',2,'Markersize',10)
    if (r==2)
        legend_str{r-1,c1} = sprintf('MMSE, Power CH1 = %d dBm',CH_power(c1));
    elseif (r==3)
        legend_str{r-1,c1} = sprintf('max-log MAP, Power CH1 = %d dBm',CH_power(c1));
    end        
    hold on
end
end
%legend_str=legend_str.';
legend(legend_str{:},'Location','SouthEast')
xlabel('Power CH2 [dBm]','Fontsize',14)
ylabel('FER Stream 1','Fontsize',14)
grid on
saveas(h_fig,fullfile(figpath,sprintf('FER_comparison2.eps')),'epsc2');

%%
h_fig = figure(2);
hold off
plot_style={'r-','g-','b-','k-'; ...
            'r--','g--','b--','k--'; ...
            'r:','g:','b:','k:'};
clear legend_str
CH_power = -20:10:0;        
for c2=1:3
for r=2:3
    semilogy(CH_power,squeeze(FER2(r,c2,:)),plot_style{r,c2},'Linewidth',2)
    legend_str{r-1,c2} = sprintf('RX %d, Power CH2 = %ddB',r,CH_power(c2));
    hold on
end
end
%legend_str=legend_str.';
legend(legend_str{:},'Location','SouthWest')
xlabel('Power CH1','Fontsize',14)
ylabel('FER Stream 2','Fontsize',14)
grid on


