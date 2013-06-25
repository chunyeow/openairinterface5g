function [pi, P, P2] = est_trans_prob(seq, Nstates)
% Estimate the stationary probabilities pi and the transtion matrix P for a
% finite-state stationary stochastic process seq with elements from 1..Nstates.

pi = hist(seq,1:Nstates)./length(seq);
P = zeros(Nstates);

for n=2:length(seq)
    P(seq(n),seq(n-1)) = P(seq(n),seq(n-1)) + 1;
end
P2 = P;
for i=1:Nstates
    if sum(P(i,:))~=0
        P(i,:) = P(i,:)./sum(P(i,:));
    end
end
