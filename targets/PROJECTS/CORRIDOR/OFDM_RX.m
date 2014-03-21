function received_f = OFDM_RX(received,num_carriers,useful_carriers,prefix_length,num_symbols_frame)
nant = size(received,2);
ofdm_symbol_length = num_carriers + prefix_length;
received_f = zeros(num_symbols_frame,useful_carriers,nant);
for j=0:num_symbols_frame-1;
    ifblock=received(j*ofdm_symbol_length+(1:ofdm_symbol_length),:);
    ifblock(1:prefix_length,:)=[];
    fblock=fft(ifblock,[],1);
    received_f(j+1,:,:) = [fblock(2:useful_carriers/2+1,:); fblock(end-useful_carriers/2+1:end,:)];
end