num=10;
openair_init;
for n=1:num
s=oarf_get_frame(0);
plot(20*log10(abs(fft(s(:,1)))))
signal=s(:,1);
fid=fopen(['/tmp/store' int2str(n) '.dat'],'wb')
fwrite(fid,signal);
fclose(fid);
end;



