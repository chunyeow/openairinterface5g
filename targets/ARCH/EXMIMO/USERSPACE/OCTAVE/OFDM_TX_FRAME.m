function sig = OFDM_TX_FRAME(num_carriers,num_zeros,prefix_length,num_symbols_frame,preamble_length)

% sig - output signal
% sig_length - output signal length
% num_carriers - number of sub-carriers
% num_zeros - number of zero carriers minus 1 (DC)
% prefix_length - length of cyclic prefix
% num_symbols_frame - number of symbols per OFDM frame
% preamble_length - length of 4-QAM preamble

num_useful_carriers = num_carriers - num_zeros -1;

sig = [];
for k=1:preamble_length
     QAM4_preamble = QAM_MOD(4,floor(256*abs(rand(1,num_useful_carriers/4))));
     sig = [sig OFDM_TX(num_carriers,num_zeros,prefix_length,QAM4_preamble)];
end

for k=1:(num_symbols_frame - preamble_length)
     QAM_data = QAM_MOD(256,floor(256*abs(rand(1,num_useful_carriers))));
     sig = [sig OFDM_TX(num_carriers,num_zeros,prefix_length,QAM_data)];
end
