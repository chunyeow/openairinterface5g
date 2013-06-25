clear all
close all

%% measured eigenvalue distribution
load eigenval_idx_32.mat

tmp = reshape(Eigval_4U_ZF,[],4);
for i=1:4
    [Eigval_4U_ZF_f(i,:),Eigval_4U_ZF_x(i,:)] = ecdf(tmp(:,i));
end
[Cond_4U_ZF_f,Cond_4U_ZF_x] = ecdf(tmp(:,4)./tmp(:,1));


%% iid case
M_Tx=4;
N_Rx=2;
Nfreq = 20;
Nsamples = 2000;
NUsers = 4;

SNRdB=10;
SNR=10^(SNRdB/10);
%%% MMSE regularization factor
mu = M_Tx/SNR;
%%% noise variance
n_p=1/SNR;

HH_4U_4x1=1/sqrt(2)*complex(randn(N_Rx,M_Tx,Nfreq,Nsamples,NUsers),randn(N_Rx,M_Tx,Nfreq,Nsamples,NUsers));

Eig_4U_ZF_iid=zeros(Nfreq,Nsamples,M_Tx);
Cap_4U_ZF_iid=zeros(Nfreq,Nsamples);

for k2=1:Nsamples
    for k1=1:Nfreq
        HS = HH_4U_4x1(1,:,k1,k2,:);
        HS = squeeze(HS).';
        [Cap_4U_ZF_iid(k1,k2),gain2,eigval] = MISO_MMSE(HS,n_p,0);
        Eig_4U_ZF_iid(k1,k2,:) = eigval; 
    end
end

tmp = reshape(Eig_4U_ZF_iid,[],4);
for i=1:4
    [Eigval_4U_ZF_iid_f(i,:),Eigval_4U_ZF_iid_x(i,:)] = ecdf(tmp(:,i));
end
[Cond_4U_ZF_iid_f,Cond_4U_ZF_iid_x] = ecdf(tmp(:,4)./tmp(:,1));

%% plot
figure(1)
plot(Eigval_4U_ZF_x.',Eigval_4U_ZF_f.');
hold on
plot(Eigval_4U_ZF_iid_x.',Eigval_4U_ZF_iid_f.','--');
xlabel('eigenvalue')
xlim([0 40])
ylabel('CDF')
title('Empirical CDF of the eigenvalues of H H''') 
legend('\lambda_1','\lambda_2','\lambda_3','\lambda_4', ...
    '\lambda_1 iid','\lambda_2 iid','\lambda_3 iid','\lambda_4 iid',...
    'Location','SouthEast')
grid on

h_fig = figure(2);
plot(Cond_4U_ZF_x,Cond_4U_ZF_f,'Linewidth',2)
hold on
plot(Cond_4U_ZF_iid_x,Cond_4U_ZF_iid_f,'--','Linewidth',2)
xlabel('condition number')
xlim([0 1000])
ylabel('CDF')
title('Empirical CDF of the condition number of H H^H','Fontsize',14,'Fontweight','bold') 
legend('measured','iid',    'Location','SouthEast')
grid on
saveas(h_fig, 'figs_spawc/idx_32_condition.eps', 'epsc2');
