function [dist, dist_travelled] = calc_dist(MS_Lat, MS_Long, site)
% [dist, dist_traveled] = calc_dist(MS_Lat, MS_Long)
% Calculates distance from Base Station as well as distance travelled in km.
% We take the reference of the BS in Cordes
% Source http://itouchmap.com/latlong.html

if nargin<3
    site = 'cordes';
end

switch(site)
    case 'cordes'
        BS_Lat = 44.084015;
        BS_Long = 1.965765;
    case 'penne'
        BS_Lat = 44.079722;
        BS_Long = 1.714722;
    case 'ambialet'
        BS_Lat = 43.945;
        BS_Long = 2.373889;
end


dist = zeros(1,length(MS_Lat));
for i = 1:length(MS_Lat)
    if (MS_Lat(i)~=0 && MS_Long(i)~=0)
        dist(i) = Dist_Calc_from_GPS(BS_Lat, BS_Long, MS_Lat(i), MS_Long(i));
    else
        dist(i) = nan;
    end
end

dist_travelled = zeros(1,length(MS_Lat));
for i = 2:length(MS_Lat);
    if (MS_Lat(i-1)~=0 && MS_Long(i-1)~=0 && MS_Lat(i)~=0 && MS_Long(i)~=0)
        dist_travelled(i) =  Dist_Calc_from_GPS(MS_Lat(i-1), MS_Long(i-1), MS_Lat(i), MS_Long(i));
    else
        dist_travelled(i) = nan;
    end
end
dist_travelled = cumsum(dist_travelled);
