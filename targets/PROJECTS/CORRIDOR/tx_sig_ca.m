% this script sends the signals previously generated with generation_ca to
% the cards. We need in total 7 cards configured as follows
% card0 - card3: 20MHz, 1 channel each, s1, freq 2590 MHz
% card4 - card5: 10MHz, 2 channels each, s2, freq 2605 MHz
% card6: 5MHz, 4 channels, s3, freq 771.5 MHz

%load('ofdm_pilots_sync_30MHz.mat');

addpath('../../../targets/ARCH/EXMIMO/USERSPACE/OCTAVE')
limeparms;

num_cards = oarf_get_num_detected_cards;

% common parameters
  rf_local= rf_local*[1 1 1 1];
  rf_rxdc = rf_rxdc*[1 1 1 1];
  rf_vcocal=rf_vcocal_19G*[1 1 1 1];

for card=0:min(3,num_cards-1)
  % card 0-3: 20MHz
  active_rf = [1 0 0 0];
  autocal = [1 1 1 1];
  resampling_factor = [0 0 0 0];

  rf_mode = (RXEN+TXEN+TXLPFNORM+TXLPFEN+TXLPF10+RXLPFNORM+RXLPFEN+RXLPF10+LNA1ON+LNAMax+RFBBNORM)*active_rf;
  rf_mode = rf_mode+((DMAMODE_RX+DMAMODE_TX)*active_rf);
  freq_rx = 2590e6*active_rf;
  freq_tx = freq_rx;
  tx_gain = 10*active_rf; %[1 1 1 1];
  rx_gain = 0*active_rf; %1 1 1 1];

  eNBflag = 0;
  tdd_config = DUPLEXMODE_FDD + TXRXSWITCH_TESTTX; 
  %tdd_config = DUPLEXMODE_FDD + TXRXSWITCH_LSB;
  if (card==0)
    syncmode = SYNCMODE_MASTER;
  else
    syncmode = SYNCMODE_SLAVE;
  end
  rffe_rxg_low = 31*active_rf; %[1 1 1 1];
  rffe_rxg_final = 63*active_rf; %[1 1 1 1];
  rffe_band = B19G_TDD*active_rf; %[1 1 1 1];
  oarf_config_exmimo(card,freq_rx,freq_tx,tdd_config,syncmode,rx_gain,tx_gain,eNBflag,rf_mode,rf_rxdc,rf_local,rf_vcocal,rffe_rxg_low,rffe_rxg_final,rffe_band,autocal,resampling_factor)
end

for card=4:min(5,num_cards-1)
  % card 4-5: 10MHz
  active_rf = [1 1 0 0];
  autocal = [1 1 1 1];
  resampling_factor = [1 1 1 1];

  rf_mode = (RXEN+TXEN+TXLPFNORM+TXLPFEN+TXLPF5+RXLPFNORM+RXLPFEN+RXLPF5+LNA1ON+LNAMax+RFBBNORM)*active_rf;
  rf_mode = rf_mode+((DMAMODE_RX+DMAMODE_TX)*active_rf);
  freq_rx = 2605e6*active_rf;
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

for card=6:min(6,num_cards-1)
  % card 6: 5MHz
  active_rf = [1 1 1 1];
  autocal = [1 1 1 1];
  resampling_factor = [2 2 2 2];

  rf_mode = (RXEN+TXEN+TXLPFNORM+TXLPFEN+TXLPF5+RXLPFNORM+RXLPFEN+RXLPF5+LNA1ON+LNAMax+RFBBNORM)*active_rf;
  rf_mode = rf_mode+((DMAMODE_RX+DMAMODE_TX)*active_rf);
  freq_rx = 771.5e6*active_rf;
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
s1p = 2*floor(amp*(s1./max([real(s1(:)); imag(s1(:))])));
s2p = 2*floor(amp*(s2./max([real(s2(:)); imag(s2(:))])));
s3p = 2*floor(amp*(s3./max([real(s3(:)); imag(s3(:))])));

for card=min(6,num_cards-1):-1:6
  oarf_send_frame(card,s3p.',16);
end
for card=min(5,num_cards-1):-1:4
  oarf_send_frame(card,s2p.',16);
end
for card=min(3,num_cards-1):-1:0
  oarf_send_frame(card,s1p.',16);
end

