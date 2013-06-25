function R = meas_feedback_rate(Codebook_ind,bit_vec)
% Estimates the feedback rate given the Codebook indices
%
%  R = meas_feedback_rate(Codebook_ind)

R = zeros(size(Codebook_ind,1),size(Codebook_ind,4));

for i=1:size(Codebook_ind,1)
    for j=1:size(Codebook_ind,4)
        [pi,P] = est_trans_prob(Codebook_ind(i,:,1,j),pow2(bit_vec));
        R(i,j) = sum(pi.'.*(1-diag(P)))*bit_vec ;
    end
end

        