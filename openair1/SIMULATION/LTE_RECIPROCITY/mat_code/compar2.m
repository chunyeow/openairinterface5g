close all;clear all;
vulb;cal1;
vdlb;
aue1;
aenb1;

dec_f = 1;

figure(1),subplot(131),plot(real(aenb(1:dec_f:300))), hold on, plot(real(cal(1:dec_f:300).*aue(1:dec_f:300)),"r")
title("dlchest")
subplot(132),plot(imag(aenb(1:dec_f:300))), hold on, plot(imag(cal(1:dec_f:300).*aue(1:dec_f:300)),"r")
subplot(133),plot(angle(aenb(1:dec_f:300))); hold on, plot(angle(cal(1:dec_f:300).*aue(1:dec_f:300)),"r")

figure(2), plot(real(aenb(1:dec_f:300))), hold on, plot(real(aue(1:dec_f:300)),"r")
%figure(5),
