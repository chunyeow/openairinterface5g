openair_init;
i=5
while (i>0)
s=oarf_get_frame(0);
plot(20*log10(abs(fft(s(:,1)))))
i=i-1;
disp(i)
end
