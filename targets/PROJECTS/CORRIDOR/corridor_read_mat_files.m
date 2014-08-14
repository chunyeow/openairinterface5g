close all;
clear all;

%n_carriers=1;% 1 for UHF files, 2 for 2.6GHz files
%file='E:\EMOS\corridor\postprocessed data\eNB_data_UHF_20140519_run4.mat'; % mat file
%file='E:\EMOS\corridor\postprocessed data\eNB_data_20140331_UHF_run2.mat'; % mat file

n_carriers=2;
n_trials=2;
n_runs=1;
%file='E:\EMOS\corridor\postprocessed data\eNB_data_20140331_2.6GHz_run2.mat'; % mat file
%file='eNB_data_UHF_20140519_run4.mat'; % mat file
file='eNB_data_20140519_2.6GHz_run1.mat'; % mat file


post_processed_data=load(file, 'PDD_totala','PDP_totala','delay_doppler_profile_beforea','delay_doppler_profile_duringa','delay_doppler_profile_aftera');
%post_processed_data=load(file, 'PDD_totala','PDP_totala');
if(n_carriers==2)
    post_processed_data=load(file, 'PDD_totala','PDP_totala','delay_doppler_profile_beforea','delay_doppler_profile_duringa','delay_doppler_profile_aftera','PDD_totalb','PDP_totalb','delay_doppler_profile_beforeb','delay_doppler_profile_duringb','delay_doppler_profile_afterb');
    
end

PDDta=post_processed_data(1,1).PDD_totala;
PDPta=post_processed_data(1,1).PDP_totala;
delay_doppler_profile_beforea=post_processed_data(1,1).delay_doppler_profile_beforea;
delay_doppler_profile_duringa=post_processed_data(1,1).delay_doppler_profile_duringa;
delay_doppler_profile_aftera=post_processed_data(1,1).delay_doppler_profile_aftera;

if(n_carriers==2)
    PDDtb=post_processed_data(1,1).PDD_totalb;
    PDPtb=post_processed_data(1,1).PDP_totalb;
    delay_doppler_profile_beforeb=post_processed_data(1,1).delay_doppler_profile_beforeb;
    delay_doppler_profile_duringb=post_processed_data(1,1).delay_doppler_profile_duringb;
    delay_doppler_profile_afterb=post_processed_data(1,1).delay_doppler_profile_afterb;
end

if n_trials==1
    if n_runs==1
        block_before=50;
        block_during=90;
        block_after=130;
    end
    if n_runs==2
        if n_carriers==1% we have changed the orientation of the antennas for the UHF channel in Trial 1 Run 2
            
            
            block_before=60;
            block_during=155;
            block_after=190;
        end
        
        if n_carriers==2
            block_before=60;
            block_during=107;
            block_after=140;
            
        end
        
    end
end

if n_trials==2
    if n_runs==1
        block_before=50;
        block_during=91;
        block_after=140;
    end
    
    if n_runs==2
        block_before=45;
        block_during=77;
        block_after=120;
    end
    
    if n_runs==3
        block_before=45;
        block_during=83;
        block_after=120;
    end
    
    if n_runs==4
        block_before=34;
        block_during=43;
        block_after=90;
    end
end


%% Doppler spectrum (choose the block you want to read)
block = 20;
figure(1)
for i=1:size(PDDta,3)
    for j=1:size(PDDta,4)
        subplot(size(PDDta,3),size(PDDta,4),(i-1)*size(PDDta,4)+j)
        F=-(100*120/2-1)*7.68E6/(2*100*120/2)/1280:7.68E6/(100*120/2)/1280:(100*120/2-1)*7.68E6/(2*100*120/2)/1280;
        if (n_carriers==2)
            F=-(50*120/2-1)*30.72E6/(2*50*120/2)/5120:30.72E6/(50*120/2)/5120:(50*120/2-1)*30.72E6/(2*50*120/2)/5120;
        end
        plot(F,10*log(PDDta(:,block,i,j)));
        ylabel('power [dB]')
        xlabel('Doppler shift [Hz]')
    end
end

if(n_carriers==2)
    
    figure(2)
    for i=1:size(PDDtb,3)
        for j=1:size(PDDtb,4)
            subplot(size(PDDtb,3),size(PDDtb,4),(i-1)*size(PDDtb,4)+j)
            F=-(50*120/2-1)*15.36E6/(2*50*120/2)/2560:15.36E6/(50*120/2)/2560:(50*120/2-1)*15.36E6/(2*50*120/2)/2560;
            plot(F,10*log(PDDtb(:,block,i,j)));
        end
    end
    
end
%% Power Delay Profile (choose the frame you want to read)
frame = 3000;
figure(3)
for i=1:size(PDDta,3)
    for j=1:size(PDDta,4)
        T=1:1:(size(PDPta,1));
        tau=linspace(0,300/4/4.5E6,300);
        T=1:1:(size(PDPta,1));
        if n_carriers==2
            tau=linspace(0,1200/4/18E6,1200/4);
        end
        subplot(size(PDDta,3),size(PDDta,4),(i-1)*size(PDDta,4)+j)
        
        plot(10*log10(PDPta(frame,:,i,j)));
        xlabel('delay [s]')
        ylabel('time [*10 ms]')
    end
end

if(n_carriers==2)
    figure(4)
    for i=1:size(PDDtb,3)
        for j=1:size(PDDtb,4)
            T=1:1:(size(PDPtb,1));
            tau=linspace(0,600/4/18E6,600/4);
          
            subplot(size(PDDtb,3),size(PDDtb,4),(i-1)*size(PDDtb,4)+j)
            
            
            plot(10*log10(PDPtb(frame,:,i,j)));
            xlabel('delay [s]')
            ylabel('time [*10 ms]')
        end
    end
end

%% Total doppler spectrum in pseudocolor plot
doppler_profile_figures_dir = 'E:\byiringi\Matlab Plots\Doppler Shift pcolor plots new sync\';

h=figure(5);
hold off
for i=1:size(PDDta,3)
    for j=1:size(PDDta,4)
        
        
        T=1:1:size(PDDta,2);
        F=-(100*120/2-1)*7.68E6/(2*100*120/2)/1280:7.68E6/(100*120/2)/1280:(100*120/2-1)*7.68E6/(2*100*120/2)/1280;
        filename=sprintf('Trial %d Run %d UHF.fig',n_trials,n_runs);
        if(n_carriers==2)
            F=-(50*120/2-1)*30.72E6/(2*50*120/2)/5120:30.72E6/(50*120/2)/5120:(50*120/2-1)*30.72E6/(2*50*120/2)/5120;
            filename=sprintf('Trial %d Run %d 2.6 GHz Carrier 2a.fig',n_trials,n_runs);
        end
        
        
        
        subplot(size(PDDta,3),size(PDDta,4),(i-1)*size(PDDta,4) + j);
        pcolor(T,F,10*log10( PDDta(:,:,i,j)));
        shading flat
        %colormap hot
        bara=colorbar;
        %ylim([])
        %xlim([])
        xlabel('time [s]')
        ylabel('Doppler shift [Hz]')
        ylabel(bara,'Power [dB]')
    end
end
saveas(h,strcat(doppler_profile_figures_dir, filename));


% for i=1:size(PDDta,1)
%     for j=1:size(PDDta,2)
%         if 10*log10(PDDta(i,j,1,1))<115
%             PDDta(i,j,1,1)=10^11.5;
%         end
%     end
%     
% end
% figure (15)
% 
% T=1:1:size(PDDta,2);
%         F=-(100*120/2-1)*7.68E6/(2*100*120/2)/1280:7.68E6/(100*120/2)/1280:(100*120/2-1)*7.68E6/(2*100*120/2)/1280;
%         if(n_carriers==2)
%             F=-(50*120/2-1)*30.72E6/(2*50*120/2)/5120:30.72E6/(50*120/2)/5120:(50*120/2-1)*30.72E6/(2*50*120/2)/5120;
%         end
%         
%         
%         
%         
%         pcolor(T,F,10*log10( PDDta(:,:,1,1)));
%         shading flat
%         colormap hot
%         bara=colorbar;
%         %ylim([])
%         %xlim([])
%         xlabel('time [s]')
%         ylabel('Doppler shift [Hz]')
 

if(n_carriers==2)
    h=figure(6);
    for i=1:size(PDDtb,3)
        for j=1:size(PDDtb,4)
            
            
            T=1:1:size(PDDtb,2);
            F=-(50*120/2-1)*15.36E6/(2*50*120/2)/2560:15.36E6/(50*120/2)/2560:(50*120/2-1)*15.36E6/(2*50*120/2)/2560;
            filename=sprintf('Trial %d Run %d 2.6 GHz Carrier 2b.fig',n_trials,n_runs);
            
            
            subplot(size(PDDtb,3),size(PDDtb,4),(i-1)*size(PDDtb,4) + j);
            pcolor(T,F,10*log10( PDDtb(:,:,i,j)));
            shading flat
            barb=colorbar;
            %colormap hot
            %ylim([])
            %xlim([])
            xlabel('time [s]')
            ylabel('Doppler shift [Hz]')
            ylabel(barb,'Power [dB]')
        end
    end
    saveas(h,strcat(doppler_profile_figures_dir, filename));
end



%% Total Power Delay Profile in pseudocolor
power_delay_profile_figures_dir = 'E:\byiringi\Matlab Plots\PDP pcolor plots new sync\';

h=figure(7);
for i=1:size(PDDta,3)
    for j=1:size(PDDta,4)
        tau=linspace(0,300/4/4.5E6,300/4);
        T=1:1:(size(PDPta,1));
        filename=sprintf('Trial %d Run %d UHF.fig',n_trials,n_runs);
        if n_carriers==2
            tau=linspace(0,1200/4/18E6,1200/4);
            filename=sprintf('Trial %d Run %d 2.6 GHz Carrier 2a.fig',n_trials,n_runs);
        end
        subplot(size(PDDta,3),size(PDDta,4),(i-1)*size(PDDta,4)+j)
        pcolor(tau,T,10*log10(PDPta(:,:,i,j)));
        bara=colorbar;
        shading flat
        %colormap hot
        xlabel('delay [s]')
        ylabel('time [*10 ms]')
        ylabel(bara,'Power [dB]')
    end
end
saveas(h,strcat(power_delay_profile_figures_dir, filename));

% for i=1:size(PDPta,1)
%     for j=1:size(PDPta,2)
%         if 10*log10(PDPta(i,j,1,1))<57
%             PDPta(i,j,1,1)=10^5.7;
%         end
%     end
%     
% end
% figure (17)
% tau=linspace(0,300/4/4.5E6,300/4);
%         T=1:1:(size(PDPta,1));
%         if n_carriers==2
%             tau=linspace(0,1200/4/18E6,1200/4);
%         end
%         
%         pcolor(tau,T,10*log10(PDPta(:,:,1,1)));
%         bara=colorbar;
%         shading flat
%         colormap hot
%         xlabel('delay [s]')
%         ylabel('time [*10 ms]')

if(n_carriers==2)
    h=figure(8);
    for i=1:size(PDDtb,3)
        for j=1:size(PDDtb,4)
            tau=linspace(0,600/4/9E6,600/4);
            T=1:1:(size(PDPtb,1));
            filename=sprintf('Trial %d Run %d 2.6 GHz Carrier 2b.fig',n_trials,n_runs);
            subplot(size(PDDtb,3),size(PDDtb,4),(i-1)*size(PDDtb,4)+j)
            pcolor(tau,T,10*log10(PDPtb(:,:,i,j)));
            barb=colorbar;
            shading flat
            %colormap hot
            xlabel('delay [s]')
            ylabel('time [*10 ms]')
            ylabel(barb,'Power [dB]')
        end
    end
    saveas(h,strcat(power_delay_profile_figures_dir, filename));
end


%% Delay Doppler Spectrum before, during and after the passing of the train

figure(9)


tau=linspace(0,300/4/4.5E6,300/4);
F=-(100*120/2-1)*7.68E6/(2*100*120/2)/1280:7.68E6/(100*120/2)/1280:(100*120/2-1)*7.68E6/(2*100*120/2)/1280;
if(n_carriers==2)
    tau=linspace(0,1200/4/18E6,1200/4);
    F=-(50*120/2-1)*30.72E6/(2*50*120/2)/5120:30.72E6/(50*120/2)/5120:(50*120/2-1)*30.72E6/(2*50*120/2)/5120;
end

subplot(1,3,1)

pcolor(tau,F,10*log10(delay_doppler_profile_beforea(:,:)))
shading flat
colormap hot
bar1=colorbar;
xlabel('delay [s]')
ylabel('Doppler shift [Hz]')
ylabel(bar1,'Power [dB]')
title(sprintf('Delay Doppler Spectrum for UHF-Trial %d-Run %d-Block %d ',n_trials,n_runs,block_before));
if(n_carriers==2)
    title(sprintf('Delay Doppler Spectrum for 2.6GHz Carrier 1-Trial %d-Run %d-Block %d ',n_trials,n_runs,block_before));
end

subplot(1,3,2)
pcolor(tau,F,10*log10(delay_doppler_profile_duringa(:,:)))
shading flat
colormap hot
bar2=colorbar;
xlabel('delay [s]')
ylabel('Doppler shift [Hz]')
ylabel(bar2,'Power [dB]')
title(sprintf('Delay Doppler Spectrum for UHF-Trial %d-Run %d-Block %d ',n_trials,n_runs,block_during));
if(n_carriers==2)
    title(sprintf('Delay Doppler Spectrum for 2.6GHz Carrier 1-Trial %d-Run %d-Block %d ',n_trials,n_runs,block_during));
end

subplot(1,3,3)
pcolor(tau,F,10*log10(delay_doppler_profile_aftera(:,:)))
shading flat
colormap hot
bar3=colorbar;
xlabel('delay [s]')
ylabel('Doppler shift [Hz]')
ylabel(bar3,'Power [dB]')
title(sprintf('Delay Doppler Spectrum for UHF-Trial %d-Run %d-Block %d ',n_trials,n_runs,block_after));
if(n_carriers==2)
    title(sprintf('Delay Doppler Spectrum for 2.6GHz Carrier 1-Trial %d-Run %d-Block %d ',n_trials,n_runs,block_after));
end



if(n_carriers==2)
    figure(10)
    tau=linspace(0,600/4/9E6,600/4);
    F=-(50*120/2-1)*15.36E6/(2*50*120/2)/2560:15.36E6/(50*120/2)/2560:(50*120/2-1)*15.36E6/(2*50*120/2)/2560;
    
    subplot(1,3,1)
    pcolor(tau,F,10*log10(delay_doppler_profile_beforeb(:,:)))
    shading flat
    colormap hot
    bar4=colorbar;
    xlabel('delay [s]')
    ylabel('Doppler shift [Hz]')
    ylabel(bar4,'Power [dB]')
    title(sprintf('Delay Doppler Spectrum for 2.6GHz Carrier 2-Trial %d-Run %d-Block %d ',n_trials,n_runs,block_before));
    
     subplot(1,3,2)
    pcolor(tau,F,10*log10(delay_doppler_profile_duringb(:,:)))
    shading flat
    bar5=colorbar;
    colormap hot
    xlabel('delay [s]')
    ylabel('Doppler shift [Hz]')
    ylabel(bar5,'Power [dB]')
    title(sprintf('Delay Doppler Spectrum for 2.6GHz Carrier 2-Trial %d-Run %d-Block %d ',n_trials,n_runs,block_during));
    
     subplot(1,3,3)
    pcolor(tau,F,10*log10(delay_doppler_profile_afterb(:,:)))
    shading flat
    bar6=colorbar;
    colormap hot
    xlabel('delay [s]')
    ylabel('Doppler shift [Hz]')
    ylabel(bar6,'Power [dB]')
    title(sprintf('Delay Doppler Spectrum for 2.6GHz Carrier 2-Trial %d-Run %d-Block %d ',n_trials,n_runs,block_after));
end