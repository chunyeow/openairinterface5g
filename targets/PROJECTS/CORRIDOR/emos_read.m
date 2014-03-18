close all
clear all

enable_plots=1; %eanbles figures

%% preload and init data
addpath('../../../openair1/PHY/LTE_REFSIG');
primary_synch; %loads the primary sync signal
pss_t = upsample(primary_synch0_time,4*4);

%load('../LOCALIZATION/ofdm_pilots_sync_2048.mat');
load('E:\EMOS\corridor\ofdm_pilots_sync_2048_v7.mat');
ofdm_symbol_length = num_carriers + prefix_length;
frame_length = ofdm_symbol_length*num_symbols_frame;
useful_carriers = num_carriers-num_zeros-1;

filename = 'E:\EMOS\corridor\eNB_data_20140108_185919.EMOS';
samples_slot = 7680*2;
slots_per_frame = 20;
nframes = 100;

d = dir(filename);
nblocks = floor(d.bytes/(samples_slot*slots_per_frame*nframes*4));
PDP_total = zeros(nblocks*nframes,useful_carriers);

%% main loop
fid = fopen(filename,'r');
block = 1;
while ~feof(fid)
    fprintf(1,'Processing block %d of %d',block,nblocks);
    
    [v,c]=fread(fid, samples_slot*slots_per_frame*nframes*2, 'int16',0,'ieee-le');
    v2 = double(v(1:2:end))+1j*double(v(2:2:end));
    
    [corr,lag] = xcorr(v2,pss_t);

    if enable_plots>=2
        figure(1)
        plot(abs(fftshift(fft(v2))))

        figure(2);
        plot(lag,abs(corr));
    end
    
    %% frame start detection
    %[m,i]=max(abs(corr));
    [m,idx]=peaksfinder(corr,frame_length);
    
    for i=1:size(idx,2)-1; % the last frame is not complite
        fprintf(1,'.');
        frame_start = lag(idx(i))-prefix_length;
        % frame_start = lag(i) - prefix_length;
        
        %% ofdm receiver
        received = v2(frame_start:frame_start+frame_length);
        received_f = zeros(num_symbols_frame,useful_carriers);
        for j=0:num_symbols_frame-1;
            ifblock=received(j*ofdm_symbol_length+(1:ofdm_symbol_length));
            ifblock(1:prefix_length)=[];
            fblock=fft(ifblock);
            received_f(j+1,:) = [fblock(2:useful_carriers/2+1); fblock(end-useful_carriers/2+1:end)];
        end
        
        %% channel estimation
        H=conj(f).*received_f;
        Ht = ifft(H,[],2);
        PDP = mean(abs(Ht(2:end,:).^2),1);
        PDP_total((block-1)*nframes+i+1,:) = PDP;
        
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
