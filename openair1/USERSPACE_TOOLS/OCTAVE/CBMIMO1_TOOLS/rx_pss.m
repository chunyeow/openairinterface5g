addpath('../../../PHY/LTE_REFSIG/');
primary_synch;
pss_t = upsample(primary_synch0_time,4);
corr = abs(conv(conj(pss_t),s(:,1)));
figure(3);
plot(corr);

[pss_amp, pss_pos] = max(corr);
sss_pos = pss_pos - 640;
ssst = s(sss_pos:(sss_pos+511));
