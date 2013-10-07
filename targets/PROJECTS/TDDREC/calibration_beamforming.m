initparams;
N = 100; % N is number ofr time-measurements
Nt = 3; % Nt is the number of antennas at node B
N_loc = 3; %check
CHA2B = {}; %this should be a cell array
CHB2A = {}; %this should be a cell array

% run measurements for all location
for loc = 1:N_loc
    run_measwoduplex;
    % alternatively load the measurements from file for testing
    % now you should have chanestA2B, fchanestA2B, tchanestA2B, chanestB2A,
    % fchanestB2A, tchanestB2A
    CHA2B{loc} = chanestA2B; %to check
    CHA2B{loc} = chanestA2B; %to check
    disp('Please move the antenna to another location and press key when finished')
    pause
end

%% calculate full F matrix
for s=1:301
	[F, HA]=alterproj(CHB2A,CHA2B, s, N, N_loc, Nt); 
	Fs{s}=F;
	HAs{s}=HA;
end

%% plot F
figure(1)
clf
hold on;
for s=1:size(Fs,3);
  F=Fs(:,:,s);
  plot(diag(F),'bo')
  plot(diag(F,1),'r+')
  plot(diag(F,2),'gx')
end
%axis([-1 1 -1 1])
hold off;

% do some beamforming
signalA2B=zeros(N,4);
signalB2A=zeros(N,4);
signalB2A_prec=zeros(N,4);

%%----------Node A to B transmission---------%%
for i=1:4
    if(indA(ia)==i)
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
    Da2b_R=vec(fblock);
end
chanestsA2B=reshape(diag(repmat(Da2b_T,Nantb)'*Da2b_R)/size(Da2b_T,1),301,Nantb);
%fchanestsA2B(:,:,meas)=[zeros(1,Nantb); chanestsA2B([1:150],:,meas); zeros(210,Nantb); chanestsA2B(151:301,:,meas)];
%tchanestsA2B(:,:,meas)=ifft(fchanestsA2B(:,:,meas));

%% calculate beamformer based on chanestA2B 
for i=1:301
    YA=squeeze(chanestA2B(i,:,:));
    F=Fs(:,:,i);
    Bd(:,i) = conj(F*YA);
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
    tmps_prec(:,[1:640]+i*640)=floor(amp*symbol_prec_cp);
end

for i=1:4
    if(indB(ib)==i)
        signalB2A(:,i)=tmps*2; %make sure LSB is 0 (switch=tx)
        signalB2A_prec(:,i)=tmps_prec(i,:)*2; %make sure LSB is 0 (switch=tx)
    else
        signalB2A(:,i)=repmat(1+1j,76800,1); %make sure LSB is 1 (switch=rx)
        signalB2A_prec(:,i)=repmat(1+1j,76800,1); %make sure LSB is 1 (switch=rx)
    end
end

%% send normal signal
oarf_send_frame(card,signalB2A,n_bit);
%keyboard
sleep(0.01);
receivedB2A=oarf_get_frame(card);
sleep(0.01);
% measure SNR

%% send beamformed DL signal
oarf_send_frame(card,signalB2A_prec,n_bit);
%keyboard
sleep(0.01);
receivedB2A=oarf_get_frame(card);
sleep(0.01);
% measure SNR
