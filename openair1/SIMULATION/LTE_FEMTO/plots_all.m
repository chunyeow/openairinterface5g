close all
fch0e
fch1e
figure;
plot(abs(ch0e),'b'); title 'Estimated 0'
hold on
plot(abs(ch1e),'r');   
hold off   
dlsch0_ch_ext00
dlsch1_ch_ext00
figure;
plot(abs(dl0_ch_ext00)); title 'Ext 0';
hold on
plot(abs(dl1_ch_ext00),'r'); 
hold off


dlsch0_rxF_comp0
dlsch1_rxF_comp0
figure;
plot(dl0_rxF_comp0,'x')  ;title 'Comp 0';
figure;
plot(dl1_rxF_comp0,'x');title 'Comp 1 interferer';


dlsch0_rxF_llr
rho
figure;
plot(real(rho_0),'x'); title 'Rho';
figure;
plot(dl0_llr,'x'); title 'Llr';
