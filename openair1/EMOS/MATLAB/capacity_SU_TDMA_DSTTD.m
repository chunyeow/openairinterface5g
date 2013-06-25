function [CAP] = capacity_SU_TDMA_DSTTD(H,SNR,normalize)
%
% [CAP] = capacity_SU_TDMA_MMSE(H,SNR,normalize)
%
%  This function calculate the multiuser (MU) capacity of the estimated
%  EMOS channels assuming a time-shared single user (SU) MIMO tranmission
%  and an MMSE receiver (see capacity_SU_MMSE for details).
%  The channel gains are normalized, such that E(|h_{i,j}|^2) = 1 hold for
%  each user. The avaraging is done over all snapshots and all frequencies.
%  
%  PARAMATERS
%      - H is the input channel (NRx,NTx,Nfreq,NFrames,NUser)
%      - SNR is the considered SNR (Linear)              
%      - normalize is a flag that indicates if the channel should be normalized
%                                                        
%  RETURNS
%      - CAP is the capacity values

%  Authors: florian.kaltenberger@eurecom.fr    
%  Copyright: Eurecom Sophia Antipolis
%  Created: 17/10/2007         
%  Modified last: 12/12/2007 by florian.kaltenberger@eurecom.fr

[NRx,NTx,NFreq,Nframes,NUser] = size(H);

if nargin<3 
    normalize = 1;
end

CAP_SU = zeros(NUser,Nframes*NFreq);

for k=1:NUser
    CAP_SU(k,:) = capacity_SU_DSTTD(H(:,:,:,:,k),SNR,normalize);
end
%CAP = mean(CAP_SU,2);
CAP = CAP_SU(:);
