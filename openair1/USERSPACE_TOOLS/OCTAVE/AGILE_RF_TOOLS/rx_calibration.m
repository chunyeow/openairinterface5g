
% Maxime Guillaud - created Wed May 10 18:08:04 CEST 2006

gpib_card=0;      % first GPIB PCI card in the computer
gpib_device=28;   % this is configured in the signal generator Utilities->System->GPIB->Address menu
freqband=2;            % frequency band used by the openair card

cables_loss_dB = 6;    % we need to account for the power loss between the signal generator and the card input (splitter, cables)


gpib_send(gpib_card,gpib_device,'*RST;*CLS');   % reset and configure the signal generator
gpib_send(gpib_card,gpib_device,'POW -70dBm');
gpib_send(gpib_card,gpib_device,'FREQ 1.91452GHz');



oarf_config(freqband,'openair_config.cfg','openair_scenario.scn')

saturation_threshold =5;              % min number of samples (real+imaginary) equal to the max per frame to declare saturation

ALL_power_dBm = [-100:1:-60 -150];
ALL_rxrfmode = 0:2;
ALL_gain2391 = 0:5:120;           % this is some strange scale
ALL_gain9862 = 0:1:18;             % this in in dB


m=[];


%for power_dBm=ALL_power_dBm
  power_dBm=-70
  gpib_send(gpib_card,gpib_device,['POW ' int2str(power_dBm+cables_loss_dB) 'dBm']);
sleep(1)
  
  
  for gain9862=ALL_gain9862
  
    for gain2391= ALL_gain2391
  
  
       gpib_send(gpib_card,gpib_device,'OUTP:STAT ON'); %  activate output 

       oarf_set_rx_gain(gain2391,gain9862,gain2391,gain9862);

       nacq=0;   % count the number of tries to get data
       do 
    
          s=oarf_get_frame(freqband);   %oarf_get_frame
          sleep(.1);
	  nacq=nacq+1;
	until max(max(abs(s)))>0                % ensures that the frame is properly read

    for gain9862=ALL_gain9862

      for gain2391= ALL_gain2391
	SpN0 = mean(abs(s(:,1)).^2) - abs(mean(s(:,1))).^2;
	SpN1 = mean(abs(s(:,2)).^2) - abs(mean(s(:,2))).^2;
 

       gpib_send(gpib_card,gpib_device,'OUTP:STAT OFF'); %  deactivate output 
 
       oarf_set_rx_gain(gain2391,gain9862,gain2391,gain9862);

	nacq=0;   % count the number of tries to get data
	do 
          sleep(.1);
	  s=oarf_get_frame(freqband);
	  nacq=nacq+1;
	until max(max(abs(s)))>0                % ensures that the frame is properly read

	N0 = mean(abs(s(:,1)).^2) - abs(mean(s(:,1))).^2;
	N1 = mean(abs(s(:,2)).^2) - abs(mean(s(:,2))).^2;

<<<<<<< rx_calibration.m
       S0 = 10*log10(SpN0-N0);
       S1 = 10*log10(SpN1-N1);
       G0 = S0 - power_dBm;
       G1 = S1 - power_dBm;
       NF0 = N0 - G0 + 108;
       NF1 = N1 - G1 + 108;

       fprintf(' %d %d : Signal strength (%d,%d), Gain (%d %d), NF (%d %d)\n',
	       gain2391,gain9862, S0,S1,G0,G1,NF0,NF1); 

  end
 end

gpib_send(gpib_card,gpib_device,'OUTP:STAT OFF');         %  deactivate output



