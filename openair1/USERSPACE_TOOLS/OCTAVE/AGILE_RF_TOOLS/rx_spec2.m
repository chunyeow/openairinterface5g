openair_init;
i=30
while (i>0)
s=oarf_get_frame(0);
figure(1)
plot(20*log10(abs(fft(s(:,1)))))
figure(2)
plot(imag(s(:,1)))
i=i-1;
disp(i)
end
