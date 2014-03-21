function [sig, sig_f] = OFDM_TX_FRAME_MIMO(num_carriers,num_zeros,prefix_length,num_symbols_frame,preamble_length,num_ant)

% sig - output signal
% sig_length - output signal length
% num_carriers - number of sub-carriers
% num_zeros - number of zero carriers minus 1 (DC)
% prefix_length - length of cyclic prefix
% num_symbols_frame - number of symbols per OFDM frame
% preamble_length - length of 4-QAM preamble
% numant - number of antennas

num_useful_carriers = num_carriers - num_zeros -1;
if (num_ant ==1) 
    t_dec = 1;
    f_dec = 1;
elseif (num_ant ==2) 
    t_dec = 1;
    f_dec = 2;
elseif (num_ant == 4)
    t_dec = 2;
    f_dec = 4;
else
    error('Only 1, 2 or 4 antennas supported');
end

sig = zeros(num_ant,(num_carriers+prefix_length)*num_symbols_frame);
sig_f = zeros(num_ant,num_symbols_frame,num_useful_carriers);

for a=1:num_ant
for k=(floor((a-1)/2)+1):t_dec:preamble_length
    QAM4_preamble = zeros(1,num_useful_carriers);
    QAM4_preamble(2*mod(a-1,2)+1:f_dec:num_useful_carriers) = QAM_MOD(4,floor(4*abs(rand(1,num_useful_carriers/f_dec))));
    sig(a,(k-1)*(num_carriers+prefix_length)+1:k*(num_carriers+prefix_length)) = OFDM_TX(num_carriers,num_zeros,prefix_length,QAM4_preamble);
    sig_f(a,k,:) = QAM4_preamble;
end

for k=preamble_length+1:num_symbols_frame
     QAM_data = QAM_MOD(256,floor(4*abs(rand(1,num_useful_carriers))));
     sig(a,(k-1)*(num_carriers+prefix_length)+1:k*(num_carriers+prefix_length)) = OFDM_TX(num_carriers,num_zeros,prefix_length,QAM_data);
     sig_f(a,k,:) = QAM_data;
end
end
