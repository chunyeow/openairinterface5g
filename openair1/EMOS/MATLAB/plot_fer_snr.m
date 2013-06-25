load metadata_MU

%%
figure(1)
hold off
plot_style = {'r-','g-','b-','k-'};
for user = 1:4
    err_ind = [est(user,:).err_ind];
    snr = [est(user,:).rx_power] - [est(user,:).n0_power];
    snr = mean(snr,1);
    bins = floor(min(snr)):3:ceil(max(snr));
    fer = zeros(1,length(bins)-1);

    for b=1:length(bins)-1
        idx = find(snr>=bins(b) & snr<bins(b+1));
        fer(b) = sum(err_ind(idx))/length(idx);
    end

    plot(bins(1:end-1)+1.5,fer,plot_style{user});
    hold on
end
legend('user 1','user 2','user 3','user 4');