CAP_4U_ZF_RVQ_corr = RVQ_corr.CAP_4U_ZF_RVQ;
CAP_4U_MMSE_RVQ_corr = RVQ_corr.CAP_4U_MMSE_RVQ;


%%  mean capacity vs. feedback bits
ECAP_4U_ZF = mean(CAP_4U_ZF(:));
ECAP_4U_MMSE = mean(CAP_4U_MMSE(:));
for b = 1:3
    tmp = reshape(CAP_4U_ZF_CVQ(:,:,b),1,[]);
    tmp2 = reshape(CAP_4U_MMSE_CVQ(:,:,b),1,[]);
    
    ECAP_4U_ZF_CVQ(b) = mean(tmp(~isnan(tmp)));
    ECAP_4U_MMSE_CVQ(b) = mean(tmp2(~isnan(tmp2)));
end
for b = 1:3
    tmp = reshape(CAP_4U_ZF_RVQ(:,:,b),1,[]);
    tmp2 = reshape(CAP_4U_MMSE_RVQ(:,:,b),1,[]);
    
    ECAP_4U_ZF_RVQ(b) = mean(tmp(~isnan(tmp)));
    ECAP_4U_MMSE_RVQ(b) = mean(tmp2(~isnan(tmp2)));
end
for b = 1:3
    tmp = reshape(CAP_4U_ZF_RVQ_corr(:,:,b),1,[]);
    tmp2 = reshape(CAP_4U_MMSE_RVQ_corr(:,:,b),1,[]);
    
    ECAP_4U_ZF_RVQ_corr(b) = mean(tmp(~isnan(tmp)));
    ECAP_4U_MMSE_RVQ_corr(b) = mean(tmp2(~isnan(tmp2)));
end

%% plot
h_fig = figure(7);
hold off
%plot([4 8 12 16],ECAP_4U_ZF_CVQ)
plot([4 8 12],ECAP_4U_MMSE_CVQ(1:3),'k-.','Linewidth',2)
hold on
%plot([4 8 12],ECAP_4U_ZF_RVQ,'g')
plot([4 8 12],ECAP_4U_MMSE_RVQ,'k-','Linewidth',2)
%plot([4 8 12],ECAP_4U_ZF_RVQ_corr,'g--')
plot([4 8 12],ECAP_4U_MMSE_RVQ_corr,'k--','Linewidth',2)

%plot([4 8 12],repmat(ECAP_4U_MMSE,1,3),'k:','Linewidth',2);

h = legend('MU-MIMO MMSE DFT codebook','MU-MIMO MMSE random codebook','MU-MIMO MMSE random correlated codebook','Location','NorthWest');
set(h,'Fontsize',12)
xlabel('Feedback bits','Fontsize',12)
ylabel('Ergodic capacity [bits/sec/Hz]','Fontsize',12)
set(gca,'Fontsize',12)
grid on
filename_cap = fullfile(filepath_figs,sprintf('idx_%d_SNR_%d_ecap_CVQ_4U.eps',idx,SNRdB));
saveas(h_fig, filename_cap, 'epsc2');
filename_cap_pdf = fullfile(filepath_figs,sprintf('idx_%d_SNR_%d_ecap_CVQ_4U.pdf',idx,SNRdB));
system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));

save(sprintf('ecap_idx%d.mat',idx),'ECAP*','-V7');

%%
figure(8)
barh([4 8 12],[ECAP_4U_MMSE_CVQ(1:3); ECAP_4U_MMSE_RVQ; ECAP_4U_MMSE_RVQ_corr].');
legend('MU-MIMO MMSE DFT codebook','MU-MIMO MMSE random codebook','MU-MIMO MMSE random correlated codebook','Location','SouthOutside')
ylabel('Feedback bits')
xlabel('Ergodic capacity [bits/sec/Hz]')


