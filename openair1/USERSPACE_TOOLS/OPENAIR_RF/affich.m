clear all
close all

fd = fopen('rx_frame.dat','r');
temp = fread(fd,'int16');
fclose(fd);

rxs = temp(1:2:length(temp)) + sqrt(-1)*temp(2:2:length(temp));
rxs = reshape(rxs,[],2);
%rxs = rxs(:,1);

% estimate frequency offset (for a fs/4 signal)
fs = 7680e3;
s_phase = unwrap(angle(rxs(1:4:length(rxs),1).'));
s_phase2 = unwrap(angle(rxs(1:4:length(rxs),2).'));
s_phase = s_phase - s_phase(1,1);
s_phase2 = s_phase2 - s_phase2(1,1);

f_off = mean(s_phase(2:length(s_phase))*fs/4./(1:(length(s_phase)-1))/2/pi)
f_off2 = mean(s_phase2(2:length(s_phase2))*fs/4./(1:(length(s_phase2)-1))/2/pi)


figure(1)
plot(real(rxs))
print('rx_frame_time.esp','-depsc2')

figure(2)
plot(20*log10(abs(fftshift(fft(rxs)))))
print('rx_frame_freq.esp','-depsc2')

%%
fd = fopen('tx_frame.dat','r');
temp = fread(fd,'uint8');
fclose(fd);

%txs(:,1) = temp(1:4:length(temp)) + sqrt(-1)*temp(2:4:length(temp));
%txs(:,2) = temp(3:4:length(temp)) + sqrt(-1)*temp(4:4:length(temp));

load('../../PHY/LTE_REFSIG/mod_table.mat')

for i=1:length(temp)
  txs(i) = table(temp(i)+1);
end

figure(3)
plot(real(txs))

