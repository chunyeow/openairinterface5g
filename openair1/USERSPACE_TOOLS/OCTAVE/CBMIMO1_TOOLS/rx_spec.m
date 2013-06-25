dual_tx=0;
oarf_config(0,1,dual_tx)

gpib_card=0;      % first GPIB PCI card in the computer
gpib_device=28;   % this is configured in the signal generator Utilities->System
cables_loss_dB = 6;    % we need to account for the power loss between the signa
power_dBm = -95;

%gpib_send(gpib_card,gpib_device,['POW ' int2str(power_dBm+cables_loss_dB) 'dBm']);
%gpib_send(gpib_card,gpib_device,'OUTP:STAT ON'); %  activate output 

oarf_set_calibrated_rx_gain(0); % turns off the AGC
oarf_set_rx_gain(80,85,0,0);
oarf_set_rx_rfmode(0);

s=oarf_get_frame(0);
f = (7.68*(0:length(s(:,1))-1)/(length(s(:,1))))-3.84;
spec0 = 20*log10(abs(fftshift(fft(s(:,1)))));
spec1 = 20*log10(abs(fftshift(fft(s(:,2)))));

clf
plot(f',spec0,'r',f',spec1,'b')
axis([-3.84,3.84,40,160]);
%gpib_send(gpib_card,gpib_device,'OUTP:STAT OFF'); %  activate output 
legend('Antenna Port 0','Antenna Port 1');
grid
