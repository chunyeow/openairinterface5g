%frequency = input('Input freq in MHz: ', 's'); 


gpib_card=0;      % first GPIB PCI card in the computer
gpib_sig_gen=28;   % this is configured in the signal generator Utilities->System->GPIB->Address menu (for SME)
gpib_spec_ana=20;    % this is configured in the spectrum analsyer


gpib_send(gpib_card,gpib_device,'*RST;*CLS');   % reset and configure the signal generator
gpib_send(gpib_card,gpib_device,'POW -70dBm');
gpib_send(gpib_card,gpib_device,'FREQ 1.91452GHz');

cmd1='/homes/khalfall/work/openairinterface/arch/openair_CardBus_MIMO1/LEON3/hostpc/user/tools/scripts/set_receiver -M ';
frequency='900';

ALL_gain1=61:1:63;
ALL_gain2=61:1:63;

for gain1=ALL_gain1
  
    for gain2= ALL_gain2
cmd=strcat(cmd1,frequency,' -g1 ',int2str(gain1),' -g2 ',int2str(gain2));
disp(cmd);
system(cmd);
%system('set_receiver -M ' frequency ' -g1 ' int2str(gain1) ' -g2 ' int2str(gain2));
system('sleep 1');
end;
end;

num=10;
openair_init;
%for n=1:num
%s=oarf_get_frame(0);
%plot(20*log10(abs(fft(s(:,1)))))
%signal=s(:,1);
%fid=fopen(['/tmp/' frequency '_' int2str(n) '.dat'],'wb')
%fwrite(fid,signal);
%fclose(fid);
%end;



