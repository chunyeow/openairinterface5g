%% parameters
load_channel = 1;
plot_figs = 1;

NTx = 4; % Number of Tx antennas
NRx = 2; % Number of Rx antennas
% NFrames = 10000;
NFreq = 160/NTx;
NTau = 256/NTx;

%% load data
%filename_emos = 'data_term1_idx4_20071011_103645.EMOS';
%filename_emos = 'data_term1_idx30_20071119_153735.EMOS';
%filename_emos = 'data_term3_idx30_20071126_153448.EMOS';
[path,name] = fileparts(filename_emos);

if load_channel
    [H1,H1_fq,est,gps_data,NFrames] = load_estimates(filename_emos,NTx);
else
    [est,gps_data,NFrames] = load_estimates_no_channel(filename_emos,NTx,Inf,0.2);
end    
%[H2,H2_fq,est2,nest2] = load_estimates('data_term2_idx0_20071009_144617.EMOS',NTx,NFrames);
% if (nest ~= nest2)
%     error('Sizes of Channel Matrices of different users don''t match');
% end
est.timestamp = est.timestamp - est.timestamp(1);

if load_channel
%% calculate and plot the PDP and frequency response over time
% The function Dprofile requires that size(H)=[N_t,N_tau,N_Tx,N_Rx]
% but load estimates returns size(H1) = [NRx,NTx,NTau,NFrames]
DProf = DProfile(permute(H1,[4 3 2 1]));

if plot_figs
h_fig = figure(1);
[X,Y] = meshgrid(est.timestamp,(-NFreq/2):(NFreq/2-1));
surf(X,Y,10*log10(abs(squeeze(H1_fq(1,1,:,:)).^2)));
shading interp
xlabel('Time [s]','Fontsize',14);
ylabel('Subcarrier index','Fontsize',14);
zlabel('Receive power [dB]','Fontsize',14);
grid on
saveas(h_fig, fullfile(path,[name '_freq_time.eps']), 'psc2');

%%
h_fig = figure(2);
plot((0:(NTau-1))/7.68,10*log10(DProf),'Linewidth',2)
xlabel('Delay [\mus]');
ylabel('Receive Power Level [dB]');
grid on
saveas(h_fig, fullfile(path,[name '_Dprofile.eps']), 'psc2');
end

%% calculate and plot single user capacity
% SNR is avaraged over all Rx antennas and all time
%SNR1 = 10^(mean(mean((est.rx_power(1:100)-est.n0_power(1:100))))/10);
%SNR2 = 10^(mean(mean((est2.rx_power(1:100)-est2.n0_power(1:100))))/10);
SNR1 = 10;
%SNR2 = 10;
CAP1 = capacity_SU_ML(H1_fq,SNR1);
%CAP2 = capacity_SU_ML(H2_fq(:,:,:,1:100),SNR2);
[Cap1_f,Cap1_x] = ecdf(CAP1);
%[Cap2_f,Cap2_x] = ecdf(CAP2);

if plot_figs
h_fig = figure(4);
load cap_iid.mat
hold off
plot(Cap2x4_iid_x,Cap2x4_iid_f)
hold on
%plot(Cap2x2_iid_x,Cap2x2_iid_f,'r')
%plot(Cap1x1_iid_x,Cap1x1_iid_f,'g')
plot(Cap1_x,Cap1_f,'k','Linewidth',2)
%plot(Cap2_x,Cap2_f,'k--','Linewidth',2)
legend('4x2 iid','4x2 measured')
xlabel('bits/sec/Hz')
xlim([0 18])
ylabel('CDF')
grid on
saveas(h_fig, fullfile(path,[name '_capacity.eps']), 'psc2');
end

%% calculate the RX correlation factor
RRx = zeros(NRx);
for k1=1:size(H1_fq,3)
    for k2=1:size(H1_fq,4)
        RRx = RRx + squeeze(H1_fq(:,:,k1,k2)) * squeeze(H1_fq(:,:,k1,k2))';
    end
end
RRx = RRx / (size(H1_fq,3)*size(H1_fq,4))

%% save calcualted data
filename_mat = fullfile(path,name);
save(filename_mat,'CAP1','DProf','RRx');
end

%% plot the received power over time
if plot_figs
h_fig = figure(3);
% err_ind = [0 diff(est.pdu_errors)];
% err_ind(err_ind<0) = 0;
% err_ind = logical(err_ind);
hold off
plot(est.timestamp,mean(est.rx_rssi_dBm,1),'b.')
hold on
plot(est.timestamp,mean(est.rx_power-est.n0_power,1),'g.')
plot(est.timestamp(est.err_ind),mean(est.rx_rssi_dBm(:,est.err_ind),1),'rx')
plot(est.timestamp(est.err_ind),mean(est.rx_power(:,est.err_ind)-est.n0_power(:,est.err_ind),1),'rx')
legend('RSSI','SNR')
xlabel('Time [s]','Fontsize',14);
ylabel('Power [dB]','Fontsize',14);
saveas(h_fig, fullfile(path,[name '_power_snr.eps']), 'psc2');

% %% plot GPS data
% h_fig = figure(5);
% plot_gps_coordinates_sophia(mm,gps_data.longitude,gps_data.latitude);
% saveas(h_fig, fullfile(path,[name '_gps']), 'jpg');
end
