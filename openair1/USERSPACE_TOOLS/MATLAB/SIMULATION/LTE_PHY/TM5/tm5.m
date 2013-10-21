%% Header
%==========================================================================
% This testbench simulations transmission mode 5 on subframe 7.
% Results can be compared to "./dlsim -x5 -S7 -R1 -u1" with perfect chest
% No OFDM Modulation and Demodulation is applied!
%
% Author: Sebastian Wagner
% Date: 24-07-2012
%
%==========================================================================

clear all;
close all;
clear mex;

addpath('../../../PHY/LTE_TRANSPORT/mexfiles');
addpath('../../../PHY/TOOLS/mexfiles');
addpath('../../../SIMULATION/TOOLS/mexfiles');

% profile on;
tic
%% System parameters
nt = 2;
nr = 2;
N = 1000; % number of frames (codewords)
nSNR = 1;
%SNRdB = [1:20];
SNRdB = 10;
% nSNR = 13;
%SNRdB = linspace(1,20,nSNR);
MCS = [9 9]; % MCS for the 2 users, currently it is assumed that mcs(2)=mcs(1)
j = sqrt(-1);
amp = 1/32;
XFORMS = 0;
% LLRs are computed for 8RE make sure enough valid memory is allocated
LLR_GUARD_SAMPLES = 8*6; % max mod_order 6 for 8 REs

%% Initialize simparms
simparms = InitSimparms( nt, nr, MCS, N, SNRdB);

%% Random data, same seed as in dlsim.c
[tmp simparms.tseeds] = taus(1,simparms.tseeds);

%% Index of REs carrying data
data_idx = [901:1400 1501:2300 2401:3500 3601:4200];
data_idx_int = [1801:2800 3001:4600 4801:7000 7201:8400]; % [Re Im] format
data_idx_int_r = data_idx_int(1:2:length(data_idx_int));
data_idx_int_i = data_idx_int(2:2:length(data_idx_int));

%% Allocate memory
llr0 = zeros(simparms.codeword(1).G, 1,'int16');
y_fxp = zeros(simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,simparms.nb_re_per_frame,'int16');
y_fxp_t = zeros(2*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,'int16');
ymf0 = zeros(2*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,'int16');

% Effective channel will contain the channel estimate at pilot positions
Heff0 = zeros(2*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,'int16');
Hmag0 = zeros(2*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,'int16');
Hmagb0 = zeros(2*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,'int16');
ymf1 = zeros(2*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,'int16');
Heff1 = zeros(2*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,'int16');
Hmag1 = zeros(2*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,'int16');
Hmagb1 = zeros(2*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,'int16');
rho10 = zeros(2*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,'int16');
H = zeros(simparms.NB_ANTENNAS_RX,simparms.NB_ANTENNAS_TX);
noise = zeros(simparms.NB_ANTENNAS_RX,simparms.nb_re);

%% XFORMS
if (XFORMS)
	scrsz = get(0,'ScreenSize');
	figure('Position',[1 scrsz(4)/2 scrsz(3)/2 scrsz(4)/2])
	subplot(1,2,1);
	xlim([1 simparms.codeword(1).G]);
	title('LLRs of UE 0');
	
	fig_llr0 = scatter(1:simparms.codeword(1).G,llr0,'.','YDataSource','llr0');
	
	subplot(1,2,2);
	title('MF output of UE 0');
	fig_ymf = scatter(ymf0(data_idx_int_r),ymf0(data_idx_int_i),'.','XDataSource','ymf0(data_idx_int_r)','YDataSource','ymf0(data_idx_int_i)');
end

%% Encode and modulate transmit signal
% The same data is used throughout the simulation -> saves time
[data0 simparms.tseeds] = taus(simparms.codeword(1).TBS/8,simparms.tseeds);
[data1 simparms.tseeds] = taus(simparms.codeword(2).TBS/8,simparms.tseeds);

% Add 4 bytes CRC
data0 = [data0; zeros(4,1,'uint8')];
data1 = [data1; zeros(4,1,'uint8')];

edata0 = dlsch_encoding(data0,simparms,simparms.codeword(1)); % user 1
edata1 = dlsch_encoding(data1,simparms,simparms.codeword(2)); % user 2

% Modulation
edata_enc(:,1) = simparms.codeword(1).base2*double(reshape(edata0,simparms.codeword(1).mod_order,simparms.nb_re));
edata_enc(:,2) = simparms.codeword(2).base2*double(reshape(edata1,simparms.codeword(2).mod_order,simparms.nb_re));
s(:,1) = simparms.codeword(1).const(edata_enc(:,1)+1);
s(:,2) = simparms.codeword(2).const(edata_enc(:,2)+1);

Ptx = sum(diag(s'*s))/simparms.nb_re; % average transmit power per RE

%% Loop over SNRs
for iSNR=1:length(simparms.snr)
	cSNR = simparms.snr(iSNR); % current SNR
	%% Loop over Frames
	for n=1:simparms.n_frames
		
		%% Channel
		% Rayleigh channel, constant for whole codeblock/frame, always 2x2
		H(1:nr,1:nt) = (randn(nr,nt) + j*randn(nr,nt))/sqrt(2);
				
		%% PMI computation
		pmi = 1;
		r = H(:,2)'*H(:,1);
        rr = real(r);
        ri = imag(r);
		if (rr>ri && rr>-ri)
			pmi = 1;
		elseif (rr<ri && rr>-ri)
			pmi = 3;
		elseif (rr<ri && rr<-ri)
			pmi = 2;
		elseif (rr>ri && rr<-ri)
			pmi = 4;			
		end
        
        % opposite pmi       
		if(pmi<3)
			pmi_o = mod(pmi,2) + 1;
		else
			pmi_o = mod(pmi,2) + 3;
		end
		
		pmi_ext = uint8(repmat(pmi-1,1,simparms.nb_rb));
		pmi_ext_o = uint8(repmat(pmi_o-1,1,simparms.nb_rb));

        % precoding vectors
        g0 = simparms.CB(:,pmi);
        g1 = simparms.CB(:,pmi_o);
		
		%% Transmit signal (frequency domain)
		x = [g0,g1]*conj(s')/sqrt(2); % E[|x|^2] = 2*amp^2		
		
		%% Noise		
		sigma2 = (Ptx/cSNR);
		% E[|noise|^2] = 2*amp^2/sqrt(SNR) -> SNR=1/sigma^2
		noise(1:nr,:) = sqrt(sigma2)*(randn(nr,simparms.nb_re) + j*randn(nr,simparms.nb_re))/sqrt(2);
		
		%% Received signal
		y = H*x + noise;
		
		% Quantization
		y_fxp_data = int16(floor(y*pow2(15)));
		H_fxp = int16(fix(H*floor(amp/sqrt(2)*pow2(15)))); % Perfect Chest		
		
		% insert dummy pilots
		y_fxp(1:2,data_idx) = y_fxp_data;
		
		% reorder for processing
		H_fxp_t = repmat([real(H_fxp(:)) imag(H_fxp(:))]',simparms.nb_re_per_frame,1);
		y_fxp_t(:,1) = reshape([real(y_fxp(1,:)); imag(y_fxp(1,:))],2*simparms.nb_re_per_frame,1);		
		y_fxp_t(:,2) = reshape([real(y_fxp(2,:)); imag(y_fxp(2,:))],2*simparms.nb_re_per_frame,1);		
		
		%% Compute Scaling
		avg = dlsch_channel_level_prec(H_fxp_t,pmi_ext,simparms);
		simparms.log2_maxh = max(double(log2_approx(avg))-13,0);		
				
		%% Inner receiver loop
		llrp = 1; % LLR pointer
		for symbol = 4:14
			idxs = 2*(symbol-1)*simparms.nb_re_per_symbol + 1;
			idxe = 2*(symbol-1)*simparms.nb_re_per_symbol + 2*simparms.nb_re_per_symbol;
			
			%% Preprocessing
			[ymf0(idxs:idxe,:)...
				Heff0(idxs:idxe,:)...
				Hmag0(idxs:idxe,:)...
				Hmagb0(idxs:idxe,:)]...
				= dlsch_channel_compensation_prec(y_fxp_t,H_fxp_t,pmi_ext,simparms,simparms.codeword(1),symbol-1);
			
			% Interfering user
			[ymf1(idxs:idxe,:)...
				Heff1(idxs:idxe,:)...
				Hmag1(idxs:idxe,:)...
				Hmagb1(idxs:idxe,:)]...
				= dlsch_channel_compensation_prec(y_fxp_t,H_fxp_t,pmi_ext_o,simparms,simparms.codeword(2),symbol-1);
			
			%% Correlation coefficient
			rho10(idxs:idxe,:) = dlsch_dual_stream_correlation(Heff0,Heff1,simparms,symbol-1);
			
			%% Combining
			if (nr>1)
				[ymf0(idxs:idxe,:)...
					ymf1(idxs:idxe,:)...
					Hmag0(idxs:idxe,:)...
					Hmagb0(idxs:idxe,:)...
					Hmag1(idxs:idxe,:)...
					Hmagb1(idxs:idxe,:)...
					rho10(idxs:idxe,:)]...
					= dlsch_detection_mrc(ymf0,ymf1,Hmag0,Hmag1,Hmagb0,Hmagb1,rho10,simparms,symbol-1);								
			end
			
			
			%% LLR computation
            llr = dlsch_mu_mimo_llr([ymf0(idxs:idxe,1); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                [ymf1(idxs:idxe,1); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                [Hmag0(idxs:idxe,1); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                [Hmag1(idxs:idxe,1); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                [rho10(idxs:idxe,1); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                simparms,...
                symbol-1);
			
			llr0(llrp:llrp+length(llr)-1,:) = llr;
			llrp = llrp + length(llr);                        
		end
		
		if (XFORMS)
			refreshdata(fig_llr0,'caller');
			drawnow;
			
			refreshdata(fig_ymf,'caller');
			drawnow; pause(0.1);
		end
		
		%% Channel decoding
		llr0 = -llr0; % invert (since no scrambling applied)				
		ret0 = dlsch_decoding(llr0,simparms,simparms.codeword(1));						
		
		% Check if decoded correctly
		if (ret0 >= simparms.MAX_TURBO_ITERATIONS)
			simparms.frame_errors(iSNR,1) = simparms.frame_errors(iSNR,1) + 1;
		end
		
	end
	fprintf('********************SNR = %3.1f dB processed on %s********************\n',real(simparms.snr_db(iSNR)),datestr(now));
	fprintf('Errors: %d/%d\nPe = %1.5f\n',simparms.frame_errors(iSNR,1),N,simparms.frame_errors(iSNR,1)/N);	
end

%% Post Processing
%  total_frame_erros = sum(simparms.frame_errors,2);
%  fer = total_frame_erros/N;
%  figure; semilogy(SNRdB,fer); grid;
%  
%  disp(fer);
% 
% profile viewer 
toc

