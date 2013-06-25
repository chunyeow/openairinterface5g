function [H, H_fq, estimates, gps_data, NFrames] = load_estimates2_lm(fid, NTx, NFrames_max, NFrames, version)
% 
% EMOS Single User Import Filter read data in chunks (low memory usage)
%
% [H, H_fq, estimates, gps_data, NFrames] = load_estimates2_lm(fid, NTx, NFrames_max, NFrames, version)
%
% Parameters:
% fid               - file descriptor of EMOS data file. Must be opened in
%                     read mode: fid = fopen('....EMOS','r');
% NTx               - Number of Tx Antenns
% NFrames_max       - Maximum number of estimates. Leave it blank to get up to the
%                     maximum file contents
% Nframes           - Number of frames already read from the file 
%                     (Use return value of previous call)
% version           - for backward compatibility (see details below)
%
% Returns:
% H                 - Time domain channel matrix of size [NRx,NTx,NDelay,NFrames]
%                     The estimates of different users are aligned in time
% H_fq              - Frequency domain channel matrix of size [NRx,NTx,NFreq,NFrames]
% estimates         - A structure array containing timestamp, etc
%                       - perror [48 x NR x NCluster x NFrames] contains
%                         the measured phase drif of the pilot symbols
% gps_data          - A structure array containing gps data
% NFrames           - the number of read estimates

% Author: Florian Kaltenberger 
% Copyright: Eurecom Sophia Antipolis

% Version History
%   Date      Version   Comment
%   20070917  0.1       Created
%   20071116  0.2       Added fields framestamp_tx and crc_status in the
%                       dumpfile. However, the crc_status is not working
%                       yet, so we still use the pdu_errors field to
%                       calculate the err_ind.
%                       Also more than one file can be read now at the same
%                       time.
%   20071121  0.3       crc_status is now giving right values, so we can
%                       use it. However, the framestamp_tx is now buggy.
%                       Therefore we recalculate it here.
%   20071127  0.4       Removed prevoius bugs in dumpfile.
%   20081030  2.0       Adapted for the new two-CH, two-way EMOS structure, 
%                       older versions not supported anymore
%   20090727  2.1       now also reads channel measurements from 2nd CH

if ((nargin == 5) && (version < 2.0))
    error('This function only works for EMOS version 2.0');
end

NRx = 2;
NFreq = 256;
NZFreq = 160;
NCH = 2;

NUMBER_OF_CHSCH_SYMBOLS_MAX = 4;

% Logfile structure: 
%  - 100 entries of type fifo_dump_emos (defined in phy_procedures_emos.h)
%  - 1 entry of type gps_fix_t defined in gps.h
TIMESTAMP_SIZE = 8;
PHY_MEASUREMENTS_SIZE = NRx * NUMBER_OF_CHSCH_SYMBOLS_MAX * 2 * 3 + ...
                        NUMBER_OF_CHSCH_SYMBOLS_MAX * 2 + ...
                        NRx * NUMBER_OF_CHSCH_SYMBOLS_MAX * 4 * 2 + ...
                        9*4;
if (version>=2.1)
    PHY_MEASUREMENTS_SIZE = PHY_MEASUREMENTS_SIZE + 2*4;
end
MAC_PDU_SIZE = 144*2;
if (version>=2.1)
    PDU_ERRORS_SIZE = 8;
else
    PDU_ERRORS_SIZE = 4;
end
OFFSET_SIZE = 4;
RX_TOTAL_GAIN_SIZE = 4;
RX_MODE_SIZE = 4;
CHANNEL_SIZE = 2 * NRx * NFreq * 4;
PERROR_SIZE = 2 * NRx * 48 * 4;
NO_ESTIMATES_DISK = 100;

FIFO_DUMP_EMOS_SIZE = ...
    TIMESTAMP_SIZE + ...
    PHY_MEASUREMENTS_SIZE + ...
    MAC_PDU_SIZE + ...
    PDU_ERRORS_SIZE + ...
    OFFSET_SIZE + ...
    RX_TOTAL_GAIN_SIZE + ...
    CHANNEL_SIZE + ...
    PERROR_SIZE;

if (version>=2.1)
    FIFO_DUMP_EMOS_SIZE = FIFO_DUMP_EMOS_SIZE + RX_MODE_SIZE;
end

DOUBLE_SIZE = 8;
INT_SIZE = 4;
GPS_FIX_T_SIZE = INT_SIZE + 13 * DOUBLE_SIZE;

CHANNEL_BUFFER_SIZE = NO_ESTIMATES_DISK * FIFO_DUMP_EMOS_SIZE + GPS_FIX_T_SIZE;
    
% Preallocate data vectors
chan0 = zeros(NFreq,NFrames_max);
chan1 = zeros(NFreq,NFrames_max);
chan2 = zeros(NFreq,NFrames_max);
chan3 = zeros(NFreq,NFrames_max);

estimates.timestamp = zeros(1,NFrames_max);

estimates.rx_power_dB = zeros(NUMBER_OF_CHSCH_SYMBOLS_MAX,NRx,NFrames_max); 
estimates.rx_avg_power_dB = zeros(NUMBER_OF_CHSCH_SYMBOLS_MAX,NFrames_max);
estimates.n0_power_dB = zeros(NUMBER_OF_CHSCH_SYMBOLS_MAX,NRx,NFrames_max);
estimates.rx_rssi_dBm = zeros(NUMBER_OF_CHSCH_SYMBOLS_MAX,NRx,NFrames_max);
estimates.rx_power = zeros(NUMBER_OF_CHSCH_SYMBOLS_MAX,NRx,NFrames_max);
estimates.n0_power = zeros(NUMBER_OF_CHSCH_SYMBOLS_MAX,NRx,NFrames_max);
if (version >= 2.1)
    estimates.framestamp_tx = zeros(2,NFrames);
else
    estimates.framestamp_tx = zeros(1,NFrames);
end
if (version >= 2.1)
    estimates.crc_status = zeros(2,NFrames);
else
    estimates.crc_status = zeros(1,NFrames);
end

estimates.mac_pdu = char(zeros(MAC_PDU_SIZE,NFrames_max));
estimates.timestamp_tx = zeros(1,NFrames_max);
if (version >= 2.1)
    estimates.pdu_errors = zeros(2,NFrames);
else
    estimates.pdu_errors = zeros(1,NFrames);
end
estimates.offset = zeros(1,NFrames_max);
estimates.total_gain_dB = zeros(NRx,NFrames_max);
estimates.Hnorm = zeros(2,NFrames_max);
estimates.perror = zeros(48,NRx,2,NFrames_max);
if (version >= 2.1)
    estimates.rx_mode = zeros(1,NFrames);
end

% estimates.sch_symbol0 = zeros(NRx,NFrames_max);
% estimates.sch_symbol1 = zeros(NRx,NFrames_max);

gps_data.timestamp = zeros(1,floor(NFrames_max/100));
gps_data.mode = zeros(1,floor(NFrames_max/100));
gps_data.ept = zeros(1,floor(NFrames_max/100));
gps_data.latitude = zeros(1,floor(NFrames_max/100));
gps_data.longitude = zeros(1,floor(NFrames_max/100));
gps_data.rest = zeros(1,floor(NFrames_max/100));
gps_data.eph = zeros(1,floor(NFrames_max/100));
gps_data.altitude = zeros(1,floor(NFrames_max/100));
gps_data.epv = zeros(1,floor(NFrames_max/100));
gps_data.track = zeros(1,floor(NFrames_max/100));
gps_data.epd = zeros(1,floor(NFrames_max/100));
gps_data.speed = zeros(1,floor(NFrames_max/100));
gps_data.eps = zeros(1,floor(NFrames_max/100));
gps_data.climb = zeros(1,floor(NFrames_max/100));
gps_data.epc = zeros(1,floor(NFrames_max/100));

NFrames_start = NFrames;

k = 1;
l = 1;

    while ~feof(fid) && k <= NFrames_max

        count = 0;
        
        % Read Timestamp Data
        [temp,c] = fread(fid,TIMESTAMP_SIZE,'uchar'); % Timestamp variable
        count = count+c;
        tt=0;
        temp=temp/256;
        for k2=1:TIMESTAMP_SIZE
            tt=(tt+temp(9-k2))*256;
        end
        estimates.timestamp(k)=tt/1e9; % to get the timestamp in sec

        % read PHY_measurements
        %[dummy_phy_measurements,c] = fread(fid,PHY_MEASUREMENTS_SIZE,'uchar');
        %count = count+c;
        
        estimates.rx_power_dB(:,:,k) = reshape(fread(fid,NUMBER_OF_CHSCH_SYMBOLS_MAX*NRx,'ushort'),NRx,NUMBER_OF_CHSCH_SYMBOLS_MAX).';
        estimates.rx_avg_power_dB(:,k) = fread(fid,NUMBER_OF_CHSCH_SYMBOLS_MAX,'short');
        estimates.n0_power_dB(:,:,k) = reshape(fread(fid,NUMBER_OF_CHSCH_SYMBOLS_MAX*NRx,'ushort'),NRx,NUMBER_OF_CHSCH_SYMBOLS_MAX).';
        estimates.rx_rssi_dBm(:,:,k) = reshape(fread(fid,NUMBER_OF_CHSCH_SYMBOLS_MAX*NRx,'short'),NRx,NUMBER_OF_CHSCH_SYMBOLS_MAX).';
        estimates.rx_power(:,:,k) = reshape(fread(fid,NUMBER_OF_CHSCH_SYMBOLS_MAX*NRx,'int'),NRx,NUMBER_OF_CHSCH_SYMBOLS_MAX).';
        estimates.n0_power(:,:,k) = reshape(fread(fid,NUMBER_OF_CHSCH_SYMBOLS_MAX*NRx,'int'),NRx,NUMBER_OF_CHSCH_SYMBOLS_MAX).';
        dummy = fread(fid,7,'int');
        %   unsigned int   chbch_search_count;
        %   unsigned int   chbch_detection_count[4];
        %   unsigned int   mrbch_search_count;
        %   unsigned int   mrbch_detection_count;
        estimates.framestamp_tx(:,k) = fread(fid,size(estimates.framestamp_tx,1),'uint');
        estimates.crc_status(:,k) = fread(fid,size(estimates.framestamp_tx,1),'int');
        
        
        % Read MAC PDU
        [estimates.mac_pdu(:,k),c] = fread(fid,MAC_PDU_SIZE,'uchar');
        count = count+c;
        tt=0;
        temp=estimates.mac_pdu(1:8,k)/256;
        for k2=1:TIMESTAMP_SIZE
            tt=(tt+temp(9-k2))*256;
        end
        estimates.timestamp_tx(k)=tt/1e9; % to get the timestamp in sec
        
        % Read PDU Errors
        [estimates.pdu_errors(:,k),c] = fread(fid,size(estimates.pdu_errors,1),'uint');
        count = count+c*4;
        [estimates.offset(k),c] = fread(fid,1,'uint');
        count = count+c*4;
        [estimates.total_gain_dB(k),c] = fread(fid,1,'uint');
        count = count+c*4;
        if (version >= 2.1)
            [estimates.rx_mode(k),c] = fread(fid,1,'uint');
        end
        

        % Read channel estimates
        [data,c]    = fread(fid,NFreq*NRx*2*2,'short'); % read
        count = count+c*2;
        if (c ~= NFreq*NRx*2*2)
            warning('not all data read')
            break
        end

        % Measurement from CH1
        chan0(:,k) = data(1:2:(NFreq*2)) + j * data(2:2:(NFreq*2));
        chan1(:,k) = data(((NFreq*2)+1):2:(4*NFreq)) + j * data(((NFreq*2)+2):2:(4*NFreq)) ;
        
        % calculate the mean squared Frobenius norm of H
        estimates.Hnorm(1,k) = (sum(abs(chan0(:,k)).^2) + sum(abs(chan1(:,k)).^2))/(NZFreq/NTx);

        % Measurement from CH2
        chan2(:,k) = data(4*NFreq+1:2:(NFreq*6)) + j * data(4*NFreq+2:2:(NFreq*6));
        chan3(:,k) = data(((NFreq*6)+1):2:(8*NFreq)) + j * data(((NFreq*6)+2):2:(8*NFreq)) ;
        
        % calculate the mean squared Frobenius norm of H
        estimates.Hnorm(2,k) = (sum(abs(chan2(:,k)).^2) + sum(abs(chan3(:,k)).^2))/(NZFreq/NTx);

        [dummy_perror,c] = fread(fid,PERROR_SIZE/2,'short');
        dummy_perror = dummy_perror(1:2:end) + 1j*dummy_perror(2:2:end);
        estimates.perror(:,:,:,k) = reshape(dummy_perror,48,NRx,2);
        count = count+c;

        %read GPS data
        if (mod(k,NO_ESTIMATES_DISK)==0)
            gps_data.timestamp(l) = fread(fid,1,'double');
            gps_data.mode(l)      = fread(fid,1,'int');
            gps_data.ept(l)       = fread(fid,1,'double');
            gps_data.latitude(l)  = fread(fid,1,'double');
            gps_data.longitude(l) = fread(fid,1,'double');
            gps_data.eph(l)       = fread(fid,1,'double');
            gps_data.altitude(l)  = fread(fid,1,'double');
            gps_data.epv(l)       = fread(fid,1,'double');
            gps_data.track(l)     = fread(fid,1,'double');
            gps_data.epd(l)       = fread(fid,1,'double');
            gps_data.speed(l)     = fread(fid,1,'double');
            gps_data.eps(l)       = fread(fid,1,'double');
            gps_data.climb(l)     = fread(fid,1,'double');
            gps_data.epc(l)       = fread(fid,1,'double');

            l=l+1;
            %estimates.gps_data(k) = gps_data;
        end

        k=k+1;
        NFrames = NFrames+1;
        
    end

    NFrames_read = NFrames - NFrames_start;

    H_fq = complex(zeros(NRx,NTx,NFreq/NTx,NFrames_read,NCH));
    H_fq(1,:,:,:,1) = reshape(chan0(:,1:NFrames_read),NTx,NFreq/NTx,NFrames_read);
    H_fq(2,:,:,:,1) = reshape(chan1(:,1:NFrames_read),NTx,NFreq/NTx,NFrames_read);
    H_fq(1,:,:,:,2) = reshape(chan2(:,1:NFrames_read),NTx,NFreq/NTx,NFrames_read);
    H_fq(2,:,:,:,2) = reshape(chan3(:,1:NFrames_read),NTx,NFreq/NTx,NFrames_read);
    
    H = ifft(H_fq,[],3);
    
    % remove zero carriers from Frequency response
    NZFreq_ind = [(176/NTx+1):(256/NTx) 1:(80/NTx)]; % Non-zero frequency indices 
    H_fq = H_fq(:,:,NZFreq_ind,:,:);

