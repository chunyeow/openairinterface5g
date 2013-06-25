function [CAP] = capacity_SU_ML(HH,SNR,normalize)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% [CAP] = capacity_SU_ML(HH,SNR)                    
%                                                   
%  This function calculate the capacity of a SU MIMO channels assuming no 
%  CSIT and a ML detector. 
%  The channel gains are normalized such that E(|h_{i,j}|^2) = 1,
%  where the avaraging is done over all snapshots and  all frequencies.                                      %%
%
%  PARAMETERS
%      - [CAP] is the capacity values;                
%      - HH is the input channel (M_Rx,M_Tx,fq,Channels)
%      - SNR is the considered SNR (Linear)             
%      - normalize is a flag that indicates if the channel should be normalized
%                                                       
%  Created: 25/04/2007 by delacerd@eurecom.fr           
%  Modified last: 12/12/2007 by florian.kaltenberger@eurecom.fr
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

[M_Rx,M_Tx,N_fq,N_channel] = size(HH);

% fk: I don't think this code is correct. 
% tmp = zeros(N_fq,N_channel);
% for kk = 1:N_channel,
%     tmp1 = 0;
%     for ii = 1:N_fq,
%         var1(:,:,ii,kk) = HH(:,:,ii,kk)*HH(:,:,ii,kk)';
%         tmp1 = tmp1 + norm(var1(:,:,ii,kk),'fro');
%     end
%     PP = tmp1/N_fq;
%     var1 = (M_Rx/PP) * var1;
%     for ii = 1:N_fq,
%         tmp(ii,kk)    = abs(log2(det(diag(ones(M_Rx,1)) + SNR * var1(:,:,ii,kk))));
%         ForPDF(ii,kk) = trace(M_Tx*var1(:,:,ii,kk)); 
%     end
% end

if nargin<3 
    normalize = 1;
end
if normalize
    % Normalization of H, such that E(|h_{i,j}|^2) = 1!
    var1 = complex(zeros(M_Rx,M_Rx,N_fq,N_channel));
    tmp = 0;
    for ii = 1:N_fq,
        for kk = 1:N_channel,
            % this is the squared frobenious norm of HH
            var1(:,:,ii,kk) = HH(:,:,ii,kk)*HH(:,:,ii,kk)';
            tmp = tmp + trace(var1(:,:,ii,kk));
        end
    end
    PP = real(tmp/(N_fq*N_channel*M_Rx*M_Tx))
else
    PP = 1;
end

% HH = HH*sqrt(M_Rx*M_Tx/PP);
% now E(|h_{i,j}|^2) = 1!

% Capacity estimation
tmp = zeros(N_fq,N_channel);
% ForPDF = zeros(N_fq,N_channel);
for ii = 1:N_fq,
    for kk = 1:N_channel,
        % the following two lines are the same, the second one should be
        % more efficient
        tmp(ii,kk)=abs(log2(det(diag(ones(M_Rx,1)) + (SNR/(M_Tx*PP)) * HH(:,:,ii,kk) * HH(:,:,ii,kk)')));
        % tmp(ii,kk)=abs(log2(det(diag(ones(M_Rx,1)) + (M_Rx*SNR/PP) *
        % var1(:,:,ii,kk))));
        % ForPDF(ii,kk) = trace(M_Tx*var1(:,:,ii,kk)); 
    end
end

% ForPDF = reshape(ForPDF,1,N_fq*N_channel);

if N_channel == 1, % if we are analyzing one single frame the output is a value
    CAP = mean(tmp);
%    CAP = (tmp);
else               % if we are analyzing a sequence of frames, the output is a vector with all capacity values
    CAP = reshape(tmp,1,N_fq*N_channel);
end

