%MAC to BC Transformation
%Authors: Nihar Jindal, Wonjong Rhee, Sriram Vishwanath, Syed Jafar, Andrea
%Goldsmith
%Date:  6/20/05

function Downlink = MAC_to_BC(H, Uplink);

%assumes users decoded in increasing order for uplink: 1,2,...,K,  
%opposite encoding order for downlink (K, K-1,..., 1)

%Function Inputs
%H:             triply indexed UPLINK channel
%               H(:,:,k) refers to the UPLINK channel of the kth receiver
%               H(:,:,k) is an (M x N) matrix
%Uplink:        triply indexed matrices containing the UPLINK covariance matrices
%               Uplink(:,:,k) is the (N x N) covariance of user k

%Function Outputs
%Downlink:      triply indexed matrices containing the DOWNLINK covariance matrices
%               Downlink(:,:,k) is the (M x M) covariance of user k

%Other Variables
%M:             # of transmit antennas in downlink channel
%N:             # of receive antennas in downlink channel
%K:             # of receivers

%Determine # of antennas, users
[M,N,K] = size(H);

for k = 1:K

    %Compute A_i (equation 34)
    temp_sum = eye(M,M);
    for l = (k+1):K
        temp_sum = temp_sum + H(:,:,l) * Uplink(:,:,l) * H(:,:,l)';
    end;
    B(:,:,k) = temp_sum;

    %Compute B_i (equation 34)
    temp_sum = eye(N,N);
    for l = 1:(k-1)
        temp_sum = temp_sum + H(:,:,k)' * Downlink(:,:,l) * H(:,:,k);
    end;
    A(:,:,k) = temp_sum;

    %take SVD of effective channel
    Hk_eff = B(:,:,k)^(-0.5) * H(:,:,k) * A(:,:,k)^(-0.5);
    [r,t] = size(Hk_eff);
    if (t > r)
        [F,D,G] = svd(Hk_eff',0);
        Fk = G;
        Gk = F;
    else
        [Fk,D,Gk] = svd(Hk_eff,0);
    end;
    
    %Compute downlink covariance (equation 35)
    Downlink(:,:,k) = B(:,:,k)^(-0.5) * Fk * Gk' * A(:,:,k)^(0.5) * Uplink(:,:,k) *  A(:,:,k)^(0.5) * Gk * Fk' * B(:,:,k)^(-0.5);

end; 





