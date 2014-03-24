close all
clear all

enable_plots=2; %enables figures

%% preload and init data
addpath('../../../openair1/PHY/LTE_REFSIG');
primary_synch; %loads the primary sync signal
pss1_t = upsample(primary_synch0_time,4*4);
pss2_t = upsample(primary_synch0_time,4*2);

%load('E:\EMOS\corridor\ofdm_pilots_sync_2048_v7.mat');
load('ofdm_pilots_sync_30MHz.mat');

%filename = 'E:\EMOS\corridor\eNB_data_20140319_133327.EMOS';
filename = 'D:\2.6GHz\eNB_data_20140324_171904.EMOS';

nb_rb1 = 100; %this can be 25, 50, or 100
num_carriers1 = 2048/100*nb_rb1;
num_zeros1 = num_carriers1-(12*nb_rb1+1);
prefix_length1 = num_carriers1/4; %this is extended CP
ofdm_symbol_length1 = num_carriers1 + prefix_length1;
frame_length1 = ofdm_symbol_length1*num_symbols_frame;
useful_carriers1 = num_carriers1-num_zeros1-1;

nb_rb2 = 50; %this can be 25, 50, or 100
num_carriers2 = 2048/100*nb_rb2;
num_zeros2 = num_carriers2-(12*nb_rb2+1);
prefix_length2 = num_carriers2/4; %this is extended CP
ofdm_symbol_length2 = num_carriers2 + prefix_length2;
frame_length2 = ofdm_symbol_length2*num_symbols_frame;
useful_carriers2 = num_carriers2-num_zeros2-1;

nant1 = 2;
nant2 = 2;
samples_slot1 = 7680*2;
samples_slot2 = 7680;
samples_slot_agg = nant1*samples_slot1 + nant2*samples_slot2;
nframes = 50;
slots_per_frame = 20;

d = dir(filename);
nblocks = floor(d.bytes/(samples_slot_agg*slots_per_frame*nframes*4));
PDP1_total = zeros(nblocks*nframes,useful_carriers1/4);
PDP2_total = zeros(nblocks*nframes,useful_carriers2/4);

%% main loop
fid = fopen(filename,'r');
block = 1;
while ~feof(fid)
    fprintf(1,'Processing block %d of %d',block,nblocks);
    
    %%
    [v,c]=fread(fid, 2*samples_slot_agg*slots_per_frame*nframes, 'int16',0,'ieee-le');
    block = block+1;   
    if (c==0)
        break
    end
    v0 = double(v(1:2:end,:))+1j*double(v(2:2:end,:));

    v1 = zeros(samples_slot1*slots_per_frame*nframes,nant1);
    v2 = zeros(samples_slot2*slots_per_frame*nframes,nant2);
    for slot=1:slots_per_frame*nframes
        for a1=1:nant1
            v1((slot-1)*samples_slot1+1:slot*samples_slot1,a1) = ...
                v0((slot-1)*samples_slot_agg+(a1-1)*samples_slot1+1:...
                   (slot-1)*samples_slot_agg+ a1   *samples_slot1,1);
        end
        for a2=1:nant2
            v2((slot-1)*samples_slot2+1:slot*samples_slot2,a2) = ...
                v0((slot-1)*samples_slot_agg+nant1*samples_slot1+(a2-1)*samples_slot2+1:...
                   (slot-1)*samples_slot_agg+nant1*samples_slot1+ a2   *samples_slot2,1);
        end
    end
    
    if enable_plots>=2
        figure(1)
        plot(20*log10(abs(fftshift(fft(v1)))))
        
        figure(2)
        plot(20*log10(abs(fftshift(fft(v2)))))
    end
    
    %% frame start detection
    [corr1,lag1] = xcorr(v1(:,1),pss1_t);
    [corr2,lag2] = xcorr(v2(:,1),pss2_t);
    %[m,idx]=max(abs(corr));
    [m1,idx1]=peaksfinder(corr1,frame_length1);
    [m2,idx2]=peaksfinder(corr2,frame_length2);

    if (enable_plots>=2)
        figure(20);
        hold off
        plot(lag1,abs(corr1));
        hold on
        plot(lag1(idx1),m1,'ro')
        figure(21);
        hold off
        plot(lag2,abs(corr2));
        hold on
        plot(lag2(idx2),m2,'ro')
    end
    
    %%
    for i=1:size(idx1,2)-1; % the last frame is not complite
        fprintf(1,'.');
        %frame_start2 = lag(i) - prefix_length2;
        frame_start1 = lag1(idx1(i))-prefix_length1;
        frame_start2 = lag2(idx2(i))-prefix_length2;
        %frame_start1 = frame_start2*2;
        
        % ofdm receiver
        received_f1 = OFDM_RX(v1(frame_start1:frame_start1+frame_length1,:),num_carriers1,useful_carriers1,prefix_length1,num_symbols_frame);
        received_f2 = OFDM_RX(v2(frame_start2:frame_start2+frame_length2,:),num_carriers2,useful_carriers2,prefix_length2,num_symbols_frame);
        
        % channel estimation (SISO)
        H1=conj(squeeze(f1(1,3:2:end,1:4:end))).*received_f1(3:2:end,1:4:end,1);
        H2=conj(squeeze(f2(1,3:2:end,1:4:end))).*received_f2(3:2:end,1:4:end,1);
        H1t = ifft(H1,[],2);
        H2t = ifft(H2,[],2);
        PDP1 = mean(abs(H1t).^2,1);
        PDP2 = mean(abs(H2t).^2,1);
        PDP1_total((block-1)*nframes+i+1,:) = PDP1;
        PDP2_total((block-1)*nframes+i+1,:) = PDP2;
        
        if enable_plots>=1
            figure(3)
            surf((abs(H1t)))
            xlabel('time [OFDM symbol]')
            ylabel('delay time [samples]')
            zlabel('power [dB]')
            title('H1t')
            shading interp
            figure(4)
            plot(10*log10(PDP1))
            xlabel('delay time [samples]')
            ylabel('power [dB]')
            title('PDP1')
            figure(30)
            surf((abs(H2t)))
            xlabel('time [OFDM symbol]')
            ylabel('delay time [samples]')
            zlabel('power [dB]')
            title('H2t')
            shading interp
            figure(40)
            plot(10*log10(PDP2))
            xlabel('delay time [samples]')
            ylabel('power [dB]')
            title('PDP2')
        end
        
    end
    fprintf(1,'\n');
end

fclose(fid);
