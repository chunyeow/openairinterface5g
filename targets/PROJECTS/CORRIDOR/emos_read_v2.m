close all
clear all

global symbols_per_slot slots_per_frame;

enable_plots=2; %enables figures

%% preload and init data
addpath('../../../openair1/PHY/LTE_REFSIG');
primary_synch; %loads the primary sync signal

%load('E:\EMOS\corridor\ofdm_pilots_sync_2048_v7.mat');
load('ofdm_pilots_sync_30MHz.mat');

n_carriers = 2; % use 1 for UHF and 2 for 2.6GHz
symbols_per_slot = 6;
slots_per_frame = 20;

switch n_carriers
    case 1,
        p = init_params(25,3,4); %this can be 25, 50, or 100
        pss_t = upsample(primary_synch0_time,4);
        
        %filename = 'E:\EMOS\corridor\trials1\eNB_data_20140331_UHF_run1.EMOS';
        filename = 'E:\EMOS\corridor\trials1\eNB_data_20140331_UHF_run1.EMOS';
        
        nframes = 100; % frames in one block
        threshold = 3e+4 ; % maybe should change that !!!!
    case 2,
        p(1) = init_params(100,2,4);
        p(2) = init_params(50,2,4);
        pss_t = upsample(primary_synch0_time,4*4); % this assumes we are doing the sync on the first carrier, which is 10MHz
        
        %filename = 'E:\EMOS\corridor\trials1\eNB_data_20140331_UHF_run1.EMOS';
        %filename = 'E:\EMOS\corridor\trials1\eNB_data_20140331_2.6GHz_run1.EMOS';
        filename = 'E:\EMOS\corridor\trials2\eNB_data_20140519_2.6GHz_run2.EMOS';
        
        nframes = 50; % frames in one block
        threshold = 3e+4 ; % maybe should change that !!!!
end

destdir = 'E:\EMOS\corridor\trials1 train';

% derived parameters
samples_slot_agg = sum([p.nant_rx].*[p.samples_slot]);
num_symbols_frame = symbols_per_slot*slots_per_frame;

d = dir(filename);
nblocks = floor(d.bytes/(samples_slot_agg*slots_per_frame*nframes*4));
PDP_total = zeros(nblocks*nframes,p(1).useful_carriers/4,p(1).nant_tx,p(1).nant_rx);

%% main loop
fid = fopen(filename,'r');

vStorage1 = [];
vStorage2 = [];

block = 1;
flag1 = 1;
start=2;

%fseek(fid,samples_slot_agg*slots_per_frame*nframes*120*2,'bof'); %advance 30 sec

while ~feof(fid)
    fprintf(1,'Processing block %d of %d',block,nblocks);
    
    [v,c]=fread(fid, 2*samples_slot_agg*slots_per_frame*nframes, 'int16',0,'ieee-le');
    if (c==0)
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
        figure(1)
        plot(20*log10(abs(fftshift(fft(v1)))))
    end
    
    %% frame start detection
    if flag1==1
        [corr,lag] = xcorr(v1(:,1),pss_t);
        %[m,idx]=max(abs(corr));
        %[m,idx]=peaksfinder(corr,frame_length);
        
        tmp   = corr(nframes*slots_per_frame*p(1).samples_slot:end);
        tmp2  = reshape(tmp,slots_per_frame*p(1).samples_slot,nframes);
        [m,idx] = max(abs(tmp2),[],1);
        
        %         meanCorr = mean(abs(tmp2));
        %         [mm,where] = max(m./meanCorr)
        
        idx(m < threshold) = [];
        if size(idx,2) <= 2
            flag1 = 1 ;
            flag2 = 0 ;
            
            vStorage1 = [];
            %         elseif size(idx,2) == nframes
            %
            %             flag1 = 0;
            %             flag2 = 1;
        else
            flag1 = 0 ;
            flag2 = 1 ;
        end
        
        frame_offset = round(median(idx)) - p(1).prefix_length;
        
        
        if enable_plots>=2
            figure(2);
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
                break
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
                
                
                H = zeros(num_symbols_frame/2,p(carrier).useful_carriers/4,p(carrier).nant_tx,p(carrier).nant_rx);
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
                Ht = ifft(H,[],2);
                PDP = mean(abs(Ht).^2,1);
                PDP_all = squeeze(mean(mean(PDP,3),4));
                %PDP_total((block-1)*nframes+i+1,:,:,:) = PDP;
                
                if enable_plots>=1
                    figure(3+2*(carrier-1))
                    for itx=1:p(carrier).nant_tx
                        for irx=1:p(1).nant_rx
                            subplot(p(1).nant_tx,p(1).nant_rx,(itx-1)*p(1).nant_rx + irx);
                            surf(20*log10(abs(Ht(:,:,itx,irx))))
                            %xlabel('time [OFDM symbol]')
                            %ylabel('delay time [samples]')
                            %zlabel('power [dB]')
                            shading interp
                        end
                    end
                    figure(4+2*(carrier-1))
                    for itx=1:p(1).nant_tx
                        for irx=1:p(1).nant_rx
                            subplot(p(1).nant_tx,p(1).nant_rx,(itx-1)*p(1).nant_rx + irx);
                            plot(10*log10(PDP(:,:,itx,irx)))
                            ylim([50 80])
                            xlim([0 75])
                            %xlabel('delay time [samples]')
                            %ylabel('power [dB]')
                        end
                    end
                    drawnow
                end
                
                if carrier==1
                    % adjust frame offset base on channel estimate to compensate for
                    % timing drift. We try to keep the peak of the impulse response at
                    % sample prefix_length/8.
                    [m,idx] = max(fft(ifft(PDP_all),p(carrier).num_carriers));
                    offset = idx - p(carrier).prefix_length/8
                    if offset > p(carrier).prefix_length
                        offset = offset - p(carrier).num_carriers;
                    end
                    if abs(offset) > 5
                        frame_offset = frame_offset + round(offset/4);
                    end
                end
            end
            
        end
    end
    fprintf(1,'\n');
    block = block+1;
end

fclose(fid);

%% save processed data
[path,name,ext]=fileparts(filename);
save([destdir filesep name '.mat'],'PDP_total');