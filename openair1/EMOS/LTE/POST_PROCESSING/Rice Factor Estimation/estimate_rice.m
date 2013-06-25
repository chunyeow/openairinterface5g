function [K, V2, s2] = estimate_rice(G)
% Ricean factor estimation
%  [K, V2, s2] = estimate_rice(h) estimates the Ricean K-Factor from the set of
%  impulse responses h using Moment Method estimation [1].
%  
%  [1] Greenstein, L., Michelson, D., and Erceg, V., 
%      "Moment-method estimation of the Ricean K-factor",
%       #IEEE_J_COML#, 1999, 3, 175-176
%
%  Parameters:
%   G       - size [NTau x Nsamp] Sequence of impulse responses (absolute square)
%
%  Author: Florian Kaltenberger 
%  Copyright: Eurecom Sophia Antipolis
%
%  Version History
%   Date      Version   Comment
%   20080620  0.1       Created
%   20080730  0.2       We caclculate the rice factor for every delay bin

[NTau, Nsamp] = size(G);

if ~isreal(G)
    error('Input G must be real');
end

Ga = mean(G,2);

Gv = zeros(NTau,1);
V2 = zeros(NTau,1);
s2 = zeros(NTau,1);
K  = zeros(NTau,1);

for t=1:NTau
    Gv(t) = sqrt(mean((G(t,:)-Ga(t)).^2));
    
    if Gv(t)>Ga(t)
        V2(t)=0;
    else
        V2(t) = sqrt(Ga(t)^2 - Gv(t)^2);
    end

    s2(t) = Ga(t) - V2(t);

    K(t) = V2(t)/s2(t);
end

% %% plot
% figure(69)
% cdfplot(G);
% hold on
% x = linspace(0,2.5,100);
% p = ricepdf(x,Ga,sqrt(s2));
% plot(x,cumsum(p)/sum(p),'r')
% legend('empirical cdf','reconstructed cdf')
% hold off
