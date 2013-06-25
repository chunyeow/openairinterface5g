%% generate iid channel matrices
M_Tx=4;
M_Rx=2;
NFreq=40;
NFrames=20000;
SNRdB=10;
SNR=10^(SNRdB/10);
K_rice = 2;

%% iid channel
H_iid = randn(M_Rx,M_Tx,NFreq,NFrames)/sqrt(2)+j*randn(M_Rx,M_Tx,NFreq,NFrames)/sqrt(2);
Cap4x2_iid = capacityEMOS(H_iid,SNR);
Cap2x2_iid = capacityEMOS(H_iid(:,1:2,:,:),SNR);
Cap1x1_iid = capacityEMOS(H_iid(1,1,:,:),SNR);
[Cap4x2_iid_f,Cap4x2_iid_x] = ecdf(Cap4x2_iid(:));
[Cap2x2_iid_f,Cap2x2_iid_x] = ecdf(Cap2x2_iid(:));
[Cap1x1_iid_f,Cap1x1_iid_x] = ecdf(Cap1x1_iid(:));

%% correlated channel
H_corr = repmat(H_iid(1,:,:,:),[2,1,1,1]);
Cap4x2_corr = capacityEMOS(H_corr,SNR);
[Cap4x2_corr_f,Cap4x2_corr_x] = ecdf(Cap4x2_corr(:));

%% rician channel
% H_LOS = repmat(randn(M_Rx,M_Tx,NFreq)/sqrt(2)+j*randn(M_Rx,M_Tx,NFreq)/sqrt(2),[1,1,1,NFrames]);
H_LOS = ones(M_Rx,M_Tx,NFreq,NFrames);
H_Rice = sqrt(K_rice/(1+K_rice)) * H_LOS + sqrt(1/(1+K_rice)) * H_iid;
Cap4x2_rice = capacityEMOS(H_Rice,SNR);
%Cap2x2_iid = capacityEMOS(H_iid(:,1:2,:,:),SNR);
%Cap1x1_iid = capacityEMOS(H_iid(1,1,:,:),SNR);
[Cap4x2_rice_f,Cap4x2_rice_x] = ecdf(Cap4x2_rice(:));
%[Cap2x2_iid_f,Cap2x2_iid_x] = ecdf(Cap2x2_iid(:));
%[Cap1x1_iid_f,Cap1x1_iid_x] = ecdf(Cap1x1_iid(:));

%% load the real channel 
%filename_emos = 'data_term1_idx0_20071128_190156.EMOS';
%[H1,H1_fq,est,gps_data,NFrames] = load_estimates(filename_emos);
%Cap4x2_meas = capacityEMOS(H1_fq,SNR);
%load data_term1_idx0_20071128_190156.mat
%[Cap4x2_meas_f,Cap4x2_meas_x] = ecdf(CAP1(:));

%% real channel 2
%filename_emos = 'data_term1_idx1_20071128_190238.EMOS';
%[H2,H2_fq,est2,gps_data2,NFrames2] = load_estimates(filename_emos);
%Cap4x2_meas2 = capacityEMOS(H2_fq,SNR);
%load data_term1_idx1_20071128_190238.mat
%[Cap4x2_meas2_f,Cap4x2_meas2_x] = ecdf(CAP1(:));


%% plot results
figure(78)
hold off
semilogy(Cap4x2_iid_x,Cap4x2_iid_f)
hold on
semilogy(Cap2x2_iid_x,Cap2x2_iid_f,'r')
semilogy(Cap1x1_iid_x,Cap1x1_iid_f,'g')
semilogy(Cap4x2_corr_x,Cap4x2_corr_f,'m')
semilogy(Cap4x2_rice_x,Cap4x2_rice_f,'k')
%plot(Cap4x2_meas_x,Cap4x2_meas_f,'k','Linewidth',2)
%plot(Cap4x2_meas2_x,Cap4x2_meas2_f,'--k','Linewidth',2)
xlabel('bits/sec/Hz')
ylabel('CDF')
title('Capacity') 
legend('SU 4x2 iid','SU 2x2 iid','SU 1x1 iid','SU 4x2 fully correlated','SU 4x2 meas small Rx spacing','SU 4x2 meas large Rx spacing')
grid on

%% store results
save('cap_iid_rice.mat','-V7','Cap4x2_iid_f','Cap4x2_iid_x','Cap2x2_iid_f','Cap2x2_iid_x','Cap1x1_iid_f','Cap1x1_iid_x','Cap4x2_meas_f','Cap4x2_meas_x','Cap4x2_meas2_f','Cap4x2_meas2_x','Cap4x2_rice_f','Cap4x2_rice_x');