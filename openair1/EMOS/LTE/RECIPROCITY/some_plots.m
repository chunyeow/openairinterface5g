figure(14)
hold off
plot(real(fftshift(H_UE3(:,frame,ant))))
hold on
plot(imag(fftshift(H_UE3(:,frame,ant))),'r')

figure(15)
hold off
plot(real(fftshift(H_eNB3(:,frame,ant))))
hold on
plot(imag(fftshift(H_eNB3(:,frame,ant))),'r')

%%
tmp = zeros(301,1);
tmp(152:6:301) = H_UE2(6:6:155,frame,ant);
tmp(2:6:151) = H_UE2(157:6:306,frame,ant);

figure(16)
hold off
plot(20*log10(abs(ifft(conj(tmp)))))

figure(17)
hold off
plot(real(tmp))
hold on
plot(imag(tmp),'r')

%%
figure(9)
frame = 1;
hold off
plot(real(g(:,frame)))
hold on
plot(real(g_hat(:,frame)),'r')
figure(10)
hold off
plot(imag(g(:,frame)))
hold on
plot(imag(g_hat(:,frame)),'r')