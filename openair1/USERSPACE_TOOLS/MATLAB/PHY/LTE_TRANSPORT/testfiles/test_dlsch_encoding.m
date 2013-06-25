%% Testfile for dlsch_encoding mex-function
clear all;
addpath('../mexfiles');

%% Input
simparms.nb_rb = 25; % fixed number of RB per OFDM symbol
simparms.rb_alloc = hex2dec('1FFFFFF'); % 25 ones
simparms.num_pdcch_symbols = 3;
simparms.subframe = 7;
simparms.abstraction_flag = 0;
simparms.frame_type = 1; % TDD frame
simparms.mode1_flag = 0; % is TM1 active?
simparms.Ncp = 0; % Normal CP

codeword.mcs = 13;
codeword.TBS = get_tbs(codeword.mcs,simparms.nb_rb);
codeword.Kmimo = 1;
codeword.harq_pid = 0; % First HARQ round
codeword.Mdlharq = 1; % Number of HARQ rounds
codeword.rvidx = 0;
codeword.Nl = 1;
codeword.Ndi = 1;

data0 = uint8(randi([0,255],codeword.TBS/8+4,1));

%% mex
edata0 = dlsch_encoding(data0,simparms,codeword);
llr = int16((double(edata0)-0.5)*128);
ret0 = dlsch_decoding(llr,simparms,codeword);

%% Print
disp(ret0);