dual_tx=0;
card=0;
limeparms;
active_rf = [1 1 1 1];
%rf_mode = (RXEN+TXEN+TXLPFNORM+TXLPFEN+TXLPF25+RXLPFNORM+RXLPFEN+RXLPF25+LNA1ON+LNAMax+RFBBNORM)*[1 1 1 1];
rf_mode = (RXEN+TXEN+TXLPFNORM+TXLPFEN+TXLPF25+RXLPFNORM+RXLPFEN+RXLPF25+LNA1ON+LNAMax+RFBBNORM)*active_rf;
rf_mode = rf_mode+(DMAMODE_RX*active_rf);
%freq_rx = 2540000000*[1 1 1 1];
freq_rx = 1917600000*active_rf;
%freq_rx = 1912600000*active_rf; %+ 2540000000*[0 1 0 0]; % + 859500000*[0 0 1 0];
%freq_rx = 1912600000*[1 1 1 1];
%freq_rx = 859500000*[1 1 1 1];
freq_tx = freq_rx; %+1.92e6;
%freq_tx = 2660000000*[1 1 1 1];
tx_gain = 0*[1 1 1 1];
rx_gain = 30*[1 1 1 1];
rf_local= [8254744   8255063   8257340   8257340]; %rf_local*[1 1 1 1];
%rf_local= [8254212   8256991   8257340   8257340]; %exmimo2 850mhz
rf_rxdc = rf_rxdc*[1 1 1 1];
%rf_vcocal=rf_vcocal_859*[1 1 1 1];
rf_vcocal=rf_vcocal_19G*[1 1 1 1];
eNBflag = 0;
tdd_config = DUPLEXMODE_FDD + TXRXSWITCH_TESTRX; 
%tdd_config = DUPLEXMODE_FDD + TXRXSWITCH_LSB;
syncmode = SYNCMODE_FREE;
rffe_rxg_low = 61*active_rf;
rffe_rxg_final = 61*active_rf;
rffe_band = B19G_TDD*active_rf;
autocal = [1 1 1 1];

oarf_config_exmimo(card,freq_rx,freq_tx,tdd_config,syncmode,rx_gain,tx_gain,eNBflag,rf_mode,rf_rxdc,rf_local,rf_vcocal,rffe_rxg_low,rffe_rxg_final,rffe_band,autocal)

gpib_card=0;      % first GPIB PCI card in the computer
gpib_device=28;   % this is configured in the signal generator Utilities->System
cables_loss_dB = 6;    % we need to account for the power loss between the signa
power_dBm = -95;

%gpib_send(gpib_card,gpib_device,['POW ' int2str(power_dBm+cables_loss_dB) 'dBm']);
%gpib_send(gpib_card,gpib_device,'OUTP:STAT ON'); %  activate output 

