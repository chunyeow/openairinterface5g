
closeplot
figure(1)


gain2391=70; gain9862=0;
for rxrfmode=0:2
  i=find( (m(:,2)==gain2391) & (m(:,3)==gain9862) & (m(:,4)==rxrfmode));
  plot(m(i,1),m(i,5:8))
  pause
end


gain2391=70;  rxrfmode=1; 
for gain9862=0:5:15
  i=find((m(:,2)==gain2391) & (m(:,3)==gain9862) & (m(:,4)==rxrfmode));
  plot(m(i,1),m(i,5:8))
  pause
end




rxrfmode=1; power_dBm=-70;
i=find((m(:,1)==power_dBm) & (m(:,4)==rxrfmode));
  plot(m(i,1),m(i,5:8))
  pause
end



power_dBm=-90;

subplot(1,3,1)
rxrfmode=0;
i=find((m(:,1)==power_dBm) & (m(:,4)==rxrfmode));
mesh(0:10:150,[0 5 10 15],reshape(m(i,5)-m(i,6),4,16))

subplot(1,3,2)
rxrfmode=1;
i=find((m(:,1)==power_dBm) & (m(:,4)==rxrfmode));
mesh(0:10:150,[0 5 10 15],reshape(m(i,5)-m(i,6),4,16))

subplot(1,3,3)
rxrfmode=2;
i=find((m(:,1)==power_dBm) & (m(:,4)==rxrfmode));
mesh(0:10:150,[0 5 10 15],reshape(m(i,5)-m(i,6),4,16))

