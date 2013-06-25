function [H_MU, H_fq_MU, estimates, gps_data, NFrames] = load_estimates_MU_lm_new(fid, NTx, NFrames_max, NFrames, align_matrix, version)
% 
% EMOS Multi User Import Filter with low memory usage
%
% [H_MU, H_fq_MU, estimates, gps_data, NFrames] =
% load_estimates_MU_new(fid, NTx, NFrames_max, NFrames_read, version)
%
% Parameters:
% fid               - vector with file ids
% N_Tx              - Number of Tx antennas
% NFrames_max       - Maximum number of frames to read 
%                     (Use Inf to get up to the maximum file contents)
% Nframes           - Number of frames already read from the file 
%                     (Use return value of previous call)
% version           - see load_estimates for details
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
% Last modified: 20071128

NRx = 2;
NFreq = 160/NTx;
NDelay = 256/NTx;
Nuser = length(fid);

H_MU = complex(zeros(NRx,NTx,NDelay,NFrames_max,Nuser));
H_fq_MU = complex(zeros(NRx,NTx,NFreq,NFrames_max,Nuser));

for u=1:Nuser
    [H_MU(:,:,:,align_matrix(u,:),u), ...
     H_fq_MU(:,:,:,align_matrix(u,:),u), ...
     estimates(u), gps_data(u), NFrames(u)] = ...
        load_estimates_lm(fid(u), NTx, ...
           sum(align_matrix(u,:)), ...
           NFrames(u), version);
end
