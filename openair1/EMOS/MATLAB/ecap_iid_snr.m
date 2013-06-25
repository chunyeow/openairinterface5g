clear all
close all

M_Tx=4;
N_Rx=1;
Nfreq = 20;
Nsamples = 1000;
NUsers = 2;
bits = 8;

SNRdB = -20:10:30;

HDl = 1/sqrt(2)*complex(randn(N_Rx,M_Tx,Nfreq,Nsamples,NUsers),randn(N_Rx,M_Tx,Nfreq,Nsamples,NUsers));
%HUl = conj(permute(HDl,[2 1 3 4 5]));

%ECap_SU_TDMA_CL_ML_iid=zeros(1,length(SNRdB));
ECap_MU_MMSE_iid=zeros(1,length(SNRdB));
ECap_MU_ZF_iid=zeros(1,length(SNRdB));
%ECap_MU_DPC_iid=zeros(1,length(SNRdB));
%ECap_MU_BD_WF_iid=zeros(1,length(SNRdB));
%ECap_MU_CVQ_iid=zeros(1,length(SNRdB));

Cap_SU_TDMA_CL_ML_iid = capacity_SU_TDMA_CL_ML(HDl,10.^(SNRdB./10),0);
ECap_SU_TDMA_CL_ML_iid = mean(Cap_SU_TDMA_CL_ML_iid,1);

for s = 1:length(SNRdB)

    disp(sprintf('SNR = %d (%d of %d)',SNRdB(s),s,length(SNRdB)))

    % SNRdB=10;
    SNR=10^(SNRdB(s)/10);
    %%% MMSE regularization factor
    mu = M_Tx/SNR;
    %%% noise variance
    n_p=1/SNR;

    %Cap_MU_CVQ_iid = zeros(Nfreq,Nsamples);
    Cap_MU_MMSE_iid = zeros(Nfreq,Nsamples);
    Cap_MU_ZF_iid = zeros(Nfreq,Nsamples);
    %Cap_MU_DPC_iid = zeros(Nfreq,Nsamples);
    %Cap_MU_BD_WF_iid=zeros(Nfreq,Nsamples);


    for k2=1:Nsamples
        for k1=1:Nfreq
            HS = HDl(1,:,k1,k2,:);
            HS = squeeze(HS).';
            [Cap_MU_MMSE_iid(k1,k2),gain] = MISO_MMSE(HS,n_p,mu);
            [Cap_MU_ZF_iid(k1,k2),gain2] = MISO_MMSE(HS,n_p,0);
            %Cap_MU_CVQ_iid(k1,k2) = MISO_CVQ(HS,n_p,0,bits);
            %eff_gain_MMSE(k2) = gain;
            %eff_gain_ZF(k2) = gain2;
        end
    end


%     for k2=1:Nsamples
%         for k1=1:Nfreq
%             HS = reshape(HDl(:,:,k1,k2,:),[N_Rx, M_Tx, NUsers]);
%             [Cap_MU_BD_WF_iid(k1,k2),gain] = BD(HS,n_p,1);
%             %eff_gain_BD(:,k2) = gain;
%         end
%     end


%     for k2=1:Nsamples
%         for k1=1:Nfreq
%             HS = HUl(:,:,k1,k2,:);
%             HS = reshape(HS,[M_Tx,N_Rx,NUsers]);
%             Cap_MU_DPC_iid(k1,k2) = iterative_waterfill(HS,SNR,50);
%         end
%     end

    ECap_MU_MMSE_iid(s)=mean(Cap_MU_MMSE_iid(:));
    ECap_MU_ZF_iid(s)=mean(Cap_MU_ZF_iid(:));
    %ECap_MU_DPC_iid(s)=mean(Cap_MU_DPC_iid(:));
    %ECap_MU_BD_WF_iid(s)=mean(Cap_MU_BD_WF_iid(:));
    %ECap_4U_CVQ_iid(s)=mean(Cap_4U_CVQ_iid(:));

%%
    figure(1)
    plot(SNRdB(1:s),ECap_SU_TDMA_CL_ML_iid(1:s),'b');
    hold on
    plot(SNRdB(1:s),ECap_MU_MMSE_iid(1:s),'r');
    plot(SNRdB(1:s),ECap_MU_ZF_iid(1:s),'g');
    %plot(SNRdB(1:s),ECap_4U_CVQ_iid(1:s),'k');
    %plot(SNRdB(1:s),ECap_MU_DPC_iid(1:s),'k');
    %plot(SNRdB(1:s),ECap_MU_BD_WF_iid(1:s),'m');
    legend('SU TDMA','MMSE','ZF')
    xlabel('SNR [dB]')
    ylabel('Sum rate')
    drawnow

%%
    save(sprintf('cap_iid_NTx%d_NRx%d_NUser%d.mat',M_Tx,N_Rx,NUsers));

end
