addpath('~/Devel/matlab/IPDM')
addpath('CapCom')
addpath('maps')

%pathname = '/media/data/AMBIALET';
%mm='ambialet';
%load ambialet.mat

% 1) Identify the common set of routes between all 3 modes
% --> saved in modex.ind_common

% 2) remove routes taken more than once
% --> saved in modex.ind_duplicate

%% combine common and not duplicate
mode1.ind_cnd = mode1.ind_common & ~mode1.ind_duplicate;
mode2.ind_cnd = mode2.ind_common & ~mode2.ind_duplicate;
mode6.ind_cnd = mode6.ind_common & ~mode6.ind_duplicate;

% %% optional: select a subset of the routes
% select1=zeros(1,length(mode1.ind_cnd));
% select2=zeros(1,length(mode2.ind_cnd));
% select6=zeros(1,length(mode6.ind_cnd));
% mode1.gps_lon_cat(~mode1.ind_cnd) = nan;
% mode1.gps_lon_cat(~mode1.ind_cnd) = nan;
% mode2.gps_lat_cat(~mode2.ind_cnd) = nan;
% mode2.gps_lon_cat(~mode2.ind_cnd) = nan;
% mode6.gps_lon_cat(~mode6.ind_cnd) = nan;
% mode6.gps_lat_cat(~mode6.ind_cnd) = nan;
% figure(2)
% hold off
% [x1,y1] = plot_gps_coordinates(mm,mode1.gps_lon_cat,mode1.gps_lat_cat,[],[],'Mode1','blue','Marker','x','Line','none');
% hold on
% [x2,y2] = plot_gps_coordinates('',mode2.gps_lon_cat,mode2.gps_lat_cat,[],[],'Mode2','blue','Marker','s','Line','none');
% %[x2_update,y2_update] = plot_gps_coordinates('',mode2_update.gps_lon_cat,mode2_update.gps_lat_cat);
% [x6,y6] = plot_gps_coordinates('',mode6.gps_lon_cat,mode6.gps_lat_cat,[],[],'Mode6','blue','Marker','o','Line','none');
% linkdata on
% set(findobj('XDataSource','x1'),'ZDataSource','select1');
% set(findobj('XDataSource','x2'),'ZDataSource','select2');
% set(findobj('XDataSource','x6'),'ZDataSource','select6');
% % you might have to set ZDataSource manually
% 
% keyboard
% 
% load ambialet_select1
% 
% mode1.ind_cnd = select1~=0;
% mode2.ind_cnd = select2~=0;
% mode6.ind_cnd = select6~=0;


% 3) Identify checkpoints (GPS coordinates) common to all routes, where any
% of the routes has a discontinuity. Checkpoints of a single route can be
% identified by using the end of file information or the data that has been
% removed in points 1 and 2. Once a checkpoint is identified, the
% corresponding checkpoint in the other routes has to be identified. We
% take the point closest to it.  At the end of this process we have split
% our data set in continuous chunks, which are common to all routes.
%
%%
mode1 = get_chunks(mode1);
mode2 = get_chunks(mode2);
mode6 = get_chunks(mode6);


%% visualize all chunks 
%if 0
h_fig = figure(1);
hold off
%colors = {'blue','green', 'red', 'cyan', 'magenta', 'yellow', 'black', 'white'};
cm = colormap(lines);
for i=1:length(mode1.start_points);
    if (i==1)
        plot_gps_coordinates(mm,mode1.gps_lon_cat(mode1.start_points(i):mode1.end_points(i)),...
                                mode1.gps_lat_cat(mode1.start_points(i):mode1.end_points(i)),...
                                [],[],sprintf('m1c%d',i),cm(mod(i-1,64)+1,:),'Marker','x','Line','none');
    else
        plot_gps_coordinates('',mode1.gps_lon_cat(mode1.start_points(i):mode1.end_points(i)),...
                                mode1.gps_lat_cat(mode1.start_points(i):mode1.end_points(i)),...
                                [],[],sprintf('m1c%d',i),cm(mod(i-1,64)+1,:),'Marker','x','Line','none');
    end        
    fprintf(1,'saved %d\n',i);
end
saveas(h_fig,fullfile(pathname,'route_align','chunks_mode1.jpg'));

h_fig = figure(2);
hold off
%colors = {'blue','green', 'red', 'cyan', 'magenta', 'yellow', 'black', 'white'};
cm = colormap(lines);
for i=1:length(mode2.start_points);
    if (i==1)
        plot_gps_coordinates(mm,mode2.gps_lon_cat(mode2.start_points(i):mode2.end_points(i)),...
                                mode2.gps_lat_cat(mode2.start_points(i):mode2.end_points(i)),...
                                [],[],sprintf('m2c%d',i),cm(mod(i-1,64)+1,:),'Marker','o','Line','none');
    else
        plot_gps_coordinates('',mode2.gps_lon_cat(mode2.start_points(i):mode2.end_points(i)),...
                                mode2.gps_lat_cat(mode2.start_points(i):mode2.end_points(i)),...
                                [],[],sprintf('m2c%d',i),cm(mod(i-1,64)+1,:),'Marker','o','Line','none');
    end        
    fprintf(1,'saved %d\n',i);
end
saveas(h_fig,fullfile(pathname,'route_align','chunks_mode2.jpg'));

h_fig = figure(3);
hold off
%colors = {'blue','green', 'red', 'cyan', 'magenta', 'yellow', 'black', 'white'};
cm = colormap(lines);
for i=1:length(mode6.start_points);
    if (i==1)
        plot_gps_coordinates(mm,mode6.gps_lon_cat(mode6.start_points(i):mode6.end_points(i)),...
                                mode6.gps_lat_cat(mode6.start_points(i):mode6.end_points(i)),...
                                [],[],sprintf('m6c%d',i),cm(mod(i-1,64)+1,:),'Marker','s','Line','none');
    else
        plot_gps_coordinates('',mode6.gps_lon_cat(mode6.start_points(i):mode6.end_points(i)),...
                                mode6.gps_lat_cat(mode6.start_points(i):mode6.end_points(i)),...
                                [],[],sprintf('m6c%d',i),cm(mod(i-1,64)+1,:),'Marker','s','Line','none');
    end
    fprintf(1,'saved %d\n',i);
end
saveas(h_fig,fullfile(pathname,'route_align','chunks_mode6.jpg'));

%keyboard
%end

% testpoints1 = [mode1.gps_lat_cat(mode1.ind_cnd).' mode2.gps_lon_cat(mode1.ind_cnd).'];
% testpoints2 = [mode2.gps_lat_cat(mode2.ind_cnd).' mode2.gps_lon_cat(mode2.ind_cnd).'];
% testpoints6 = [mode6.gps_lat_cat(mode6.ind_cnd).' mode2.gps_lon_cat(mode6.ind_cnd).'];
                
%%
startpoint1 = [mode1.gps_lat_cat(mode1.start_points).' mode1.gps_lon_cat(mode1.start_points).'];
endpoint1   = [mode1.gps_lat_cat(mode1.end_points).' mode1.gps_lon_cat(mode1.end_points).'];
startpoint2 = [mode2.gps_lat_cat(mode2.start_points).' mode2.gps_lon_cat(mode2.start_points).'];
endpoint2   = [mode2.gps_lat_cat(mode2.end_points).' mode2.gps_lon_cat(mode2.end_points).'];
startpoint6 = [mode6.gps_lat_cat(mode6.start_points).' mode6.gps_lon_cat(mode6.start_points).'];
endpoint6   = [mode6.gps_lat_cat(mode6.end_points).' mode6.gps_lon_cat(mode6.end_points).'];

% %%
% [mode2,startpoint2_1] = addpoints(mode2,startpoint1);
% [mode2,startpoint2_6] = addpoints(mode2,startpoint6);
% [mode1,startpoint1_2] = addpoints(mode1,startpoint2);
% [mode1,startpoint1_6] = addpoints(mode1,startpoint6);
% [mode6,startpoint6_1] = addpoints(mode6,startpoint1);
% [mode6,startpoint6_2] = addpoints(mode6,startpoint2);
% 
% [mode2,endpoint2_1] = addpoints(mode2,endpoint1);
% [mode2,endpoint2_6] = addpoints(mode2,endpoint6);
% [mode1,endpoint1_2] = addpoints(mode1,endpoint2);
% [mode1,endpoint1_6] = addpoints(mode1,endpoint6);
% [mode6,endpoint6_1] = addpoints(mode6,endpoint1);
% [mode6,endpoint6_2] = addpoints(mode6,endpoint2);

%% for each start and endpoint get the corresponing point in the other modes
startpoint2_1 = find_closest_points(mode2,startpoint1);
startpoint2_6 = find_closest_points(mode2,startpoint6);
startpoint1_2 = find_closest_points(mode1,startpoint2);
startpoint1_6 = find_closest_points(mode1,startpoint6);
startpoint6_1 = find_closest_points(mode6,startpoint1);
startpoint6_2 = find_closest_points(mode6,startpoint2);

endpoint2_1 = find_closest_points(mode2,endpoint1);
endpoint2_6 = find_closest_points(mode2,endpoint6);
endpoint1_2 = find_closest_points(mode1,endpoint2);
endpoint1_6 = find_closest_points(mode1,endpoint6);
endpoint6_1 = find_closest_points(mode6,endpoint1);
endpoint6_2 = find_closest_points(mode6,endpoint2);

%%
num_chunks = length(startpoint1)+length(startpoint1_2)+length(startpoint1_6);

startpoint1_all = [mode1.start_points   startpoint1_2 startpoint1_6];
startpoint2_all = [startpoint2_1 mode2.start_points   startpoint2_6];
startpoint6_all = [startpoint6_1 startpoint6_2 mode6.start_points  ];

endpoint1_all = [mode1.end_points   endpoint1_2 endpoint1_6];
endpoint2_all = [endpoint2_1 mode2.end_points   endpoint2_6];
endpoint6_all = [endpoint6_1 endpoint6_2 mode6.end_points ];

%% make a list of structure with fields
%gps_data = struct('lon',0,'lat',0,'time',0);
chunk_data = struct('start_gps', zeros(1,2), ...
                    'end_gps', zeros(1,2), ...
                    'start_idx', 0, ...
                    'end_idx', 0, ...
                    'indices', []); 
chunk_list = struct('mode1',chunk_data,'mode2',chunk_data,'mode6',chunk_data);

%% old version
for i=1:num_chunks
    chunk_list(i).mode1.start_gps = [mode1.gps_lat_cat(startpoint1_all(i)) mode1.gps_lon_cat(startpoint1_all(i))];
    chunk_list(i).mode1.end_gps   = [mode1.gps_lat_cat(endpoint1_all(i)) mode1.gps_lon_cat(endpoint1_all(i))];
    chunk_list(i).mode1.start_idx = startpoint1_all(i);
    chunk_list(i).mode1.end_idx   = endpoint1_all(i);
    if chunk_list(i).mode1.start_idx>chunk_list(i).mode1.end_idx %time reverse
        chunk_list(i).mode1.indices = chunk_list(i).mode1.start_idx:-1:chunk_list(i).mode1.end_idx;
    else
        chunk_list(i).mode1.indices = chunk_list(i).mode1.start_idx:chunk_list(i).mode1.end_idx;
    end                                
    % puncutre indices that are not in mode1.ind_cnd;
    % chunk_list(i).mode1.indices = chunk_list(i).mode1.indices(mode1.ind_cnd(chunk_list(i).mode1.indices));
    if all(mode1.ind_cnd(chunk_list(i).mode1.indices)) % it is within a chunk
        chunk_list(i).mode1.ok = 1;
    else
        chunk_list(i).mode1.ok = 0;
    end        
    
    chunk_list(i).mode2.start_gps = [mode2.gps_lat_cat(startpoint2_all(i)) mode2.gps_lon_cat(startpoint2_all(i))];
    chunk_list(i).mode2.end_gps   = [mode2.gps_lat_cat(endpoint2_all(i)) mode2.gps_lon_cat(endpoint2_all(i))];
    chunk_list(i).mode2.start_idx = startpoint2_all(i);
    chunk_list(i).mode2.end_idx   = endpoint2_all(i);
    if chunk_list(i).mode2.start_idx>chunk_list(i).mode2.end_idx %time reverse
        chunk_list(i).mode2.indices = chunk_list(i).mode2.start_idx:-1:chunk_list(i).mode2.end_idx;
    else
        chunk_list(i).mode2.indices = chunk_list(i).mode2.start_idx:chunk_list(i).mode2.end_idx;
    end                                
    % puncutre indices that are not in mode2.ind_cnd;
    % chunk_list(i).mode2.indices = chunk_list(i).mode2.indices(mode2.ind_cnd(chunk_list(i).mode2.indices));
    if all(mode2.ind_cnd(chunk_list(i).mode2.indices)) % it is within a chunk
        chunk_list(i).mode2.ok = 1;
    else
        chunk_list(i).mode2.ok = 0;
    end        
    
    chunk_list(i).mode6.start_gps = [mode6.gps_lat_cat(startpoint6_all(i)) mode6.gps_lon_cat(startpoint6_all(i))];
    chunk_list(i).mode6.end_gps   = [mode6.gps_lat_cat(endpoint6_all(i)) mode6.gps_lon_cat(endpoint6_all(i))];
    chunk_list(i).mode6.start_idx = startpoint6_all(i);
    chunk_list(i).mode6.end_idx   = endpoint6_all(i);
    if chunk_list(i).mode6.start_idx>chunk_list(i).mode6.end_idx %time reverse
        chunk_list(i).mode6.indices = chunk_list(i).mode6.start_idx:-1:chunk_list(i).mode6.end_idx;
    else
        chunk_list(i).mode6.indices = chunk_list(i).mode6.start_idx:chunk_list(i).mode6.end_idx;
    end                                
    % puncutre indices that are not in mode6.ind_cnd;
    %chunk_list(i).mode6.indices = chunk_list(i).mode6.indices(mode6.ind_cnd(chunk_list(i).mode6.indices));
    if all(mode6.ind_cnd(chunk_list(i).mode6.indices)) % it is within a chunk
        chunk_list(i).mode6.ok = 1;
    else
        chunk_list(i).mode6.ok = 0;
    end        
    

end    

%
% 4) For each chunk identify if it is time reversed by comparing it to the
% time indices of the starting and the end point
%
% 5) reorder the routes to give one continuous route. This has to be done
% manually for one route. The others can be automatically aligned to the
% reference route.

%% compute some statistics of the chunks
% historgram of length
chunk_size1 = zeros(1,num_chunks);
chunk_size2 = zeros(1,num_chunks);
chunk_size6 = zeros(1,num_chunks);
chunk_idx = false(1,num_chunks);
for i=1:num_chunks
    chunk_size1(i) = length(chunk_list(i).mode1.indices);
    chunk_size2(i) = length(chunk_list(i).mode2.indices);
    chunk_size6(i) = length(chunk_list(i).mode6.indices);
%     if (max(chunk_size1(i),chunk_size2(i))/min(chunk_size1(i),chunk_size2(i))<10 && ...
%         max(chunk_size1(i),chunk_size6(i))/min(chunk_size1(i),chunk_size6(i))<10 && ...
%         max(chunk_size2(i),chunk_size6(i))/min(chunk_size2(i),chunk_size6(i))<10)
%         chunk_idx(i) = true;
%     end
    if (chunk_list(i).mode1.ok && chunk_list(i).mode2.ok && chunk_list(i).mode6.ok && ...
        length(chunk_list(i).mode1.indices)>=10 && length(chunk_list(i).mode2.indices)>=10 && length(chunk_list(i).mode6.indices)>=10)
        chunk_idx(i)=true;
    end
end

h_fig = figure(4);
hold off
bar([chunk_size1(chunk_idx);chunk_size2(chunk_idx);chunk_size6(chunk_idx)].','stacked')
%set(gca,'yscale','log');
legend('mode1','mode2','mode6')
xlabel('chunk')
ylabel('# samples')
saveas(h_fig,fullfile(pathname,'route_align','chunk_stats.eps'), 'epsc2');

%% visualize chunks ver 1
h_fig=figure(10);
hold off
first_plot=1;
cm = colormap(lines);
for i=1:num_chunks
    if chunk_idx(i)
        h_fig=figure(10);
        if (first_plot==1)
            plot_gps_coordinates(mm,mode1.gps_lon_cat(chunk_list(i).mode1.indices),...
                                            mode1.gps_lat_cat(chunk_list(i).mode1.indices),...
                                            [],[],sprintf('chunk %d',i),cm(mod(i-1,64)+1,:),...
                                            'Marker','x','Line','none');
            first_plot = 0;
        else
            plot_gps_coordinates('',mode1.gps_lon_cat(chunk_list(i).mode1.indices),...
                                    mode1.gps_lat_cat(chunk_list(i).mode1.indices),...
                                    [],[],sprintf('chunk %d',i),cm(mod(i-1,64)+1,:),...
                                    'Marker','x','Line','none');
        end            
        plot_gps_coordinates('',mode2.gps_lon_cat(chunk_list(i).mode2.indices),...
                                mode2.gps_lat_cat(chunk_list(i).mode2.indices),...
                                    [],[],'',cm(mod(i-1,64)+1,:),...
                                    'Marker','o','Line','none');
        plot_gps_coordinates('',mode6.gps_lon_cat(chunk_list(i).mode6.indices),...
                                mode6.gps_lat_cat(chunk_list(i).mode6.indices),...
                                [],[],'',cm(mod(i-1,64)+1,:),...
                                    'Marker','s','Line','none');
        %fprintf(1,'saved %d\n',i);
        %drawnow;
        plot_distance_travelled(mode1,chunk_list(i).mode1.indices,...
                                mode2,mode2_ideal,chunk_list(i).mode2.indices,...
                                mode6,chunk_list(i).mode6.indices,...
                                fullfile(pathname,'route_align'), i, mm);
    end
end
h_fig=figure(10);
saveas(h_fig,fullfile(pathname,'route_align','chunks.jpg'));


% %%
% h_fig = figure(6);
% align_matrix = ones(length(mode1.start_points),length(mode1.ind_cnd));
% for i=1:length(mode1.start_points)
%     align_matrix(i,mode1.start_points(i):mode1.end_points(i)) = 0;
% end
% imagesc(align_matrix)
% xlabel('time [sec]')
% ylabel('chunk')