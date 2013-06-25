function mode1 = get_chunks(mode1)

%% identify continious sets in modex_ind
tmp = diff([0 double(mode1.ind_cnd) 0]);
mode1.start_points = find(tmp==1);
mode1.end_points = find(tmp==-1)-1;

%% identify last and first points of files
file_end_points = cumsum([0 mode1.NFrames/100]);
for i=1:length(file_end_points)
    % if file_end_points is contained in one chunk split that chunk
    split = find(file_end_points(i)<mode1.end_points & file_end_points(i)>mode1.start_points);
    if ~isempty(split)
        mode1.end_points = [mode1.end_points(1:(split-1)) file_end_points(i) mode1.end_points(split:end)];
        mode1.start_points = [mode1.start_points(1:split) file_end_points(i)+1 mode1.start_points((split+1):end)];
    end
end

% %% make a list of structure with fields
% gps_data = struct('lon',0,'lat',0,'time',0);
% chunk_list(length(start_points)) = struct('start_gps', gps_data, ...
%                     'end_gps', gps_data, ...
%                     'indices', []); 
%                 
% %% find the corresponding gps points
% for i=1:length(start_points)
%     chunk_list(i).indices = start_points(i):end_points(i);
%     chunk_list(i).start_gps.lon = mode1.gps_lon_cat(start_points(i));
%     chunk_list(i).start_gps.lat = mode1.gps_lat_cat(start_points(i));
%     chunk_list(i).start_gps.time = mode1.gps_time_cat(start_points(i));
%     chunk_list(i).end_gps.lon = mode1.gps_lon_cat(end_points(i));
%     chunk_list(i).end_gps.lat = mode1.gps_lat_cat(end_points(i));
%     chunk_list(i).end_gps.time = mode1.gps_time_cat(end_points(i));
% end