%% Header
%==========================================================================
% This testbench simulates transmission mode 4 on subframe 7.
% Results can be compared to "./dlsim -x5 -S7 -R1 -u1" with perfect chest
% No OFDM Modulation and Demodulation is applied!
% Only 5 MHz bandwidth simulated. The Channel is Rayleigh block-fading.
% 
% The matrices ymf0, ymf1, etc are in the form samples x antennas.
% More precisely: 
% row dimension = [Re Im Re Im ...]
% col dimension: Heffx: [tx1->rx1 tx1->rx2 tx2->rx1 tx2->rx2]
%                ymfx, Hmagx, Hmagbx, rho10 : [rx1 rx2 rx3 rx4]
%
% The same random data is generate as in dlsim.c by setting the right
% seed value simparms.tseeds. 
%
%
% Author: Sebastian Wagner
% Date: 10-04-2013
%
%==========================================================================

clear all;
close all;
clear mex;

addpath('../../../PHY/LTE_TRANSPORT/mexfiles');
addpath('../../../PHY/TOOLS/mexfiles');
addpath('../../../SIMULATION/TOOLS/mexfiles');

% profile on;
%% System parameters
nt = 2; % number of transmit antennas
nr = 2; % number of receive antennas
N = 100; % number of frames (or subframes, codewords)
nSNR = 1; % number of SNR points
SNRdB = 5; % SNR per receive antenna in dB
% nSNR = 13;
% SNRdB = linspace(8,20,nSNR);
MCS = [16 16]; % MCS for the 2 users
j = sqrt(-1);
amp = 1/32; % amplitude scaling for FXP conversion
XFORMS = 0; % flag to activate graphics

% LLRs are computed for 8 RE make sure enough valid memory is allocated
LLR_GUARD_SAMPLES = 8*6; % max mod_order 6 for 8 REs

%% Initialize simparms
simparms = InitSimparms(MCS, N, SNRdB);

%% Random data, same seed as in dlsim.c
[tmp simparms.tseeds] = taus(1,simparms.tseeds);

%% Index of REs carrying data
data_idx = [901:1400 1501:2300 2401:3500 3601:4200];
data_idx_int = [1801:2800 3001:4600 4801:7000 7201:8400]; % [Re Im] format
data_idx_int_r = data_idx_int(1:2:length(data_idx_int));
data_idx_int_i = data_idx_int(2:2:length(data_idx_int));

%% Allocate memory
% The LLRs of subframe
llr0 = zeros(simparms.codeword(1).G, 1,'int16');
llr1 = zeros(simparms.codeword(2).G, 1,'int16');
% received signal transformed to FXP
y_fxp = zeros(simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,simparms.nb_re_per_frame,'int16');
% FXP received signal with transformed dimensions
y_fxp_t = zeros(2*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,'int16');

% MF output of desired user
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
rho01 = zeros(2*simparms.nb_re_per_frame,simparms.NB_ANTENNAS_RX*simparms.NB_ANTENNAS_TX,'int16');
H = zeros(simparms.NB_ANTENNAS_RX,simparms.NB_ANTENNAS_TX);
noise = zeros(simparms.NB_ANTENNAS_RX,simparms.nb_re);

%% XFORMS
if (XFORMS)
	scrsz = get(0,'ScreenSize');mu_mimo
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

% Turbo encoding of 2 streams for 2 users
edata0 = dlsch_encoding(data0,simparms,simparms.codeword(1)); % user 1
edata1 = dlsch_encoding(data1,simparms,simparms.codeword(2)); % user 2

% Symbol mapping and modulation
edata_enc(:,1) = simparms.codeword(1).base2*double(reshape(edata0,simparms.codeword(1).mod_order,simparms.nb_re));
edata_enc(:,2) = simparms.codeword(2).base2*double(reshape(edata1,simparms.codeword(2).mod_order,simparms.nb_re));
s(:,1) = simparms.codeword(1).const(edata_enc(:,1)+1);
s(:,2) = simparms.codeword(2).const(edata_enc(:,2)+1);

%% Loop over SNRs
for iSNR=1:length(simparms.snr)
    cSNR = simparms.snr(iSNR); % current SNR
    %% Loop over Frames
    for n=1:simparms.n_frames
        
        %% Channel
        % Rayleigh channel, constant for whole codeblock/frame
        H(1:nr,1:nt) = (randn(nr,nt) + j*randn(nr,nt))/sqrt(2);
        
        %% PMI computation
        pmi = 1;
        r = H(:,2)'*H(:,1); % correlation
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
        
        % normelized precoding vectors
        g0 = simparms.CB(:,pmi);
        g1 = simparms.CB(:,pmi_o);
        
        %% Transmit signal (frequency domain)
        x = [g0,g1]*conj(s'); % E[|x|^2] = amp^2
        Ptx = sum(diag(x'*x))/simparms.nb_re; % average transmit power per RE
        
        %% Noise
        sigma2 = (Ptx/cSNR);
        % E[|noise|^2] = amp^2/sqrt(SNR) -> SNR=1/sigma^2
        noise(1:nr,:) = sqrt(sigma2)*(randn(nr,simparms.nb_re) + j*randn(nr,simparms.nb_re))/sqrt(2);
        % real SNR per receive antenna
        SNR_real = 10*log10(Ptx / ((noise(1,:)*noise(1,:)')/simparms.nb_re));
        
        %% Received signal
        y = H*x + noise; % flat-fading channel model
        
        % Quantization
        y_fxp_data = int16(floor(y*pow2(15)));
        H_fxp = int16(fix(H*floor(amp*pow2(15)))); % Perfect Chest
        
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
        llr0p = 1; % LLR pointer for codeword 0
        llr1p = 1; % LLR pointer for codeword 1
        for symbol = 4:14
            idxs = 2*(symbol-1)*simparms.nb_re_per_symbol + 1;
            idxe = 2*(symbol-1)*simparms.nb_re_per_symbol + 2*simparms.nb_re_per_symbol;
            
            %% Preprocessing
            [ymf0(idxs:idxe,:)...
                Heff0(idxs:idxe,:)...
                Hmag0(idxs:idxe,:)...
                Hmagb0(idxs:idxe,:)]...
                = dlsch_channel_compensation_prec(y_fxp_t,H_fxp_t,pmi_ext,simparms,simparms.codeword(1),symbol-1);
            
            % Interfering stream
            [ymf1(idxs:idxe,:)...
                Heff1(idxs:idxe,:)...
                Hmag1(idxs:idxe,:)...
                Hmagb1(idxs:idxe,:)]...
                = dlsch_channel_compensation_prec(y_fxp_t,H_fxp_t,pmi_ext_o,simparms,simparms.codeword(2),symbol-1);
            
            %% Correlation coefficient
            rho10(idxs:idxe,:) = dlsch_dual_stream_correlation(Heff0,Heff1,simparms,symbol-1);
            
            %% Combining
            [ymf0(idxs:idxe,:)...
                ymf1(idxs:idxe,:)...
                Hmag0(idxs:idxe,:)...
                Hmagb0(idxs:idxe,:)...
                Hmag1(idxs:idxe,:)...
                Hmagb1(idxs:idxe,:)...
                rho10(idxs:idxe,:)]...
                = dlsch_detection_mrc(ymf0,ymf1,Hmag0,Hmag1,Hmagb0,Hmagb1,rho10,simparms,symbol-1);
            
            %% LLR computation
            % first stream
            llr0_t = dlsch_mu_mimo_llr([ymf0(idxs:idxe,1); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                [ymf1(idxs:idxe,1); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                [Hmag0(idxs:idxe,1); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                [Hmag1(idxs:idxe,1); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                [rho10(idxs:idxe,1); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                simparms,...
                symbol-1);
            
            llr0(llr0p:llr0p+length(llr0_t)-1,:) = llr0_t;
            llr0p = llr0p + length(llr0_t);
            
            % second stream
            % compute conj. correlation term
            tmp = rho10(idxs:idxe,:);
            tmp(2:2:end) = -tmp(2:2:end);
            rho01(idxs:idxe,:) = tmp;
            
            llr1_t = dlsch_mu_mimo_llr([ymf1(idxs:idxe,1); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                [ymf0(idxs:idxe,1); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                [Hmag1(idxs:idxe,1); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                [Hmag0(idxs:idxe,1); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                [rho01(idxs:idxe,1); zeros(LLR_GUARD_SAMPLES,1,'int16')],...
                simparms,...
                symbol-1);
            
            llr1(llr1p:llr1p+length(llr1_t)-1,:) = llr1_t;
            llr1p = llr1p + length(llr1_t);
            
        end
        
        if (XFORMS)
            refreshdata(fig_llr0,'caller');
            drawnow;
            
            refreshdata(fig_ymf,'caller');
            drawnow; pause(0.1);
        end
        
        %% Channel decoding
        llr0 = -llr0; % invert (since no scrambling applied)
        llr1 = -llr1; % invert (since no scrambling applied)
        ret0 = dlsch_decoding(llr0,simparms,simparms.codeword(1));
        ret1 = dlsch_decoding(llr1,simparms,simparms.codeword(2));
        
        % Check if decoded correctly
        if (ret0 >= simparms.MAX_TURBO_ITERATIONS)
            simparms.frame_errors(iSNR,1) = simparms.frame_errors(iSNR,1) + 1;
        end
        if (ret1 >= simparms.MAX_TURBO_ITERATIONS)
            simparms.frame_errors(iSNR,2) = simparms.frame_errors(iSNR,2) + 1;
        end
        
    end
    fprintf('********************SNR = %3.1f dB processed on %s********************\n',real(simparms.snr_db(iSNR)),datestr(now));
    fprintf('Errors: %d/%d\n',simparms.frame_errors(iSNR,1),N);
    fprintf('Errors: %d/%d\n',simparms.frame_errors(iSNR,2),N);
end

%% Post Processing
% total_frame_errors = sum(frame_errors,2);
% fer = total_frame_errors/N;
% figure; semilogy(SNRdB,fer); grid;
% 
% disp(fer);

% profile viewer 


