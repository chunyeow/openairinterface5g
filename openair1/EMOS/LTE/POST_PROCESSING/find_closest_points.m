function checkpoint2 = find_closest_points(mode2,checkpoint1)
% adds all gps point in checkpoint1 to the chunks in mode2

testpoints2 = [mode2.gps_lat_cat(mode2.ind_cnd).' mode2.gps_lon_cat(mode2.ind_cnd).'];
testpoints2_idx = find(mode2.ind_cnd);
checkpoint2 = zeros(1,length(checkpoint1));

for i=1:length(checkpoint1)
    %    find point in route y that is closest to checkpoint
    out = ipdm(checkpoint1(i,:),testpoints2,'Subset', 'NearestNeighbor','Result','Structure');
    checkpoint2(i) = testpoints2_idx(out.columnindex);
    %checkpoint2(i,:) = [testpoints2(out.columnindex,1) testpoints2(out.columnindex,2)];
end
