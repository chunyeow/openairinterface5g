h_fig = figure(7)

load /extras/kaltenbe/EMOS/data/20080213_MU_Outdoor/ecap_idx50.mat
axes1 = axes('Position',[0.13 0.25 0.35 0.6]);
bar([4 8 12],[ECAP_4U_MMSE_CVQ; ECAP_4U_MMSE_RVQ; ECAP_4U_MMSE_RVQ_corr].') 
title('outdoor')
xlabel('Feedback bits')
ylabel('Ergodic capacity')

load /extras/kaltenbe/EMOS/data/20080205_MU_Indoor_EN_EC/ecap_idx100.mat
axes2 = axes('Position',[0.57 0.25 0.35 0.6]);
bar([4 8 12],[ECAP_4U_MMSE_CVQ; ECAP_4U_MMSE_RVQ; ECAP_4U_MMSE_RVQ_corr].')
title('indoor')
xlabel('Feedback bits')
ylabel('Ergodic capacity')

legend({'MU-MIMO MMSE DFT codebook','MU-MIMO MMSE random codebook','MU-MIMO MMSE random correlated codebook'},...
  'Position',[0.3 0.07 0.40 0.1]);
%  'Orientation','horizontal');

filename_cap = 'ecap_CVQ_4U.eps';
saveas(h_fig, filename_cap, 'epsc2');
filename_cap_pdf = 'ecap_CVQ_4U.pdf';
system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));
