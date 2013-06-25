%Block Diagonal Waterfilling
%Authors: Nihar Jindal, Wonjong Rhee, Sriram Vishwanath, Syed Jafar, Andrea
%Goldsmith
%Date:  6/20/05

function [capacity, Ex] = waterfill_block(H, P);
%Returns optimal covariance matrix and capacity for
%block diagonal channel H, where H(:,:,1) is first channel, H(:,:,2) is 2nd
%etc.

%Function Inputs
%H:             triply indexed channel
%               H(:,:,k) refers to the UPLINK channel of the kth receiver
%               H(:,:,k) is an (M x N) matrix
%P:             power in linear units

%Function Outputs
%capacity:      Capacity in bps/Hz
%Ex:            triply indexed matrices containing the capacity achieving
%               covariance matrices
%               Ex(:,:,k) is the (N x N) covariance of user k


%Other Variables
%M:  # of base station antennas in downlink channel
%N:  # of terminal antennas in downlink channel



[M,N,K] = size(H);

for index = 1:K

    %Perform SVD of channel
    v = svd(H(:,:,index)' * H(:,:,index));

    %Create vector with all singular values
    if (index == 1)
        s = v;
    else
        s = [s; v];
    end;
end;

channels = length(s);

s = real(s);
%Eliminate zeroes
s = max(s, 1e-10);

noise_vector = 1./s;
unsortedN = noise_vector;
%Put noise variances in increasing order
noise_vector = sort(noise_vector);  

last = channels;   %index of worst channel allocated non-zero power
loop = 1;

%Find channels allocated non-zero power
while (loop)
    water = noise_vector(last);    %water level
    power = sum(max(water - noise_vector, 0));
    if (power > P)
        last = last - 1;
    else
        loop = 0;
    end;
end; 

%Calculate optimal water level
water = 1/last * (P + sum(noise_vector(1:last)));

capacity = 0;

Ex = zeros(N,N,K);
%Compute covariance matrix for each user
for index = 1:K
%Take svd of channel
[u,v,w] = svd(H(:,:,index)' * H(:,:,index));
s = svd(H(:,:,index)' * H(:,:,index));
s = real(s);
%eliminate zeroes
s = max(s, 1e-10);
noise_vector = 1./s;

Ex(:,:,index) = u * diag(max((water- noise_vector),0)) * u';
capacity = capacity + real(log2(det(eye(M) +  H(:,:,index) *Ex(:,:,index)* H(:,:,index)')));
end;


