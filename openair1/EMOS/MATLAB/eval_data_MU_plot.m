filename_mat = sprintf('results_idx_%d.mat',idx);

filepath = 'figs_spawc';
logscale = 1;

if 0
    %%
    h_fig = figure(1);
    plot_style = {'r.','m.','b.','k.'};
    for k = 1:NUser
        plot(est(k,idx).framestamp_tx(~est(k,idx).err_ind),mean(est(k,idx).rx_rssi_dBm(~est(k,idx).err_ind),1),plot_style{k});
        %plot(round(est(k,idx).timestamp*3000/8)+offset_frames(k),mean(est(k,idx).rx_rssi_dBm,1),plot_style{k});
        hold on
        %plot(est(k,idx).framestamp_tx(est(k,idx).err_ind),mean(est(k,idx).rx_rssi_dBm(est(k,idx).err_ind),1),'rx');
        %plot(round(est(k,idx).timestamp(logical(est(k,idx).err_ind))*3000/8)+offset_frames(k),mean(est(k,idx).rx_rssi_dBm(logical(est(k,idx).err_ind)),1),'rx');
    end
    hold off
    xlabel('Frame')
    ylabel('RSSI [dBm]')
    legend('User 1','User 2','User 3','User 4')
    filename_rssi = fullfile(filepath,sprintf('idx_%d_rx_rssi.eps',idx));
    saveas(h_fig, filename_rssi, 'epsc2');

    %%    
    h_fig = figure(2);
    colors = {'red','magenta','blue','white'};
    if exist('mm','var')
        image(mm);
        hold on;
    end
    for k = 1:NUser
        plot_gps_coordinates_sophia([], gps_data(k,idx).longitude, gps_data(k,idx).latitude, est(k,idx).rx_rssi_dBm(1:100:end), sprintf('User %d',k),colors{k});
        hold on;
    end
    hold off
    axis([3264    5819    2610    4556])
    filename_gps = fullfile(filepath,sprintf('idx_%d_map_rx_rssi.eps',idx));
    saveas(h_fig, filename_gps, 'epsc2');
    filename_gps_pdf = fullfile(filepath,sprintf('idx_%d_map_rx_rssi.pdf',idx));
    system(sprintf('epstopdf --outfile=%s %s',filename_gps_pdf,filename_gps));

    %%
    h_fig = figure(3);
    align_matrix = repmat(-1,4,max(framestamp_max(:,idx)) - min(framestamp_min(:,idx)) + 1);
    for u = 1:4
        align_matrix(u,est(u,idx).framestamp_tx - min(framestamp_min(:,idx)) + 1) = est(u,idx).err_ind;
    end

    map = [1 1 1; ...
           0 1 0; ...
           1 0 0];

    image(align_matrix+2); % image values have to be in range 1:m
    colormap(map);
    set(gca,'YTickLabel',{'','User 1','','User 2','','User 3','','User 4'},...
        'YTick',[0.5 1 1.5 2 2.5 3 3.5 4],...
        'XTick',zeros(1,0));
    %pos = get(h_fig,'Position');
    set(h_fig,'Position',[360 504 560 104])
    %pos = get(h_fig,'PaperPosition');
    set(h_fig,'PaperPosition',[0.25 2.5 8 1.5])
    filename_align = fullfile(filepath,sprintf('idx_%d_align.eps',idx));
    saveas(h_fig, filename_align, 'epsc2');
end

if exist(filename_mat,'file')
    load(filename_mat)

%%
    % post processing
    [Cap_SU_TDMA_CL_ML_f,Cap_SU_TDMA_CL_ML_x] = ecdf(CAP_SU_TDMA_CL_ML(:));
    [Cap_SU_TDMA_ML_f,Cap_SU_TDMA_ML_x] = ecdf(CAP_SU_TDMA_ML(:));
    [Cap_SU_TDMA_MMSE_f,Cap_SU_TDMA_MMSE_x] = ecdf(CAP_SU_TDMA_MMSE(:));
    [Cap_4U_ZF_f,Cap_4U_ZF_x] = ecdf(CAP_4U_ZF(:));
    [Cap_4U_MMSE_f,Cap_4U_MMSE_x] = ecdf(CAP_4U_MMSE(:));
    [Cap_2U_ZF_f,Cap_2U_ZF_x] = ecdf(CAP_2U_ZF(:));
    [Cap_2U_MMSE_f,Cap_2U_MMSE_x] = ecdf(CAP_2U_MMSE(:));
    [Cap_4U_AS_ZF_f,Cap_4U_AS_ZF_x] = ecdf(CAP_4U_AS_ZF(:));
    [Cap_4U_AS_MMSE_f,Cap_4U_AS_MMSE_x] = ecdf(CAP_4U_AS_MMSE(:));
    [Cap_2U_AS_ZF_f,Cap_2U_AS_ZF_x] = ecdf(CAP_2U_AS_ZF(:));
    [Cap_2U_AS_MMSE_f,Cap_2U_AS_MMSE_x] = ecdf(CAP_2U_AS_MMSE(:));
    [Cap_2U_BD_WF_f,Cap_2U_BD_WF_x] = ecdf(CAP_2U_BD_WF(:));
    [Cap_2U_BD_EP_f,Cap_2U_BD_EP_x] = ecdf(CAP_2U_BD_EP(:));
    [Cap_2U2_ZF_f,Cap_2U2_ZF_x] = ecdf(CAP_2U2_ZF(:));
    [Cap_2U2_MMSE_f,Cap_2U2_MMSE_x] = ecdf(CAP_2U2_MMSE(:));

    % use the following color coding
    % ZF = blue, MMSE = red, SUTS = black, BD = cyan, MMSE AS = magenta, 
    % 4U = '-', 2U = '--'
    % measured = 2pt, iid = 1pt
    
%% SU-MIMO TDMA vs 2U MU-MIMO ZF and 2U MU-MIMO MMSE (measured only)
    h_fig = figure(4);
    %load('cap_iid.mat')
    hold off
    %plot(Cap_MU_SUTS_iid_x,Cap_MU_SUTS_iid_f,'k','Linewidth',1)
    %hold on

    %load('Cap_iid_N_Rx=1.mat')
    %plot(Cap_4U_ZF_iid_x,Cap_4U_ZF_iid_f,'b','Linewidth',1)
    %plot(Cap_4U_MMSE_iid_x,Cap_4U_MMSE_iid_f,'r','Linewidth',1)

    %plot(Cap_2U_ZF_iid_x,Cap_2U_ZF_iid_f,'b--','Linewidth',1)
    %plot(Cap_2U_MMSE_iid_x,Cap_2U_MMSE_iid_f,'r--','Linewidth',1)

    %plot(Cap_SU_TDMA_ML_x,Cap_SU_TDMA_ML_f,'k','Linewidth',2)
    plot(Cap_SU_TDMA_MMSE_x,Cap_SU_TDMA_MMSE_f,'k','Linewidth',2)
    hold on

    %plot(Cap_4U_ZF_x,Cap_4U_ZF_f,'b','Linewidth',2)
    %plot(Cap_4U_MMSE_x,Cap_4U_MMSE_f,'r','Linewidth',2)

    plot(Cap_2U_ZF_x,Cap_2U_ZF_f,'b--','Linewidth',2)
    plot(Cap_2U_MMSE_x,Cap_2U_MMSE_f,'r--','Linewidth',2)

%     legend('iid SUTS 4U','iid ZF 4U','iid MMSE 4U','iid ZF 2U','iid MMSE 2U',...
%         'measured SUTS 4U',...
%         'measured ZF 4U','measured MMSE 4U','measured ZF 2U','measured MMSE 2U',...
%         'Location','SouthEast')
    legend('SU-MIMO TDMA MMSE measured','MU-MIMO ZF 2U measured','MU-MIMO MMSE 2U measured',...
        'Location','SouthEast')
    title('SU-MIMO vs. MU-MIMO with equal multiplexing gain','Fontsize',14,'Fontweight','bold')
    xlabel('bits/sec/Hz')
    xlim([0 18])
    ylabel('CDF')
    if logscale
        set(gca,'yscale','log')
        ylim([1e-2 1])
    end
    grid on
    filename_cap = fullfile(filepath,sprintf('idx_%d_capacity_2U.eps',idx));
    saveas(h_fig, filename_cap, 'epsc2');
    filename_cap_pdf = fullfile(filepath,sprintf('idx_%d_capacity_2U.pdf',idx));
    system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));
    
%% SU-MIMO TDMA vs 4U MU-MIMO ZF and 4U MU-MIMO MMSE (measured only)
    h_fig = figure(5);
    %load('cap_iid.mat')
    hold off
    %plot(Cap_MU_SUTS_iid_x,Cap_MU_SUTS_iid_f,'k','Linewidth',1)
    %hold on

    %load('Cap_iid_N_Rx=1.mat')
    %plot(Cap_4U_ZF_iid_x,Cap_4U_ZF_iid_f,'b','Linewidth',1)
    %plot(Cap_4U_MMSE_iid_x,Cap_4U_MMSE_iid_f,'r','Linewidth',1)

    %plot(Cap_2U_ZF_iid_x,Cap_2U_ZF_iid_f,'b--','Linewidth',1)
    %plot(Cap_2U_MMSE_iid_x,Cap_2U_MMSE_iid_f,'r--','Linewidth',1)

    %plot(Cap_SU_TDMA_ML_x,Cap_SU_TDMA_ML_f,'k','Linewidth',2)
    %plot(Cap_SU_TDMA_MMSE_x,Cap_SU_TDMA_MMSE_f,'k','Linewidth',2)
    plot(Cap_SU_TDMA_CL_ML_x,Cap_SU_TDMA_CL_ML_f,'k','Linewidth',2)
    hold on

    plot(Cap_4U_ZF_x,Cap_4U_ZF_f,'b','Linewidth',2)
    plot(Cap_4U_MMSE_x,Cap_4U_MMSE_f,'r','Linewidth',2)

    %plot(Cap_2U_ZF_x,Cap_2U_ZF_f,'b--','Linewidth',2)
    %plot(Cap_2U_MMSE_x,Cap_2U_MMSE_f,'r--','Linewidth',2)

%     legend('iid SUTS 4U','iid ZF 4U','iid MMSE 4U','iid ZF 2U','iid MMSE 2U',...
%         'measured SUTS 4U',...
%         'measured ZF 4U','measured MMSE 4U','measured ZF 2U','measured MMSE 2U',...
%         'Location','SouthEast')
    legend('SU-MIMO TDMA CL measured','MU-MIMO ZF 4U measured','MU-MIMO MMSE 4U measured',...
        'Location','SouthEast')
    title('SU-MIMO vs. MU-MIMO with max. multipexing gain','Fontsize',14,'Fontweight','bold')
    xlabel('bits/sec/Hz')
    xlim([0 18])
    ylabel('CDF')
    if logscale
        set(gca,'yscale','log')
        ylim([1e-2 1])
    end
    grid on
    filename_cap = fullfile(filepath,sprintf('idx_%d_capacity_4U.eps',idx));
    saveas(h_fig, filename_cap, 'epsc2');
    filename_cap_pdf = fullfile(filepath,sprintf('idx_%d_capacity_4U.pdf',idx));
    system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));

%% SU-MIMO TDMA vs 4U MU-MIMO ZF and 4U MU-MIMO MMSE (measured+iid)
    h_fig = figure(6);
    load('cap_iid.mat')
    hold off
    plot(Cap_SU_TDMA_ML_iid_x,Cap_SU_TDMA_ML_iid_f,'k-.','Linewidth',1)
    hold on

    load('Cap_iid_N_Rx=1.mat')
    plot(Cap_4U_ZF_iid_x,Cap_4U_ZF_iid_f,'b--','Linewidth',1)
    hold on
    plot(Cap_4U_MMSE_iid_x,Cap_4U_MMSE_iid_f,'r','Linewidth',1)

    %plot(Cap_2U_ZF_iid_x,Cap_2U_ZF_iid_f,'b--','Linewidth',1)
    %plot(Cap_2U_MMSE_iid_x,Cap_2U_MMSE_iid_f,'r--','Linewidth',1)

    plot(Cap_SU_TDMA_ML_x,Cap_SU_TDMA_ML_f,'k-.','Linewidth',2)

    plot(Cap_4U_ZF_x,Cap_4U_ZF_f,'b--','Linewidth',2)
    plot(Cap_4U_MMSE_x,Cap_4U_MMSE_f,'r','Linewidth',2)

    %plot(Cap_2U_ZF_x,Cap_2U_ZF_f,'b--','Linewidth',2)
    %plot(Cap_2U_MMSE_x,Cap_2U_MMSE_f,'r--','Linewidth',2)

%     legend('iid SUTS 4U','iid ZF 4U','iid MMSE 4U','iid ZF 2U','iid MMSE 2U',...
%         'measured SUTS 4U',...
%         'measured ZF 4U','measured MMSE 4U','measured ZF 2U','measured MMSE 2U',...
%         'Location','SouthEast')
    legend('SU-MIMO TDMA iid','MU-MIMO ZF 4U iid','MU-MIMO MMSE 4U iid',...
        'SU-MIMO TDMA measured','MU-MIMO ZF 4U measured','MU-MIMO MMSE 4U measured',...
        'Location','SouthEast')
    title('Theoretical ideal models vs. measured channels','Fontsize',14,'Fontweight','bold')
    xlabel('bits/sec/Hz')
    xlim([0 18])
    ylabel('CDF')
    if logscale
        set(gca,'yscale','log')
        ylim([1e-2 1])
    end
    grid on
    filename_cap = fullfile(filepath,sprintf('idx_%d_capacity_4U_iid.eps',idx));
    saveas(h_fig, filename_cap, 'epsc2');
    filename_cap_pdf = fullfile(filepath,sprintf('idx_%d_capacity_4U_iid.pdf',idx));
    system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));
    
%%  AS vs. no AS for 2 and 4 Users
    h_fig = figure(7);
    % AS = load('Cap_iid_N_Rx=2.mat');
    hold off
    %plot(Cap_4U_ZF_iid_x,Cap_4U_ZF_iid_f,'b','Linewidth',1)
    %hold on
    %plot(Cap_4U_MMSE_iid_x,Cap_4U_MMSE_iid_f,'r','Linewidth',1)
    %hold on
    %plot(AS.Cap_4U_MMSE_iid_x,AS.Cap_4U_MMSE_iid_f,'m','Linewidth',1)

    %plot(Cap_2U_ZF_iid_x,Cap_2U_ZF_iid_f,'b--','Linewidth',1)
    %plot(Cap_2U_MMSE_iid_x,Cap_2U_MMSE_iid_f,'r--','Linewidth',1)
    %plot(AS.Cap_2U_MMSE_iid_x,AS.Cap_2U_MMSE_iid_f,'m--','Linewidth',1)
    
    %plot(Cap_4U_AS_ZF_x,Cap_4U_AS_ZF_f,'b','Linewidth',2)
    plot(Cap_4U_MMSE_x,Cap_4U_MMSE_f,'r','Linewidth',2)
    hold on
    plot(Cap_4U_AS_MMSE_x,Cap_4U_AS_MMSE_f,'m','Linewidth',2)

    %plot(Cap_2U_AS_ZF_x,Cap_2U_AS_ZF_f,'b--','Linewidth',2)
    plot(Cap_2U_MMSE_x,Cap_2U_MMSE_f,'r--','Linewidth',2)
    plot(Cap_2U_AS_MMSE_x,Cap_2U_AS_MMSE_f,'m--','Linewidth',2)

    legend('MU-MIMO MMSE 4U measured','MU-MIMO MMSE AS 4U measured','MU-MIMO MMSE 2U measured','MU-MIMO MMSE AS 2U measured',...
        'Location','SouthEast')
    title('No antenna selection vs. antenna selection','Fontsize',14,'Fontweight','bold')
    xlabel('bits/sec/Hz')
    xlim([0 18])
    ylabel('CDF')
    if logscale
        set(gca,'yscale','log')
        ylim([1e-2 1])
    end
    grid on
    filename_cap = fullfile(filepath,sprintf('idx_%d_capacity_AS.eps',idx));
    saveas(h_fig, filename_cap, 'epsc2');
    filename_cap_pdf = fullfile(filepath,sprintf('idx_%d_capacity_AS.pdf',idx));
    system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));
    
%% BD vs. MMSE maximum multiplexing gain
    h_fig = figure(8);
    AS = load('Cap_iid_N_Rx=2.mat');
    hold off
    plot(AS.Cap_2U_BD_WF_iid_x,AS.Cap_2U_BD_WF_iid_f,'k--','Linewidth',1)
    hold on

    plot(AS.Cap_4U_MMSE_iid_x,AS.Cap_4U_MMSE_iid_f,'r','Linewidth',1)
    %plot(AS.Cap_2U_MMSE_iid_x,AS.Cap_2U_MMSE_iid_f,'m--','Linewidth',1)
        
    plot(Cap_2U_BD_WF_x,Cap_2U_BD_WF_f,'k--','Linewidth',2)

    plot(Cap_4U_AS_MMSE_x,Cap_4U_AS_MMSE_f,'r','Linewidth',2)
    %plot(Cap_2U_AS_MMSE_x,Cap_2U_AS_MMSE_f,'m--','Linewidth',2)

    %plot(Cap_2U2_MMSE_x,Cap_2U2_MMSE_f,'r--','Linewidth',2);
    %plot(Cap_2U2_ZF_x,Cap_2U2_ZF_f,'b--','Linewidth',2);

    legend('MU-MIMO BD 2U iid ',...
        'MU-MIMO MMSE AS 4U iid',...
        'MU-MIMO BD 2U measured ',...
        'MU-MIMO MMSE AS 4U measured',...
        'Location','SouthEast')
    title('MMSE vs. BD with maximum multiplexing gain','Fontsize',14,'Fontweight','bold')
    xlabel('bits/sec/Hz')
    xlim([0 18])
    ylabel('CDF')
    if logscale
        set(gca,'yscale','log')
        ylim([1e-2 1])
    end
    grid on
    filename_cap = fullfile(filepath,sprintf('idx_%d_capacity_BD.eps',idx));
    saveas(h_fig, filename_cap, 'epsc2');
    filename_cap_pdf = fullfile(filepath,sprintf('idx_%d_capacity_BD.pdf',idx));
    system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));
    
%% BD vs. MMSE vs. DPC maximum multiplexing gain
    h_fig = figure(8);
    load results_DPC_idx_32.mat
    [Cap_4U_DPC_f,Cap_4U_DPC_x] = ecdf(CAP_4U_DPC(:));
    
    AS = load('Cap_iid_N_Rx=2.mat');
    hold off
    plot(AS.Cap_2U_BD_WF_iid_x,AS.Cap_2U_BD_WF_iid_f,'k--','Linewidth',1)
    hold on

    plot(AS.Cap_4U_MMSE_iid_x,AS.Cap_4U_MMSE_iid_f,'r','Linewidth',1)
    %plot(AS.Cap_2U_MMSE_iid_x,AS.Cap_2U_MMSE_iid_f,'m--','Linewidth',1)
        
    plot(AS.Cap_4U_DPC_iid_x,AS.Cap_4U_MMSE_iid_f,'b-.','Linewidth',1);

    plot(Cap_2U_BD_WF_x,Cap_2U_BD_WF_f,'k--','Linewidth',2)

    plot(Cap_4U_AS_MMSE_x,Cap_4U_AS_MMSE_f,'r','Linewidth',2)
    %plot(Cap_2U_AS_MMSE_x,Cap_2U_AS_MMSE_f,'m--','Linewidth',2)

    %plot(Cap_2U2_MMSE_x,Cap_2U2_MMSE_f,'r--','Linewidth',2);
    %plot(Cap_2U2_ZF_x,Cap_2U2_ZF_f,'b--','Linewidth',2);

    plot(Cap_4U_DPC_x,Cap_4U_MMSE_f,'b-.','Linewidth',2);

    legend('MU-MIMO BD 2U iid ',...
        'MU-MIMO MMSE AS 4U iid',...
        'MU-MIMO DPC 4U iid', ...
        'MU-MIMO BD 2U measured ',...
        'MU-MIMO MMSE AS 4U measured',...
        'MU-MIMO DPC 4U measured',...        
        'Location','SouthEast')
    title('MMSE vs. BD vs. DPC with maximum multiplexing gain','Fontsize',14,'Fontweight','bold')
    xlabel('bits/sec/Hz')
    xlim([0 18])
    ylabel('CDF')
    if logscale
        set(gca,'yscale','log')
        ylim([1e-2 1])
    end
    grid on
    filename_cap = fullfile(filepath,sprintf('idx_%d_capacity_DPC.eps',idx));
    saveas(h_fig, filename_cap, 'epsc2');
    filename_cap_pdf = fullfile(filepath,sprintf('idx_%d_capacity_DPC.pdf',idx));
    system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));
    
    

%%    
    h_fig = figure(9);
    colors = {'red','magenta','blue','black'};
    for k = 1:NUser
        plot((0:63)/7.68,10*log10(DProf(k,:)),'Linewidth',2,'Color',colors{k});
        hold on
    end
    hold off
    xlabel('Delay [\mus]');
    ylabel('Receive Power Level [dB]');
    legend('User 1','User 2','User 3','User 4')
    grid on
    filename_pdp = fullfile(filepath,sprintf('idx_%d_Dprofile.eps',idx));
    saveas(h_fig, filename_pdp, 'epsc2');
    filename_pdp_pdf = fullfile(filepath,sprintf('idx_%d_Dprofile.pdf',idx));
    system(sprintf('epstopdf --outfile=%s %s',filename_pdp_pdf,filename_pdp));

%%
    h_fig = figure(10);
    colors = {'red','magenta','blue','black'};
    for k = 1:2
        plot(min(framestamp_min(:,idx)):max(framestamp_max(:,idx)),filter(ones(1,50)/50,1,gain_2U_BD(:,k)),'Linewidth',1,'Color',colors{k});
        hold on
    end
    hold off
    xlabel('Frame');
    ylabel('Gain');
    legend('User 1','User 2','User 3','User 4')
    grid on
    filename_MUgains = fullfile(filepath,sprintf('idx_%d_MUgains.eps',idx));
    saveas(h_fig, filename_MUgains, 'epsc2');

%%    
end
