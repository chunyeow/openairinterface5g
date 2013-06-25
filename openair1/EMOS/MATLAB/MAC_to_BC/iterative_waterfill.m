%Sum Power Iterative Waterfilling Algorithm
%Authors: Nihar Jindal, Wonjong Rhee, Sriram Vishwanath, Syed Jafar, Andrea
%Goldsmith
%Date:  6/20/05

function [capacity, Covariances] = iterative_waterfill(H, P, iterations);

%Function Inputs
%H:             triply indexed UPLINK channel
%               H(:,:,k) refers to the UPLINK channel of the kth receiver
%               H(:,:,k) is an (M x N) matrix
%P:             power in linear units
%iterations:    # of iterations performed

%Function Outputs
%capacity:      Sum rate capacity in bps/Hz
%Covariances:         triply indexed matrices containing the capacity achieving
%               UPLINK covariance matrices
%               Covariances(:,:,k) is the (N x N) covariance of user k

%Other Variables
%M:  # of transmit antennas in downlink channel
%N:  # of receive antennas in downlink channel
%K:  # of receivers

%Determine # of antennas, users
[M,N,K] = size(H);

%Initialize covariances to zero
Covariances = zeros(N,N,K); 

for wf_iterations = 1:iterations

    sum_int = eye(M);
    for k = 1:K
        sum_int = sum_int + H(:,:,k) * Covariances(:,:,k) * H(:,:,k)';
    end;

    %Generate effective channels of each user (using covariances from the
    %previous step)
    for k = 1:K
        H_eff(:,:,k) = (sum_int - H(:,:,k) * Covariances(:,:,k) * H(:,:,k)')^(-0.5) * H(:,:,k);
    end;

    %Perform waterfilling across effective channels
    [temp, Ex] = waterfill_block(H_eff, P);

    %Update covariances of all users
    %Use original algorithm for first 5 iterations to speed up convergence
    if (wf_iterations <= 5)
        %Updated covariances set equal to new covariances
        for k = 1:K
             Covariances(:,:,k) = Ex(:,:,k);
        end;
    else
        %Update covariances with mixture of old and new covariances
        for k = 1:K
            Covariances(:,:,k) = ((K-1)/K)*Covariances(:,:,k) + (1/K) * Ex(:,:,k);
        end;
    end;
end; 

%Compute final sum rate
sum_int = eye(M);
for k = 1:K
    sum_int = sum_int + H(:,:,k) * Covariances(:,:,k) * H(:,:,k)';
end;
capacity = real(log2(det(sum_int)));





