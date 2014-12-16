close all
clear all

global symbols_per_slot slots_per_frame;

addpath('E:\Synchro\kaltenbe\My Documents\Matlab\dpss_chanest')
addpath('../../../openair1/PHY/LTE_REFSIG');

enable_plots=0; %enables figures
record=0; %put 1 to enable the video record of the delay doppler profile

%% preload and init data
primary_synch; %loads the primary sync signal

%load('E:\EMOS\corridor\ofdm_pilots_sync_2048_v7.mat');
load('ofdm_pilots_sync_30MHz.mat');

n_carriers = 2; % use 1 for UHF and 2 for 2.6GHz
n_trials=2;%use 1 for trial1 and 2 for trial2
n_run=1;
symbols_per_slot = 6;
slots_per_frame = 20;

sourcedir = 'F:\trials2 train extracted\';
destdir = 'F:\trials2 train processed\';


switch n_carriers
    case 1,
        p = init_params(25,3,4); %this can be 25, 50, or 100
        pss_t = upsample(primary_synch0_time,4);
        
        if n_trials==1
            filename = sprintf('eNB_data_20140331_UHF_run%d',n_run);
        else
            filename = sprintf('eNB_data_UHF_20140519_run%d',n_run);
        end
        
        nframes = 100; % frames in one block
        threshold = 3e+4 ; % maybe should change that !!!!
    case 2,
        p(1) = init_params(100,2,4);
        p(2) = init_params(50,2,4);
        pss_t = upsample(primary_synch0_time,4*4); % this assumes we are doing the sync on the second carrier, which is 10MHz
        
        if (n_trials==1)
            filename = sprintf('eNB_data_20140331_2.6GHz_run%d',n_run);
        else
            filename = sprintf('eNB_data_20140519_2.6GHz_run%d',n_run);
        end
        
        nframes = 50; % frames in one block
        threshold = 3e+4 ; % maybe should change that !!!!
end

% derived parameters
samples_slot_agg = sum([p.nant_rx].*[p.samples_slot]);
num_symbols_frame = symbols_per_slot*slots_per_frame;

d = dir(fullfile(sourcedir,[filename '.EMOS']));
nblocks = floor(d.bytes/(samples_slot_agg*slots_per_frame*nframes*4));

%frequency offset
if(n_carriers==1)
    if(n_trials==1)
        f_offset=840;
    end
    if(n_trials==2)
        %f_offset=;
    end
end
if(n_carriers==2)
    if(n_trials==1)
        %f_offset=;
    end
    if(n_trials==2)
        %f_offset=;
    end
end




doppler_freq_of_max_a=zeros(1,nblocks);
doppler_freq_of_max_b=zeros(1,nblocks);
if(n_carriers==1)
    
    fm_total=zeros(1,nblocks);%vector containing the mean Doppler Shift for each block
    freqOffset_total=zeros(1,nblocks);%vector containing the mean frequency offset for each block
    TGVspeed_total=zeros(1,nblocks);%vector containing the TGV speed for each block
end

PDP_totala = zeros(nblocks*nframes,p(1).useful_carriers/4,p(1).nant_tx,p(1).nant_rx);
PDD_totala = zeros(nframes*num_symbols_frame/2,nblocks,p(1).nant_tx,p(1).nant_rx);

% delay doppler spectrum

delay_doppler_profile_videoa=VideoWriter(sprintf('Trial%d_Run%d_UHF_delayDopplerProfile.avi',n_trials,n_run));%variable used to make a video of the evolution of the delay doppler profile
if n_carriers==2
    delay_doppler_profile_videoa=VideoWriter(sprintf('Trial%d_Run%d_2.6GHzCarrier2a_delayDopplerProfile.avi',n_trials,n_run));
end

delay_doppler_profile_beforea=zeros(nframes*num_symbols_frame/2,p(1).useful_carriers/4);%contains the delay doppler spectrum for a block before the passing of the train
delay_doppler_profile_duringa=zeros(nframes*num_symbols_frame/2,p(1).useful_carriers/4);%contains the delay doppler spectrum for a block during the passing of the train
delay_doppler_profile_aftera=zeros(nframes*num_symbols_frame/2,p(1).useful_carriers/4);%contains the delay doppler spectrum for a block after the passing of the train

if n_carriers==2
    delay_doppler_profile_videob=VideoWriter(sprintf('Trial%d_Run%d_2.6GHzCarrier2b_delayDopplerProfile.avi',n_trials,n_run));
    delay_doppler_profile_beforeb=zeros(nframes*num_symbols_frame/2,p(2).useful_carriers/4);%contains the delay doppler spectrum for a block before the passing of the train
    delay_doppler_profile_duringb=zeros(nframes*num_symbols_frame/2,p(2).useful_carriers/4);%contains the delay doppler spectrum for a block during the passing of the train
    delay_doppler_profile_afterb=zeros(nframes*num_symbols_frame/2,p(2).useful_carriers/4);%contains the delay doppler spectrum for a block after the passing of the train
end

if n_trials==1
    if n_run==1
        block_before=50;
        block_during=90;
        block_after=130;
    end
    if n_run==2
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
    if n_run==1
        block_before=50;
        block_during=91;
        block_after=140;
    end
    
    if n_run==2
        block_before=45;
        block_during=77;
        block_after=120;
    end
    
    if n_run==3
        block_before=45;
        block_during=83;
        block_after=120;
    end
    
    if n_run==4
        block_before=34;
        block_during=43;
        block_after=90;
    end
end

if(n_carriers==2)
    PDP_totalb = zeros(nblocks*nframes,p(2).useful_carriers/4,p(2).nant_tx,p(2).nant_rx);
    PDD_totalb=zeros(nframes*num_symbols_frame/2,nblocks,p(2).nant_tx,p(2).nant_rx);
    interesting_delay_doppler_profileb=zeros(nframes*num_symbols_frame/2,p(2).useful_carriers/4);%contains the delay doppler spectrum for the wanted block
    if (n_trials==2)
        
        interesting_block=60;%contains the value of one interesting block for the delay_doppler_spectrum
        
    end
end


syncblock=0;%variable containing the number of the synchronization block


%% init DPSS parameters
max_tau = 1e-6;
for carrier=1:n_carriers
    p_dpss(carrier) = init_dpss(p(carrier),max_tau);
end


%% main loop
fid = fopen(fullfile(sourcedir,[filename '.EMOS']),'r');

vStorage1 = [];
vStorage2 = [];



block = 1;
flag1 = 1;
start = 1; % Maybe 2; if it works with 1, then the variable is useless


%fseek(fid,samples_slot_agg*slots_per_frame*nframes*60*2,'bof'); %advance 30 sec
NFRAMES = 100;
if(n_carriers==2)
    NFRAMES=50;
end
nframes = NFRAMES;

open(delay_doppler_profile_videoa);
if n_carriers==2
    open(delay_doppler_profile_videob);
end

noise1 = zeros(nblocks*NFRAMES,p(1).nant_rx);
if n_carriers==2
    noise2 = zeros(nblocks*NFRAMES,p(2).nant_rx);
end
H_power1 = zeros(nblocks*NFRAMES,p(1).nant_tx,p(1).nant_rx);
if n_carriers==2
    H_power2 = zeros(nblocks*NFRAMES,p(2).nant_tx,p(2).nant_rx);
end

while ~feof(fid)
    
    
    
    fprintf(1,'Processing block %d of %d',block,nblocks);
    
    
    
    [v,c]=fread(fid, 2*samples_slot_agg*slots_per_frame*nframes, 'int16',0,'ieee-le');
    
    if (c<2*samples_slot_agg*slots_per_frame*nframes)
        break
    end
    v0 = double(v(1:2:end))+1j*double(v(2:2:end));
    
    v1 = zeros(p(1).samples_slot*slots_per_frame*nframes,p(1).nant_rx);
    if n_carriers==2
        v2 = zeros(p(2).samples_slot*slots_per_frame*nframes,p(2).nant_rx);
    end
    
    for slot=1:slots_per_frame*nframes
        for a=1:p(1).nant_rx
            v1((slot-1)*p(1).samples_slot+1:slot*p(1).samples_slot,a) = ...
                v0((slot-1)*samples_slot_agg+(a-1)*p(1).samples_slot+1:...
                (slot-1)*samples_slot_agg+ a   *p(1).samples_slot,1);
        end
        
        if n_carriers==2
            for a=1:p(2).nant_rx
                v2((slot-1)*p(2).samples_slot+1:slot*p(2).samples_slot,a) = ...
                    v0((slot-1)*samples_slot_agg+p(1).nant_rx*p(1).samples_slot+(a-1)*p(2).samples_slot+1:...
                    (slot-1)*samples_slot_agg+p(1).nant_rx*p(1).samples_slot+ a   *p(2).samples_slot,1);
            end
        end
    end
    
    
    v1 = [vStorage1; v1] ;
    
    
    if size(v1,1) > p(1).frame_length*nframes ;
        nframes = floor(size(v1,1) / p(1).frame_length) ;
        vStorage1 = v1(p(1).frame_length*nframes+1:end,:) ;
        v1(p(1).frame_length*nframes + 1 : end,:) = [] ;
        start = 1 ;
    end
    
    if n_carriers==2
        v2 = [vStorage2; v2] ;
        if size(v2,1) > p(2).frame_length*nframes ;
            nframes = floor(size(v2,1) / p(2).frame_length) ;
            vStorage2 = v2(p(2).frame_length*nframes+1:end,:) ;
            v2(p(2).frame_length*nframes + 1 : end,:) = [] ;
            start = 1 ;
        end
    end
    
    if enable_plots>=2
        if(n_carriers==1)
            figure(1)
            title('');
            plot(20*log10(abs(fftshift(fft(v1)))))
        end
        if(n_carriers==2)
            figure(1)
            subplot(1,2,1);
            plot(20*log10(abs(fftshift(fft(v1)))))
            subplot(1,2,2);
            plot(20*log10(abs(fftshift(fft(v2)))))
        end
    end
    
    %% frame start detection
    
    
    if flag1==1
        [corr,lag] = xcorr(v1(:,1),pss_t);
%         if(n_carriers==2)
%             [corrb,lagb] = xcorr(v2(:,1),pss_t);
%         end
        
        %[m,idx]=max(abs(corr));
        %[m,idx]=peaksfinder(corr,frame_length);
        
        tmp   = corr(nframes*slots_per_frame*p(1).samples_slot:end);
        tmp2  = reshape(tmp,slots_per_frame*p(1).samples_slot,nframes);
        [m,idx] = max(abs(tmp2),[],1);
        
%         if(n_carriers==2)
%             tmp   = corrb(nframes*slots_per_frame*p(2).samples_slot:end);
%             tmp2  = reshape(tmp,slots_per_frame*p(2).samples_slot,nframes);
%             [m,idx] = max(abs(tmp2),[],1);
%         end
        
        idx(m < threshold) = [];
        if size(idx,2) <= 3
            flag1 = 1 ;
            flag2 = 0 ;
            
            vStorage1 = [];
            vStorage2 = [];
            %         elseif size(idx,2) == nframes
            %
            %             flag1 = 0;
            %             flag2 = 1;
        else
            flag1 = 0 ;
            flag2 = 1 ;
            syncblock=block;
        end
        
        frame_offset = round(median(idx)) - p(1).prefix_length;
%         if(n_carriers==2)
%             frame_offset = round(median(idx)) - p(2).prefix_length;
%         end
        
        
        if enable_plots>=2
            
            figure(2)
            hold off
            plot(lag,abs(corr));
            hold on
            plot(frame_offset,m(1),'ro')
        end
    else
        frame_offset = 0;
    end
    
    %%
    
    if flag2 == 1
        
        H1a=[];
        if(n_carriers==2)
            H1b=[];
        end
        fma=0;%maximum of the doppler spectrum
        sa=0;
        if(n_carriers==2)
            fmb=0;%maximum of the doppler spectrum
            sb=0;
        end
        max2=0;
        if(n_carriers==1)
            fm1=0;%First maximum of the doppler spectrum
            s1=0;
            fm2=0;%Second maximum of the doppler spectrum
            s2=0;
            max1=0;%variable containing a maximum
        end
        
        H_dpss1 = zeros(nframes*num_symbols_frame/2,p(1).useful_carriers,p(1).nant_tx,p(1).nant_rx);
        if (n_carriers==2)
            H_dpss2 = zeros(nframes*num_symbols_frame/2,p(2).useful_carriers,p(2).nant_tx,p(2).nant_rx);
        end
        for i=start:nframes;
            
            fprintf(1,'.');
            frame_start1 = (slots_per_frame*p(1).samples_slot)*(i-1)+frame_offset+1;
            
            if n_carriers==2
                frame_start2 = (slots_per_frame*p(2).samples_slot)*(i-1)+round(frame_offset/2)+1;
            end
            
            if i<nframes
                %% ofdm receiver
                received_f1 = OFDM_RX(v1(frame_start1:frame_start1+p(1).frame_length,:),p(1).num_carriers,p(1).useful_carriers,p(1).prefix_length,num_symbols_frame);
                if n_carriers==2
                    received_f2 = OFDM_RX(v2(frame_start2:frame_start2+p(2).frame_length,:),p(2).num_carriers,p(2).useful_carriers,p(2).prefix_length,num_symbols_frame);
                end
            else
                vStorage1 = [v1(frame_start1:end,:) ; vStorage1];  %%
                
                if n_carriers==2
                    vStorage2 = [v2(frame_start2:end,:) ; vStorage2];  %%
                end
                %break
            end
            %disp(i);
            
            %% noise estimation
            noise1((block-1)*NFRAMES+i,:) = squeeze(10*log10(mean(abs(received_f1(1,61:end-60,:)).^2)));
            if (n_carriers==2)
                noise2((block-1)*NFRAMES+i,:) = squeeze(10*log10(mean(abs(received_f2(1,61:end-60,:)).^2)));
            end
            
        
            
            %% MIMO channel estimation
            if (n_carriers==1)
                transmit_f1 = f3;
            else
                transmit_f1 = f1;
                transmit_f2 = f2;
            end
            for carrier=1:n_carriers
                if (carrier==1)
                    transmit_f = transmit_f1;
                    received_f = received_f1;
                else
                    transmit_f = transmit_f2;
                    received_f = received_f2;
                end
                
                % standard channel estimation
                H = zeros(num_symbols_frame/2,p(carrier).useful_carriers/4,p(carrier).nant_tx,p(carrier).nant_rx);
                H_dpss = zeros(num_symbols_frame/2,p(carrier).useful_carriers/4,p(carrier).nant_tx,p(carrier).nant_rx);
                for itx=1:p(carrier).nant_tx
                    % f_start and t_start indicate the start of the pilots in time
                    % and frequency according to the specifications (see .doc file).
                    % t_start has to be >=2, since the first symbol is the PSS.
                    f_start = mod(itx-1,2)*2+1;
                    t_start = floor((itx-1)/2)+1;
                    for irx=1:p(carrier).nant_rx
                        H(:,:,itx,irx)=conj(squeeze(transmit_f(itx,t_start:2:end,f_start:4:end))).*received_f(t_start:2:end,f_start:4:end,irx);
                    end
                end
                
                if (carrier==1)
                    H_power1((block-1)*NFRAMES+i,:,:) = 10*log10(mean(mean(abs(H).^2,1),2));
                else
                    H_power2((block-1)*NFRAMES+i,:,:) = 10*log10(mean(mean(abs(H).^2,1),2));
                end
                
                %% dpss channel estimation
                snr = zeros(size(H_power1));
                for itx=1:p(carrier).nant_tx
                    f_start = mod(itx-1,2)*2+1;
                    for irx=1:p(carrier).nant_rx
                        snr((block-1)*NFRAMES+i,itx,irx)=squeeze(H_power1((block-1)*NFRAMES+i,itx,irx))-noise1((block-1)*NFRAMES+i,irx);
                        snr_rounded = min(max(round(snr((block-1)*NFRAMES+i,itx,irx)),0),30);
                        for it=1:num_symbols_frame/2
                            if (carrier==1)
                                H_dpss1((i-1)*num_symbols_frame/2+it,:,itx,irx) = dpss_smooth(H(it,:,itx,irx),p_dpss(carrier).V,p_dpss(carrier).Dopt(snr_rounded+1),f_start);
                            else
                                H_dpss2((i-1)*num_symbols_frame/2+it,:,itx,irx) = dpss_smooth(H(it,:,itx,irx),p_dpss(carrier).V,p_dpss(carrier).Dopt(snr_rounded+1),f_start);
                            end
                        end
                    end
                end
                
               
                
                %% compute delay Doppler power profiles 
                Ht = ifft(H,[],2)*sqrt(size(H,2)); %impulse response
                PDP = mean(abs(Ht).^2,1);
                PDP_all = squeeze(mean(mean(PDP,3),4));
                PDD=sum(abs(fftshift(fft(Ht,[],1)/sqrt(size(Ht,1)))).^2,2);
                
                if(carrier==1)
                    PDP_totala((block-1)*NFRAMES+i,:,:,:) = PDP;
                    Ha=H;
                end
                
                if(carrier==2)
                    PDP_totalb((block-1)*NFRAMES+i,:,:,:) = PDP;
                    Hb=H;
                end
                
                %                 % frequency offset correction
                %                 Hprime=H*exp(2*i*pi*167E-6*f_offset);
                %                 Htprime = ifft(Hprime,[],2); %impulse response
                %                 PDPprime = mean(abs(Htprime).^2,1);
                %
                %                 PDDprime=sum(abs(fftshift(fft(Htprime,[],1))).^2,2);
                %                 if(carrier==1)
                %                     PDP_totala((block-1)*NFRAMES+i,:,:,:) = PDP;
                %                     Ha=H;
                %                 end
                %
                %                 if(carrier==2)
                %                     PDP_totalb((block-1)*NFRAMES+i,:,:,:) = PDP;
                %                     Hb=H;
                %                 end
                
                if enable_plots>=1
                    figure(3+3*(carrier-1))
                    for itx=1:p(carrier).nant_tx
                        for irx=1:p(1).nant_rx
                            
                            subplot(p(1).nant_tx,p(1).nant_rx,(itx-1)*p(1).nant_rx + irx);
                            surf(20*log10(abs(Ht(:,:,itx,irx))))
                            ylabel('time [OFDM symbol]')
                            xlabel('delay time [samples]')
                            zlabel('power [dB]')
                            shading interp
                        end
                    end
                    figure(4+3*(carrier-1))
                    for itx=1:p(1).nant_tx
                        for irx=1:p(1).nant_rx
                            subplot(p(1).nant_tx,p(1).nant_rx,(itx-1)*p(1).nant_rx + irx);
                            plot(10*log10(PDP(:,:,itx,irx)))
                            ylim([50 80])
                            xlim([0 75])
                            xlabel('delay time [samples]')
                            ylabel('power [dB]')
                        end
                    end
                    figure(5+3*(carrier-1))
                    for itx=1:p(1).nant_tx
                        for irx=1:p(1).nant_rx
                            if(n_carriers==1)
                                F=-(num_symbols_frame/2-1)*7.68E6/(2*num_symbols_frame/2)/1280:7.68E6/(num_symbols_frame/2)/1280:(num_symbols_frame/2)*7.68E6/(2*num_symbols_frame/2)/1280;
                            end
                            
                            if(n_carriers==2)
                                if(carrier==1)
                                    F=-(num_symbols_frame/2-1)*30.72E6/(2*num_symbols_frame/2)/5120:30.72E6/(num_symbols_frame/2)/5120:(num_symbols_frame/2)*30.72E6/(2*num_symbols_frame/2)/5120;
                                end
                                if(carrier==2)
                                    F=-(num_symbols_frame/2-1)*15.36E6/(2*num_symbols_frame/2)/2560:15.36E6/(num_symbols_frame/2)/2560:(num_symbols_frame/2)*15.36E6/(2*num_symbols_frame/2)/2560;
                                end
                                
                            end
                            subplot(p(1).nant_tx,p(1).nant_rx,(itx-1)*p(1).nant_rx + irx);
                            plot(F,10*log10(PDD(:,:,itx,irx)))
                            %ylim([])
                            %xlim([])
                            xlabel('F=f-ftx [Hz]')
                            ylabel('power [dB]')
                        end
                    end
                    drawnow
                end
                
                
                if(n_carriers==1)
                    if carrier==1
                        % adjust frame offset base on channel estimate to compensate for
                        % timing drift. We try to keep the peak of the impulse response at
                        % sample prefix_length/8.
                        [m,idx] = max(fft(ifft(PDP_all),p(carrier).num_carriers));
                        offset = idx - p(carrier).prefix_length/8;
                        if offset > p(carrier).prefix_length
                            offset = offset - p(carrier).num_carriers;
                        end
                        if abs(offset) > 5
                            frame_offset = frame_offset + round(offset/4);
                        end
                    end
                end
                
                if(n_carriers==2)
                    
                    if carrier==2
                        % adjust frame offset base on channel estimate to compensate for
                        % timing drift. We try to keep the peak of the impulse response at
                        % sample prefix_length/8.
                        [m,idx] = max(fft(ifft(PDP_all),p(carrier).num_carriers));
                        offset = idx - p(carrier).prefix_length/8;
                        if offset > p(carrier).prefix_length
                            offset = offset - p(carrier).num_carriers;
                        end
                        if abs(offset) > 5
                            frame_offset = frame_offset + round(offset/4);
                        end
                    end
                    
                end
                
                
            end
            H1a=cat(1,H1a,Ha);
            
            if(n_carriers==2)
                H1b=cat(1,H1b,Hb);
            end
        end
        
        %save postprocessed channels
        for carrier=1:n_carriers
                varname1 = sprintf('H_dpss%d',carrier);
                varname2 = sprintf('H_dpss_block%d',block);
                eval([varname2 '='  varname1 ';']);
                filename_H = sprintf('_H_dpss_carrier%d.mat',carrier);
                if exist(fullfile(destdir,[filename filename_H]),'file')
                    save(fullfile(destdir,[filename filename_H]),varname2,'-append');
                else
                    save(fullfile(destdir,[filename filename_H]),varname2);
                end
                clear(varname2);
        end
        
        Ht1a=ifft(H1a,[],2);
        
        PDD1a=sum(abs(fftshift(fft(Ht1a,[],1))).^2,2);
        delayPDD1a=mean(mean(abs(fftshift(fft(Ht1a,[],1))).^2,3),4);
        if(n_carriers==2)
            Ht1b=ifft(H1b,[],2);
            PDD1b=sum(abs(fftshift(fft(Ht1b,[],1))).^2,2);
            delayPDD1b=mean(mean(abs(fftshift(fft(Ht1b,[],1))).^2,3),4);
        end
        
        if(enable_plots>=2)
            figure(9)
            for itx=1:p(1).nant_tx
                for irx=1:p(1).nant_rx
                    F=-(NFRAMES*num_symbols_frame/2-1)*7.68E6/(2*NFRAMES*num_symbols_frame/2)/1280:7.68E6/(NFRAMES*num_symbols_frame/2)/1280:(NFRAMES*num_symbols_frame/2-1)*7.68E6/(2*NFRAMES*num_symbols_frame/2)/1280;
                    if(n_carriers==2)
                        F=-(NFRAMES*num_symbols_frame/2-1)*30.72E6/(2*NFRAMES*num_symbols_frame/2)/5120:30.72E6/(NFRAMES*num_symbols_frame/2)/5120:(NFRAMES*num_symbols_frame/2)*30.72E6/(2*NFRAMES*num_symbols_frame/2)/5120;
                    end
                    
                    subplot(p(1).nant_tx,p(1).nant_rx,(itx-1)*p(1).nant_rx + irx);
                    title(sprintf('Doppler Spectrum for UHF-Trial %d-Run %d-Block %d ',n_trials,n_run,block));
                    if n_carriers==2
                        title(sprintf('Doppler Spectrum for 2.6GHz Carrier 1-Trial %d-Run %d-Block %d ',n_trials,n_run,block));
                    end
                    
                    plot(F,10*log10(PDD1a(:,:,itx,irx)))
                    
                    
                    xlabel('F=f-ftx [Hz]')
                    ylabel('power [dB]')
                end
            end
            
            if(n_carriers==2)
                figure(10)
                for itx=1:p(1).nant_tx
                    for irx=1:p(1).nant_rx
                        F=-(NFRAMES*num_symbols_frame/2-1)*15.36E6/(2*NFRAMES*num_symbols_frame/2)/2560:15.36E6/(NFRAMES*num_symbols_frame/2)/2560:(NFRAMES*num_symbols_frame/2)*15.36E6/(2*NFRAMES*num_symbols_frame/2)/2560;
                        
                        subplot(p(2).nant_tx,p(2).nant_rx,(itx-1)*p(2).nant_rx + irx);
                        title(sprintf('Doppler Spectrum for 2.6GHz Carrier 2-Trial %d-Run %d-Block %d ',n_trials,n_run,block));
                        plot(F,10*log10(PDD1b(:,:,itx,irx)))
                        
                        
                        xlabel('F=f-ftx [Hz]')
                        ylabel('power [dB]')
                    end
                end
            end
        end
        
        if record==1
            ha=figure(20);
            set(gca,'nextplot','replacechildren');
            set(gcf,'Renderer','zbuffer');
            
            tau=linspace(0,p(1).useful_carriers/4/4.5E6,p(1).useful_carriers/4);
            F=-(NFRAMES*num_symbols_frame/2-1)*7.68E6/(2*NFRAMES*num_symbols_frame/2)/1280:7.68E6/(NFRAMES*num_symbols_frame/2)/1280:(NFRAMES*num_symbols_frame/2-1)*7.68E6/(2*NFRAMES*num_symbols_frame/2)/1280;
            if(n_carriers==2)
                tau=linspace(0,p(1).useful_carriers/4/18E6,p(1).useful_carriers/4);
                F=-(NFRAMES*num_symbols_frame/2-1)*30.72E6/(2*NFRAMES*num_symbols_frame/2)/5120:30.72E6/(NFRAMES*num_symbols_frame/2)/5120:(NFRAMES*num_symbols_frame/2)*30.72E6/(2*NFRAMES*num_symbols_frame/2)/5120;
            end
            title(sprintf('Delay Doppler Spectrum for UHF-Trial %d-Run %d-Block %d ',n_trials,n_run,block));
            if(n_carriers==2)
                title(sprintf('Delay Doppler Spectrum for 2.6GHz Carrier 1-Trial %d-Run %d-Block %d ',n_trials,n_run,block));
            end
            pcolor(tau,F,10*log10(delayPDD1a(:,:)))
            shading flat
            bara=colorbar;
            xlabel('delay [s]')
            ylabel('Doppler shift [Hz]')
            
            framea = getframe(ha);
            writeVideo(delay_doppler_profile_videoa,framea);
            
            
            if(n_carriers==2)
                hb=figure(21);
                set(gca,'nextplot','replacechildren');
                set(gcf,'Renderer','zbuffer');
                tau=linspace(0,p(2).useful_carriers/4/9E6,p(2).useful_carriers/4);
                F=-(NFRAMES*num_symbols_frame/2-1)*15.36E6/(2*NFRAMES*num_symbols_frame/2)/2560:15.36E6/(NFRAMES*num_symbols_frame/2)/2560:(NFRAMES*num_symbols_frame/2)*15.36E6/(2*NFRAMES*num_symbols_frame/2)/2560;
                title(sprintf('Delay Doppler Spectrum for 2.6GHz Carrier 2-Trial %d-Run %d-Block %d ',n_trials,n_run,block));
                pcolor(tau,F,10*log10(delayPDD1b(:,:)))
                shading flat
                barb=colorbar;
                xlabel('delay [s]')
                ylabel('Doppler shift [Hz]')
                
                frameb = getframe(hb);
                writeVideo(delay_doppler_profile_videob,frameb);
                
            end
            
        end
        
        
        PDD_totala(:,block,:,:)=PDD1a;
        if(block==block_before)
            delay_doppler_profile_beforea=delayPDD1a;
            
        end
        
        if(block==block_during)
            delay_doppler_profile_duringa=delayPDD1a;
            
        end
        
        if(block==block_after)
            delay_doppler_profile_aftera=delayPDD1a;
            
        end
        
        
        if(n_carriers==2)
            PDD_totalb(:,block,:,:)=PDD1b;
            if(block==block_before)
                delay_doppler_profile_beforeb=delayPDD1b;
                
            end
            
            if(block==block_during)
                delay_doppler_profile_duringb=delayPDD1b;
                
            end
            
            if(block==block_after)
                delay_doppler_profile_afterb=delayPDD1b;
                
            end
        end
        
        
        
        
        %%
        
        for itx=1:p(1).nant_tx
            for irx=1:p(1).nant_rx
                for i=1:NFRAMES*num_symbols_frame/2
                    if(10*log10(PDD1a(i,:,itx,irx))>max2)
                        max2=10*log10(PDD1a(i,:,itx,irx));
                        fma=i;
                    end
                end
                sa=sa+fma;
            end
        end
        sa=sa/(p(1).nant_tx*p(1).nant_rx)-2999.5;
        doppler_freq_of_max_a(block)=sa;
        max2=0;
        if(n_carriers==2)
            for itx=1:p(2).nant_tx
                for irx=1:p(2).nant_rx
                    for i=1:NFRAMES*num_symbols_frame/2
                        if(10*log10(PDD1a(i,:,itx,irx))>max2)
                            max2=10*log10(PDD1a(i,:,itx,irx));
                            fmb=i;
                        end
                    end
                    sb=sb+fmb;
                end
            end
            sb=sb/(p(2).nant_tx*p(2).nant_rx)-2999.5;
            doppler_freq_of_max_b(block)=sb;
            
        end
        
        
        %% Doppler shift, tgv speed and frequency offset for trial1 UHF run1
        if(n_carriers==1)
            if(n_trials==1)
                for itx=1:p(1).nant_tx
                    for irx=1:p(1).nant_rx
                        for i=1940:1960
                            if(10*log10(PDD1a(i,:,itx,irx))>max1)
                                max1=10*log10(PDD1a(i,:,itx,irx));
                                fm1=i;
                            end
                        end
                        s1=s1+fm1;
                    end
                end
                s1=s1/(p(1).nant_tx*p(1).nant_rx);
                
                for itx=1:p(1).nant_tx
                    for irx=1:p(1).nant_rx
                        for i=2340:2370
                            if(10*log10(PDD1a(i,:,itx,irx))>max1)
                                max1=10*log10(PDD1a(i,:,itx,irx));
                                fm2=i;
                            end
                        end
                        s2=s2+fm2;
                    end
                end
                s2=s2/(p(1).nant_tx*p(1).nant_rx);
                
                fm=(s2-s1)/2;
                if(abs(300-fm*3/7.7715*3.6)<50)
                    fm_total(block)=fm;
                    TGVspeed_total(block)=fm*3/7.7715*3.6;
                    freqOffset_total(block)=abs((s1+fm)-3000.5);
                end
                
                
            end
        end
        
        
        
        
        
        
        
    end
    
    
    
    fprintf(1,'\n');
    
    block = block+1;
    
    if (size(vStorage1,1)>=p(1).frame_length)
        
        nframes=NFRAMES-floor((size(vStorage1,1))/(p(1).frame_length));
        
    else
        nframes=NFRAMES;
    end
    
end

close(delay_doppler_profile_videoa);

if n_carriers==2
    close(delay_doppler_profile_videob);
end


%%

if(enable_plots>=2)
    figure(11)
    for itx=1:p(1).nant_tx
        for irx=1:p(1).nant_rx
            T=1:1:block-1;
            F=-(NFRAMES*num_symbols_frame/2-1)*7.68E6/(2*NFRAMES*num_symbols_frame/2)/1280:7.68E6/(NFRAMES*num_symbols_frame/2)/1280:(NFRAMES*num_symbols_frame/2-1)*7.68E6/(2*NFRAMES*num_symbols_frame/2)/1280;
            if(n_carriers==2)
                F=-(NFRAMES*num_symbols_frame/2-1)*30.72E6/(2*NFRAMES*num_symbols_frame/2)/5120:30.72E6/(NFRAMES*num_symbols_frame/2)/5120:(NFRAMES*num_symbols_frame/2)*30.72E6/(2*NFRAMES*num_symbols_frame/2)/5120;
            end
            subplot(p(1).nant_tx,p(1).nant_rx,(itx-1)*p(1).nant_rx + irx);
            title(sprintf('Doppler spectrum UHF Trial %d-Run %d',n_trials,n_run));
            if n_carriers==2
                title(sprintf('Doppler spectrum 2.6GHz Carrier 1 Trial %d-Run %d',n_trials,n_run));
            end
            pcolor(T,F,10*log10( PDD_totala(:,:,itx,irx)));
            shading flat
            bara=colorbar;
            
            ylabel(bara, 'dBm')
            ylabel('F=f-ftx [Hz]')
            xlabel('time [s]')
        end
    end
    
    if(n_carriers==2)
        figure(12)
        for itx=1:p(2).nant_tx
            for irx=1:p(2).nant_rx
                T=1:1:block-1;
                F=-(NFRAMES*num_symbols_frame/2-1)*15.36E6/(2*NFRAMES*num_symbols_frame/2)/2560:15.36E6/(NFRAMES*num_symbols_frame/2)/2560:(NFRAMES*num_symbols_frame/2)*15.36E6/(2*NFRAMES*num_symbols_frame/2)/2560;
                subplot(p(2).nant_tx,p(2).nant_rx,(itx-1)*p(2).nant_rx + irx);
                title(sprintf('Doppler spectrum 2.6GHz Carrier 2 Trial %d-Run %d',n_trials,n_run));
                pcolor(T,F,10*log10( PDD_totalb(:,:,itx,irx)));
                shading flat
                barb=colorbar;
                
                ylabel(barb, 'dBm')
                ylabel('F=f-ftx [Hz]')
                xlabel('time [s]')
            end
        end
    end
end

%%

%% Mean Delay

Pma=zeros((block-1)*NFRAMES,1,p(1).nant_tx,p(1).nant_rx);% zeroth-order moment
Pma1=zeros((block-1)*NFRAMES,1,p(1).nant_tx,p(1).nant_rx);
atau=linspace(0,p(1).useful_carriers/4/4.5E6,p(1).useful_carriers/4);
if(n_carriers==2)
    atau=linspace(0,p(1).useful_carriers/4/18E6,p(1).useful_carriers/4);
end
for i=1:p(1).useful_carriers/4
    Pma(:,1,:,:)=Pma(:,1,:,:)+PDP_totala(:,i,:,:);
    Pma1(:,1,:,:)=Pma1(:,1,:,:)+atau(i)*PDP_totala(:,i,:,:);
end

mean_delay_a=Pma1./Pma;% mean delay: first-order moment


if(n_carriers==2)
    Pmb=zeros((block-1)*NFRAMES,1,p(2).nant_tx,p(2).nant_rx);
    Pmb1=zeros((block-1)*NFRAMES,1,p(2).nant_tx,p(2).nant_rx);
    btau=linspace(0,p(2).useful_carriers/4/9E6,p(2).useful_carriers/4);
    for i=1:p(2).useful_carriers/4
        Pmb(:,1,:,:)=Pmb(:,1,:,:)+PDP_totalb(:,i,:,:);
        Pmb1(:,1,:,:)=Pmb1(:,1,:,:)+btau(i)*PDP_totalb(:,i,:,:);
    end
    mean_delay_b=Pmb1./Pmb;
end

figure(13)
for itx=1:p(1).nant_tx
    for irx=1:p(1).nant_rx
        
        subplot(p(1).nant_tx,p(1).nant_rx,(itx-1)*p(1).nant_rx + irx);
        title(sprintf('Mean Delay UHF Trial %d-Run %d',n_trials,n_run));
        if n_carriers==2
            title(sprintf('Mean Delay 2.6GHz Carrier 1 Trial %d-Run %d',n_trials,n_run));
        end
        plot(mean_delay_a(:,:,itx,irx));
        ylabel('delay [s]')
        xlabel('time [s]')
        
    end
end

if (n_carriers==2)
    figure(14)
    for itx=1:p(2).nant_tx
        for irx=1:p(2).nant_rx
            
            subplot(p(2).nant_tx,p(2).nant_rx,(itx-1)*p(2).nant_rx + irx);
            title(sprintf('Mean Delay 2.6GHz Carrier 2 Trial %d-Run %d',n_trials,n_run));
            plot(mean_delay_b(:,:,itx,irx));
            ylabel('delay [s]')
            xlabel('time [s]')
        end
    end
end

%% Mean Doppler Shift

PDma=zeros(block-1,1,p(1).nant_tx,p(1).nant_rx);
PDma1=zeros(block-1,1,p(1).nant_tx,p(1).nant_rx);
theta=linspace(-NFRAMES*num_symbols_frame/2/2,NFRAMES*num_symbols_frame/2/2,NFRAMES*num_symbols_frame/2);

for j=1:block-1
    
    for i=1:NFRAMES*num_symbols_frame/2
        PDma(j,1,:,:)=PDma(j,1,:,:)+PDD_totala(i,j,:,:);
        PDma1(j,1,:,:)=PDma1(j,1,:,:)+theta(i)*PDD_totala(i,j,:,:);
    end
end

mean_doppler_shift_a=PDma1./PDma; % mean doppler shift


if(n_carriers==2)
    PDmb=zeros(block-1,1,p(2).nant_tx,p(2).nant_rx);
    PDmb1=zeros(block-1,1,p(2).nant_tx,p(2).nant_rx);
    for j=1:block-1
        for i=1:NFRAMES*num_symbols_frame/2
            PDmb(j,1,:,:)=PDmb(j,1,:,:)+PDD_totalb(i,j,:,:);
            PDmb1(j,1,:,:)=PDmb1(j,1,:,:)+theta(i)*PDD_totalb(i,j,:,:);
        end
    end
    mean_doppler_shift_b=PDmb1./PDmb;
end

figure(15)
for itx=1:p(1).nant_tx
    for irx=1:p(1).nant_rx
        
        subplot(p(1).nant_tx,p(1).nant_rx,(itx-1)*p(1).nant_rx + irx);
        title(sprintf('Mean Doppler shift UHF Trial %d-Run %d',n_trials,n_run));
        if n_carriers==2
            title(sprintf('Mean Doppler shift 2.6GHz Carrier 1 Trial %d-Run %d',n_trials,n_run));
        end
        plot(mean_doppler_shift_a(:,:,itx,irx));
        ylabel('f-ftx [Hz]')
        xlabel('time [s]')
    end
end

if (n_carriers==2)
    figure(16)
    for itx=1:p(2).nant_tx
        for irx=1:p(2).nant_rx
            
            subplot(p(2).nant_tx,p(2).nant_rx,(itx-1)*p(2).nant_rx + irx);
            title(sprintf('Mean Doppler shift 2.6GHz Carrier 2 Trial %d-Run %d',n_trials,n_run));
            plot(mean_doppler_shift_b(:,:,itx,irx));
            ylabel('f-ftx [Hz]')
            xlabel('time [s]')
        end
    end
end
%%
figure(17)

plot(doppler_freq_of_max_a);
title(sprintf('Main Doppler peak for UHF Trial %d-Run%d',n_trials,n_run));
if n_carriers==2
    title(sprintf('Main Doppler peak for 2.6GHz Carrier 1 Trial %d-Run%d',n_trials,n_run));
end
xlabel('time [s]');
ylabel('f-ftx [Hz]');

if(n_carriers==2)
    figure(18)
    
    plot(doppler_freq_of_max_b);
    title(sprintf('Main Doppler peak for 2.6GHz Carrier 2 Trial %d-Run%d',n_trials,n_run));
    xlabel('time [s]');
    ylabel('f-ftx [Hz]');
end

%%

if(n_carriers==1)
    if(n_trials==1)
        subplot(2,2,1);
        title('variation of the mean fdop');
        plot(fm_total);
        xlabel('time [s]');
        ylabel('fdop [Hz]');
        
        subplot(2,2,2);
        title('variation of the TGV speed');
        plot(TGVspeed_total);
        xlabel('time [s]');
        ylabel('TGV speed [km/h]');
        
        subplot(2,2,3);
        title('variation of the mean frequency offset');
        plot(freqOffset_total);
        xlabel('time [s]');
        ylabel('frequency Offset [Hz]');
    end
end


fclose(fid);

%% save processed data
if(n_carriers==1)
    save(fullfile(destdir,[filename '.mat']),'PDP_totala','PDD_totala','mean_delay_a','mean_doppler_shift_a','doppler_freq_of_max_a','delay_doppler_profile_beforea','delay_doppler_profile_duringa','delay_doppler_profile_aftera','noise1','H_power1');
    
end
if(n_carriers==2)
    save(fullfile(destdir,[filename '.mat']),'PDP_totala','PDD_totala','mean_delay_a','mean_doppler_shift_a','doppler_freq_of_max_a','delay_doppler_profile_beforea','delay_doppler_profile_duringa','delay_doppler_profile_aftera','PDP_totalb','PDD_totalb','mean_delay_b','mean_doppler_shift_b','doppler_freq_of_max_b','delay_doppler_profile_beforeb','delay_doppler_profile_duringb','delay_doppler_profile_afterb','noise1','H_power1','noise2','H_power2');
    
end