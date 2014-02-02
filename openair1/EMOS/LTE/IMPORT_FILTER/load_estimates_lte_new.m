function [estimates, NFrames, gps_data, H] = load_estimates_lte_new(filename, NFrames_max, decimation, is_eNb, EMOS_CHANNEL)
% 
% EMOS Single User Import Filter
%
% [estimates, NFrames] = 
%       load_estimates_lte(filename, NFrames_max, decimation, is_eNb, EMOS_CHANNEL)
%
% Parameters:
% filename          - filename(s) of the EMOS data file
% NFrames_max       - Maximum number of estimates. Leave it blank to get up to the
%                     maximum file contents
% decimation        - read every 'decimation' frame
% is_eNb            - if ~= 0 we load data from an eNb
% EMOS_CHANNEL      - if the measurement contains the full channel
%                     estimate, set this to 1
%
% Returns:
% estimates         - A structure array containing timestamp, etc
% gps_data          - A structure array containing gps data
% NFrames           - the number of read estimates

% Author: Florian Kaltenberger 
% Copyright: Eurecom Sophia Antipolis

% Version History
%   Date      Version   Comment
%   20100317  0.1       Created based on load_estimates

if nargin < 5
    EMOS_CHANNEL = 0;
end
if nargin < 4
    is_eNb = 1;
end
if nargin < 3
    decimation = 1;
end
if nargin < 2
    NFrames_max = Inf;
end

% Logfile structure: 
%  - 100 entries of type fifo_dump_emos (defined in phy_procedures_emos.h)

if exist('dump_size','file') && isunix
    [dummy,result] = system('./dump_size');
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

struct_template_new;

gps_fix_t_size = gps_data_struct_a.size;
fifo_dump_emos_UE_size = fifo_dump_emos_struct_UE_a.size;
fifo_dump_emos_eNb_size = fifo_dump_emos_struct_eNb_a.size;


NO_ESTIMATES_DISK = 100;
if (is_eNb)
    CHANNEL_BUFFER_SIZE = NO_ESTIMATES_DISK * fifo_dump_emos_eNb_size + gps_fix_t_size;
else
    CHANNEL_BUFFER_SIZE = NO_ESTIMATES_DISK * fifo_dump_emos_UE_size  + gps_fix_t_size;
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
NFrames = min(sum(NFrames_file), NFrames_max);

if (is_eNb)
    estimates = repmat(fifo_dump_emos_struct_eNb,1,floor(NFrames/decimation));
    if nargout==4
        H = complex(zeros(floor(NFrames/decimation),size(fifo_dump_emos_struct_eNb.channel,1)/2,size(fifo_dump_emos_struct_eNb.channel,2)));
    end
else
    estimates = repmat(fifo_dump_emos_struct_UE,1,floor(NFrames/decimation));
    if nargout==4
        H = complex(zeros(floor(NFrames/decimation),size(fifo_dump_emos_struct_UE.channel,1)/2,size(fifo_dump_emos_struct_UE.channel,2)));
    end
end

%gps_data = repmat(gps_data_struct,1,NFrames/100);

k = 1;
l = 1;
for n=1:NFiles
    fid = fopen(filename{n},'r');

    while (~feof(fid) && (k <= min(sum(NFrames_file(1:n)),NFrames_max)))

        if (is_eNb)
            estimates_tmp = binread(fid,fifo_dump_emos_struct_eNb,1,4,'l'); 
        else
            estimates_tmp = binread(fid,fifo_dump_emos_struct_UE,1,4,'l'); 
        end

        if (mod((k-1),decimation) == 0)
            estimates(((k-1)/decimation)+1) = estimates_tmp;
        end
        if nargout==4
            H(k,:,:) = (double(estimates_tmp.channel(1:2:end,:)) + 1j*double(estimates_tmp.channel(2:2:end,:)));
        end
        
        %read GPS data
        if ((mod(k,NO_ESTIMATES_DISK)==0) && ~feof(fid))
           gps_data(l) = binread(fid,gps_data_struct,1,4,'l');
           l=l+1;
        end
        k=k+1;
    end

    fclose(fid);
end
