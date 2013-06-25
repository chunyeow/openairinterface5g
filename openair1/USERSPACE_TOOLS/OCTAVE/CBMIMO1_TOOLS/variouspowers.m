function retvalue=variouspowers(s)

W=25;

fftsize=2^ceil(log2(length(s)));
powspectrum=fftshift(abs(fft(s-mean(s),fftsize)/sqrt(fftsize)));

powspectrumfilt=conv(powspectrum,ones(1,2*W+1));

[sigpower carrier_idx]=max(powspectrumfilt);

if (carrier_idx<=3*W)
  carrier_idx=3*W+1;
end
sigpower_dB=20*log10(sigpower);


if (carrier_idx>3*W)

 noiseplusintfspectrum=powspectrum;
 noiseplusintfspectrum(carrier_idx+(-W:W)-2*W)=zeros(1,2*W+1);

 noiseplusintfpower_dB=10*log10(sum(noiseplusintfspectrum.^2));

 noiseplusintfspectrum_dB=20*log10(noiseplusintfspectrum);
 noiseplusintfspectrum_dB(carrier_idx+(-W:W)-2*W)=-20;

%plot([  20*log10(powspectrum)  noiseplusintfspectrum_dB])
%pause
 retvalue=[sigpower_dB noiseplusintfpower_dB];
else
  retvalue=[sigpower_dB +Inf];
end
