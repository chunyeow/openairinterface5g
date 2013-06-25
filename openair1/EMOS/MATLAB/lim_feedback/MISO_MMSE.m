function [CAP,eff_gain,eigval] = MISO_MMSE(HS,n_p,mu)

CAP =0;

tt= size(HS);

if (HS == zeros(tt(1),tt(2)))
    CAP = 0;
    eff_gain = 0;
else
    HH = HS*HS'+mu*eye(tt(1));
    
    if nargout == 3
        eigval = eig(HH);
    end
    
    HH_inv = inv(HH);
    %eff_gain = real(1./diag(tmp)).';
    eff_gain = 1/real(trace(HH_inv));

    % W = inv(HS'*HS+mu*eye(t(2)))*HS'
    W = HS'*HH_inv;

    % normalize W such that trace(W'*W)=1
    % this way we do not need the factor 1/NTx in the capacity formula
    W = W/sqrt(trace(W'*W));
    HW = abs(HS*W).^2;

    for v = 1:length(HW)
        CAP = CAP + log2(1 + (HW(v,v)/(sum(HW(v,:)) - HW(v,v) + n_p)));
    end %of for loop

end %of if loop HS

