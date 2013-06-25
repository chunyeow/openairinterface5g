function [Hq, code_idx] = quantize_H(H,bits)
% Hq = quantize_H(H)
%
%  [1] Philips, "System-level simulation results for channel vector
%  quantisation feedback for MU-MIMO" 3gpp TSG RAN WG1 R1-063028, Nov. 2006 

[N_Tx, N_User] = size(H);

N_codes = pow2(bits);

for k=1:N_User
    normH(k) = norm(H(:,k));
    H(:,k) = H(:,k)./norm(H(:,k));
end

d = ifft(H,N_codes,1)*N_codes;

%plot(abs(d));

[m,code_idx] = max(abs(d));

Hq = (exp(-1j*2*pi*(0:N_Tx-1)'*(code_idx-1)/N_codes))./sqrt(N_Tx);

% Codebook = exp(-1j*2*pi*(0:N_Tx-1)'*(0:N_codes-1)/N_codes)./sqrt(N_Tx);
% 
% d = zeros(N_codes,N_User);
% for k=1:N_User
%     normH(k) = norm(H(:,k));
%     for i=1:N_codes
%         d(i,k) = Codebook(:,i)'*(H(:,k)./normH(k));
%     end
% end
% 
% plot(abs(d))
% 
% [m,idx] = max(abs(d));
% 
% Hq = Codebook(:,idx);

% Hq has to have the same amplitude as H
for i=1:N_User
   % Hq(:,i) = Hq(:,i).*d(idx(i),i).*normH(i);
   Hq(:,i) = Hq(:,i).*normH(i);
end

