clear all
close all

affich

%%
primary_synch0 = [exp(-1j*pi*25*(0:30).*(1:31)/63) 0 exp(-1j*pi*25*(32:62).*(33:63)/63)];
primary_synch0_mod2 = zeros(1,256);
primary_synch0_mod2((256-30):end)=primary_synch0(1:31);
primary_synch0_mod2(1:32)=primary_synch0(32:end);
primary_synch0_time = ifft(primary_synch0_mod2)*sqrt(256);

%%
c1 = xcorr(rxs(:,1),primary_synch0_time);
c2 = xcorr(rxs(:,2),primary_synch0_time);
c = c1+c2;
figure(3)
plot(abs(c))
[m,i] = max(abs(c));
sync_pos = i-38400;
figure(1)
hold on
%plot(sync_pos,0,'rx','Markersize',20,'Linewidth',5)
plot(sync_pos,0,'rx')

%%
temp = rxs(sync_pos-5*320:sync_pos-320*4-1,1);
tempF = fft(temp(1:256));
symb0 = [tempF(167:end,1); tempF(2:91,1)];


%% "standard" channel estimation and FFT interpolation
% load the pilots for the first OFDM symbol
pilotsF  
pilot_symb0 = [ rsF(167:end,1); rsF(2:91,1)];

h = 1./(pilot_symb0(2:6:end)).*symb0(2:6:end);
h = fft([ifft(h); zeros(150,1)]);

figure(4)
plot(abs(h))