function CAP = cap_MMSE(G,n_p)

CAP = 0;

[N_Rx, N_Tx] = size(G);

if (G == zeros(N_Rx,N_Tx))
    CAP = 0;
else
    iG = inv(1./(n_p*N_Tx)*G'*G + eye(N_Tx));
    %MF = iG * G';
    %MF = G';

    % HW = abs(MF*G);

    for v = 1:N_Tx
        % MMSE (see Paulraj 2002 Eqn. 7.49)
        CAP = CAP + log2(1/iG(v,v));
    end %of for loop

end %of if loop HS

