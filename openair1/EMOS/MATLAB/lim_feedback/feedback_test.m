% This script compares the capacity of MU-MIMO using ZF/MMSE precoding 
% with and without feedback quantization
%
% Author: Florian Kaltenberger
% Date: 19.2.2008
% Last changes: 28.3.2008

clear all
close all

bit_vec = [4 6]; %  8  12 16];
N_User = 4;
N_Tx = 4;
N_runs = 100;
N_codebooks = 1;
%AoD = rand(1,N_User)*pi;
%AoD = [3.0070    1.5249    2.5142    0.4457];

SNRdB = 0:10:40;
%SNR = 10.^(SNRdB./10);
%n_p = 1./SNR;
%mu = N_Tx/SNR;

CAP_4U_MMSE_CVQ = zeros(length(SNRdB),N_runs,length(bit_vec));
CAP_4U_ZF_CVQ = zeros(length(SNRdB),N_runs,length(bit_vec));
CAP_4U_MMSE_RVQ = zeros(length(SNRdB),N_runs,length(bit_vec),N_codebooks);
CAP_4U_ZF_RVQ = zeros(length(SNRdB),N_runs,length(bit_vec),N_codebooks);
CAP_4U_MMSE = zeros(length(SNRdB),N_runs);
CAP_4U_ZF = zeros(length(SNRdB),N_runs);

%Hn = zeros(N_User,N_Tx);

%Codebook = codebookRVQ(N_Tx,max(bit_vec),N_codebooks);
load Codebook
Fourier_Codebook_ind = zeros(N_runs,length(bit_vec),N_User);
Random_Codebook_ind = zeros(N_runs,length(bit_vec),N_User);

%%
for d=1:length(SNRdB)
    SNR = 10.^(SNRdB(d)./10);
    n_p = 1./SNR;
    mu = N_Tx/SNR;

    for a = 1:N_runs
        disp(a)
        HS = complex(randn(N_User,N_Tx),randn(N_User,N_Tx))./sqrt(2); %+ ...
        %10*exp(1j*2*AoD.'*(0:N_Tx-1)./N_Tx);
        %10 * complex(randn(N_User,N_Tx),randn(N_User,N_Tx))./sqrt(2);
        %HS = HS./norm(HS);
 
        Random_Codebook_ind(a,:,:) = get_codebook_idx(HS,Codebook(:,1:pow2(max(bit_vec)),1:4),bit_vec);

        for b = 1:length(bit_vec)
            
            [HqF, Fourier_Codebook_ind(a,b,:)] = quantize_H(HS,bit_vec(b));

            CAP_4U_MMSE_CVQ(d,a,b) = MISO_CVQ(HS.',HqF.',n_p,mu);
            %CAP_4U_ZF_CVQ(d,a,b) = MISO_CVQ(HS.',HqF.',n_p,0);
            
            HqR = Codebook(:,Random_Codebook_ind(a,b,:),5);
            % Hq has to have the same amplitude as H
            for k=1:N_User
                HqR(:,k) = HqR(:,k).*norm(HS(:,k));
            end
            
            CAP_4U_MMSE_RVQ(d,a,b,1) = MISO_CVQ(HS.',HqR.',n_p,mu);
            %CAP_4U_ZF_RVQ(d,a,b,1) = MISO_CVQ(HS.',HqR.',n_p,0);
           
        end

        CAP_4U_MMSE(d,a) = MISO_MMSE(HS.',n_p,mu);
        %CAP_4U_ZF(d,a) = MISO_MMSE(HS.',n_p,0);

    end
end

% CAP(isnan(CAP)) = 0;

%%
%save cap_lim_feedback_iid.mat

%% 
ECAP_4U_MMSE_CVQ = squeeze(mean(mean(CAP_4U_MMSE_CVQ,2),4));
ECAP_4U_MMSE_RVQ = squeeze(mean(mean(CAP_4U_MMSE_RVQ,2),4));
ECAP_4U_MMSE = squeeze(mean(CAP_4U_MMSE,2));

%%
figure(2)
hold off
plot(bit_vec,squeeze(mean(mean(CAP_4U_MMSE_CVQ,2),4)))
hold on
plot(bit_vec,squeeze(mean(mean(CAP_4U_MMSE_RVQ,2),4)),'r')
plot(bit_vec,repmat(mean(CAP_4U_MMSE),length(bit_vec),1),'k')
legend('CVQ feedback','RVQ feedback','perfect feedback')
xlabel('feedback bits')
ylabel('capacity')

%%
figure(3)
hold off
cdfplot(CAP_4U_MMSE_CVQ(end,:,end,:))
hold on
h=cdfplot(CAP_4U_MMSE_RVQ(end,:,end,:));
set(h,'color','r');
h=cdfplot(CAP_4U_MMSE(end,:));
set(h,'color','k');
legend('CVQ feedback','RVQ feedback', 'perfect feedback')
