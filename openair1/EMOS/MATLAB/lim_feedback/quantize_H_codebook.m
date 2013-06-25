function [Hq,idx] = quantize_H_codebook(H,Codebook)
% Hq = quantize_H(H, codebook)
%
% This function quantizes H according to the given codebook
% 
% Parameters:
%   H           [N_Tx x N_User]     Channel matrix to be quatized
%   codebook    [N_Tx x N_codes]    Codebook

[N_Tx, N_User] = size(H);

N_codes = size(Codebook,2);

if size(Codebook,1) ~= N_Tx
    error('codebook must be of size [N_Tx x N_codes]');
end

% normalize the channel
for k=1:N_User
    normH(k) = norm(H(:,k));
    H(:,k) = H(:,k)./norm(H(:,k));
end

d = zeros(N_codes,N_User);
for k=1:N_User
    for i=1:N_codes
        d(i,k) = Codebook(:,i)'*H(:,k);
    end
end

% plot(abs(d))

[m,idx] = max(abs(d));

Hq = Codebook(:,idx);

% Hq has to have the same amplitude as H
for i=1:N_User
   % Hq(:,i) = Hq(:,i).*d(idx(i),i).*normH(i);
   Hq(:,i) = Hq(:,i).*normH(i);
end

