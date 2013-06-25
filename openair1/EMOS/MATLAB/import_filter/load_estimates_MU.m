function [H_MU, H_fq_MU, estimates, gps_data, NFrames] = load_estimates_MU(filenames, NTx, NFrames_max, remove_zeros)
% 
% EMOS Multi User Import Filter
%
% [H, H_fq, estimates, gps_data, NFrames] = load_estimates_MU(filename, NTx, NFrames_max)
%
% Parameters:
% filenames         - cell array of strings with the filenames of the EMOS data
%                     files of all users for one measurement
% NFrames_max       - Maximum number of estimates. Leave it blank to get up to the
%                     maximum file contents
% remove_zeros      - if this flag is set, only the part of the MU channel
%                     matrix, where signals from all users are availiable is calculated
%
% Returns:
% H                 - Time domain channel matrix of size [NRx,NTx,NDelay,NFrames,Nuser]
%                     The estimates of different users are aligned in time
% H_fq              - Frequency domain channel matrix of size [NRx,NTx,NFreq,NFrames,Nuser]
% estimates         - A [1 x Nuser] structure array containing timestamp, etc
% gps_data          - A [1 x Nuser] structure array containing gps data
% NFrames           - the number of read estimates

% Author: Florian Kaltenberger 
% Copyright: Eurecom Sophia Antipolis
% Created: 20071016
% Last modified: 20071116

if nargin < 4
    remove_zeros = 0;
end
if nargin < 3
    NFrames_max = Inf;
end
if nargin < 2
    NTx = 4;
end
NRx = 2;

Nuser = length(filenames);

H = cell(1,Nuser);
H_fq = cell(1,Nuser);

estimates = repmat(struct(...
       'timestamp', [], ...
    'timestamp_tx', [], ...
   'framestamp_tx', [], ...
      'pdu_errors', [], ...
         'err_ind', [], ...
        'rx_power', [], ...
        'n0_power', [], ...
     'rx_rssi_dBm', [], ...
         'mac_pdu', [], ...
            'flag', []), ...
   1, Nuser);
gps_data = repmat(struct(...
    'timestamp', [], ...
         'mode', [], ...
          'ept', [], ...
     'latitude', [], ...
    'longitude', [], ...
         'rest', []), ...
         1, Nuser);
NFrames = zeros(1,Nuser);

framestamp_min = Inf;
framestamp_max = 0;
%framestamp = cell(1,Nuser);

for i=1:Nuser
    if isempty(filenames{i})
        disp(sprintf('[load_estimates_MU] Skipping user %i of %i (no data)',i,Nuser));
    else
        disp(sprintf('[load_estimates_MU] Loading data from user %i of %i',i,Nuser));
        [H{i}, H_fq{i}, estimates(i), gps_data(i), NFrames(i)] = load_estimates(filenames{i}, NTx, NFrames_max);

        % ignore the faulty packets (the timestamp is not reliable)
        framestamp_clean = estimates(i).framestamp_tx(~estimates(i).err_ind);
        if isempty(framestamp_clean)
            framestamp_min = framestamp_max;
        else
            framestamp_min = min(framestamp_min,framestamp_clean(1));
            framestamp_max = max(framestamp_max,framestamp_clean(end));
        end
    end
end

disp('[load_estimates_MU] Aligning data...');
NDelay = size(H{1},3);
NFreq = size(H_fq{1},3);
   
NFrames = framestamp_max - framestamp_min;
H_MU = complex(zeros(NRx,NTx,NDelay,NFrames,Nuser));
H_fq_MU = complex(zeros(NRx,NTx,NFreq,NFrames,Nuser));

if NFrames==0
    return
end

for i=1:Nuser
    H_MU(:,:,:,estimates(i).framestamp_tx(~estimates(i).err_ind) - framestamp_min + 1,i) = H{i}(:,:,:,~estimates(i).err_ind);
    H_fq_MU(:,:,:,estimates(i).framestamp_tx(~estimates(i).err_ind) - framestamp_min + 1,i) = H_fq{i}(:,:,:,~estimates(i).err_ind);
end

%keyboard

if remove_zeros
    % calculate the intersection of frames
    framestamp_is = framestamp_min:framestamp_max;
    for i=1:Nuser
        framestamp_is = intersect(framestamp_is,estimates(i).framestamp_tx(~estimates(i).err_ind));
    end
    H_MU = H_MU(:,:,:,framestamp_is - framestamp_min + 1,:);
    H_fq_MU = H_fq_MU(:,:,:,framestamp_is - framestamp_min + 1,:);
end