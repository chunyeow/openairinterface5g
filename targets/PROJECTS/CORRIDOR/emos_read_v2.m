close all
clear all

enable_plots=1; %eanbles figures

%% preload and init data
addpath('../../../openair1/PHY/LTE_REFSIG');
primary_synch; %loads the primary sync signal
pss_t = upsample(primary_synch0_time,4);

%load('E:\EMOS\corridor\ofdm_pilots_sync_2048_v7.mat');
load('ofdm_pilots_sync_30MHz.mat');

ofdm_symbol_length = num_carriers + prefix_length;
frame_length = ofdm_symbol_length*num_symbols_frame;
useful_carriers = num_carriers-num_zeros-1;

%filename = 'E:\EMOS\corridor\lab tests\eNB_data_20140321_184441.EMOS';
filename = 'D:\trials1 train extracted\eNB_data_20140331_UHF_run1.EMOS';
destdir = 'E:\EMOS\corridor\trials1 train';

samples_slot = 7680/2;
slots_per_frame = 20;
nframes = 100;
nant_rx=3;
nant_tx=4;

d = dir(filename);
nblocks = floor(d.bytes/(samples_slot*slots_per_frame*nframes*nant_rx*4));
PDP_total = zeros(nblocks*nframes,useful_carriers/4,nant_tx,nant_rx);

%% main loop
fid = fopen(filename,'r');
% advance 1 minute of measurements (the synchronization algorithm does not
% work so well for low SNR yet)
fseek(fid,samples_slot*slots_per_frame*nframes*nant_rx*60*2,'bof'); 

vStorage = []; 
block = 1;

while ~feof(fid)
    fprintf(1,'Processing block %d of %d',block,nblocks);
    
    [v,c]=fread(fid, samples_slot*slots_per_frame*nframes*nant_rx*2, 'int16',0,'ieee-le');
    if (c==0)
        break
    end
    v1 = double(v(1:2:end))+1j*double(v(2:2:end));
    
    v2 = zeros(samples_slot*slots_per_frame*nframes,nant_rx);
    for slot=1:slots_per_frame*nframes
        for a=1:nant_rx
            v2((slot-1)*samples_slot+1:slot*samples_slot,a) = ...
                v1((slot-1)*samples_slot*nant_rx+(a-1)*samples_slot+1:...
                   (slot-1)*samples_slot*nant_rx+ a   *samples_slot,1);
        end
    end

    
    v2 = [vStorage; v2] ; %handle boundaries between blocks
    if size(v2,1) > frame_length*nframes ;
        nframes = floor(size(v2,1) / frame_length) ;
    vStorage = v2(frame_length*nframes+1:end,:) ;
    v2(frame_length*nframes + 1 : end,:) = [] ;
    start = 1 ;
    end
    
    if enable_plots>=2
        figure(1)
        plot(20*log10(abs(fftshift(fft(v2)))))
    end
    
    %% frame start detection 
    if block==1 
        [corr,lag] = xcorr(v2(:,1),pss_t);
        %[m,idx]=max(abs(corr));
        %[m,idx]=peaksfinder(corr,frame_length);

        tmp   = corr(nframes*slots_per_frame*samples_slot:end);
        tmp2  = reshape(tmp,slots_per_frame*samples_slot,nframes);
        [m,idx] = max(abs(tmp2),[],1);
        frame_offset = round(mean(idx(1:10))) - prefix_length;

        if enable_plots>=2
            figure(2);
            hold off
            plot(lag,abs(corr));
            hold on
            plot(frame_offset,m(1),'ro')
        end
    else
        % after the first block we are synchronized
        frame_offset = prefix_length/8;
    end
    
    %%
    for i=1:nframes;
        fprintf(1,'.');
        frame_start = (slots_per_frame*samples_slot)*(i-1)+frame_offset+1;
        %frame_start = lag(idx(i))-prefix_length;
        % frame_start = lag(i) - prefix_length;
        
        if i<nframes
            %% ofdm receiver
            received_f = OFDM_RX(v2(frame_start:frame_start+frame_length,:),num_carriers,useful_carriers,prefix_length,num_symbols_frame);
        else
            vStorage = [v2(frame_start:end,:) ; vStorage];  % handle block boundaries
        end
        %% MIMO channel estimation
        H = zeros(num_symbols_frame/2,useful_carriers/4,nant_tx,nant_rx);
        for itx=1:nant_tx
            % f_start and t_start indicate the start of the pilots in time
            % and frequency according to the specifications (see .doc file). 
            % t_start has to be >=2, since the first symbol is the PSS.
            f_start = mod(itx-1,2)*2+1;
            t_start = floor((itx-1)/2)+1;
            for irx=1:nant_rx
                H(:,:,itx,irx)=conj(squeeze(f3(itx,t_start:2:end,f_start:4:end))).*received_f(t_start:2:end,f_start:4:end,irx);
            end
        end
        Ht = ifft(H,[],2);
        PDP = mean(abs(Ht).^2,1);
        PDP_all = squeeze(mean(mean(PDP,3),4));
        PDP_total((block-1)*nframes+i+1,:,:,:) = PDP;
        
        % adjust frame offset base on channel estimate to compensate for
        % timing drift. We try to keep the peak of the impulse response at
        % sample prefix_length/8.
        [m,idx] = max(fft(ifft(PDP_all),num_carriers));
        offset = idx - prefix_length/8;
        if offset > prefix_length
            offset = offset - num_carriers;
        end
        if abs(offset) > 5
            frame_offset = frame_offset + round(offset/4);
        end
        
        if enable_plots>=1
            figure(3)
            for itx=1:nant_tx
                for irx=1:nant_rx
                    subplot(nant_tx,nant_rx,(itx-1)*nant_rx + irx);
                    surf(20*log10(abs(Ht(:,:,itx,irx))))
                    %xlabel('time [OFDM symbol]')
                    %ylabel('delay time [samples]')
                    %zlabel('power [dB]')
                    shading interp
                end
            end
            figure(4)
            for itx=1:nant_tx
                for irx=1:nant_rx
                    subplot(nant_tx,nant_rx,(itx-1)*nant_rx + irx);
                    plot(10*log10(PDP(:,:,itx,irx)))
                    ylim([50 80])
                    xlim([0 75])
                    %xlabel('delay time [samples]')
                    %ylabel('power [dB]')
                end
            end
            drawnow
        end
        
    end
    fprintf(1,'\n');
    block = block+1;
end

fclose(fid);

%% save processed data
[path,name,ext]=fileparts(filename);
save([destdir filesep name '.mat'],'PDP_total');