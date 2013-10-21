function [ simparms ] = InitSimparms( mcs, n_frames, SNRdB )
%INITSIMPARMS initialize simparms struct for TM5
% There are always 2 user schedules for transmission
% INPUT:
%   nb_antennas_tx = Number of transmit antennas, max = 2
%   nb_antennas_rx = Number of receive antennas per UE, max = 2
%   MCS(2) = Modulation and Coding Scheme [1,...,27]
%   n_frames = Number of frames to simulate
%   SNRdB = SNR in dB

addpath('../../../PHY/LTE_TRANSPORT/mexfiles');

j = sqrt(-1);

%% Custom parameters 
simparms.nb_antennas_tx = 2;
simparms.nb_antennas_tx_eNB = simparms.nb_antennas_tx;
simparms.nb_antennas_rx = 2;
simparms.n_frames = n_frames;
simparms.snr_db = SNRdB;

%% Constant parameters
simparms.NB_ANTENNAS_TX = 2; % max number of TX antennas
simparms.NB_ANTENNAS_RX = 2; % max number of RX antennas
simparms.snr = 10.^(SNRdB/10);
simparms.num_pdcch_symbols = 3;
simparms.subframe = 7;
simparms.abstraction_flag = 0;
simparms.nb_rb = 25; % fixed number of RB per OFDM symbol
simparms.rb_alloc = hex2dec('1FFF'); % 25 RBs
simparms.MAX_TURBO_ITERATIONS = 5;
simparms.nb_re = 3000; % there are 3000 REs in subframe 7
simparms.frame_type = 1; % TDD frame
simparms.mode1_flag = 0; % no is TM1 active
simparms.Ncp = 0; % Normal CP
simparms.ofdm_symbol_size = 512;
simparms.log2_maxh = 16;
simparms.dl_power_offset= 1; % no 3dB power offset
simparms.nb_slots = 14;
simparms.nb_re_per_symbol = simparms.nb_rb*12;
simparms.nb_re_per_frame = simparms.nb_slots*simparms.nb_re_per_symbol;
simparms.frame_errors = zeros(length(simparms.snr),simparms.NB_ANTENNAS_RX);
simparms.CB = [[1;1],[1;-1],[1;j],[1;-j]]/sqrt(2); % codebook
simparms.tseeds = set_taus_seed(1); % taus seeds

% Turbo decoder: set the pointer to the td tables allocated by init_td()
simparms.ptr_td = dlsch_decoding_init();

% Init codewords
% simparms.codeword(1) = InitCodeword(simparms,mcs(1));
% simparms.codeword(2) = InitCodeword(simparms,mcs(2));

simparms.codeword(1) = InitCodeword(simparms,mcs(1));
simparms.codeword(2) = InitCodeword(simparms,mcs(2));
% simparms.codeword(3) = InitCodeword(simparms,mcs(1));
% simparms.codeword(4) = InitCodeword(simparms,mcs(1));

end

function codeword = InitCodeword(simparms,mcs)

% Constellation
j = sqrt(-1);
amp = 1/32;
Q4 =  amp*[ 1+j,  1- j,  -1+j, -1-j]./sqrt(2);
Q16 = amp*[ 1+j,  1+3*j,  3+j,  3+3*j,  1-j,  1-3*j,  3-j,  3-3*j,...
           -1+j, -1+3*j, -3+j, -3+3*j, -1-j, -1-3*j, -3-j, -3-3*j]./sqrt(10);
Q64 = amp*[ 3+3*j, 3+j, 1+3*j, 1+  j, 3+5*j, 3+7*j, 1+5*j, 1+7*j,...
	        5+3*j, 5+j, 7+  j, 7+3*j, 5+5*j, 5+7*j, 7+5*j, 7+7*j,...
	        3-3*j, 3-j, 1-3*j, 1-  j, 3-5*j, 3-7*j, 1-5*j, 1-7*j,...
	        5-3*j, 5-j, 7-3*j, 7-  j, 5-5*j, 5-7*j, 7-5*j, 7-7*j,...
	       -3+3*j,-3+j,-1+3*j,-1+  j,-3+5*j,-3+7*j,-1+5*j,-1+7*j,...
	       -5+3*j,-5+j,-7+3*j,-7+  j,-5+5*j,-5+7*j,-7+5*j,-7+7*j,...
	       -3-3*j,-3-j,-1-3*j,-1-  j,-3-5*j,-3-7*j,-1-5*j,-1-7*j,...
	       -5-3*j,-5-j,-7-3*j,-7-  j,-5-5*j,-5-7*j,-7-5*j,-7-7*j]./sqrt(42);

if (mcs <= 9)
		codeword.mod_order = 2;
		codeword.base2 = [2 1];
		codeword.const = Q4;
		
elseif (mcs <= 16)			
		codeword.mod_order = 4;
		codeword.base2 = [8 4 2 1];
		codeword.const = Q16;
		
elseif (mcs <= 27)
		codeword.mod_order = 6;
		codeword.base2 = [32 16 8 4 2 1];
		codeword.const = Q64;
		
else
		error('invalid MCS: %d\n', mcs);
		
end

codeword.mcs = mcs;
codeword.G = simparms.nb_re*codeword.mod_order; % Number of softbits
codeword.TBS = get_tbs(mcs,simparms.nb_rb);
codeword.Kmimo = 2;
codeword.harq_pid = 0; % First HARQ round
codeword.Mdlharq = 1; % Number of HARQ rounds
codeword.rvidx = 0;
codeword.Nl = 1;
codeword.Ndi = 1;
end

