% Channel Capacity of LTE mode 5 with the eNodeB with 2 antennas and 2
% single antenna users using the scheduling algorithm of aligning
% interference in the opposite quadrant


% clear;
% clc;
% close all;
% warning off all
% format('long');

%Input Data
%  7 LTE with scheduling in opposite quadrant or without scheduling
sigmasq_x2_all = 0:0.1:1;
for i = 1:length(sigmasq_x2_all)
 M1=16;% QAM on first antenna
 M2=4;% QAM on second antenna
 SNR = [0:1:50];
 N = 100000;  % No of channel realizations

for v=1:length(SNR)
    seed(1) = 13579;
    seed(2) = 24680;
    rand('state', seed(1));
    randn('state', seed(2));
    N0=10^(-SNR(v)/10);
    sigmasq_x1 = 1;
    sigmasq_x2 = sigmasq_x2_all(i);
    [stream1]=sqrt(sigmasq_x1)*mapping(M1);     %x1   1x648
    [stream2]=sqrt(sigmasq_x2)*mapping(M2);     %x2   1x648
    logsum_lte_sch1=0;
    logsum_lte_sch2=0;
    for s1=1:M1
        x1=stream1(s1);
        for s2=1:M2
            x2=stream2(s2);
            for k=1:N
                [h1 h2]=channel(N0);
                z=sqrt(N0)*sqrt(1/2).*(randn(1,1)+sqrt(-1)*randn(1,1));
                H=[h1 h2];  %first row is the channel to user-1 and second row is the channel to user-2
                match_precoder_norm=(1/sqrt(2))*[H(1,:)'/norm(H(1,:)) H(2,:)'/norm(H(2,:))]; %Frobenius norm is 1
                lte_precoder=[(match_precoder_norm(1,1))'*match_precoder_norm(:,1)  (match_precoder_norm(1,2))'*match_precoder_norm(:,2)];
                lte_precoder_norm=[lte_precoder(:,1)/lte_precoder(1,1)  lte_precoder(:,2)/lte_precoder(1,2)];
                lte_std_precoder=[1 -1 sqrt(-1) -sqrt(-1)];
                [val pos]=min([-real(lte_precoder(2,1))  real(lte_precoder(2,1))  -imag(lte_precoder(2,1))   imag(lte_precoder(2,1))]);
                lte_precoder_norm1=[1;lte_std_precoder(pos)];
                [val pos]=min([-real(lte_precoder(2,2))  real(lte_precoder(2,2))  -imag(lte_precoder(2,2))   imag(lte_precoder(2,2))]);
                lte_precoder_norm2=[1;lte_std_precoder(pos)];
                lte_precoder_qtz_orig=(1/sqrt(4))*[ lte_precoder_norm1   lte_precoder_norm2];                 
                %Scheduling to put interference in the opposite quadrant
                lte_precoder_qtz1=[ lte_precoder_qtz_orig(1,1)   lte_precoder_qtz_orig(1,2); lte_precoder_qtz_orig(2,1)   -lte_precoder_qtz_orig(2,1)];  %For opposite precoder
                lte_precoder_qtz2=[ lte_precoder_qtz_orig(1,1)   lte_precoder_qtz_orig(1,2); -lte_precoder_qtz_orig(2,2)   lte_precoder_qtz_orig(2,2)];  %For opposite precoder              
                alpha_1=H(1,:)*lte_precoder_qtz1(:,1);
                beta_2=H(1,:)*lte_precoder_qtz1(:,2);
                beta_1=H(2,:)*lte_precoder_qtz2(:,1);
                alpha_2=H(2,:)*lte_precoder_qtz2(:,2);
                y_lte1=alpha_1*x1+beta_2*x2+z;
                y_lte2=beta_1*x1+alpha_2*x2+z;
                den_lte_sch1=0;
                den_lte_sch2=0;
                %For first stream
                for t=1:M2
                    x2_d=stream2(t);
                    den_lte_sch1=den_lte_sch1+exp((-1/N0)*norm(y_lte1-H(1,:)*lte_precoder_qtz1(:,1)*x1-H(1,:)*lte_precoder_qtz1(:,2)*x2_d)^2);
                end
                % For second stream
                for t=1:M1
                    x1_d=stream1(t);
                    den_lte_sch2=den_lte_sch2+exp((-1/N0)*norm(y_lte2-H(2,:)*lte_precoder_qtz2(:,1)*x1_d-H(2,:)*lte_precoder_qtz2(:,2)*x2)^2);
                end
                neu_lte_sch1=0;
                neu_lte_sch2=0;
                for g=1:M1;
                    x1_dd=stream1(g);
                    for h=1:M2
                        x2_dd=stream2(h);
                        neu_lte_sch1=neu_lte_sch1+exp((-1/N0)*norm(y_lte1-H(1,:)*lte_precoder_qtz1(:,1)*x1_dd-H(1,:)*lte_precoder_qtz1(:,2)*x2_dd)^2);
                    end
                end
                %For second stream
                for g=1:M2;
                    x2_ddd=stream2(g);
                    for h=1:M1
                        x1_ddd=stream1(h);
                        neu_lte_sch2=neu_lte_sch2+exp((-1/N0)*norm(y_lte2-H(2,:)*lte_precoder_qtz2(:,1)*x1_ddd-H(2,:)*lte_precoder_qtz2(:,2)*x2_ddd)^2);
                    end
                end
                logsum_lte_sch1=logsum_lte_sch1+log2(neu_lte_sch1/den_lte_sch1);
                logsum_lte_sch2=logsum_lte_sch2+log2(neu_lte_sch2/den_lte_sch2);
            end
        end
    end

    chcap_lte_sch1(i, v)=log2(M1)-logsum_lte_sch1/(M1*M2*N);
    chcap_lte_sch2(i, v)=log2(M2)-logsum_lte_sch2/(M1*M2*N);
    chcap_lte_sch(i, v)=log2(M1)-logsum_lte_sch1/(M1*M2*N)+log2(M2)-logsum_lte_sch2/(M1*M2*N);
end    
end

save QAM16_QPSK_lte_Mode5.mat;  







