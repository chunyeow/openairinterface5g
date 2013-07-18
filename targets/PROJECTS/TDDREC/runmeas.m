if(paramsinitialized)
	N=76800;
	M=4;
	signalB2A=zeros(N,1);	
	signalA2B=zeros(N,4);	
	indA=find(active_rfA==1);
	indB=find(active_rfB==1);	
	Nanta=length(indA);
	Nantb=length(indB);
	Niter=1;
	# %% ------- Node A to B channel measurements ------- %%	
	# rf_mode_current = rf_mode + (DMAMODE_TX+TXEN)*active_rfA +(DMAMODE_RX+RXEN)*active_rfB;
	# oarf_config_exmimo(card, freq_rx,freq_tx,tdd_config,syncmode,rx_gain,tx_gain,eNB_flag,rf_mode_current,rf_rxdc,rf_local,rf_vcocal,rffe_rxg_low,rffe_rxg_final,rffe_band,autocal_mode);
	# [Da2b_T, signalA2B]=genrandpskseq(N,M,amp);
	# signalA2B=repmat(signalA2B,1,4);
	# oarf_send_frame(card,signalA2B,n_bit);
	# fprintf('A to B: Frame sent to Tx buffer.\n')	
	# fprintf('A to B: Acquiring chan. measurements....\n')
	# loop=true;
	# chanests=[];
	# Da2b_R=zeros(Niter*120,Nantb*301);
	# Da2b_T=repmat(Da2b_T,Niter,Nantb);
	# for iter=0:(Niter-1) % Do the chan. est until SNR is good enough. That is, do the "oarf_get_fram(card)
	#   receivedA2B=oarf_get_frame(card);
	#   fprintf('A to B: Frame grabbed from Rx buffer.\n')	  
	#   for i=0:119;
	#     ifblock=receivedA2B(i*640+[1:640],indB);
	#     ifblock(1:128,:)=[];
	#     fblock=fft(ifblock);
	#     fblock(1,:)=[];
	#     fblock(151:360,:)=[];
	#     Da2b_R(iter*120+i+1,:)=vec(fblock);	      
	#   endfor
	# endfor
	# H=conj(Da2b_T).*Da2b_R;
	# phases=unwrap(angle(H));
	# chanests=diag(Da2b_T'*Da2b_R)/size(Da2b_T,1);
	# for i=0:(Nantb-1)
	#   fchanests(:,i+1)=[0; chanests(301*i+[1:150]); zeros(210,1); chanests(301*i+[151:301])];
	# endfor
	# tchanests=ifft(fchanests);
	
	# %% -- Do some plotting
	# clf
	# figure(1)
	# for i=1:4 
	#   subplot(220+i);plot(20*log10(abs(fftshift(fft(receivedA2B(:,i)))))); 
	# endfor
	
	# figure(2)
	# t=[0:512-1]/512*1e-2;
	# plot(t,abs(tchanests))
	# xlabel('time')
	# ylabel('|h|')
	
	# figure(3)
	# % wndw = 50;
	# % for i=1:5:Nantb*301             %# sliding window size
	# %   phamean = filter(ones(wndw,1)/wndw, 1, phases(:,i)); %# moving average
	# %   plot(phamean(wndw:end),'LineWidth',2);
	# %   title(['subcarrier ' num2str(i)]);	  
	# %   xlabel('time')
	# %   ylabel('phase')
	# %   ylim([-pi pi])
	# %   drawnow;
	# %   pause(0.1)
	# % endfor
	# phavar=var(phases);
	# plotphavar=[];
	# for i=0:Nantb-1
	#   plotphavar=[plotphavar; phavar([1:301]+i*301)];
	# endfor
	# plot([1:150 362:512],plotphavar,'o');
	# %ylim([0 pi])
	# xlabel('subcarrier')
	# ylabel('phase variance')
	

	# figure(4)
	# plot(20*log10(abs(fchanests))), ylim([40 100])

	# %end
	# oarf_stop(card);
	# fprintf(' done\n')	

	%% ------- Node B to A channel measurements ------- %%
	rf_mode_current = rf_mode + (DMAMODE_TX+TXEN)*active_rfB +(DMAMODE_RX+RXEN)*active_rfA;
	oarf_config_exmimo(card, freq_rx,freq_tx,tdd_config,syncmode,rx_gain,tx_gain,eNB_flag,rf_mode_current,rf_rxdc,rf_local,rf_vcocal,rffe_rxg_low,rffe_rxg_final,rffe_band,autocal_mode);
	signalB2A=zeros(N,4);
	Db2a_R=zeros(Niter*120,Nanta*301);
	[Db2a_T,tmps]=genorthqpskseq(Nantb,N,amp);
	ind=1;
	for i=1:4
	  if(indB(ind)==i)	    
	    signalB2A(:,i)=tmps(:,ind);
	    if(length(indB)> ind) ind=ind+1; endif
	  endif	  	  
	endfor
	%receivedB2A=sum(signalB2A*diag([1 2 3 4]),2)+100*randn(N,1);
	
	oarf_send_frame(card,signalB2A,n_bit);
	if(length(indA)!=1) error("Node A can only have one antenna active\n"); endif
	for iter=0:(Niter-1) % Do the chan. est until SNR is good enough. That is, do the "oarf_get_fram(card)	   
	   fprintf('B to A: Frame grabbed from Rx buffer.\n')	  
	   receivedB2A=oarf_get_frame(card);
	   
	   for i=0:119;
	     ifblock=receivedB2A(i*640+[1:640],indA);
	     ifblock(1:128,:)=[];
	     fblock=fft(ifblock);
	     fblock(1,:)=[];
	     fblock(151:360,:)=[];
	     Db2a_R(iter*120+i+1,:)=fblock.';
	   endfor
	endfor
	oarf_stop(card);
	
	# # -- Do the channel estimation
	chanests=reshape(diag(repmat(Db2a_T,Niter,1)'*repmat(Db2a_R,1,Nantb)/(Niter*120)),301,Nantb)	
	H=conj(repmat(Db2a_T,Niter,1)).*repmat(Db2a_R,1,Nantb);
	phases=unwrap(angle(H));
	# for i=0:(Nantb-1)
	#   fchanests(:,i+1)=[0; chanests(301*i+[1:150]); zeros(210,1); chanests(301*i+[151:301])];
	# endfor
	# tchanests=ifft(fchanests);
	
		
else
	error('You have to run init.params.m first!')
end
