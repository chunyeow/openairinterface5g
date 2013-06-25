function [estimates, gps_data, NFrames, align_matrix, crc_matrix] = load_estimates_MU_no_channel(filenames, NTx, NFrames_max)
% 
% EMOS Multi User Import Filter without channel estimates
%
% [estimates, gps_data, NFrames, align_matrix, crc_matrix] = load_estimates_MU(filename, NTx, NFrames_max)
%
% Parameters:
% filenames         - cell of strings with the filenames of the EMOS data
%                     files of all users for one measurement
% NTx               - Number of Tx antennas
% NFrames_max       - Maximum number of estimates. Leave it blank to get up to the
%                     maximum file contents
%
% Returns:
% estimates         - A [1 x Nuser] structure array containing timestamp, etc
% gps_data          - A [1 x Nuser] structure array containing gps data
% NFrames           - A [1 x NUser] matrix with the number of read estimates
% align_matrix      - A [NUser x NFrames] logical matrix used for aligning
%                     data from different users
% crc_matrix        - A [NUser x NFrames] logical matrix with CRCs


% Author: Florian Kaltenberger 
% Copyright: Eurecom Sophia Antipolis
% Created: 20071016
% Last modified: 20080612

if nargin < 3
    NFrames_max = Inf;
end
if nargin < 2
    NTx = 4;
end

Nuser = length(filenames);

% estimates = repmat(struct(...
%        'timestamp', [], ...
%     'timestamp_tx', [], ...
%    'framestamp_tx', [], ...
%       'pdu_errors', [], ...
%          'err_ind', [], ...
%         'rx_power', [], ...
%         'n0_power', [], ...
%      'rx_rssi_dBm', [], ...
%          'mac_pdu', [], ...
%             'flag', []), ...
%    1, Nuser);
% gps_data = repmat(struct(...
%     'timestamp', [], ...
%          'mode', [], ...
%           'ept', [], ...
%      'latitude', [], ...
%     'longitude', [], ...
%          'rest', []), ...
%          1, Nuser);
NFrames = zeros(1,Nuser);

framestamp_min = Inf;
framestamp_max = 0;
%framestamp = cell(1,Nuser);

for i=1:Nuser
    if isempty(filenames{i})
        disp(sprintf('[load_estimates_MU] Skipping user %i of %i (no data)',i,Nuser));
    else
        disp(sprintf('[load_estimates_MU] Loading data from user %i of %i',i,Nuser));
        [estimates(i), gps_data(i), NFrames(i)] = load_estimates_no_channel(filenames{i}, NTx, NFrames_max);

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

NFrames_Tx = framestamp_max-framestamp_min+1;

align_matrix = false(Nuser,NFrames_Tx);
for u = 1:Nuser
    align_matrix(u,estimates(u).framestamp_tx - framestamp_min + 1) = true;
end

crc_matrix = false(Nuser,NFrames_Tx);
for u = 1:Nuser
    crc_matrix(u,estimates(u).framestamp_tx - framestamp_min + 1) = ~estimates(u).err_ind;
end


