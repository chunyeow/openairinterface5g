% Comparaison Matlab FACTEUR P

clear all
clc
K=10;
SC=300;
M=1;N=1;
H_h=zeros(N,M,300,K);
G_g=zeros(M,N,300,K);
Vis=zeros(SC,N,M,8);

vudl0;
vudrs0;
vudl1;
vudrs1;
vudl2;
vudrs2;
vudl3;
vudrs3;
vudl4;
vudrs4;
vudl5;
vudrs5;
vudl6;
vudrs6;
vudl7;
vudrs7;
vudl8;
vudrs8;
vudl9;
vudrs9;
cal1;
vul1;
vdl1;

for s_c=1:SC
    for n_k=1:K
        eval(['H_h(1,1,' int2str(s_c) ',' int2str(n_k) ')=vudrs' int2str(n_k-1) '(' int2str(s_c) ');']);        
        eval(['G_g(1,1,' int2str(s_c) ',' int2str(n_k) ')=vudl' int2str(n_k-1) '(' int2str(s_c) ');']);        
    end    
end

Psyst(1,1,:)=cal;

for s_c=1:SC
    
    for ii= 1:N % nb antennes emission
        for jj= 1:M % nb antennes reception
            %if(max(abs(squeeze(H_h(ii,jj,s_c,1:K))))~=0 && max(abs(squeeze(G_g(jj,ii,s_c,1:K))))~=0)
            Pud(ii,jj,s_c)=basictls(squeeze(H_h(ii,jj,s_c,1:K)),squeeze(G_g(jj,ii,s_c,1:K)) );
            
                            C=squeeze(H_h(ii,jj,s_c,1:K));
                            D=squeeze(G_g(jj,ii,s_c,1:K));
                             px =[C D]'*[C D];
                             ar=real(px(1,1));
                             ai=imag(px(1,1));
                             br=real(px(1,2));
                             bi=imag(px(1,2));
                             dr=real(px(2,2));
                             di=imag(px(2,2));
                             r1=2*br/(ar-dr+sqrt((ar-dr)*(ar-dr)+((br*br+bi*bi)*4)));
                             r2=2*bi/(ar-dr+sqrt((ar-dr)*(ar-dr)+((br*br+bi*bi)*4)));
                             Vis(s_c,ii,jj,:)=[ar ai br bi dr di r1 r2 ];
            
        end
    end
    
end

figure,plot(abs(cal(1:300).*vul(901:1200)));
figure,plot(abs(vdl(1537:1836)),"r");

break
figure,plot(abs(cal(1:300).*aue(1:300)));
figure,plot(abs(aenb(1:300)),"r");

A=vul(1:300);
B=vdl(1:300);
figure,plot(abs(fft(ifft(A,512),300)),"b");
figure,plot(abs(fft(ifft(B,512),300)),"b");



close all;clear all;
vul1;
vdl1;
aue1;
aenb1;
figure(1),subplot(231),plot(real(vdl(1537:1836)))
subplot(232),plot(imag(vdl(1537:1836)))
subplot(233),plot(angle(vdl(1537:1836)),"m");
subplot(234),plot(real(vul(901:1200)),"r")
subplot(235),plot(imag(vul(901:1200)),"r")
subplot(236),plot(angle(vul(901:1200)),"m")


figure(2),subplot(231),plot(real(aenb(1:300)))
subplot(232),plot(imag(aenb(1:300)))
subplot(233),plot(angle(aenb(1:300)),"m");
subplot(234),plot(real(aue(1:300)),"r")
subplot(235),plot(imag(aue(1:300)),"r")
subplot(236),plot(angle(aue(1:300)),"m")


cal1;
figure,plot(real(cal(1:300)),"m");
figure,plot(imag(cal(1:300)),"m");
figure,plot(angle(cal(1:300)),"m");

