function CAP = cap_ZF(G,n_p)

CAP = 0;

[N_Rx, N_Tx] = size(G);

if (G == zeros(N_Rx,N_Tx))
    CAP = 0;
else
    iG = inv(G'*G); 
    %MF = iG * G';
    %MF = G';
    
    % HW = abs(MF*G);

    for v = 1:N_Tx
        % ZF (see Paulraj 2002 Eqn. 7.43)
        CAP = CAP + log2(1 + 1/(N_Tx*n_p*iG(v,v)));
    end %of for loop

end %of if loop HS

