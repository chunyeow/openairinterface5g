La = 5;
M = 512;
L = 30;
ant = 2;
frames = 201:210;
%frames2 = frames;
frames2 = 501:510;

%method = 'tlsdeconv';
method = 'tlsdeconvangles';


%%
%H_UE2 = reshape(permute(H_UE(frames,2049:2560,:),[2 1 3]),512,[],2);
H_UE2 = reshape(permute(H_UE(:,2561:3072,:),[2 1 3]),512,[],2);
H_UE3 = complex(zeros(size(H_UE2)));
H_UE3(363:512,:,:) = H_UE2(6:155,:,:); 
H_UE3(1:151,:,:) = H_UE2(156:306,:,:);
H_UE_t = ifft(conj(H_UE3),512,1); %the channel estimates are stored in conjugated format
H_UE_t = circshift(H_UE_t,[-15,0,0]); %shift the peak to the desired position
figure(2)
hold off
%waterfall(squeeze(20*log10(abs(H_UE_t))).')
plot(squeeze(20*log10(abs(H_UE_t(:,frames,ant)))))
title('PDP DL channel')

%%
H_eNB2 = reshape(permute(H_eNB(:,1501:1800,:),[2 1 3]),300,[],2);
H_eNB3 = complex(zeros(512,NFrames,2));
H_eNB3(363:512,:,:) = H_eNB2(1:150,:,:);
H_eNB3(1:150,:,:) = H_eNB2(151:300,:,:);
H_eNB_t = ifft(H_eNB3,512,1);
H_eNB_t = circshift(H_eNB_t,[8,0,0]);
figure(3)
hold off
%waterfall(squeeze(20*log10(abs(H_eNB_t))).')
plot(squeeze(20*log10(abs(H_eNB_t(:,frames,ant)))))
title('PDP UL channel')

%% compensate the time drift
Lanczos=@(x,a)sinc(x).*sinc(x/a);
Lt=@(t)toeplitz([Lanczos((0:-1:-La)+t,La).'; zeros(M-1-La,1)],[Lanczos((0:1:La)+t,La) zeros(1,M-1-La)]);

abs_timing_offset=double([estimates_UE(1:1000).timing_offset]);
slope = (abs_timing_offset(1000)-abs_timing_offset(122))./(1000-122);
rel_timing_offset=abs_timing_offset-abs_timing_offset(122)-slope*(-121:1000-122);

for i=1:NFrames
    for j=1:2
    H_UE_t(:,i,j) = Lt(rel_timing_offset(i))*H_UE_t(:,i,j);
    H_eNB_t(:,i,j) = Lt(-rel_timing_offset(i))*H_eNB_t(:,i,j);
    end
end

H_UE_t = H_UE_t(1:L,:,:);
H_eNB_t = H_eNB_t(1:L,:,:);

figure(12)
hold off
waterfall(squeeze(20*log10(abs(H_UE_t(:,frames,ant)))).')
title('PDP compensated DL channel')
figure(13)
hold off
waterfall(squeeze(20*log10(abs(H_eNB_t(:,frames,ant)))).')
title('PDP compensated UL channel')

%%
addpath('E:\Synchro\kaltenbe\My Documents\Matlab\reciprocity')
clear p
for j=1:2
    if strcmp(method,'tlsdeconvangles')
        fprintf('\ntlsdeconvangles\n')
        [p(:,j),phi_hat,costnew(j)]=tlsdeconvangles(H_UE_t(:,frames,j),H_eNB_t(:,frames,j),20);
        phi_hat_last(:,j) = phi_hat(:,end);
    elseif strcmp(method,'tlsdeconv')
        fprintf('\ntlsdeconv\n')
        [p(:,j),costnew(j)]=tlsdeconv(H_UE_t(:,frames,j),H_eNB_t(:,frames,j),20);
        phi_hat_last(:,j) = zeros(length(frames),1);
    else
        error('unknow method %s',method);
    end
            
end

%%
colors=['b' 'g' 'r' 'c' 'm' 'y' 'k' 'b' 'g' 'r'];

if strcmp(method,'tlsdeconvangles')
    figure(5)
    clf; hold on
    niter=size(phi_hat,2);
    for k=2:size(phi_hat,1);
      polar(phi_hat(k,:),1:niter,colors(k-1));
      %polar(-reference(k)/180*pi,niter,[colors(k-1) 'o']);
    end
end

figure(6)
hold off
plot(abs(p))
title('p')

%% test the estimated reciprocity filter p by applying it to a differnt part of the measurements
g = H_eNB_t(:,frames2,:);
h = H_UE_t(:,frames2,:);
% reconstruct g from h
for j=1:2
    g_hat(:,:,j) = filter(p(:,j),1,h(:,:,j));
    if all(frames==frames2)
        for i=1:length(frames)
            g_hat(:,i,j) = exp(-1j*phi_hat_last(i,j)).*g_hat(:,i,j);
        end
    end
end
G = fft(g,512,1);
G_hat = fft(g_hat,512,1);
H = fft(h,512,1);

% normalize G and G_hat
scale_G = sqrt(mean(mean(mean(abs(G).^2,1),2),3));
scale_G_hat = sqrt(mean(mean(mean(abs(G_hat).^2,1),2),3));
scale_H = sqrt(mean(mean(mean(abs(H).^2,1),2),3));

G=G./scale_G;
G_hat = G_hat./scale_G_hat;
H=H./scale_H;

%%
addpath('E:\Synchro\kaltenbe\My Documents\Matlab\reciprocity\ICC paper');
SNR_vec_dB = -10:10:40;
SNR_vec = 10.^(SNR_vec_dB./10);
C_CSIR = zeros(512,length(frames2),length(SNR_vec));
C_CSIT_perf = zeros(512,length(frames2),length(SNR_vec));
C_CSIT_calib = zeros(512,length(frames2),length(SNR_vec));
C_CSIT_nocalib = zeros(512,length(frames2),length(SNR_vec));
for i=1:512
    for j=1:length(frames2)
        [C_CSIR(i,j,:), C_CSIT_perf(i,j,:), C_CSIT_calib(i,j,:)] = cap_miso(squeeze(G(i,j,:)).', squeeze(G_hat(i,j,:)).', SNR_vec.');
        [~, ~, C_CSIT_nocalib(i,j,:)] = cap_miso(squeeze(G(i,j,:)).', squeeze(H(i,j,:)).', SNR_vec.');
    end
end

scale_C = 1/(1e-3/14) * 1/4.5e6;

%%
figure(10)
hold off
plot(SNR_vec_dB,scale_C*squeeze(mean(sum(C_CSIT_perf,1),2)),'ro--');
hold on
plot(SNR_vec_dB,scale_C*squeeze(mean(sum(C_CSIT_calib,1),2)),'mx:');
%plot(SNR_vec_dB,scale_C*squeeze(mean(sum(C_CSIT_calib_nofreqoff,1),2)),'c+:');
plot(SNR_vec_dB,scale_C*squeeze(mean(sum(C_CSIR,1),2)),'bs--');
plot(SNR_vec_dB,scale_C*squeeze(mean(sum(C_CSIT_nocalib,1),2)),'kd-');
%legend('CSIR + perfect CSIT', 'CSIR + calibrated CSIT', 'CSIR + calibrated no freq offset', ...
%    'CSIR only', 'CSIR + uncalibrated CSIT', 'Location', 'Northwest')
legend('CSIR + perfect CSIT', 'CSIR + calibrated CSIT', ...
    'CSIR only', 'CSIR + uncalibrated CSIT', 'Location', 'Northwest')
xlabel('SNR [dB]')
ylabel('Capacity [bit/sec/Hz]')

%%
figure(7)
p = squeeze(20*log10(abs(g(:,:,ant)))).';
waterfall(max(p,zeros(size(p))))
zlim([0 50])
title('|g|^2 [dB]')
xlabel('Delay Time [samples]')
ylabel('Snapshots')

figure(8)
p = squeeze(20*log10(abs(g_hat(:,:,ant)))).';
waterfall(max(p,zeros(size(p))))
zlim([0 50])
title('|\hat{g}|^2 [dB]')
xlabel('Delay Time [samples]')
ylabel('Snapshots')

figure(9)
hold off
mse = abs(g-g_hat).^2./abs(g).^2;
surf(squeeze(10*log10(mse(:,:,ant))).')
title('mse [dB]')