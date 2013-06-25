fc = 1907600000;
fidx=1;
tdd=1;
dual_tx=1;
cbmimo1=0;
rxgain=30;
txgain=25;
eNB_flag = 0;

off = 0; % -994;
if (cbmimo1)
  oarf_config(fidx,tdd,dual_tx,255);
  amp = pow2(7)-1;
  n_bit = 8;
else
  limeparms;
  rf_mode   = RXEN+TXEN+TXLPFNORM+TXLPFEN+TXLPF25+RXLPFNORM+RXLPFEN+RXLPF25+LNA1ON+LNAMax+RFBBNORM * ones(1,4);
%  rf_mode   = RXEN+TXEN+TXLPFNORM+TXLPFEN+TXLPF25+RXLPFNORM+RXLPFEN+RXLPF25+LNA1ON+LNAByp+RFBBLNA1;

  rf_local  = rf_local * ones(1,4);
  rf_rxdc = rf_rxdc * ones(1,4);
  rf_vcocal = rf_vcocal * ones(1,4);
  rxgain = rxgain*ones(1,4);
  txgain = txgain*ones(1,4);
  freq_rx = fc*[1 1 1 1];
  freq_tx = freq_rx+1920000;

  oarf_config_exmimo(freq_rx,freq_tx,1,dual_tx,rxgain,txgain,eNB_flag,rf_mode,rf_rxdc,rf_local,rf_vcocal);
  amp = pow2(13)-1;
  n_bit = 16;
end

s = zeros(76800,2);

select = 1;

switch(select)

case 1
  s(:,1) = floor(amp * (exp(sqrt(-1)*.5*pi*(0:((76800)-1)))));
  s(:,2) = floor(amp * (exp(sqrt(-1)*.5*pi*(0:((76800)-1)))));

case 2
  s(38400+128,1)= 80-1j*40;
  s(38400+138,1)= -80+1j*40;

case 3
  for i=0:(12*5-1)
    s((38401+640*i):(38400+640*(i+1)),1)=floor(linspace(-127,128,640)); %+1j*floor(linspace(128,-127,640));
    end

case 4
  pss0_f0=[0,0,0,0,0,0,0,0,0,0,32767,0,-26120,-19785,11971,-30502,-24020,-22288,32117,6492,31311,9658,-16384,-28378,25100,-21063,-7292,-31946,20429,25618,14948,29158,11971,-30502,31311,9658,25100,-21063,-16384,28377,-24020,22287,32117,6492,-7292,31945,20429,25618,-26120,-19785,-16384,-28378,-16384,28377,-26120,-19785,-32402,4883,31311,-9659,32117,6492,-7292,-31946,32767,-1,25100,-21063,-24020,22287,-32402,4883,-32402,4883,-24020,22287,25100,-21063,32767,-1,-7292,-31946,32117,6492,31311,-9659,-32402,4883,-26120,-19785,-16384,28377,-16384,-28378,-26120,-19785,20429,25618,-7292,31945,32117,6492,-24020,22287,-16384,28377,25100,-21063,31311,9658,11971,-30502,14948,29158,20429,25618,-7292,-31946,25100,-21063,-16384,-28378,31311,9658,32117,6492,-24020,-22288,11971,-30502,-26120,-19785,32767,0,0,0,0,0,0,0,0,0,0,0];

  pss0_f = pss0_f0(1:2:length(pss0_f0)) + sqrt(-1)*pss0_f0(2:2:length(pss0_f0));
  
  pss0_f = [0 pss0_f(37:72) zeros(1,512-73) pss0_f(1:36)];
  pss0_t = ifft(pss0_f);
  pss0_t = [pss0_t((512-127):512) pss0_t];
  pss0_t = [pss0_t zeros(1,512+36) pss0_t]; 
  pss0_max = max(max(abs(real(pss0_t))),max(abs(imag(pss0_t))));
  
  pss0_t_fp_re = floor(real(8192*pss0_t/pss0_max)); 
  pss0_t_fp_im = floor(imag(8192*pss0_t/pss0_max)); 
  
  s(38400+(1:length(pss0_t_fp_re)),1) = 2*floor(pss0_t_fp_re) + 2*sqrt(-1)*floor(pss0_t_fp_im);
  s(38400+(1:length(pss0_t_fp_re)),2) = 2*floor(pss0_t_fp_re) + 2*sqrt(-1)*floor(pss0_t_fp_im);
otherwise 
  error('unknown case')
endswitch

if (cbmimo1)
  oarf_set_tx_gain(110,110,110,110);
else
  s = s*2;
end
oarf_send_frame(0,s,n_bit);

figure(1)
hold off
plot(real(s(:,1)),'r')
hold on
plot(imag(s(:,2)),'b')
