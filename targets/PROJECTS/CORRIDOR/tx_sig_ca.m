limeparms;
freq = 2.6e9;

num_cards = oarf_get_num_detected_cards;

% common parameters
  rf_local= rf_local*[1 1 1 1];
  rf_rxdc = rf_rxdc*[1 1 1 1];
  rf_vcocal=rf_vcocal_19G*[1 1 1 1];

if (num_cards>0)
  % card 0: 20MHz
  card = 0;
  active_rf = [1 0 0 0];
  autocal = [1 1 1 1];
  resampling_factor = [0 0 0 0];

  rf_mode = (RXEN+TXEN+TXLPFNORM+TXLPFEN+TXLPF10+RXLPFNORM+RXLPFEN+RXLPF10+LNA1ON+LNAMax+RFBBNORM)*active_rf;
  rf_mode = rf_mode+((DMAMODE_RX+DMAMODE_TX)*active_rf);
  freq_rx = (freq-10e6)*active_rf;
  freq_tx = freq_rx;
  tx_gain = 10*active_rf; %[1 1 1 1];
  rx_gain = 0*active_rf; %1 1 1 1];

  eNBflag = 0;
  tdd_config = DUPLEXMODE_FDD + TXRXSWITCH_TESTTX; 
  %tdd_config = DUPLEXMODE_FDD + TXRXSWITCH_LSB;
  if (num_cards>1)
    syncmode = SYNCMODE_MASTER;
  else
    syncmode = SYNCMODE_FREE;
  end
  rffe_rxg_low = 31*active_rf; %[1 1 1 1];
  rffe_rxg_final = 63*active_rf; %[1 1 1 1];
  rffe_band = B19G_TDD*active_rf; %[1 1 1 1];
  oarf_config_exmimo(card,freq_rx,freq_tx,tdd_config,syncmode,rx_gain,tx_gain,eNBflag,rf_mode,rf_rxdc,rf_local,rf_vcocal,rffe_rxg_low,rffe_rxg_final,rffe_band,autocal,resampling_factor)
end

if (num_cards>1)
  % card 1: 10MHz
  card = 1;
  active_rf = [1 1 0 0];
  autocal = [1 1 1 1];
  resampling_factor = [1 1 1 1];

  rf_mode = (RXEN+TXEN+TXLPFNORM+TXLPFEN+TXLPF5+RXLPFNORM+RXLPFEN+RXLPF5+LNA1ON+LNAMax+RFBBNORM)*active_rf;
  rf_mode = rf_mode+((DMAMODE_RX+DMAMODE_TX)*active_rf);
  freq_rx = (freq+5e6)*active_rf;
  freq_tx = freq_rx;
  tx_gain = 13*active_rf; %[1 1 1 1];
  rx_gain = 0*active_rf; %1 1 1 1];
  eNBflag = 0;
  tdd_config = DUPLEXMODE_FDD + TXRXSWITCH_TESTTX; 
  %tdd_config = DUPLEXMODE_FDD + TXRXSWITCH_LSB;
  syncmode = SYNCMODE_SLAVE;
  rffe_rxg_low = 31*active_rf; %[1 1 1 1];
  rffe_rxg_final = 63*active_rf; %[1 1 1 1];
  rffe_band = B19G_TDD*active_rf; %[1 1 1 1];
  oarf_config_exmimo(card,freq_rx,freq_tx,tdd_config,syncmode,rx_gain,tx_gain,eNBflag,rf_mode,rf_rxdc,rf_local,rf_vcocal,rffe_rxg_low,rffe_rxg_final,rffe_band,autocal,resampling_factor)
end

amp = pow2(14)-1;
s1p = 2*floor(amp*(s1./max([real(s1) imag(s1)])));
s2p = 2*floor(amp*(s2./max([real(s2) imag(s2)])));

if (num_cards>1)
  oarf_send_frame(1,repmat(s2p,4,1).',16);
end
oarf_send_frame(0,repmat(s1p,4,1).',16);

