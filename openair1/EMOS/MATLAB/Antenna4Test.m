%% parameters
NTx = 4; % Number of Tx antennas
NRx = 2; % Number of Rx antennas
NFrames = 10000;
NFreq = 160/NTx;
NTau = 256/NTx;

%% load data
filename_emos = 'Z:\EMOS\data\20071106_Antenna4Test\data_term1_idx0_20071106_113402.EMOS';
[path,name] = fileparts(filename_emos);

[H1,H1_fq,est,gps_data,NFrames] = load_estimates(filename_emos,NTx,NFrames);
%[H2,H2_fq,est2,nest2] = load_estimates('data_term2_idx0_20071009_144617.EMOS',NTx,NFrames);
% if (nest ~= nest2)
%     error('Sizes of Channel Matrices of different users don''t match');
% end

%% calculate and plot the PDP and frequency response over time
% The function Dprofile requires that size(H)=[N_t,N_tau,N_Tx,N_Rx]
% but load estimates returns size(H1) = [NRx,NTx,NTau,NFrames]
H1p = permute(H1,[4 3 2 1]);

for i=1:NTx
    DProf(i,:) = DProfile(H1p(:,:,i,1));
end

h_fig = figure(2);
plot((0:(NTau-1))/7.68,10*log10(DProf.'),'Linewidth',2)
xlabel('Delay [\mus]');
ylabel('Receive Power Level [dB]');
legend('1','2','3','4')
grid on
%saveas(h_fig, fullfile(path,[name '_Dprofile.eps']), 'psc2');
