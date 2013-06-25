
closeplot
figure(1);


%for power_dBm=ALL_power_dBm
for power_dBm=-65
   
   disp(['input: ' num2str(power_dBm) ' dBm']);

    xlabel('Gain 2391');
    ylabel('Gain 9862');
    zlabel('SNR');

    rxrfmode=0;
    idx=find((m(:,1)==power_dBm) & (m(:,4)==rxrfmode));
    M0=zeros(length(ALL_gain2391),length(ALL_gain9862));
    M1=zeros(length(ALL_gain2391),length(ALL_gain9862));
    for i=idx.'
      i1=find(ALL_gain2391==m(i,2));
      i2=find(ALL_gain9862==m(i,3));
      M0(i1,i2)=m(i,5)-m(i,6);
      M1(i1,i2)=m(i,7)-m(i,8);
    end
    subplot(2,3,1); clg;
    title('Channel 0, rxrfmode 0');
    mesh(ALL_gain2391,ALL_gain9862,M0.')
    subplot(2,3,4); clg;
    title('Channel 1, rxrfmode 0');
    mesh(ALL_gain2391,ALL_gain9862,M1.')

    rxrfmode=1;
    idx=find((m(:,1)==power_dBm) & (m(:,4)==rxrfmode));
    M0=zeros(length(ALL_gain2391),length(ALL_gain9862));
    M1=zeros(length(ALL_gain2391),length(ALL_gain9862));
    for i=idx.'
      i1=find(ALL_gain2391==m(i,2));
      i2=find(ALL_gain9862==m(i,3));
      M0(i1,i2)=m(i,5)-m(i,6);
      M1(i1,i2)=m(i,7)-m(i,8);
    end
    subplot(2,3,2); clg;
    title('Channel 0, rxrfmode 1');
    mesh(ALL_gain2391,ALL_gain9862,M0.')
    subplot(2,3,5); clg;
    title('Channel 1, rxrfmode 1');
    mesh(ALL_gain2391,ALL_gain9862,M1.')

    rxrfmode=2;
    idx=find((m(:,1)==power_dBm) & (m(:,4)==rxrfmode));
    M0=zeros(length(ALL_gain2391),length(ALL_gain9862));
    M1=zeros(length(ALL_gain2391),length(ALL_gain9862));
    for i=idx.'
      i1=find(ALL_gain2391==m(i,2));
      i2=find(ALL_gain9862==m(i,3));
      M0(i1,i2)=m(i,5)-m(i,6);
      M1(i1,i2)=m(i,7)-m(i,8);
    end
    subplot(2,3,3); clg;
    title('Channel 0, rxrfmode 2');
    mesh(ALL_gain2391,ALL_gain9862,M0.')
    subplot(2,3,6); clg;
    title('Channel 1, rxrfmode 2');
    mesh(ALL_gain2391,ALL_gain9862,M1.')

    pause

end
