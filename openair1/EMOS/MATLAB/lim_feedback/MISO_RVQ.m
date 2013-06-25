function [CAP] = MISO_RVQ(HS,Hq,n_p,mu)
% [CAP] = MISO_CVQ(HS,Hq,n_p,mu)
%
%  This function calculates the capacity of a multiuser system
%  assuming random channel vector quatization.
%
%  Note: This function is the same as MISO_CVQ!  It is just kept for
%  compatibility reasons.

[N_User, N_Tx] = size(HS);

if (HS == zeros(N_User,N_Tx))
    CAP = 0;
else

    HH = Hq*Hq'+mu*eye(N_User);
    
    HH_inv = inv(HH);

    W = Hq'*HH_inv;
    
    % normalize W such that trace(W'*W)=1
    % this way we do not need the factor 1/NTx in the capacity formula
    W = W/sqrt(trace(W'*W));
    HW = abs(HS*W).^2;

    CAP = 0;
    for v = 1:length(HW)
        CAP = CAP + log2(1 + (HW(v,v)/(sum(HW(v,:)) - HW(v,v) + n_p)));
    end %of for loop

end %of if loop HS

