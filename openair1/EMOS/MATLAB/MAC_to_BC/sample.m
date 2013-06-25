clear;

M = 2; % BS antennas
N = 1; % UE antennas
K = 2; % users

P = 10; % total transmit power = linear SNR 
        % (the noise variance is 1)

NFrames = 1000;
capacity = zeros(1,NFrames);
        
for l = 1:NFrames
%Choose channel for iteration
%UPLINK CHANNEL!!!!!!
%H(:,:,k) represents the M x N uplink channel of the kth user
H = 1/sqrt(2) * (randn(M,N,K) + 1i * randn(M,N,K));

% if Hd(:,:,k) is the N x M downlink channel of the kth user 
% H(:,:,k) = Hd(:,:,k)' is the M x N uplink channel

%calculate DPC sum rate with iterative waterfilling
[capacity(l) Covar] = iterative_waterfill(H,P,50);

%compute downlink covariance matrices
%Downlink = MAC_to_BC(H, Covar)
end

cdfplot(capacity)