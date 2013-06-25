function [CAP] = MISO_CVQ(HS,Hq,n_p,mu)
% [CAP] = MISO_CVQ(HS,Hq,n_p,mu)
%
%  This function calculates the capacity of a multiuser system
%  assuming channel vector quatization [1].
%
%  [1] Philips, "System-level simulation results for channel vector
%  quantisation feedback for MU-MIMO" 3gpp TSG RAN WG1 R1-063028, Nov. 2006 

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

