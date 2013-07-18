function [carrierdata, s]=genorthqpskseq(Ns,N,amp)
  
if(N!=512*150)
  error('The sequence length must be 76800.');
endif

s = zeros(N,Ns);
H=1; for k=1:log2(128) H=[H H; H -H]; end; H=H(:,1:120);
i=1; while(i<size(H,1)) h=H(i,:); inds=find(h*H'!=0); H(inds,:)=[]; H=[h; H]; i=i+1; end
Hc=H+sqrt(-1)*H;
if(sum(Hc*Hc'-240*eye(8))>0) error("The code is not orhtogonal\n"); endif

carrierdata=zeros(120,Ns*301);
orthinds=1:8;
for i=1:301
  for k=1:Ns    
    randind=ceil((9-k)*rand());
    carrierdata(:,i+(k-1)*301)=Hc(orthinds(randind),:)';
    orthinds(randind)=[];
  endfor
endfor

for k=1:Ns  
  for i=0:119
    fblock=[0 carrierdata(i,1:150) zeros(1,210) carrierdata(i,151:301)];
    ifblock=ifft(fblock,512);				
    block = [ifblock(end-127:end) ifblock]; # Cycl. prefix 
    s([1:640]+i*640,k)=floor(amp*block);
  endfor
endfor

endfunction