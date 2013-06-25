close all;clear all;
vulb;cal1;
vdlb;
aue1;
aenb1;
figure(1),subplot(231),plot(real(vdl(1537:1836)))
title("dlchest et drschest")
subplot(232),plot(imag(vdl(1537:1836)))
subplot(233),plot(angle(vdl(1537:1836)),"m");
subplot(234),plot(real(vul(901:1200)),"r")
subplot(235),plot(imag(vul(901:1200)),"r")
subplot(236),plot(angle(vul(901:1200)),"m")

dec_f = 4;

figure(2),subplot(231),plot(real(aenb(1:dec_f:300)))
title("dlchest et drschest avec doquant")
subplot(232),plot(imag(aenb(1:dec_f:300)))
subplot(233),plot(angle(aenb(1:dec_f:300)),"m");
subplot(234),plot(real(aue(1:dec_f:300)),"r")
subplot(235),plot(imag(aue(1:dec_f:300)),"r")
subplot(236),plot(angle(aue(1:dec_f:300)),"m")



figure(3),subplot(131),plot(real(cal(1:dec_f:300)),"g");
title("facteur de calib")
subplot(132),plot(imag(cal(1:dec_f:300)),"g");
subplot(133),plot(angle(cal(1:dec_f:300)),"g");

figure(4),
subplot(131),plot(real(cal(1:dec_f:300).*aue(1:dec_f:300)),"r")
title("dl reconstruit")
subplot(132),plot(imag(cal(1:dec_f:300).*aue(1:dec_f:300)),"r")
subplot(133),plot(angle(cal(1:dec_f:300).*aue(1:dec_f:300)),"r")

%figure(5),

break

vdrs2;vdrs4;cal1;
vdl2;vdl4
aue1;
aenb1;
figure(20),subplot(231),plot(real(vudl2(1:300)))
title("dlchest et drschest")
subplot(232),plot(imag(vudl2(1:300)))
subplot(233),plot(angle(vudl2(1:300)),"m");
subplot(234),plot(real(vudrs2(1:300)),"r")
subplot(235),plot(imag(vudrs2(1:300)),"r")
subplot(236),plot(angle(vudrs2(1:300)),"m")

figure(21),subplot(231),plot(real(vudl4(1:300)))
title("dlchest et drschest")
subplot(232),plot(imag(vudl4(1:300)))
subplot(233),plot(angle(vudl4(1:300)),"m");
subplot(234),plot(real(vudrs4(1:300)),"r")
subplot(235),plot(imag(vudrs4(1:300)),"r")
subplot(236),plot(angle(vudrs4(1:300)),"m")


