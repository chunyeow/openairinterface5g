frequency = input('Input freq in MHz: ', 's'); 
num=1;
openair_init;
for n=1:num
s=oarf_get_frame(0);
plot(20*log10(abs(fft(s(:,1)))))
signal=s(:,1);
disp(signal(1:10));
fid=fopen(['/tmp/' frequency '_ref' int2str(n) '.dat'],'wb')
sig=zeros(2*length(signal),1);
sig(1:2:length(sig))=real(signal);
sig(2:2:length(sig))=imag(signal);
fwrite(fid,sig,'float');
fclose(fid);
end;



