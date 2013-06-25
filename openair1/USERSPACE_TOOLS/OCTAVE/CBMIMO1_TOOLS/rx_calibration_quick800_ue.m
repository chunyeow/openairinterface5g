close all
clear all
hold off

% Maxime Guillaud - created Wed May 10 18:08:04 CEST 2006
% Raymond & Florian - last modified Fri Oct 26 17:12:10 CEST 2007


gpib_card=0;      % first GPIB PCI card in the computer
gpib_device=28;   % this is configured in the signal generator Utilities->System->GPIB->Address menu
freqband=0;            % frequency band used by the openair card

fc = 1902600e3+5e6*freqband-(1043.1e6);   % this has to be the same as in the config file
#fc = 1902600e3+5e6*freqband;

fs = 7680e3;
%fs = 6500e3;
fref = fc+fs/4;

dual_tx = 0;
cables_loss_dB = 6;    % we need to account for the power loss between the signal generator and the card input (splitter, cables)


gpib_send(gpib_card,gpib_device,'*RST;*CLS');   % reset and configure the signal generator
gpib_send(gpib_card,gpib_device,'POW -90dBm');
%gpib_send(gpib_card,gpib_device,'FREQ 1.91860GHz');
%gpib_send(gpib_card,gpib_device,'FREQ 1.919225GHz');
%gpib_send(gpib_card,gpib_device,'FREQ 1.909225GHz');
gpib_send(gpib_card,gpib_device,sprintf("FREQ %dHz",fref));

oarf_config(freqband,'config.cfg','scenario.scn',dual_tx)
#oarf_set_rx_rfmode(1);

saturation_threshold =5;              % min number of samples (real+imaginary) equal to the max per frame to declare saturation

ALL_power_dBm = [-102];
ALL_rxrfmode = 0:2;
ALL_gain2391 = 5:10:160;           % this is some strange scale
ALL_gain9862 = 0; %:1:18;             % this in in dB

%fprintf(fid, 'Tx Power (dBm), gain2391, gain9862, Signal strength Rx0, Signal Strength Rx1, Gain Rx0, Gain Rx1, Noise Rx0, Noise Rx1, SNR Rx0, SNR Rx1, NF Rx0, NF Rx1\n');

SpN0 = zeros(length(ALL_power_dBm),length(ALL_gain2391));
SpN1 = zeros(length(ALL_power_dBm),length(ALL_gain2391));
N0 = zeros(length(ALL_power_dBm),length(ALL_gain2391));
N1 = zeros(length(ALL_power_dBm),length(ALL_gain2391));
S0 = zeros(length(ALL_power_dBm),length(ALL_gain2391));
S1 = zeros(length(ALL_power_dBm),length(ALL_gain2391));
G0 = zeros(length(ALL_power_dBm),length(ALL_gain2391));
G1 = zeros(length(ALL_power_dBm),length(ALL_gain2391));
NF0 = zeros(length(ALL_power_dBm),length(ALL_gain2391));
NF1 = zeros(length(ALL_power_dBm),length(ALL_gain2391));
SNR0 = zeros(length(ALL_power_dBm),length(ALL_gain2391));
SNR1 = zeros(length(ALL_power_dBm),length(ALL_gain2391));

min_agc_level = 130; %min_agc_level = 90;  %ceil(min(max(G0,[],1)));
max_agc_level = 150; %floor(max(max(G0,[],1)));

oarf_set_calibrated_rx_gain(0); % turns off the AGC
#oarf_set_rx_rfmode(2);

idx_power = 1;
for power_dBm=ALL_power_dBm

  gpib_send(gpib_card,gpib_device,['POW ' int2str(power_dBm+cables_loss_dB) 'dBm']);
  
  for gain9862=ALL_gain9862
  
    idx_gain2391 = 1; 
    for gain2391= [0 0 0 ALL_gain2391] %the first 3 runs give strange results, so we run the loop 3 times more
 
       oarf_set_rx_gain(gain2391,gain2391,gain9862,gain9862);
	   oarf_set_rx_rfmode(1);
       oarf_stop(freqband); %this actually writes the gain to the registers
       sleep(.5);

       if 1
       % signal measurement
       gpib_send(gpib_card,gpib_device,'OUTP:STAT ON'); %  activate output 
       sleep(.1);

       nacq=0;   % count the number of tries to get data
       do 
          s=oarf_get_frame(freqband);   %oarf_get_frame
	  s2 = s(1:8192,:);
          sleep(.1);
	  nacq=nacq+1;
       until max(max(abs(s)))>0                % ensures that the frame is properly read

       SpN0(idx_power,idx_gain2391) = mean(abs(s2(:,1)).^2) - abs(mean(s2(:,1))).^2;
       SpN1(idx_power,idx_gain2391) = mean(abs(s2(:,2)).^2) - abs(mean(s2(:,2))).^2;

       figure(1);
       hold off
       plot(20*log10(abs(fft(s2(:,1)))),'r',20*log10(abs(fft(s2(:,2)))),'b')
       title("Signal");
 
       % noise measurement
       gpib_send(gpib_card,gpib_device,'OUTP:STAT OFF'); %  deactivate output 
       sleep(.1);

       nacq=0;   % count the number of tries to get data
       do 
          s=oarf_get_frame(freqband);   %oarf_get_frame
	  s2 = s(1:8192,:);
	  sleep(.1);
	  nacq=nacq+1;
       until max(max(abs(s)))>0                % ensures that the frame is properly read

       N0(idx_power,idx_gain2391) = mean(abs(s2(:,1)).^2) - abs(mean(s2(:,1))).^2;
       N1(idx_power,idx_gain2391) = mean(abs(s2(:,2)).^2) - abs(mean(s2(:,2))).^2;

       figure(2);
       hold off
       plot(20*log10(abs(fft(s2(:,1)))),'r',20*log10(abs(fft(s2(:,2)))),'b')
       title("Noise");

       % do some plausibility checks
       % if ((N0(idx_power,idx_gain2391) > SpN0(idx_power,idx_gain2391)) ||
       %   (N1(idx_power,idx_gain2391) > SpN1(idx_power,idx_gain2391)))
       %  error("something is wrong");
       % end

       S0(idx_power,idx_gain2391) = 10*log10(SpN0(idx_power,idx_gain2391)-N0(idx_power,idx_gain2391));
       S1(idx_power,idx_gain2391) = 10*log10(SpN1(idx_power,idx_gain2391)-N1(idx_power,idx_gain2391));
       G0(idx_power,idx_gain2391) = S0(idx_power,idx_gain2391) - power_dBm;
       G1(idx_power,idx_gain2391) = S1(idx_power,idx_gain2391) - power_dBm;
       NF0(idx_power,idx_gain2391) = 10*log10(N0(idx_power,idx_gain2391)) - G0(idx_power,idx_gain2391) + 107;   % 108 is the thermal noise
       NF1(idx_power,idx_gain2391) = 10*log10(N1(idx_power,idx_gain2391)) - G1(idx_power,idx_gain2391) + 107;
       SNR0(idx_power,idx_gain2391) = S0(idx_power,idx_gain2391)-10*log10(N0(idx_power,idx_gain2391));
       SNR1(idx_power,idx_gain2391) = S1(idx_power,idx_gain2391)-10*log10(N1(idx_power,idx_gain2391));

       printf(' %d %d : Signal strength (%f,%f), Gain (%f %f), N (%f %f) SNR (%f %f) NF (%f %f)\n',
	      gain2391,gain9862, S0(idx_power,idx_gain2391),S1(idx_power,idx_gain2391),
	      G0(idx_power,idx_gain2391),G1(idx_power,idx_gain2391),
	      10*log10(N0(idx_power,idx_gain2391)),10*log10(N1(idx_power,idx_gain2391)),
	      SNR0(idx_power,idx_gain2391),SNR1(idx_power,idx_gain2391),
	      NF0(idx_power,idx_gain2391),NF1(idx_power,idx_gain2391)); 
       fflush(stdout);
       %fprintf(fid,'%d, %d, %d, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f\n',
	%      power_dBm,gain2391,gain9862, S0,S1,G0,G1,10*log10(N0),10*log10(N1),SNR0,SNR1,NF0,NF1); 
 

       if (gain2391 > 0)
	 idx_gain2391 = idx_gain2391 + 1;
       end

       figure(3)
       hold off
       plot(ALL_gain2391,G0,'r')
       hold on
       plot(ALL_gain2391,G1,'b')
       legend('Rx0','Rx1');
       title('Gains')
       
       figure(4)
       hold off
       plot(ALL_gain2391,NF0,'r')
       hold on
       plot(ALL_gain2391,NF1,'b')
       legend('Rx0','Rx1');
       title('Noise Figure')
       end

     end
   end
   %power_dBm = power_dBm + 20;
   idx_power = idx_power + 1;
 end

gpib_send(gpib_card,gpib_device,'OUTP:STAT OFF');         %  deactivate output

%l0 = [ALL_gain2391; ones(size(ALL_gain2391))].'\G0;
%l1 = [ALL_gain2391; ones(size(ALL_gain2391))].'\G1;

gain_levels = zeros(max_agc_level-min_agc_level+1,2);

gain_levels(:,1) = round(interp1(max(G0,[],1),ALL_gain2391,min_agc_level:max_agc_level,'linear','extrap')).';
gain_levels(:,2) = round(interp1(max(G1,[],1),ALL_gain2391,min_agc_level:max_agc_level,'linear','extrap')).';

%gain_levels(isnan(gain_levels)) = 0;

write_table
