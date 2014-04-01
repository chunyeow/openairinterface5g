close all
clear all

enable_plots=2; %eanbles figures

%% preload and init data
addpath('../../../openair1/PHY/LTE_REFSIG');
primary_synch; %loads the primary sync signal
pss_t = upsample(primary_synch0_time,4);

%load('E:\EMOS\corridor\ofdm_pilots_sync_2048_v7.mat');
load('ofdm_pilots_sync_30MHz.mat');

ofdm_symbol_length = num_carriers + prefix_length;
frame_length = ofdm_symbol_length*num_symbols_frame;
useful_carriers = num_carriers-num_zeros-1;

filename = 'E:\EMOS\corridor\lab tests\eNB_data_20140321_184441.EMOS';
%filename = 'D:\711MHz\eNB_data_20140324_113931.EMOS';
%filename = 'D:\trials1 day 1 771.5MHz\eNB_data_20140327_153151.EMOS';
destdir = 'E:\EMOS\corridor\trials1 day1\711.5MHz';

samples_slot = 7680/2;
slots_per_frame = 20;
nframes = 100;
nant=3;

d = dir(filename);
nblocks = floor(d.bytes/(samples_slot*slots_per_frame*nframes*nant*4));
PDP_total = zeros(nblocks*nframes,useful_carriers/4);

%% main loop
fid = fopen(filename,'r');
block = 1;
while ~feof(fid)
    fprintf(1,'Processing block %d of %d',block,nblocks);
    
    [v,c]=fread(fid, samples_slot*slots_per_frame*nframes*nant*2, 'int16',0,'ieee-le');
    if (c==0)
        break
    end
    v1 = double(v(1:2:end))+1j*double(v(2:2:end));
    
    v2 = zeros(samples_slot*slots_per_frame*nframes,nant);
    for slot=1:slots_per_frame*nframes
        for a=1:nant
            v2((slot-1)*samples_slot+1:slot*samples_slot,a) = ...
                v1((slot-1)*samples_slot*nant+(a-1)*samples_slot+1:...
                   (slot-1)*samples_slot*nant+ a   *samples_slot,1);
        end
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
        frame_offset = round(mean(idx)) - prefix_length;

        if enable_plots>=2
            figure(2);
            hold off
            plot(lag,abs(corr));
            hold on
            plot(frame_offset,m(1),'ro')
        end
    
        start=2;
    else
        start=1;
    end
    
    for i=start:nframes;
        fprintf(1,'.');
        frame_start = (slots_per_frame*samples_slot)*(i-1)+frame_offset+1;
        %frame_start = lag(idx(i))-prefix_length;
        % frame_start = lag(i) - prefix_length;
        
        %% ofdm receiver
        received_f = OFDM_RX(v2(frame_start:frame_start+frame_length,:),num_carriers,useful_carriers,prefix_length,num_symbols_frame);
        
        %% channel estimation
        H=conj(squeeze(f3(1,3:2:end,1:4:end))).*received_f(3:2:end,1:4:end,1);
        Ht = ifft(H,[],2);
        PDP = mean(abs(Ht).^2,1);
        PDP_total((block-1)*nframes+i+1,:) = PDP;
        
        [m,idx] = max(fft(ifft(PDP),num_carriers));
        offset = idx - prefix_length/8;
        if offset > prefix_length
            offset = offset - num_carriers;
        end
        if abs(offset) > 5
            frame_offset = frame_offset + offset;
        end
        
        if enable_plots>=1
            figure(3)
            surf(20*log10(abs(Ht)))
            xlabel('time [OFDM symbol]')
            ylabel('delay time [samples]')
            zlabel('power [dB]')
            shading interp
            figure(4)
            plot(10*log10(PDP))
            xlabel('delay time [samples]')
            ylabel('power [dB]')
        end
        
    end
    fprintf(1,'\n');
    block = block+1;
end

fclose(fid);

%% save processed data
[path,name,ext]=fileparts(filename);
save([destdir filesep name '.mat'],'PDP_total');