Nfreq = 20;
Nsamples = 2000;
SNRdB=10;
M_Tx=4;
N_Rx=2;
SNR=10^(SNRdB/10);
%%% MMSE regularization factor
mu = M_Tx/SNR;
%%% noise variance
n_p=1/SNR;

Users=4;
for k=1:Users
    HH_4U_4x1=1/sqrt(2)*complex(randn(N_Rx,M_Tx,Nfreq,Nsamples,k),randn(N_Rx,M_Tx,Nfreq,Nsamples,k));
end


Cap_2U_BD_iid=zeros(Nfreq,Nsamples);
Cap_2U_BD_WF_iid=zeros(Nfreq,Nsamples);

for k2=1:Nsamples
    for k1=1:Nfreq
        HS = squeeze(HH_4U_4x1(:,:,k1,k2,:));
        [Cap_2U_BD_iid(k1,k2),gain] = BD(HS(1,:,:),n_p,0);
        [Cap_2U_BD_WF_iid(k1,k2),gain] = BD(HS(1,:,:),n_p,1);
        
     end
end


[Cap_2U_BD_iid_f,Cap_2U_BD_iid_x] = ecdf(Cap_2U_BD_iid(:));
[Cap_2U_BD_WF_iid_f,Cap_2U_BD_WF_iid_x] = ecdf(Cap_2U_BD_WF_iid(:));


load Cap_iid_N_Rx=1.mat
plot(Cap_4U_ZF_iid_x,1-Cap_4U_ZF_iid_f,'r')
hold on
plot(Cap_2U_BD_iid_x,1-Cap_2U_BD_iid_f,'k','Linewidth',2)
plot(Cap_2U_BD_WF_iid_x,1-Cap_2U_BD_WF_iid_f,'y','Linewidth',2)

ylabel('CCDF');
xlabel('Capacity (bits/ch.use)');
grid on
legend('ZF 4x1 iid')