function [out,n,n2] = plot_in_bins(x,y,edges,service)
%  [out] = plot_in_bins(data,bins)
% This function puts the data y = y(x) in bins of x. For every bin it
% calculates the average and the (95/85/50/5%) percentiles. The output is a 
% matrix with 4 columns corresponding to the computed statistics and
% length(edges) row. See help histc for treatment of edges.

clf

if (length(x) ~= length(y))
    error('x and y must be the same size!');
end
x=x(:);
y=y(:);

[n, bin] = histc(x,edges);
n1 = histc(x(y>0),edges);

out = zeros(length(edges),5);
n2 = zeros(size(n));

for k=1:length(edges)
    n2(k) = sum(bin==k  & ~isnan(y));
    out(k,1) = mean(y(bin==k & ~isnan(y)));
    % we would like the percentiles where (95/85/50/5%) of the values are
    % above. Matlab returns the percentiles of the values below. So we
    % calculate 
    out(k,2:5) = prctile(y(bin==k & ~isnan(y)),[5 15 50 95]);
end

midpoints = (edges(1:end-1) + edges(2:end))/2;
plot(midpoints,out(1:end-1,:));
xlim([edges(1),edges(end)])
yl = ylim;
for i=1:length(midpoints)
    %text(edges(i),max(out(i,:)),sprintf('%d (%d %%)',n2(i),round(n2(i)/n(i))), 'HorizontalAlignment','left','VerticalAlignment','middle','Rotation',90);
    text(midpoints(i),min(0,yl(1)),sprintf(' %d',n2(i)), 'HorizontalAlignment','left','VerticalAlignment','middle','Rotation',90);
end
legend('Mean','95% prctile above','85% prctile above','50% prctile above','5% prctile above','Location','Best');

if nargin == 4
    ax1 = gca;
    ax2 = axes('Position',get(ax1,'Position'));
    plot(midpoints,n1(1:end-1)./n2(1:end-1)*100,'Color','k','Linewidth',2,'Parent',ax2);
    set(ax2,'YAxisLocation','right');
    set(ax2,'Color','none');
    set(ax2,'XColor','k');
    set(ax2,'YColor','k');
    set(ax2,'ylim',[0, 100]);
    set(get(ax2,'ylabel'),'String','Service (%)');
    set(gcf,'CurrentAxes',ax1)
end