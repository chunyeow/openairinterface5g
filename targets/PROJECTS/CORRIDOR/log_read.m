top_dir = 'E:\EMOS\corridor\trials2 train';
d1 = dir(fullfile(top_dir,'UHF','*.log'));
d2 = dir(fullfile(top_dir,'2.6GHz','*.log'));

start_time = [1.400489088000000e+09 1.400493112000000e+09 1.400499696000000e+09 1.400506864000000e+09];

for idx=1:length(d1)
    data1{idx}=csvread(fullfile(top_dir,'UHF',d1(idx).name),1,0);
    data2{idx}=csvread(fullfile(top_dir,'2.6GHz',d2(idx).name),1,0);
    
    frame_start1(idx) = ceil(data1{idx}(find(data1{idx}(:,1)>start_time(idx),1,'first'),3)/92160000)*100;
    frame_start2(idx) = ceil(data2{idx}(find(data2{idx}(:,1)>start_time(idx),1,'first'),3)/368640000)*100;
    
    %% find the first dataset with valid GPS signal and throw away everything before
    idx1_start = find(data1{idx}(:,6)>0,1,'first');
    data1{idx}(1:idx1_start-1,:)=[];
    idx2_start = find(data2{idx}(:,6)>0,1,'first');
    data2{idx}(1:idx2_start-1,:)=[];
    
    rtime1 = data1{idx}(:,1) - data1{idx}(1,1);
    rtime2 = data2{idx}(:,1) - data2{idx}(1,1);
        
    %% plot gps coordinates
    figure(idx*10+1);
    hold off
    plot(data1{idx}(1:100:end,7),data1{idx}(1:100:end,8),'rx')
    hold on
    plot(data2{idx}(1:100:end,7),data2{idx}(1:100:end,8),'bx')
    xlabel('lat [deg]');
    ylabel('lon [deg]')
    legend('UHF','2.6GHz')
    
    %% compute distance
    distances1=zeros(1,size(data1{idx},1));
    distances2=zeros(1,size(data2{idx},1));
    for i=1:size(data1{idx},1)
        distances1(i)=Dist_Calc_from_GPS(data1{idx}(i,7),data1{idx}(i,8),48.25073056,1.55481944);
    end

    for i=1:size(data2{idx},1)
        distances2(i)=Dist_Calc_from_GPS(data2{idx}(i,7),data2{idx}(i,8),48.25073056,1.55481944);
    end

   
    %% plot RSSI
    % TODO: convert time (in unix epoch) into something more meaninful
    figure(idx*10+2);
    hold off
    plot(rtime1,smooth(data1{idx}(:,13),100),'r')
    hold on
    plot(rtime2,smooth(data2{idx}(:,13),100),'b')
    plot(rtime2,smooth(data2{idx}(:,19),100),'c')
    plot(rtime2,smooth(data2{idx}(:,25),100),'m')
    legend('UHF','2.6GHz card 1','2.6GHz card 2','2.6GHz card 3');
    xlabel('time [seconds]')
    ylabel('RSSI [dBm]')
    title(sprintf('Run %d',idx));
    
    %%
    figure(idx*10+3)
    hold off
    plot(rtime1,distances1,'r')
    hold on
    plot(rtime2,distances2,'b')
    xlabel('time [seconds]')
    ylabel('dist [km]')
    legend('UHF','2.6GHz')
    
    %%
    figure(idx*10+4)
    hold off
    plot(distances1,data1{idx}(:,13),'rx')
    hold on
    plot(distances2,data2{idx}(:,13),'bx')
    plot(distances2,data2{idx}(:,19),'cx')
    plot(distances2,data2{idx}(:,25),'mx')
    xlabel('dist [km]')
    ylabel('RSSI [dBm]')
    legend('UHF','2.6GHz card 1','2.6GHz card 2','2.6GHz card 3');
   
end