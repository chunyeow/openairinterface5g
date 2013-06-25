function [VS] = codebook_corr(N,Bi,MC,R_Tx)        

% MC: monte carlo run
% Bi: number of bits used for CDI (quantized channel directions)
% N: number of Tx antennas
% R_Tx: [N x N] Tx correlation matrix used for codebook design
% ALL users, for a given MC run, have the same codebook

VS = zeros(N,2^Bi,MC);
%PrefPartner = zeros(length(K),MC,2^Bi);
%orthMatrix = zeros(2^Bi,2^Bi,length(K),MC); 

for mc = 1:MC
%    for k = 1:length(K)
        aux = randn(N,2^Bi) + i*randn(N,2^Bi);
        aux = sqrtm(R_Tx)*aux;
        for p = 1:2^Bi
            aux(:,p) = aux(:,p)./norm(aux(:,p)); 
        end    
%         for s=1:2^Bi
%             for t=1:2^Bi
%                 orthMatrix(s,t,k,mc) = abs(aux(:,s)'*aux(:,t));
%             end
%         end
%         for p=1:2^Bi
%             [aux2 PrefPartner(k,mc,p)] = min(orthMatrix(p,:,k,mc));
%         end            
        VS(:,:,mc) = aux;
%    end
end