function p_dpss = init_dpss(p,tau_max)

%% init DPSS
% channel parameters
theta_max = tau_max * p.frame_length*100 / p.useful_carriers; % TS = 1/(p.frame_length*100)
N_sub = floor(theta_max*p.useful_carriers)+1; % number of approximate subspace dimensions
%N_sub = 1;

% DPSS generation
nDmax=min(ceil(N_sub*1.25),p.useful_carriers);
[Udps,Sdps]=dpss(p.useful_carriers,theta_max*p.useful_carriers/2,nDmax);

% the fftshift is needed dependig in which format the frequency response is used 
%V=fftshift(diag(exp(-2*pi*1i*theta_max/2*(0:(p.nN-1))))*Udps);
p_dpss.V=diag(exp(-2*pi*1i*theta_max/2*(0:( p.useful_carriers-1))))*Udps;

% compute the optimal subspace dimension based on the expected SNR
p_dpss.SNR=0:30;
for SNR=p_dpss.SNR
    % calculate optimum subspace dimension as bias variance tradeoff for a
    % given noise variance (assuming pilot symbols with energy one)
    sigma2 = 10.^(-SNR/10);
    [~,p_dpss.Dopt(SNR+1)]=findminimumd(sigma2, p.useful_carriers,nDmax,1/theta_max*Sdps);
end

% % precompute the filters for DPSS estimation for the required number of
% % basis function
% Dvec = min(Dopt):max(Dopt);
% for ind=1:length(Dvec)
%     Dind = Dvec(ind);
%     
%     for itx=1:p.nant_tx
%         % f_start and t_start indicate the start of the pilots in time
%         % and frequency according to the specifications (see .doc file).
%         % t_start has to be >=2, since the first symbol is the PSS.
%         f_start = mod(itx-1,2)*2+1;
%         t_start = floor((itx-1)/2)+1;
%         
%         % filter for DPSS channel estimation
%         M = conj(diag(squeeze(transmit_f(itx,t_start,f_start:4:end))) * V(f_start:4:end,1:Dind));
%         Mpinv = (M'*M)\M.';
%         % to compute the basis coefficients do
%         % psi = Mpinv*(symb0(2:6:end));
%         
%     end
%     
% end
