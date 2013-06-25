function megaMISO(SNRdB,Nsamples)

%%% Flags 
save_mode = 0;

Nfreq = 20;
% Nsamples = 2000;

% SNRdB=10;
M_Tx=4;
N_Rx=1;
SNR=10^(SNRdB/10);
%%% MMSE regularization factor
mu = M_Tx/SNR;
factor = 0.5*mu;
%%% noise variance
n_p=1/SNR;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Channel generation
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% generate iid channel matrices SU
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


% MU Capacity calculations - 2users
Cap_2U_MMSE_iid=zeros(Nfreq,Nsamples);
Cap_2U_MMSEfactor_iid=zeros(Nfreq,Nsamples);

Users=4;
for k=1:Users
HH_4U_4x1=1/sqrt(2)*complex(randn(N_Rx,M_Tx,Nfreq,Nsamples,k),randn(N_Rx,M_Tx,Nfreq,Nsamples,k));
end
HH_2U_4x1=HH_4U_4x1(:,:,:,:,[1:2]);



for k1=1:Nfreq
    for k2=1:Nsamples
    HS = HH_2U_4x1(:,:,k1,k2,[1:2]);
% HS = squeeze(HS).';
[gain,Cap_2U_MMSE_iid(k1,k2)] = MISO_MMSE(M_Tx,HS,n_p,mu);
[gain_factor,Cap_2U_MMSEfactor_iid(k1,k2)] = MISO_MMSE(M_Tx,HS,n_p,factor);

    end
end


% MU Capacity calculations - 4users
Cap_4U_MMSE_iid=zeros(Nfreq,Nsamples);

for k1=1:Nfreq
    for k2=1:Nsamples
    HS = HH_4U_4x1(:,:,k1,k2,[1:Users]);
% HS = squeeze(HS).';
[gain,Cap_4U_MMSE_iid(k1,k2)] = MISO_MMSE(M_Tx,HS,n_p,mu);
    end
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% CDF calculation
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% MU
[Cap_2U_MMSE_iid_f,Cap_2U_MMSE_iid_x] = ecdf(Cap_2U_MMSE_iid(:));
[Cap_2U_MMSEfactor_iid_f,Cap_2U_MMSEfactor_iid_x] = ecdf(Cap_2U_MMSEfactor_iid(:));
[Cap_4U_MMSE_iid_f,Cap_4U_MMSE_iid_x] = ecdf(Cap_4U_MMSE_iid(:));
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Plot results
figure(20)
hold off
plot(Cap_4U_MMSE_iid_x,Cap_4U_MMSE_iid_f,'r')
hold on
plot(Cap_2U_MMSE_iid_x,Cap_2U_MMSE_iid_f,'b')
plot(Cap_2U_MMSEfactor_iid_x,Cap_2U_MMSEfactor_iid_f,'k')
ylabel('CDF');
xlabel('Capacity (bits/ch.use)');
legend('MMSE 4x1 4us iid','MMSE 4x1 2 us iid ','MMSE 4x1 2 us iid factor test')
grid on
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



