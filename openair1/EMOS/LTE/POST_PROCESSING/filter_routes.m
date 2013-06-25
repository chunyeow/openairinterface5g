function ind = filter_routes(lat,lon,time)
% ind = filter_routes(lat,lon,time)
%  This returns true for every point that has been take more than once 
%  all vecotrs must be column vectors

len = length(lat);
ind = false(size(lat));
tol_gps = 0.0001;
tol_time = 15*60; % 15 min in sec

for i=2:len
    % if we find points in geographical vicinity of lat(i) lon(i) that are
    % further than 15min away, we flag the point
    t_gps = ipdm([lat(i),lon(i)],[lat(1:i-1).',lon(1:i-1).']);
    t_time = ipdm(time(i),time(1:i-1).');
    if any(t_gps<tol_gps & t_time>tol_time)
        ind(i) = true;
    end
end
    