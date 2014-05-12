function p = init_params(nb_rb,nant_rx,nant_tx)

global symbols_per_slot slots_per_frame;
num_symbols_frame = symbols_per_slot*slots_per_frame;

p.nb_rb = nb_rb;
p.num_carriers = 2048/(100/p.nb_rb);
p.num_zeros = p.num_carriers-(12*p.nb_rb+1);
p.useful_carriers = p.num_carriers-p.num_zeros-1;
p.prefix_length = p.num_carriers/4; %this is extended CP
p.ofdm_symbol_length = p.num_carriers + p.prefix_length;
p.samples_slot = p.ofdm_symbol_length*symbols_per_slot;
p.frame_length = p.ofdm_symbol_length*num_symbols_frame;

p.nant_rx=nant_rx;
p.nant_tx=nant_tx;

