function [CAP,eff_gain] = BD(HS,n_p,wf)

% size(HS) = [NRx,NTx,Nuser]
% n_p noise power

% the block diagonalization here assumes that we have NRx streams per user

if nargin<3
   wf=1;
end

S = [];
M = [];

tt= size(HS);
NRx = tt(1);
NTx = tt(2);
NUser = tt(3);

CAP = 0;
eff_gain = zeros(1,NUser);

if (HS == zeros(tt(1),tt(2),tt(3)))
    return
else
    H_all_users = reshape(permute(HS,[1 3 2]),NRx*NUser,NTx);
    for k = 1:tt(3)
        % form H_tilde_k
        H_tilde = H_all_users([1:(k-1)*NRx k*NRx+1:NUser*NRx],:);
        V_tilde_null = null(H_tilde);
        %[U1 S1 V1] = svd(HS(:,:,k)*V_tilde_null);
        S1 = svd(HS(:,:,k)*V_tilde_null);
        S = [S S1.'];
        %M = [M V_tilde_null * V1.*S1];

        % check this:
        eff_gain(k) = sum(S1.^2);
    end
    
    % we assume a total transmit power of sum(power)=1
    % this way we do not need the factor 1/NTx in the capacity formula
    if wf
        % power loading: use waterfilling
        power = IWF_4users(S.^2,1./n_p);
    else
        % no power loading
        power = repmat(1/(n_p*NUser*NRx),1,NUser*NRx);
    end
        
    CAP = sum(log2(1 + power.*S.^2));

 end %of if loop HS

