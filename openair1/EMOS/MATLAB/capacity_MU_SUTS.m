function [CAP] = capacity_MU_SUTS(H,SNR,normalize)
%
% [CAP] = capacity_MU_SUTS(H,SNR)
%
%  This function calculate the multiuser (MU) capacity of the estimated
%  EMOS channels assuming a time-shared single user (SU) MIMO tranmission.  
%  The channel gains are normalized, such that E(|h_{i,j}|^2) = 1 hold for
%  each user. The avaraging is done over all snapshots and all frequencies.
%  
%  PARAMATERS
%      - H is the input channel (NRx,NTx,Nfreq,NFrames,NUser)
%      - SNR is the considered SNR (Linear)              
%                                                        
%  RETURNS
%      - CAP is the capacity values

%  Authors: florian.kaltenberger@eurecom.fr    
%  Copyright: Eurecom Sophia Antipolis
%  Created: 17/10/2007         
%  Modified last: 17/10/2007 by florian.kaltenberger@eurecom.fr

[NRx,NTx,NFreq,Nframes,NUser] = size(H);

if nargin<3 
    normalize = 1;
end
if normalize
    % Normalization of H, such that E(|h_{i,j}|^2) = 1!
    disp('[capacity_MU_SUTS] Normalization of H')
    HH = complex(zeros(NRx,NRx,NFreq,Nframes,NUser));
    mean_pow = zeros(1,NUser);
    for kk = 1:NUser ,
        for ii = 1:NFreq,
            for jj = 1:Nframes,
                % this is the frobenious norm of H
                HH(:,:,ii,jj,kk) = H(:,:,ii,jj,kk)*H(:,:,ii,jj,kk)';
                mean_pow(kk) = mean_pow(kk) + trace(HH(:,:,ii,jj,kk));
            end
        end
         mean_pow(kk) = mean_pow(kk)/(NFreq*Nframes);
         H(:,:,:,:,kk) = H(:,:,:,:,kk) .* sqrt(NRx*NTx/real(mean_pow(kk))); %*
    end
    disp(sqrt(NRx*NTx./real(mean_pow)))
end

% * make sure the power is real (due to numerical inaccuracies, there might
% be a small imaginary part)
% mean_pow = real(mean_pow);

disp('[capacity_MU_SUTS] Capacity calculation')

CAP_SU = zeros(Nframes*NFreq,NUser);

for k=1:NUser
    CAP_SU(:,k) = capacityEMOS(H(:,:,:,:,k),SNR,normalize);
end
CAP = mean(CAP_SU,2);
