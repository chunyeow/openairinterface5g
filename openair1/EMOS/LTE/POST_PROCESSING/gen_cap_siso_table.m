SNR = 0:10:20;
%h=sqrt(1/2).*(randn(1,1)+sqrt(-1)*randn(1,1));
h = exp(2*pi*1j*rand(1));
c_siso = zeros(size(SNR));
for v=1:length(SNR)
   N0=10^(-SNR(v)/10);
   %P=10^(SNR(v)/10);
   c_siso(v) = capacity_siso(h,N0)
end
