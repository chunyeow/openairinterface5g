n2=1;

%% generate iid channel matrices
HH1=randn(1,4,n2,10000)/sqrt(2)+j*randn(1,4,n2,10000)/sqrt(2);
HH2=randn(2,4,n2,10000)/sqrt(2)+j*randn(2,4,n2,10000)/sqrt(2);
%HH2=randn(2,2,n2,10000)/sqrt(2)+j*randn(2,2,n2,10000)/sqrt(2);
HH4=randn(4,4,n2,10000)/sqrt(2)+j*randn(4,4,n2,10000)/sqrt(2);

HMU=randn(4,4,1,10000,4)/sqrt(2)+j*randn(4,4,1,10000,4)/sqrt(2);

M_Tx=4;
M_Rx=2;
SNR=10;

% fk: We do not need this normalization, since E(|h_{i,j}|^2) = 1!
% PP=squeeze(mean(mean(mean(abs(HH).^2,1),2),3));
% PP_mean=PP;
% PP=squeeze(mean(mean(mean(abs(HH2).^2,1),2),3));
% PP2_mean=PP;
% PP=squeeze(mean(mean(mean(abs(HH1).^2,1),2),3));
% PP1_mean=PP;

%% calcualte capacity
%Cap4x2_iid=zeros(size(HH,3),size(HH,4));
Cap1x4_iid=zeros(size(HH1,3),size(HH1,4));
Cap2x4_iid=zeros(size(HH1,3),size(HH1,4));
Cap4x4_iid=zeros(size(HH1,3),size(HH1,4));
for k1=1:size(HH1,3)
    for k2=1:size(HH1,4)
        %Cap4x2_iid(k1,k2)=abs(log2(det(diag(ones(2,1))+SNR/4*HH(:,:,k1,k2)*HH(:,:,k1,k2)')));
        Cap1x4_iid(k1,k2)=abs(log2(det(diag(ones(1))+SNR/4*HH1(:,:,k1,k2)*HH1(:,:,k1,k2)')));
        Cap2x4_iid(k1,k2)=abs(log2(det(diag(ones(2,1))+SNR/4*HH2(:,:,k1,k2)*HH2(:,:,k1,k2)')));
        Cap4x4_iid(k1,k2)=abs(log2(det(diag(ones(4,1))+SNR/4*HH4(:,:,k1,k2)*HH4(:,:,k1,k2)')));
    end
end

CAP_SU1x1_TDMA_ML_iid = capacity_SU_TDMA_ML(HMU(1,1,:,:,:),SNR,0);
CAP_SU2x1_TDMA_ML_iid = capacity_SU_TDMA_ML(HMU(1:2,1,:,:,:),SNR,0);
CAP_SU2x2_TDMA_ML_iid = capacity_SU_TDMA_ML(HMU(1:2,1:2,:,:,:),SNR,0);
CAP_SU_TDMA_ML_iid = capacity_SU_TDMA_ML(HMU(1:2,:,:,:,:),SNR,0);
CAP_SU_TDMA_DSTTD_iid = capacity_SU_TDMA_DSTTD(HMU(1:2,:,:,:,:),SNR,0);
CAP_SU2x2_TDMA_DSTTD_iid = capacity_SU_TDMA_DSTTD(HMU(1,1:2,:,:,:),SNR,0);
CAP_SU1x4_TDMA_CL_ML_iid = capacity_SU_TDMA_CL_ML(HMU(1,:,:,:,:),SNR,0);
CAP_SU2x4_TDMA_CL_ML_iid = capacity_SU_TDMA_CL_ML(HMU(1:2,:,:,:,:),SNR,0);
CAP_SU4x4_TDMA_CL_ML_iid = capacity_SU_TDMA_CL_ML(HMU(1:4,:,:,:,:),SNR,0);


%% plot results
%[Cap4x2_iid_f,Cap4x2_iid_x] = ecdf(Cap4x2_iid(:));
[Cap1x4_iid_f,Cap1x4_iid_x] = ecdf(Cap1x4_iid(:));
[Cap2x4_iid_f,Cap2x4_iid_x] = ecdf(Cap2x4_iid(:));
[Cap4x4_iid_f,Cap4x4_iid_x] = ecdf(Cap4x4_iid(:));

[Cap_SU_TDMA_ML_iid_f,Cap_SU_TDMA_ML_iid_x] = ecdf(CAP_SU_TDMA_ML_iid(:));
[Cap_SU1x1_TDMA_ML_iid_f,Cap_SU1x1_TDMA_ML_iid_x] = ecdf(CAP_SU1x1_TDMA_ML_iid(:));
[Cap_SU2x1_TDMA_ML_iid_f,Cap_SU2x1_TDMA_ML_iid_x] = ecdf(CAP_SU2x1_TDMA_ML_iid(:));
[Cap_SU2x2_TDMA_ML_iid_f,Cap_SU2x2_TDMA_ML_iid_x] = ecdf(CAP_SU2x2_TDMA_ML_iid(:));
[Cap_SU_TDMA_DSTTD_iid_f,Cap_SU_TDMA_DSTTD_iid_x] = ecdf(CAP_SU_TDMA_DSTTD_iid(:));
[Cap_SU2x2_TDMA_DSTTD_iid_f,Cap_SU2x2_TDMA_DSTTD_iid_x] = ecdf(CAP_SU2x2_TDMA_DSTTD_iid(:));
[Cap_SU1x4_TDMA_CL_ML_iid_f,Cap_SU1x4_TDMA_CL_ML_iid_x] = ecdf(CAP_SU1x4_TDMA_CL_ML_iid(:));
[Cap_SU2x4_TDMA_CL_ML_iid_f,Cap_SU2x4_TDMA_CL_ML_iid_x] = ecdf(CAP_SU2x4_TDMA_CL_ML_iid(:));
[Cap_SU4x4_TDMA_CL_ML_iid_f,Cap_SU4x4_TDMA_CL_ML_iid_x] = ecdf(CAP_SU4x4_TDMA_CL_ML_iid(:));

%%
figure(78)
hold off
%plot(Cap1x1_iid_x,Cap1x1_iid_f,'g')
plot(Cap1x4_iid_x,Cap1x4_iid_f)
hold on
plot(Cap2x4_iid_x,Cap2x4_iid_f,'r')
plot(Cap4x4_iid_x,Cap4x4_iid_f,'k')
plot(Cap_SU2x2_TDMA_ML_iid_x,Cap_SU2x2_TDMA_ML_iid_f,'c')
plot(Cap_SU_TDMA_DSTTD_iid_x,Cap_SU_TDMA_DSTTD_iid_f,'m')
%plot(Cap_SU2x2_TDMA_DSTTD_iid_x,Cap_SU2x2_TDMA_DSTTD_iid_f,'k')
%plot(Cap_SU1x4_TDMA_CL_ML_iid_x,Cap_SU1x4_TDMA_CL_ML_iid_f,'b--')
%plot(Cap_SU2x4_TDMA_CL_ML_iid_x,Cap_SU2x4_TDMA_CL_ML_iid_f,'r--')
%plot(Cap_SU4x4_TDMA_CL_ML_iid_x,Cap_SU4x4_TDMA_CL_ML_iid_f,'k--')
xlabel('bits/sec/Hz')
ylabel('CDF')
title('Capacity') 
legend('SU 4x1 iid','SU 4x2 iid','SU 4x4 iid','SU TDMA 2x2 iid ML','SU DSTTD TDMA 4x2 iid'); %,'SU CL TDMA 4x1 iid','SU CL TDMA 4x2 iid','SU CL TDMA 4x4 iid')
grid on

%% store results
% save('cap_iid.mat','-V7','Cap*x','Cap*f');