filename_mat = sprintf('results_v2_idx_%d.mat',idx);

load(filename_mat,'R');
max_bits = max(bit_vec);

%% calculate the Tx correlation matrix taking the first Rx antenna
m = 0;
Codebook = zeros(NTx,pow2(max_bits),NUser);

for k=0:3;
for n1=0:3
    for n2=0:3
        tmp = R(k+n1*NUser+m*NUser*NTx+1,k+n2*NUser+m*NUser*NTx+1);
        RTx(n1+1,n2+1) = tmp;
    end
end

Codebook(:,:,k+1) = codebook_corr(NTx,max_bits,1,RTx);
end

save(sprintf('Codebook_idx_new%d.mat',idx),'Codebook');