load metadata_MU

%%
framestamp_max_all = max(max(framestamp_max));
framestamp_min_all = min(min(framestamp_min(framestamp_min>0)));

align_matrix = repmat(-1,4,framestamp_max_all-framestamp_min_all);
for idx = 1:size(est,2)
    for u = 1:size(est,1)
        align_matrix(u,est(u,idx).framestamp_tx - framestamp_min_all + 1) = est(u,idx).err_ind;
    end
end

map = [1 1 1; ...
       0 1 0; ...
       1 0 0];

h_fig = figure;
image(framestamp_min_all:framestamp_max_all,1:4,align_matrix+2);
colormap(map);

xtick = floor(framestamp_min_all/22500)*22500:22500:floor(framestamp_max_all/22500)*22500;
set(gca,'xtick',xtick)
set(gca,'xticklabel',mod(floor(xtick/22500),100))
xlabel('file index');
set(gca,'YTick',[0.5 1 1.5 2 2.5 3 3.5 4]);
set(gca,'YTickLabel',{'','User 1','','User 2','','User 3','','User 4'});

saveas(h_fig, 'rx_crc_time.eps', 'psc2');
