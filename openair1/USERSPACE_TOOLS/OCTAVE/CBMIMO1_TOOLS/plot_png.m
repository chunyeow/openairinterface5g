close all

load gain_table_v2_7_800_s1

figure(1)
plot(ALL_gain2391,real(NF0),'r;RX0;',ALL_gain2391,real(NF1),'g;RX1;')
title('Noise Figure vs. Digital Gain (Sector 0, CBMIMO1 v2.7)')
xlabel('Digital Gain')
ylabel('Noise Figure (dB)')
print -FTimes-Roman -dpdf s0_v27_nf.pdf 

figure(2)
plot(ALL_gain2391,real(G0),'r;RX0;',ALL_gain2391,real(G1),'g;RX1;')
title('RF Gain vs. Digital Gain (Sector 0, CBMIMO1 v2.7)')
xlabel('Digital Gain')
ylabel('RF Gain (dB)')
print -dpdf s0_v27_gain.pdf

load gain_table_v2_8_800_s2

figure(3)
plot(ALL_gain2391,real(NF0),'r;RX0;',ALL_gain2391,real(NF1),'g;RX1;')
title('Noise Figure vs. Digital Gain (Sector 1, CBMIMO v2.8)')
xlabel('Digital Gain')
ylabel('Noise Figure (dB)')
print -dpdf s1_v28_nf.pdf

figure(4)
plot(ALL_gain2391,real(G0),'r;RX0;',ALL_gain2391,real(G1),'g;RX1;')
title('RF Gain vs. Digital Gain (Sector 1, CBMIMO1 v2.8)')
xlabel('Digital Gain')
ylabel('RF Gain (dB)')
print -dpdf s1_v28_gain.pdf

load gain_table_v2_13_800_s3


figure(5)
plot(ALL_gain2391,real(NF0),'r;RX0;',ALL_gain2391,real(NF1),'g;RX1;')
title('Noise Figure vs. Digital Gain (Sector 2, CBMIMO v2.13)')
xlabel('Digital Gain')
ylabel('Noise Figure (dB)')
print -dpdf s2_v213_nf.pdf

figure(6)
plot(ALL_gain2391,real(G0),'r;RX0;',ALL_gain2391,real(G1),'g;RX1;')
title('RF Gain vs. Digital Gain (Sector 2, CBMIMO1 v2.13)')
xlabel('Digital Gain')
ylabel('RF Gain (dB)')
print -dpdf s2_v213_gain.pdf

load gain_table_v2_3_800_ue


figure(7)
plot(ALL_gain2391,real(NF0),'r;RX0;',ALL_gain2391,real(NF1),'g;RX1;')
title('Noise Figure vs. Digital Gain (UE, CBMIMO v2.3)')
xlabel('Digital Gain')
ylabel('Noise Figure (dB)')
print -dpdf s2_v23_nf.pdf

figure(8)
plot(ALL_gain2391,real(G0),'r;RX0;',ALL_gain2391,real(G1),'g;RX1;')
title('RF Gain vs. Digital Gain (UE, CBMIMO1 v2.3)')
xlabel('Digital Gain')
ylabel('RF Gain (dB)')
print -dpdf s2_v23_gain.pdf


