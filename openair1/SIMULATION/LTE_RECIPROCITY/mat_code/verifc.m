%% Verif Calibration
close all, clear all;
run("./LTE_RECIPROCITY/aue1.m");
run("./LTE_RECIPROCITY/aenb1.m");
run("./LTE_RECIPROCITY/cal1.m");
run("./LTE_RECIPROCITY/vul1.m");
run("./LTE_RECIPROCITY/vdl1.m");



ul=vul(901:1200);
dl=vdl(1537:1836);

dl_est=cal(1:300).*ul;

res=abs(dl)-abs(dl_est);

%plot(res);
figure();plot(abs(dl(50:150)),"g"),figure,plot(abs(ul(50:150)),"r");
figure,plot(abs(cal(50:150)),"r");
%figure(),plot(abs(ul),"m");
