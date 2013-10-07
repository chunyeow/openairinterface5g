% do some beamforming
signalA2B=zeros(N,4);
signalB2A=zeros(N,4);
signalB2A_prec=zeros(N,4);

%%----------Node A to B transmission---------%%
for i=1:4
    if(active_rfA(i))
        [Da2b_T, tmps]=genrandpskseq(N,M,amp);
        signalA2B(:,i)=tmps*2; %make sure LSB is 0 (switch=tx)
    else
        signalA2B(:,i)=repmat(1+1j,76800,1); %make sure LSB is 1 (switch=rx)
    end
end

oarf_send_frame(card,signalA2B,n_bit);
%keyboard
sleep(0.01);
receivedA2B=oarf_get_frame(card);
%oarf_stop(card); %not good, since it does a reset
sleep(0.01);

%% ------- Do the A to B channel estimation ------- %%
for i=0:119;
    ifblock=receivedA2B(i*640+[1:640],indB);
    ifblock(1:128,:)=[];
    fblock=fft(ifblock);
    fblock(1,:)=[];
    fblock(151:360,:)=[];
    Da2b_R(i+1,:)=vec(fblock);
end
chanestsA2B=reshape(diag(repmat(Da2b_T,1,Nantb)'*Da2b_R)/size(Da2b_T,1),301,Nantb);
%fchanestsA2B(:,:,meas)=[zeros(1,Nantb); chanestsA2B([1:150],:,meas); zeros(210,Nantb); chanestsA2B(151:301,:,meas)];
%tchanestsA2B(:,:,meas)=ifft(fchanestsA2B(:,:,meas));

%% calculate beamformer based on chanestA2B 
for i=1:301
    YA=chanestsA2B(i,:).';
    F=Fs(:,:,i);
    Bd(:,i) = conj(F*YA)./norm(F*YA);
end

%% generate normal and beamformed signals
[seqf, tmps]=genrandpskseq(N,M,amp);
for i=1:size(seqf,1)
    % precoding
    for j=1:size(seqf,2)
        symbol_prec(:,j)=Bd(:,j)*seqf(i,j);
    end
    % insert zero subcarriers
    symbol_prec=cat(2,zeros(3,1),symbol_prec(:,1:150),zeros(3,210),symbol_prec(:,151:301));
    % ofdm modulation
    symbol_prec_t=ifft(symbol_prec,512,2);
    % Adding cycl. prefix making the block of 640 elements
    symbol_prec_cp = cat(2,symbol_prec_t(:,end-127:end), symbol_prec_t);
    tmps_prec(:,[1:640]+(i-1)*640)=floor(amp*symbol_prec_cp);
end

for i=1:4
    if(active_rfB(i))
      signalB2A(:,i)=floor(tmps/sqrt(3))*2; %make sure LSB is 0 (switch=tx)
        signalB2A_prec(:,i)=tmps_prec(i-1,:)*2; %make sure LSB is 0 (switch=tx)
    else
        signalB2A(:,i)=repmat(1+1j,76800,1); %make sure LSB is 1 (switch=rx)
        signalB2A_prec(:,i)=repmat(1+1j,76800,1); %make sure LSB is 1 (switch=rx)
    end
end

%% send normal signal
P_tx = 10*log10(sum(mean(abs(signalB2A(:,2:4)).^2)))
oarf_send_frame(card,signalB2A,n_bit);
%keyboard
sleep(0.01);
receivedB2A=oarf_get_frame(card);
sleep(0.01);
% measure SNR
P_rx = 10*log10(mean(abs(receivedB2A(:,1)).^2))

%% send beamformed DL signal
P_tx_prec = 10*log10(sum(mean(abs(signalB2A_prec(:,2:4)).^2)))
oarf_send_frame(card,signalB2A_prec,n_bit);
%keyboard
sleep(0.01);
receivedB2A_prec=oarf_get_frame(card);
sleep(0.01);
% measure SNR
P_rx_prec = 10*log10(mean(abs(receivedB2A_prec(:,1)).^2))

figure(10)
bar([P_tx P_tx_prec; P_rx P_rx_prec]);
legend('normal','beamformed')
ylim([0 60])
drawnow
