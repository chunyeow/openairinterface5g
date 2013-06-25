function [CAP] = capacity_MU_ZF(H,SNR,normalize)
%
% [CAP] = capacity_MU_ZF(H,SNR)
%
%  This function calculate the multiuser (MU) capacity of the estimated
%  EMOS channels assuming a zero forcing (ZF) precoder and one stream per
%  user. This requires that NTx >= NUser. At the receiver antenna
%  selection is assumed. 
%  The channel gains are normalized, such that E(|h_{i,j}|^2) = 1 hold for
%  each user. The averaging is done over all snapshots and all frequencies.
%  
%  PARAMATERS
%      - H is the input channel (NRx,NTx,Nfreq,NFrames,NUser)
%      - SNR is the considered SNR (Linear)              
%                                                        
%  RETURNS
%      - [CAP] is the capacity values

%  Authors: florian.kaltenberger@eurecom.fr    
%  Copyright: Eurecom Sophia Antipolis
%  Created: 17/10/2007         
%  Modified last: 17/10/2007 by florian.kaltenberger@eurecom.fr

[NRx,NTx,NFreq,Nframes,NUser] = size(H);
if NRx~=2
    warning('[capacity_MU_ZF] The antenna selection in this script might not work properly for more than 2 Rx antennas');
end

if nargin<3 
    normalize = 1;
end
if normalize
    % Normalization of H, such that E(|h_{i,j}|^2) = 1!
    disp('[capacity_MU_ZF] Normalization of H')
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

% Capacity calculation
disp('[capacity_MU_ZF] Capacity calculation')

% H_MISO = complex(zeros(NUser,NTx));
lambda = zeros(Nframes,NFreq);

for ii = 1:NFreq,
    for jj = 1:Nframes,
        % Select the antenna with the higher channel weight (the
        % received power at Rx antenna m is already computed in 
        % HH(m,m,ii,jj,kk)) and form the MU channel matrix.
        
        % Warning: This antenna selection does not really work
        %          For now we take antenna 1
        
        % if HH(1,1,ii,jj,kk) < HH(2,2,ii,jj,kk)
        %     % take antenna 2
        %     H_MISO = squeeze(H(2,:,ii,jj,:)).';
        % else
            % take antenna 1
            H_MISO = squeeze(H(1,:,ii,jj,:)).';
        % end
        % The precoder matrix W is given by 
        %       W = pinv(H_MISO)/norm(pinv(H_MISO))
        % However, we only need the denominator of this expression for
        % the capacity calculations.
        if all(all(H_MISO==0))
            lambda(jj,ii) = Inf;
        else
            lambda(jj,ii) = norm(pinv(H_MISO),'fro');
        end
    end
end

CAP = zeros(Nframes,NFreq);
for kk = 1:NUser
    CAP = CAP + log2(1+ SNR ./ lambda.^2);
end

