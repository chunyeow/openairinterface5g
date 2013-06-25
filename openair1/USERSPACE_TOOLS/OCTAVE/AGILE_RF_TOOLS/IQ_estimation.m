function [ratio,phi_est]=IQ_estimation(freq)
gpib_card=0;      % first GPIB PCI card in the computer
sme=28;   % this is configured in the signal generator Utilities->System->GPIB->Address menu (for SME)
gpib_send(gpib_card,sme,'*RST;*CLS');   % reset and configure the signal generator
sleep(.1);
gpib_send(gpib_card,sme,'POW -63dBm');
disp(freq)
gpib_send(gpib_card,sme,strcat('FREQ ',int2str(freq),'MHz'));
gpib_send(gpib_card,sme,'OUTP:STAT ON'); %  activate output 

cmd1='/homes/nussbaum/openair1/ARCH/LEON3/hostpc/user/tools/scripts/set_receiver -M ';
gain1=61;
gain2=61;
frequency=int2str(freq);
cmd=strcat(cmd1,frequency,' -g1 ',int2str(gain1),' -g2 ',int2str(gain2));
system(cmd);
sleep(.1);
s=oarf_get_frame(0);
signal=s(:,1);


size_data=20480;
Fs=7680; %in kHz
band = 2000; %in kHz 
max_pos=0;
max=0;
f=abs(fft(signal)).^2;
for n=2:size_data
  if(f(n) > max)
    max=f(n);
    max_pos=n;
  end;
end;

disp(max_pos)
freq_max=Fs/size_data*max_pos - Fs/2   % corresponding frequency
if(abs(freq_max-2000)>100)
disp('Warning')
end;

cumI=0;
cumQ=0;

I=real(signal(1:2000));
Q=imag(signal(1:2000));

for i=1:2000
    cumI=cumI + I(i)*I(i);
    cumQ=cumQ + Q(i)*Q(i);
end;
amp= sqrt(2*cumI/length(I))
I=I/amp;
Q=Q/amp;
ratio=sqrt(cumI/cumQ)
n_fft=1024;
f=abs(fft(I(1:n_fft)));
max = 0;
index=0;
for i=1:n_fft/2
    if f(i)> max
        max=f(i);
        index=i;
    end;
end;
disp(max)
disp(index)
%figure
%plot(20*log10(abs(f)));
freq_estim=(index-1)/n_fft;
p=Q*ratio.*I;
sinphi=2*sum(p)/length(Q);
phi_est=-asin(sinphi)
Qcor=(sin(phi_est)*I+ratio*Q)/cos(phi_est);
%figure
%plot(I(1:100))
%hold on
%plot(Qcor(1:100),'g');
comp=I+j*Qcor;
%hold off
%figure
%plot(20*log10(abs(fft(comp))));
endfunction;
% per=round(1/freq_estim)
% figure;plot(I(1:per+1))
% opt_offset=-5
% min_diff=10^20;
% for offset=0:per
%     diff=0;
%     for k=1:n_fft
%         diff=diff+abs(I(k+offset)-Q(k));
%     end;
%     if diff<min_diff
%         min_diff=diff;
%         opt_offset= offset;
%     end;
% end;
% disp(opt_offset)
% % normally, as to be 3/4*per
% opt_offset=opt_offset-3*per/4; 
% disp(opt_offset/per*2*pi)
