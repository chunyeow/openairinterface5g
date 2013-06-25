%%
NFrames_cum = cumsum(mode2.NFrames);
parcours1 = false(1,length(mode2.timestamp_cat));

switch mm
    case 'cordes'
        idx(1) = 1; %start
        idx(2) = strmatch('data_term3_idx30_20100511T164443.EMOS',char(mode2.filenames)); %end
        idx(3) = strmatch('data_term3_idx22_20100518T145355.EMOS',char(mode2.filenames)); %start
        idx(4) = strmatch('data_term3_idx11_20100520T153238.EMOS',char(mode2.filenames)); %end

        frame_idx = NFrames_cum(idx)/100;
        parcours1(frame_idx(1):frame_idx(2)) = true;
        parcours1(frame_idx(3):frame_idx(4)) = true;
    case 'penne'
        % double check this - do we neglect the coverage roads here?
        idx = strmatch('data_term3_idx72_20100701T151158.EMOS',char(mode2.filenames));
        frame_idx = NFrames_cum(idx)/100;
        parcours1(1:frame_idx) = true;
    case 'ambialet'
        idx(1) = strmatch('data_term3_idx34_20100719T191120.EMOS',char(mode2.filenames)); %last file before parcours 1
        idx(2) = strmatch('data_term3_idx07_20100721T180138.EMOS',char(mode2.filenames)); %last file of parcours 1

        frame_idx = NFrames_cum(idx)/100;
        parcours1(frame_idx(1)+1:frame_idx(2)) = true;
end

%%
% mode1_ind = mode1.ind_common & ~mode1.ind_duplicate;
% mode2_ind = mode2.ind_common & ~mode2.ind_duplicate;
% mode6_ind = mode6.ind_common & ~mode6.ind_duplicate;
mode1_ind = true(1,length(mode1.timestamp_cat));
mode2_ind = true(1,length(mode2.timestamp_cat));
mode6_ind = true(1,length(mode6.timestamp_cat));


%%
figure(1)
hold off
time1 = mode1.gps_time_cat(mode1_ind);
time2 = mode2.gps_time_cat(mode2_ind);
%time2_update = mode2_update.gps_time_cat;
time6 = mode6.gps_time_cat(mode6_ind);
select1 = zeros(size(time1));
select2 = zeros(size(time2));
%select2_update = zeros(size(time2_update));
select6 = zeros(size(time6));
plot(time1,select1,'x')
hold on
plot(time2,select2,'s')
%plot(time2_update,select2_update,'d')
plot(time6,select6,'o')
%legend('mode1','mode2','mode2_update','mode6')
legend('mode1','mode2','mode6')
linkdata on

%%
figure(2)
hold off
[x1,y1] = plot_gps_coordinates(mm,mode1.gps_lon_cat(mode1_ind),mode1.gps_lat_cat(mode1_ind),[],[],'Mode1','blue','Marker','x','Line','none');
hold on
[x2,y2] = plot_gps_coordinates('',mode2.gps_lon_cat(mode2_ind),mode2.gps_lat_cat(mode2_ind),[],[],'Mode2','blue','Marker','s','Line','none');
%[x2_update,y2_update] = plot_gps_coordinates('',mode2_update.gps_lon_cat,mode2_update.gps_lat_cat);
[x6,y6] = plot_gps_coordinates('',mode6.gps_lon_cat(mode6_ind),mode6.gps_lat_cat(mode6_ind),[],[],'Mode6','blue','Marker','o','Line','none');
linkdata on
set(findobj('XDataSource','x1'),'ZDataSource','select1');
set(findobj('XDataSource','x2'),'ZDataSource','select2');
%set(findobj('XDataSource','x2_update'),'ZDataSource','select2_update');
set(findobj('XDataSource','x6'),'ZDataSource','select6');
% you might have to set ZDataSource manually


% %%
% file_idx = zeros(size(select));
% NFrames_idx = [1 cumsum(NFrames/100)];
% for i=1:length(filenames)
%     file_idx(NFrames_idx(i):NFrames_idx(i+1)) = i;
% end
% 
% file_idxs = unique(file_idx(select==1));
% {filenames{file_idxs}}.'

