function [h1 h2] = channel(N0);

%z=sqrt(1/2).*(randn(2,1)+j*randn(2,1));
%z=N0.*z;     %incorporating SNR in the noise
h1=sqrt(1/2).*(randn(2,1)+sqrt(-1)*randn(2,1));
h2=sqrt(1/2).*(randn(2,1)+sqrt(-1)*randn(2,1));










