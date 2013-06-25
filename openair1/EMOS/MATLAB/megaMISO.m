function megaMISO(SNRdB,Nsamples)

%%% Flags
save_mode = 0;

bits = 8;

M_Tx=4;
N_Rx=1;
Nfreq = 20;
% Nsamples = 2000;
NUsers = 4;

% SNRdB=10;
SNR=10^(SNRdB/10);
%%% MMSE regularization factor
mu = M_Tx/SNR;
%%% noise variance
n_p=1/SNR;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Channel generation
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% generate iid channel matrices SU
HH_4x2=1/sqrt(2)*complex(randn(2,4,Nfreq,Nsamples),randn(2,4,Nfreq,Nsamples));
HH_2x2=1/sqrt(2)*complex(randn(2,2,Nfreq,Nsamples),randn(2,2,Nfreq,Nsamples));
HH_1x1=1/sqrt(2)*complex(randn(1,1,Nfreq,Nsamples),randn(1,1,Nfreq,Nsamples));
HH_4x1=1/sqrt(2)*complex(randn(1,4,Nfreq,Nsamples),randn(1,4,Nfreq,Nsamples));
HH_4x4=1/sqrt(2)*complex(randn(4,4,Nfreq,Nsamples),randn(4,4,Nfreq,Nsamples));

% generate iid channel matrices MU
HH_4U_4x1=1/sqrt(2)*complex(randn(N_Rx,M_Tx,Nfreq,Nsamples,NUsers),randn(N_Rx,M_Tx,Nfreq,Nsamples,NUsers));
HH_2U_4x1=HH_4U_4x1(:,:,:,:,1:2);
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%% SU Capacity calculations
Cap_SU_4x2_iid=zeros(Nfreq,Nsamples);
Cap_SU_2x2_iid=zeros(Nfreq,Nsamples);
Cap_SU_1x1_iid=zeros(Nfreq,Nsamples);
Cap_SU_4x1_iid=zeros(Nfreq,Nsamples);
Cap_SU_4x4_iid=zeros(Nfreq,Nsamples);

%eff_gain_ZF = zeros(1,Nsamples);
%eff_gain_MMSE = zeros(1,Nsamples);
%eff_gain_BD = zeros(2,Nsamples);

for k1=1:Nfreq
    for k2=1:Nsamples
        Cap_SU_4x2_iid(k1,k2)=abs(log2(det(eye(2)+SNR/4*HH_4x2(:,:,k1,k2)*HH_4x2(:,:,k1,k2)')));
        Cap_SU_2x2_iid(k1,k2)=abs(log2(det(eye(2)+SNR/2*HH_2x2(:,1:2,k1,k2)*HH_2x2(:,1:2,k1,k2)')));
        Cap_SU_1x1_iid(k1,k2)=abs(log2(1+SNR*HH_1x1(1,1,k1,k2)*HH_1x1(1,1,k1,k2)'));
        Cap_SU_4x1_iid(k1,k2)=abs(log2(1+SNR/4*HH_4x1(:,:,k1,k2)*HH_4x1(:,:,k1,k2)'));
        Cap_SU_4x4_iid(k1,k2)=abs(log2(det(eye(4)+SNR/4*HH_4x4(:,:,k1,k2)*HH_4x4(:,:,k1,k2)')));
    end
end
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%% MU Capacity calculations - 2users
Cap_2U_MMSE_iid=zeros(Nfreq,Nsamples);
Cap_2U_ZF_iid=zeros(Nfreq,Nsamples);


for k2=1:Nsamples
    [HSf,antenna_index] = antenna_selection(HH_2U_4x1(:,:,:,k2,:));
    for k1=1:Nfreq
        HS = HSf(1,:,k1,1,:);
        HS = squeeze(HS).';
        [Cap_2U_MMSE_iid(k1,k2),gain] = MISO_MMSE(HS,n_p,mu);
        [Cap_2U_ZF_iid(k1,k2),gain2] = MISO_MMSE(HS,n_p,0);
        %eff_gain_MMSE(k2) = gain;
        %eff_gain_ZF(k2) = gain2;
     end
end

% %% MU Capacity calculations - block diagonalization
% Cap_2U_BD_iid=zeros(Nfreq,Nsamples);
% Cap_2U_BD_WF_iid=zeros(Nfreq,Nsamples);
% 
% for k2=1:Nsamples
%     for k1=1:Nfreq
%         if N_Rx == 1
%             NUsers = 4;
%             HS = reshape(HH_4U_4x1(:,:,k1,k2,:),[N_Rx, M_Tx, NUsers]);
%         elseif N_Rx == 2
%             NUsers = 2;
%             HS = reshape(HH_2U_4x1(:,:,k1,k2,:),[N_Rx, M_Tx, NUsers]);
%         else 
%             error('N_Rx must be 1 or 2');
%         end
%         [Cap_2U_BD_iid(k1,k2),gain] = BD(HS,n_p,0);
%         [Cap_2U_BD_WF_iid(k1,k2),gain] = BD(HS,n_p,1);
%         %eff_gain_BD(:,k2) = gain;
%     end
% end


%% MU Capacity calculations - 4users
Cap_4U_MMSE_iid=zeros(Nfreq,Nsamples);
Cap_4U_ZF_iid=zeros(Nfreq,Nsamples);
Cap_4U_CVQ_iid=zeros(Nfreq,Nsamples);

for k2=1:Nsamples
    [HSf,antenna_index] = antenna_selection(HH_4U_4x1(:,:,:,k2,:));
    for k1=1:Nfreq
        %HS = HSf(1,:,k1,1,:);
        HS = HH_4U_4x1(1,:,k1,k2,:);
        HS = squeeze(HS).';
        [Cap_4U_MMSE_iid(k1,k2),gain] = MISO_MMSE(HS,n_p,mu);
        [Cap_4U_ZF_iid(k1,k2),gain2] = MISO_MMSE(HS,n_p,0);
        Cap_4U_CVQ_iid(k1,k2) = MISO_CVQ(HS,n_p,0,bits);
        %eff_gain_MMSE(k2) = gain;
        %eff_gain_ZF(k2) = gain2;
    end
end

% %% DPC
% Cap_4U_DPC_iid = zeros(Nfreq,Nsamples);
% for k2=1:Nsamples
%     for k1=1:Nfreq
%         HS = conj(permute(HH_4U_4x1(:,:,k1,k2,:),[2 1 3 4 5]));
%         Cap_4U_DPC_iid(k1,k2) = iterative_waterfill(reshape(HS,[M_Tx,N_Rx,NUsers]),SNR,50);
%     end
% end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% CDF calculation
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% SU
[Cap_SU_4x2_iid_f,Cap_SU_4x2_iid_x] = ecdf(Cap_SU_4x2_iid(:));
[Cap_SU_2x2_iid_f,Cap_SU_2x2_iid_x] = ecdf(Cap_SU_2x2_iid(:));
[Cap_SU_1x1_iid_f,Cap_SU_1x1_iid_x] = ecdf(Cap_SU_1x1_iid(:));
[Cap_SU_4x1_iid_f,Cap_SU_4x1_iid_x] = ecdf(Cap_SU_4x1_iid(:));
[Cap_SU_4x4_iid_f,Cap_SU_4x4_iid_x] = ecdf(Cap_SU_4x4_iid(:));

% MU
[Cap_2U_MMSE_iid_f,Cap_2U_MMSE_iid_x] = ecdf(Cap_2U_MMSE_iid(:));
[Cap_2U_ZF_iid_f,Cap_2U_ZF_iid_x] = ecdf(Cap_2U_ZF_iid(:));
%[Cap_2U_BD_iid_f,Cap_2U_BD_iid_x] = ecdf(Cap_2U_BD_iid(:));
%[Cap_2U_BD_WF_iid_f,Cap_2U_BD_WF_iid_x] = ecdf(Cap_2U_BD_WF_iid(:));
[Cap_4U_MMSE_iid_f,Cap_4U_MMSE_iid_x] = ecdf(Cap_4U_MMSE_iid(:));
[Cap_4U_ZF_iid_f,Cap_4U_ZF_iid_x] = ecdf(Cap_4U_ZF_iid(:));
[Cap_4U_CVQ_iid_f,Cap_4U_CVQ_iid_x] = ecdf(Cap_4U_CVQ_iid(:));
%[Cap_4U_DPC_iid_f,Cap_4U_DPC_iid_x] = ecdf(Cap_4U_DPC_iid(:));
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Plot results
figure(20)
hold off
%plot(Cap_SU_4x2_iid_x,Cap_SU_4x2_iid_f,'k')
hold on
plot(Cap_SU_2x2_iid_x,Cap_SU_2x2_iid_f,'k')
%plot(Cap_SU_1x1_iid_x,Cap_SU_1x1_iid_f,'g')
%plot(Cap_SU_4x1_iid_x,Cap_SU_4x1_iid_f,'y')
%plot(Cap_SU_4x4_iid_x,Cap_SU_4x4_iid_f,'m')
plot(Cap_4U_MMSE_iid_x,Cap_4U_MMSE_iid_f,'g')
plot(Cap_4U_ZF_iid_x,Cap_4U_ZF_iid_f,'r')
plot(Cap_4U_CVQ_iid_x,Cap_4U_CVQ_iid_f,'m')
%plot(Cap_4U_DPC_iid_x,Cap_4U_DPC_iid_f,'b')
%plot(Cap_2U_MMSE_iid_x,1-Cap_2U_MMSE_iid_f,'b')
%plot(Cap_2U_ZF_iid_x,1-Cap_2U_ZF_iid_f,'c')
%plot(Cap_2U_BD_iid_x,Cap_2U_BD_iid_f,'k','Linewidth',2)
%  plot(Cap_2U_BD_WF_iid_x,Cap_2U_BD_WF_iid_f,'y','Linewidth',2)

ylabel('CDF');
xlabel('Capacity (bits/ch.use)');
legend('SU 4x2 iid','SU 4x1 iid','SU 4x4 iid','MMSE 4x1 4us iid','ZF 4x1 4us iid','DPC 4x1 4us iid');
%'MMSE 4x1 2 us iid ','ZF 4x1 2us iid','BD 4x2 2us iid','BD 4x2 2us WF iid')
grid on
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%
% figure(40)
% plot([1:Nsamples],eff_gain_ZF,'-r',[1:Nsamples],eff_gain_MMSE,'-b')
% ylabel('effective gain');
% xlabel('sample index');
% legend('ZF','MMSE')
% grid on

%keyboard

% store results
if (save_mode == 1)
    save(sprintf('Cap_iid_N_Rx=%d.mat',N_Rx),'Cap_SU_4x2_iid','Cap_SU_4x2_iid_f','Cap_SU_4x2_iid_x','Cap_SU_4x1_iid','Cap_SU_4x1_iid_f','Cap_SU_4x1_iid_x','Cap_SU_4x4_iid','Cap_SU_4x4_iid_f','Cap_SU_4x4_iid_x','Cap_2U_MMSE_iid','Cap_2U_MMSE_iid_f','Cap_2U_MMSE_iid_x','Cap_4U_MMSE_iid','Cap_4U_MMSE_iid_f','Cap_4U_MMSE_iid_x','Cap_2U_ZF_iid','Cap_2U_ZF_iid_f','Cap_2U_ZF_iid_x','Cap_4U_ZF_iid','Cap_4U_ZF_iid_f','Cap_4U_ZF_iid_x','Cap_2U_BD_iid_f','Cap_2U_BD_iid_x','Cap_2U_BD_WF_iid_x','Cap_2U_BD_WF_iid_f','Cap_4U_DPC_iid_x','Cap_4U_DPC_iid_f');
end

