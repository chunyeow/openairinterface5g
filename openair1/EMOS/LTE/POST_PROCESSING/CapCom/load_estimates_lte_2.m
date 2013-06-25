function [H, H_fq, gps_data, NFrames, minestimates, throughput, SNR] = load_estimates_lte_2(filename, NFrames_max, decimation, is_eNb,mumimo_on, version)
%
% EMOS Single User Import Filter
%
% [H, H_fq, estimates, gps_data, NFrames] =
%       load_estimates_lte(filename, NFrames_max, version)
%
% Parameters:
% filename          - filename(s) of the EMOS data file
% NFrames_max       - Maximum number of estimates. Leave it blank to get up to the
%                     maximum file contents
% decimation        - read every 'decimation' frame
% is_eNb            - if ~= 0 we load data from an eNb
% version           - for backward compatibility (see details below)
%
% Returns:
% estimates         - A structure array containing timestamp, etc
% gps_data          - A structure array containing gps data
% NFrames           - the number of read estimates

% Author: Florian Kaltenberger, Imran Latif
% Copyright: Eurecom Sophia Antipolis

% Version History
%   Date      Version   Comment
%   20100317  0.1       Created based on load_estimates

if nargin < 5
    version = Inf;
end
if nargin < 4
    is_eNb = 0;
end
if nargin < 3
    decimation = 1;
end
if nargin < 2
    NFrames_max = Inf;
end

% NTx = 2;
% NRx = 2;
% NFreq = 512;
% NZFreq = 300;

% Logfile structure:
%  - 100 entries of type fifo_dump_emos (defined in phy_procedures_emos.h)
%  - 1 entry of type gps_fix_t defined in gps.h

if exist('../../IMPORT_FILTER/a.out','file')
    [dummy,result] = system('../../IMPORT_FILTER/a.out');
    eval(result);
else
    warning('File dump_size.c has to be compiled to enable error checking of sizes');
    %PHY_measurements_size = 1120;
    %UCI_data_t_size = 49;
    %DCI_alloc_t_size = 16;
    %eNb_UE_stats_size = 20;
    %fifo_dump_emos_UE_size = 33492;
    %fifo_dump_emos_eNb_size = 36980;
    %gps_fix_t_size = 108;
end

struct_template;

% if (version < 1)
%     fifo_dump_emos_struct_eNb = fifo_dump_emos_struct_eNb_old;
%     fifo_dump_emos_struct_eNb_a = fifo_dump_emos_struct_eNb_old_a;
% end

gps_fix_t_size = gps_data_struct_a.size;
fifo_dump_emos_UE_size = fifo_dump_emos_struct_UE_a.size;
fifo_dump_emos_eNb_size = fifo_dump_emos_struct_eNb_a.size;


NO_ESTIMATES_DISK = 100;
if (is_eNb)
    CHANNEL_BUFFER_SIZE = NO_ESTIMATES_DISK * fifo_dump_emos_eNb_size + gps_fix_t_size;
else
    CHANNEL_BUFFER_SIZE = NO_ESTIMATES_DISK * fifo_dump_emos_UE_size + gps_fix_t_size;
end

if (mod(NO_ESTIMATES_DISK,decimation) ~= 0)
    error('Decimation must be a divisor of %d',NO_ESTIMATES_DISK);
end

% Estimate the size of the file for a pre-allocation of memory
if ~iscell(filename)
    filename = {filename};
end
NFiles = length(filename);
NFrames_file = zeros(1,NFiles);
for n=1:NFiles
    if ~exist(filename{n},'file')
        error('File does not exist!')
    end
    info_file = dir(filename{n});
    NFrames_file(n) = floor(info_file.bytes/CHANNEL_BUFFER_SIZE)*NO_ESTIMATES_DISK;
    if (mod(info_file.bytes,CHANNEL_BUFFER_SIZE) ~= 0)
        warning('File size not a multiple of buffer size. File might be corrupt or is_eNb flag is wrong.');
    end
end
NFrames = min(min(NFrames_file), NFrames_max);

if (is_eNb)
    estimates_tmp = repmat(fifo_dump_emos_struct_eNb,NFiles,100);
    
    Ratepersec_4Qam_SISO_1Rx = zeros(1,floor(NFrames/100));
    Ratepersec_16Qam_SISO_1Rx = zeros(1,floor(NFrames/100));
    Ratepersec_64Qam_SISO_1Rx = zeros(1,floor(NFrames/100));
    
    Ratepersec_4Qam_SISO_2Rx = zeros(1,floor(NFrames/100));
    Ratepersec_16Qam_SISO_2Rx = zeros(1,floor(NFrames/100));
    Ratepersec_64Qam_SISO_2Rx = zeros(1,floor(NFrames/100));
    
    siso_SNR_1Rx = zeros(1,floor(NFrames/100));
    siso_SNR_2Rx = zeros(1,floor(NFrames/100));
    
    minestimates = repmat(min_estimates_struct_eNb, NFiles, NFrames);
else
    estimates_tmp = repmat(fifo_dump_emos_struct_UE,NFiles,100);
    
%%     Ratepersec_4Qam_alamouti = zeros(1,floor(NFrames/100));
%     Ratepersec_4Qam_siso = zeros(1,floor(NFrames/100));
%     Ratepersec_16Qam_alamouti = zeros(1,floor(NFrames/100));
%     Ratepersec_16Qam_siso = zeros(1,floor(NFrames/100));
%     Ratepersec_64Qam_alamouti = zeros(1,floor(NFrames/100));
%     Ratepersec_64Qam_siso = zeros(1,floor(NFrames/100));
     Ratepersec_4Qam_beamforming = zeros(1,floor(NFrames/100));
%    Ratepersec_4Qam_MUMIMO_ue1 = zeros(1,floor(NFrames/100));
%    Ratepersec_4Qam_MUMIMO_ue2 = zeros(1,floor(NFrames/100));
         Ratepersec_16Qam_beamforming = zeros(1,floor(NFrames/100));
    Ratepersec_64Qam_beamforming = zeros(1,floor(NFrames/100));
    %     Ratepersec_supportedQam_MUMIMO_1stRx = zeros(1,floor(NFrames/100));
    %
    %     Ratepersec_4Qam_MUMIMO_2ndRx = zeros(1,floor(NFrames/100));
    %     Ratepersec_16Qam_MUMIMO_2ndRx = zeros(1,floor(NFrames/100));
    %     Ratepersec_64Qam_MUMIMO_2ndRx = zeros(1,floor(NFrames/100));
    %     Ratepersec_supportedQam_MUMIMO_2ndRx = zeros(1,floor(NFrames/100));
    %
    %     Ratepersec_4Qam_MUMIMO_2Rx = zeros(1,floor(NFrames/100));
    %     Ratepersec_16Qam_MUMIMO_2Rx = zeros(1,floor(NFrames/100));
    %     Ratepersec_64Qam_MUMIMO_2Rx = zeros(1,floor(NFrames/100));
    %     Ratepersec_supportedQam_MUMIMO_2Rx = zeros(1,floor(NFrames/100));
    
    SNR=[];
    
    minestimates = repmat(min_estimates_struct, NFiles, NFrames);
    
end
gps_data = repmat(gps_data_struct,NFiles,NFrames/100);

k = 1;
l = 1;
sec = 1;
count =1;
fid = zeros(1,NFiles);
for n=1:NFiles
    fid(n) = fopen(filename{n},'r');
end

while (any(~feof_vec(fid)) && (k <= min([NFrames_file,NFrames_max])))
 %while (sec<70)   
    for n=1:NFiles
        %H = complex(zeros(2,2,2,200)); % NUser * NTx x NRx * Nsymb
        
        if (is_eNb)
            estimates_tmp(n,k) = binread(fid(n),fifo_dump_emos_struct_eNb,1,4,'l');
            
            minestimates(n,k).mcs = get_mcs(estimates_tmp(n,k).dci_alloc(10,1).dci_pdu,'format0');
            minestimates(n,k).tbs = get_tbs(minestimates(k).mcs,25);
            minestimates(n,k).rx_rssi_dBm = estimates_tmp(n,k).phy_measurements_eNb(1).rx_rssi_dBm(1);
            minestimates(n,k).frame_tx = estimates_tmp(n,k).frame_tx;
            minestimates(n,k).timestamp = estimates_tmp(n,k).timestamp;
            if (version<1)
                minestimates(n,k).UE_mode = estimates_tmp(n,k).eNb_UE_stats(3,1).UE_mode;
            else
                minestimates(n,k).UE_mode = estimates_tmp(n,k).eNb_UE_stats(1,1).UE_mode;
            end
            minestimates(n,k).phy_measurements = estimates_tmp(n,k).phy_measurements_eNb(1);
            minestimates(n,k).ulsch_errors = estimates_tmp(n,k).ulsch_errors;
            minestimates(n,k).mimo_mode = estimates_tmp(n,k).mimo_mode;
            minestimates(n,k).eNb_id = estimates_tmp(n,k).eNb_UE_stats.sector;
            
        else
            estimates_tmp(n,count) = binread(fid(n),fifo_dump_emos_struct_UE,1,4,'l');
            
            minestimates(n,k).mcs = get_mcs(estimates_tmp(n,count).dci_alloc(7,1).dci_pdu);
            minestimates(n,k).tbs = get_tbs(minestimates(n,k).mcs,25);
            minestimates(n,k).rx_rssi_dBm = estimates_tmp(n,count).phy_measurements(1).rx_rssi_dBm(1);
            minestimates(n,k).frame_tx = estimates_tmp(n,count).frame_tx;
            minestimates(n,k).frame_rx = estimates_tmp(n,count).frame_rx;
            minestimates(n,k).pbch_fer = estimates_tmp(n,count).pbch_fer(1);
            minestimates(n,k).timestamp = estimates_tmp(n,count).timestamp;
            minestimates(n,k).UE_mode = estimates_tmp(n,count).UE_mode;
            minestimates(n,k).phy_measurements = estimates_tmp(n,count).phy_measurements(1);
            minestimates(n,k).dlsch_fer = estimates_tmp(n,count).dlsch_fer;
            minestimates(n,k).dlsch_errors = estimates_tmp(n,count).dlsch_errors;
            minestimates(n,k).mimo_mode = estimates_tmp(n,count).mimo_mode;
            minestimates(n,k).eNb_id = estimates_tmp(n,count).eNb_id;
            
            
        end % if (is_eNb)
        
        %read GPS data and estimates every second
        if ((mod(k,NO_ESTIMATES_DISK)==0) && ~any(feof_vec(fid)))
            
            
            gps_data(n,l) = binread(fid(n),gps_data_struct,1,4,'l');
        end
        %k=k+1;
    end %NFiles
    
    
    if ((mod(k,NO_ESTIMATES_DISK)==0) && ~any(feof_vec(fid)))
        
        % do MU-MIMO processing
         [Ratepersec_4Qam_beamforming(sec),Ratepersec_16Qam_beamforming(sec),...
          Ratepersec_64Qam_beamforming(sec)] = calc_rps_mu_mimo(estimates_tmp,mumimo_on);
        %plot(Ratepersec_4Qam_MUMIMO(1:sec),'r-x');
%        [Ratepersec_4Qam_alamouti(sec),Ratepersec_16Qam_alamouti(sec),Ratepersec_64Qam_alamouti(sec)] = calc_rps_alamouti_2(estimates_tmp);
%        [Ratepersec_4Qam_siso(sec),Ratepersec_16Qam_siso(sec),Ratepersec_64Qam_siso(sec)] = calc_rps_siso_2(estimates_tmp);
     
        l=l+1;
        sec = sec+1
        count = 0;
    end
    count = count+1;
    k=k+1;
    
end %while

for n=1:NFiles
    fclose(fid(n));
end

H=[];
H_fq=[];

if (is_eNb)
    throughput.rateps_SISO_4Qam_eNB1_1Rx = Ratepersec_4Qam_SISO_1Rx;
    throughput.rateps_SISO_16Qam_eNB1_1Rx = Ratepersec_16Qam_SISO_1Rx;
    throughput.rateps_SISO_64Qam_eNB1_1Rx = Ratepersec_64Qam_SISO_1Rx;
    throughput.rateps_SISO_4Qam_eNB1_2Rx = Ratepersec_4Qam_SISO_2Rx;
    throughput.rateps_SISO_16Qam_eNB1_2Rx = Ratepersec_16Qam_SISO_2Rx;
    throughput.rateps_SISO_64Qam_eNB1_2Rx = Ratepersec_64Qam_SISO_2Rx;
    SNR.siso_1Rx = siso_SNR_1Rx;
    SNR.siso_2Rx = siso_SNR_2Rx;
else
    
%     throughput.rateps_alamouti_4Qam = Ratepersec_4Qam_alamouti;
%     throughput.rateps_siso_4Qam = Ratepersec_4Qam_siso;
%     throughput.rateps_alamouti_16Qam = Ratepersec_16Qam_alamouti;
%     throughput.rateps_siso_16Qam = Ratepersec_16Qam_siso;
%     throughput.rateps_alamouti_64Qam = Ratepersec_64Qam_alamouti;
%     throughput.rateps_siso_64Qam = Ratepersec_64Qam_siso;
%     throughput.rateps_MUMIMO_4Qam_eNB1 = Ratepersec_4Qam_MUMIMO;
%     throughput.rateps_MUMIMO_4Qam_eNB1_ue1 = Ratepersec_4Qam_MUMIMO_ue1;
%     throughput.rateps_MUMIMO_4Qam_eNB1_ue2 = Ratepersec_4Qam_MUMIMO_ue2;
     throughput.rateps_bmfr_4Qam = Ratepersec_4Qam_beamforming;
    throughput.rateps_bmfr_16Qam = Ratepersec_16Qam_beamforming;
    throughput.rateps_bmfr_64Qam = Ratepersec_64Qam_beamforming;
    %     throughput.rateps_MUMIMO_16Qam_eNB1_1stRx = Ratepersec_16Qam_MUMIMO_1stRx;
    %     throughput.rateps_MUMIMO_64Qam_eNB1_1stRx = Ratepersec_64Qam_MUMIMO_1stRx;
    %     throughput.rateps_MUMIMO_supportedQam_eNB1_1stRx = Ratepersec_supportedQam_MUMIMO_1stRx;
    %
    %     throughput.rateps_MUMIMO_4Qam_eNB1_2ndRx = Ratepersec_4Qam_MUMIMO_2ndRx;
    %     throughput.rateps_MUMIMO_16Qam_eNB1_2ndRx = Ratepersec_16Qam_MUMIMO_2ndRx;
    %     throughput.rateps_MUMIMO_64Qam_eNB1_2ndRx = Ratepersec_64Qam_MUMIMO_2ndRx;
    %     throughput.rateps_MUMIMO_supportedQam_eNB1_2ndRx = Ratepersec_supportedQam_MUMIMO_2ndRx;
    %
    %     throughput.rateps_MUMIMO_4Qam_eNB1_2Rx = Ratepersec_4Qam_MUMIMO_2Rx;
    %     throughput.rateps_MUMIMO_16Qam_eNB1_2Rx = Ratepersec_16Qam_MUMIMO_2Rx;
    %     throughput.rateps_MUMIMO_64Qam_eNB1_2Rx = Ratepersec_64Qam_MUMIMO_2Rx;
    %     throughput.rateps_MUMIMO_supportedQam_eNB1_2Rx = Ratepersec_supportedQam_MUMIMO_2Rx;
end
end


function eof = feof_vec(fid)
eof = false(size(fid));
for i=1:length(fid)
    eof(i) = feof(fid(i));
end
end
