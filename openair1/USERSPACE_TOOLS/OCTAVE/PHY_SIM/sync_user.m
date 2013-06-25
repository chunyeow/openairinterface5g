%system('source /opt/intel/cc/10.0.023/bin/iccvars.sh')
%system('./sync_user');

clear all
close all

% fd = fopen('rx_frame_2streams.dat','r');
% temp = fread(fd,20480*8,'int16');
% fclose(fd);

NRx=2;

%rxs = temp(1:2:length(temp)) + sqrt(-1)*temp(2:2:length(temp));
%rxs = reshape(rxs,[],NRx);

rxsig0
rxsig1
rxs(:,1) = rxs0;
rxs(:,2) = rxs1;

% do synchronization here
CHSCH0_sync_4xf0
CHSCH0_sync_4xf1

c0 = abs(conv(rxs(:,1),fliplr(ifft(chsch0_sync_f_4x0)')));
c1 = abs(conv(rxs(:,1),fliplr(ifft(chsch0_sync_f_4x1)')));
csum = (c0.^2) + (c1.^2);
[maxlev0,maxpos0] = max(csum);
%sync_pos = maxpos0-1024
%sync_pos = 16158 -1024
sync_pos=1

figure(1)
plot(csum);

% do Channel estimation here

CPL = 64;
SL = 256;
SLP = CPL + SL;

rxchsch = rxs(sync_pos + 1: sync_pos + 4*SLP,:);
rxchbch = rxs(sync_pos + 4*SLP + 1: sync_pos + 12*SLP,:);
plot_style = {'r','g','b','k'};

for i=0:3
    RXchsch = fft(rxchsch(i*SLP+CPL+1 : (i+1)*SLP,:));
    eval(sprintf('chsch%d_syncf',i));
    chsch_sync_f = eval(sprintf('chsch%d_sync_f',i));
    for a=0:NRx-1
        Hest(:,a+1,i+1) = RXchsch(:,a+1) .* conj(chsch_sync_f);
        Hest2(:,a+1,i+1) = fft(ifft(Hest(:,a+1,i+1)).*[zeros(1,160) ones(1,96)].');
    end
    figure(2)
    plot(20*log10(1+abs(Hest2(:,1,i+1))),plot_style{i+1})
    hold on
    figure(3)
    plot(abs(ifft(Hest(1:2:end,1,i+1))),plot_style{i+1})
    hold on
    %plot(imag(ifft(Hest)),'g')
    %hold off
end

%%
symb_comp = zeros(8,SL);
mask = [true(1,80) false(1,96) true(1,80)];

for s = 0:7
    symb = fft(rxchbch((s*(CPL+SL)+CPL+1):(((s+1)*(CPL+SL))),:));
    for c=0:255
        symb_comp(s+1,c+1) = conj(Hest2(c+1,:,2))*symb(c+1,:)';
    end
end



figure(6)

plot(squeeze(symb_comp(1,:)),'rx')
hold on
plot(squeeze(symb_comp(2,:)),'gx')
%  plot(symb_comp(3,:),'bx')
%  plot(symb_comp(4,:),'kx')
%  plot(symb_comp(5,:),'cx')
%  plot(symb_comp(6,:),'mx')
%  plot(symb_comp(7,:),'wx')
hold off
