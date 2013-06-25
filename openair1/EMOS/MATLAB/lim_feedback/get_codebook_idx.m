function idx = get_codebook_idx(H,Codebook,bit_vec)
% Hq = quantize_H(H, codebook)
%
% This function quantizes H according to the given codebook
% 
% Parameters:
%   H           [N_Tx x N_User]     Channel matrix to be quatized
%   codebook    [N_Tx x N_codes x N_User]    Codebook

[N_Tx, N_User] = size(H);
N_codes = size(Codebook,2);

if size(Codebook,1) ~= N_Tx
    error('codebook must be of size [N_Tx x N_codes x N_User]');
end

if size(Codebook,3) ~= N_User
    error('codebook must be of size [N_Tx x N_codes x N_User]');
end

if pow2(bit_vec(end)) > N_codes
    error('pow2(bit_vec) must be < than N_codes')
end

% normalize the channel
for k=1:N_User
    normH(k) = norm(H(:,k));
    H(:,k) = H(:,k)./norm(H(:,k));
end

d = zeros(N_codes,N_User);
for k=1:N_User
    for i=1:N_codes
        d(i,k) = Codebook(:,i,k)'*H(:,k);
    end
end

% plot(abs(d))

for b=1:length(bit_vec)
    [m,idx(b,:)] = max(abs(d(1:pow2(bit_vec(b)),:)),[],1);
end

