% OCTAVE Code to generate upsampled pss0-2

n=0:61;
d0 = zeros(1,62);
d1 = zeros(1,62);
d2 = zeros(1,62);
d0(1+(0:30)) = exp(-sqrt(-1)*pi*25*(0:30).*(1:31)/63);
d1(1+(0:30)) = exp(-sqrt(-1)*pi*29*(0:30).*(1:31)/63);
d2(1+(0:30)) = exp(-sqrt(-1)*pi*34*(0:30).*(1:31)/63);
d0(1+(31:61)) = exp(-sqrt(-1)*pi*25*(32:62).*(33:63)/63);
d1(1+(31:61)) = exp(-sqrt(-1)*pi*29*(32:62).*(33:63)/63);
d2(1+(31:61)) = exp(-sqrt(-1)*pi*34*(32:62).*(33:63)/63);

pss0f = zeros(1,2048);
pss0f(2:32) = d0(1:31);
pss0f(2048+(-30:0)) = d0(32:62);
pss1f = zeros(1,2048);
pss1f(2:32) = d1(1:31);
pss1f(2048+(-30:0)) = d1(32:62);
pss2f = zeros(1,2048);
pss2f(2:32) = d2(1:31);
pss2f(2048+(-30:0)) = d2(32:62);

pss0_6144f = fftshift(fft(ifft(pss0f)*sqrt(2048),6144)/sqrt(6144));
pss1_6144f = fftshift(fft(ifft(pss1f)*sqrt(2048),6144)/sqrt(6144));
pss2_6144f = fftshift(fft(ifft(pss2f)*sqrt(2048),6144)/sqrt(6144));

pss0_6144_fp = zeros(1,512);
pss0_6144_fp(1:2:512) = (floor(32767*real(pss0_6144f(3072+(-128:127)))));
pss0_6144_fp(2:2:512) = (floor(32767*imag(pss0_6144f(3072+(-128:127)))));
pss1_6144_fp = zeros(1,512);
pss1_6144_fp(1:2:512) = (floor(32767*real(pss1_6144f(3072+(-128:127)))));
pss1_6144_fp(2:2:512) = (floor(32767*imag(pss1_6144f(3072+(-128:127)))));
pss2_6144_fp = zeros(1,512);
pss2_6144_fp(1:2:512) = (floor(32767*real(pss2_6144f(3072+(-128:127)))));
pss2_6144_fp(2:2:512) = (floor(32767*imag(pss2_6144f(3072+(-128:127)))));

fprintf("int16_t pss6144_0[512]={");
fprintf("%d,",pss0_6144_fp(1:511));
fprintf("%d};\n",pss0_6144_fp(512));
fprintf("int16_t pss6144_1[512]={");
fprintf("%d,",pss1_6144_fp(1:511));
fprintf("%d};\n",pss1_6144_fp(512));
fprintf("int16_t pss6144_2[512]={");
fprintf("%d,",pss2_6144_fp(1:511));
fprintf("%d};\n",pss2_6144_fp(512));
