function out = cat_files(filenames)

s(1) = load(filenames{1});
for i=2:length(filenames)
    s(i) = orderfields(load(filenames{i}),s(1));
end

out.CAP_4U_ZF_RVQ = cat(3,s.CAP_4U_ZF_RVQ);
out.CAP_4U_MMSE_RVQ = cat(3,s.CAP_4U_MMSE_RVQ);
out.CAP_4U_ZF_CVQ = cat(3,s.CAP_4U_ZF_CVQ);
out.CAP_4U_MMSE_CVQ = cat(3,s.CAP_4U_MMSE_CVQ);
out.bit_vec = cat(2,s.bit_vec);
out.Codebook_ind = cat(3,s.Codebook_ind);


