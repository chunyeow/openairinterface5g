%frequency = input('Input freq in MHz: ', 's'); 
% script to setup mxa and sign generator 

gpib_card=0;      % first GPIB PCI card in the computer
sme=28;   % this is configured in the signal generator Utilities->System->GPIB->Address menu (for SME)
mxa=20;    % this is configured in the spectrum analsyer


%gpib_send(gpib_card,sme,'*RST;*CLS');   % reset and configure the signal generator
%gpib_send(gpib_card,sme,'POW -70dBm');
%gpib_send(gpib_card,sme,'FREQ 1.91452GHz');

gpib_send(gpib_card,mxa,'*CLS');
gpib_send(gpib_card,mxa,'SENS:FREQ:CENT 642MHz'); 

sleep(1);
gpib_send(gpib_card,mxa,'SENS:FREQ:SPAN 50MHz');

sleep(1);
%gpib_send(gpib_card,mxa,'SENS:BAND:RES 10KHZ');

sleep(1);
gpib_send(gpib_card,mxa,'CONF:ACP');
gpib_send(gpib_card,mxa,'INIT:ACP'); 

gpib_send(gpib_card,mxa,'SENS:ACP:FREQ:SPAN 20MHz');

gpib_send(gpib_card,mxa,'SENS:ACP:CARR1:LIST:BAND:INT 8MHz');

gpib_send(gpib_card,mxa,'SENS:ACP:OFFS1:LIST:STAT ON');

gpib_send(gpib_card,mxa,'SENS:ACP:OFFS1:LIST:FREQ 8MHZ,8MHZ,8MHZ,8MHZ,8MHZ,8MHZ');
%frequency = input('Input freq in MHz: ', 's'); 
 gpib_send(gpib_card,mxa,'SENS:ACP:OFFS1:LIST:BAND:INT 8MHZ,8MHZ,8MHZ,8MHZ,8MHZ,8MHZ');

gpib_send(0,20,'SENS:POW:RANG:OPT:ATT COMB')
gpib_send(0,20,'SENS:POW:RF:RANG:OPT IMM')


%gpib_send(gpib_card,mxa,'POW:ATT 40dB');
x=gpib_query(gpib_card,mxa,'SENS:ACP:CARR1:POW?',1);



cmd1='/homes/khalfall/work/openairinterface/arch/openair_CardBus_MIMO1/LEON3/hostpc/user/tools/scripts/set_transmitter -M ';
frequency='900';

ALL_gain1=0:16:63;
ALL_gain2=0:16:63;

i=0;
for gain1=ALL_gain1
for gain2= ALL_gain2

i=i+1;
end;
end;
table=zeros(i,4);
i=1;
for gain1=ALL_gain1
for gain2= ALL_gain2
cmd=strcat(cmd1,frequency,' -g1 ',int2str(gain1),' -g2 ',int2str(gain2));
%disp(cmd);
system(cmd);
%system('set_receiver -M ' frequency ' -g1 ' int2str(gain1) ' -g2 ' int2str(gain2));
%system('sleep 1');
sleep(.1);
table(i,1)=gain1;
table(i,2)=gain2;
%frequency = input('Input freq in MHz: ', 's'); 
x=gpib_query(gpib_card,mxa,'READ:ACP?',3);
table(i,3)=x(1);
table(i,4)=(x(2)+x(3))/2;
i=i+1;
end;
end;
disp(table);

P_num =100; %(warning, to be updated)



% Post processing of the table started here
Pmin = -40; % in dBm
Pmax = 20;  % in dBm
optimal_gain=zeros(Pmax-Pmin+1,4);



% a Tx calibration file is calculated for a given board and frequency

%fid=fopen(['/tmp/cal_TX' frequency '_' int2str(n) '.dat'],'wb')
%fwrite(fid,signal);
%fclose(fid);
