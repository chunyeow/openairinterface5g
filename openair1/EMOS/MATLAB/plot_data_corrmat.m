filename_mat = sprintf('results_v2_idx_%d.mat',idx);

load(filename_mat,'R');

% R is the full correlation matrix
% R(k+n*NUser+m*NUser*NTx,k'+n'*NUser+m'*NUser*NTx)
% where k .. User, n .. tx antenna, m .. Rx antenna

h_fig = figure(1);
set(h_fig,'Position',[360 504 560 104])
set(h_fig,'PaperPosition',[0.25 2.5 8 1.5])
cmap = colormap('gray');
cmap = cmap(end:-1:1,:);

%% calculate the Rx correlation matrix between users taking the first Rx antenna
n = 0:3;
m = 0;

for k1=0:3
    for k2=0:3
        tmp = R(k1+n*NUser+m*NUser*NTx+1,k2+n*NUser+m*NUser*NTx+1);
        RRx(k1+1,k2+1) = mean(tmp(:));
    end
end

subplot(1,3,1)
imagesc(abs(RRx))
set(gca,'xtick',1:4)
set(gca,'ytick',1:4)
colormap(cmap)
colorbar
xlabel('User')
ylabel('User')

%% calculate the Tx correlation matrix taking the first Rx antenna
k = 0:3;
m = 0;

for n1=0:3
    for n2=0:3
        tmp = R(k+n1*NUser+m*NUser*NTx+1,k+n2*NUser+m*NUser*NTx+1);
        RTx(n1+1,n2+1) = mean(tmp(:));
    end
end

subplot(1,3,2)
imagesc(abs(RTx))
set(gca,'xtick',1:4)
set(gca,'ytick',1:4)
colormap(cmap)
colorbar
xlabel('Tx antenna')
ylabel('Tx antenna')

%% calculate the Rx correlation matrix of the first user 
n = 0:3;
k = 1;

for m1=0:1
    for m2=0:1
        tmp = R(k+n*NUser+m1*NUser*NTx+1,k+n*NUser+m2*NUser*NTx+1);
        RRxUser(m1+1,m2+1) = mean(tmp(:));
    end
end

subplot(1,3,3)
imagesc(abs(RRxUser))
set(gca,'xtick',1:2)
set(gca,'ytick',1:2)
colormap(cmap)
colorbar
xlabel('Rx antenna')
ylabel('Rx antenna')

filename_corr = fullfile(filepath_figs,sprintf('idx_%d_corr.eps',idx));
saveas(h_fig, filename_corr, 'epsc2');
