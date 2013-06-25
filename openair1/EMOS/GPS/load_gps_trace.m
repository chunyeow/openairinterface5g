function [gps_data, NFrames] = load_gps_trace(filename, NFrames_max, decimation)
% 
% EMOS GPS trace import filter
%
% [gps_data, NFrames] = 
%       load_estimates_lte(filename, NFrames_max)
%
% Parameters:
% filename          - filename(s) of the EMOS data file
% NFrames_max       - Maximum number of estimates. Leave it blank to get up to the
%                     maximum file contents
% decimation        - read every 'decimation' frame
%
% Returns:
% gps_data          - A structure array containing gps data
% NFrames           - the number of read estimates

% Author: Florian Kaltenberger 
% Copyright: Eurecom Sophia Antipolis

% Version History
%   Date      Version   Comment
%   20120123  0.1       Created based on load_estimates

if nargin < 3
    decimation = 1;
end
if nargin < 2
    NFrames_max = Inf;
end

gps_data_struct = struct(...
    'timestamp', double(0),...
    'mode', int32(0),...
    'ept',double(0),...
    'latitude',double(0),...
    'longitude',double(0),...
    'eph',double(0),...
    'altitude',double(0),...
    'epv',double(0),...
    'track',double(0),...
    'epd',double(0),...
    'speed',double(0),...
    'eps',double(0),...
    'climb',double(0),...
    'epc',double(0));

gps_data_struct_a = cstruct(gps_data_struct,[],4);

gps_fix_t_size = gps_data_struct_a.size;

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
    NFrames_file(n) = floor(info_file.bytes/gps_fix_t_size);
    if (mod(info_file.bytes,gps_fix_t_size) ~= 0)
        warning('File size not a multiple of buffer size. File might be corrupt.');
    end
end
NFrames = min(sum(NFrames_file), NFrames_max);

gps_data = repmat(gps_data_struct,1,NFrames);

l = 1;
for n=1:NFiles
    fid = fopen(filename{n},'r');

    while (~feof(fid) && (l <= min(sum(NFrames_file(1:n)),NFrames_max)))

      %read GPS data
      gps_data(l) = binread(fid,gps_data_struct,1,4,'l');
      l=l+1;
    end

    fclose(fid);
end

