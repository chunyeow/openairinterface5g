function chcap_siso_single_stream = capacity_siso_tab(SNR, M1)
%Channel Capacity of SISO system with QAM modulation

%M1=4;% QAM on first antenna
N = 100000;  % No of noise realizations
%seed(1) = 13579;
%seed(2) = 24680;
%rand('state', seed(1));
%randn('state', seed(2));
h = exp(2*pi*1j*rand(1));
N0=10^(-SNR/10);
sigmasq_x1=1;
[stream1]=sqrt(sigmasq_x1)*mapping(M1);     %x1   1x648
logsum_siso = 0;
for s1=1:M1
    x1=stream1(s1);
    for k=1:N
        z=sqrt(N0)*sqrt(1/2).*(randn(1,1)+sqrt(-1)*randn(1,1));
        alpha_siso=h;
        y_siso=alpha_siso*x1+z;
        den_siso=exp((-1/N0)*norm(y_siso-alpha_siso*x1)^2);
        neu_siso=0;
        for g=1:M1;
            x1_dd=stream1(g);
            neu_siso=neu_siso+exp((-1/N0)*norm(y_siso-alpha_siso*x1_dd)^2);
        end
        logsum_siso=logsum_siso+log2(neu_siso/den_siso);
    end
end
chcap_siso_single_stream=log2(M1)-logsum_siso/(M1*N);  % For SISO
