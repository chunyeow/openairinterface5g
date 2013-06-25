filename_mat = sprintf('results_v2_idx_%d.mat',idx);
filename_SU_mat = sprintf('results_SU_idx_%d.mat',idx);

logscale = 0;

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
    filename_rssi = fullfile(filepath_figs,sprintf('idx_%d_rx_rssi.eps',idx));
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
    filename_gps = fullfile(filepath_figs,sprintf('idx_%d_map_rx_rssi.eps',idx));
    saveas(h_fig, filename_gps, 'epsc2');
    filename_gps_pdf = fullfile(filepath_figs,sprintf('idx_%d_map_rx_rssi.pdf',idx));
    system(sprintf('epstopdf --outfile=%s %s',filename_gps_pdf,filename_gps));

end

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
    filename_align = fullfile(filepath_figs,sprintf('idx_%d_align.eps',idx));
    saveas(h_fig, filename_align, 'epsc2');

if exist(filename_mat,'file')
    load(filename_mat)
    %load(filename_SU_mat,'CAP_SU_TDMA_MMSE','CAP_SU1x1_TDMA_ML','CAP_SU1x2_TDMA_ML')

%%
    % post processing
    NFrames_Tx_proc = sum(all(align_matrix==0));
    [Cap_SU_TDMA_CL_ML_f,Cap_SU_TDMA_CL_ML_x] = ecdf(CAP_SU_TDMA_CL_ML(:));
    [Cap_SU1x1_TDMA_ML_f,Cap_SU1x1_TDMA_ML_x] = ecdf(CAP_SU1x1_TDMA_ML(:));
    [Cap_SU1x2_TDMA_ML_f,Cap_SU1x2_TDMA_ML_x] = ecdf(CAP_SU1x2_TDMA_ML(:));
    [Cap_SU2x2_TDMA_ML_f,Cap_SU2x2_TDMA_ML_x] = ecdf(CAP_SU2x2_TDMA_ML(:));
    [Cap_SU_TDMA_ML_f,Cap_SU_TDMA_ML_x] = ecdf(CAP_SU_TDMA_ML(:));
    [Cap_SU_TDMA_DSTTD_f,Cap_SU_TDMA_DSTTD_x] = ecdf(CAP_SU_TDMA_DSTTD(:));
    [Cap_4U_ZF_f,Cap_4U_ZF_x] = ecdf(reshape(CAP_4U_ZF(:,1:NFrames_Tx_proc),1,[]));
    [Cap_4U_MMSE_f,Cap_4U_MMSE_x] = ecdf(reshape(CAP_4U_MMSE(:,1:NFrames_Tx_proc),1,[]));
    [Cap_2U_ZF_f,Cap_2U_ZF_x] = ecdf(reshape(CAP_2U_ZF(:,1:NFrames_Tx_proc),1,[]));
    [Cap_2U_MMSE_f,Cap_2U_MMSE_x] = ecdf(reshape(CAP_2U_MMSE(:,1:NFrames_Tx_proc),1,[]));
    [Cap_4U_AS_ZF_f,Cap_4U_AS_ZF_x] = ecdf(reshape(CAP_4U_AS_ZF(:,1:NFrames_Tx_proc),1,[]));
    [Cap_4U_AS_MMSE_f,Cap_4U_AS_MMSE_x] = ecdf(reshape(CAP_4U_AS_MMSE(:,1:NFrames_Tx_proc),1,[]));
    [Cap_2U_AS_ZF_f,Cap_2U_AS_ZF_x] = ecdf(reshape(CAP_2U_AS_ZF(:,1:NFrames_Tx_proc),1,[]));
    [Cap_2U_AS_MMSE_f,Cap_2U_AS_MMSE_x] = ecdf(reshape(CAP_2U_AS_MMSE(:,1:NFrames_Tx_proc),1,[]));
    %[Cap_2U_BD_WF_f,Cap_2U_BD_WF_x] = ecdf(reshape(CAP_2U_BD_WF(:,1:NFrames_Tx_proc),1,[]));
    %[Cap_2U_BD_EP_f,Cap_2U_BD_EP_x] = ecdf(reshape(CAP_2U_BD_EP(:,1:NFrames_Tx_proc),1,[]));
    [Cap_2U2Tx_ZF_f,Cap_2U2Tx_ZF_x] = ecdf(reshape(CAP_2U2Tx_ZF(:,1:NFrames_Tx_proc),1,[]));
    [Cap_2U2Tx_MMSE_f,Cap_2U2Tx_MMSE_x] = ecdf(reshape(CAP_2U2Tx_MMSE(:,1:NFrames_Tx_proc),1,[]));

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
    
    plot(Cap_SU1x1_TDMA_ML_x,Cap_SU1x1_TDMA_ML_f,'c:','Linewidth',2)
    hold on
    plot(Cap_SU1x2_TDMA_ML_x,Cap_SU1x2_TDMA_ML_f,'c-.','Linewidth',2)

    %plot(Cap_SU_TDMA_ML_x,Cap_SU_TDMA_ML_f,'k','Linewidth',2)
    plot(Cap_SU_TDMA_DSTTD_x,Cap_SU_TDMA_DSTTD_f,'k','Linewidth',2)
    hold on

    %plot(Cap_4U_ZF_x,Cap_4U_ZF_f,'b','Linewidth',2)
    %plot(Cap_4U_MMSE_x,Cap_4U_MMSE_f,'r','Linewidth',2)

    plot(Cap_2U_ZF_x,Cap_2U_ZF_f,'b--','Linewidth',2)
    plot(Cap_2U_MMSE_x,Cap_2U_MMSE_f,'r--','Linewidth',2)

%     legend('iid SUTS 4U','iid ZF 4U','iid MMSE 4U','iid ZF 2U','iid MMSE 2U',...
%         'measured SUTS 4U',...
%         'measured ZF 4U','measured MMSE 4U','measured ZF 2U','measured MMSE 2U',...
%         'Location','SouthEast')
    legend('SU-SISO 1x1 TDMA measured','SU-SIMO 1x2 TDMA measured','SU-MIMO 4x2 TDMA measured',...\
        'MU-MIMO ZF 2U 4Tx measured','MU-MIMO MMSE 2U 4Tx measured',...
        'Location','SouthEast')
    %title('SU-MIMO vs. MU-MIMO with equal multiplexing gain','Fontsize',14,'Fontweight','bold')
    xlabel('bits/sec/Hz')
    xlim([0 18])
    ylabel('Prob(capacity < x-axis)')
    if logscale
        set(gca,'yscale','log')
        ylim([1e-2 1])
    end
    grid on
    filename_cap = fullfile(filepath_figs,sprintf('idx_%d_capacity_2U.eps',idx));
    saveas(h_fig, filename_cap, 'epsc2');
    filename_cap_pdf = fullfile(filepath_figs,sprintf('idx_%d_capacity_2U.pdf',idx));
    system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));
    
%% SU-MIMO TDMA vs 4U MU-MIMO ZF and 4U MU-MIMO MMSE (measured only)
    h_fig = figure(5);
    %load('cap_iid.mat')
    hold off
    %plot(Cap_MU_SUTS_iid_x,Cap_MU_SUTS_iid_f,'k','Linewidth',1)
    %hold on

    plot(Cap_SU1x1_TDMA_ML_x,Cap_SU1x1_TDMA_ML_f,'c:','Linewidth',2)
    hold on
    plot(Cap_SU1x2_TDMA_ML_x,Cap_SU1x2_TDMA_ML_f,'c-.','Linewidth',2)

    %load('Cap_iid_N_Rx=1.mat')
    %plot(Cap_4U_ZF_iid_x,Cap_4U_ZF_iid_f,'b','Linewidth',1)
    %plot(Cap_4U_MMSE_iid_x,Cap_4U_MMSE_iid_f,'r','Linewidth',1)

    %plot(Cap_2U_ZF_iid_x,Cap_2U_ZF_iid_f,'b--','Linewidth',1)
    %plot(Cap_2U_MMSE_iid_x,Cap_2U_MMSE_iid_f,'r--','Linewidth',1)

    %plot(Cap_SU_TDMA_ML_x,Cap_SU_TDMA_ML_f,'k','Linewidth',2)
    plot(Cap_SU_TDMA_DSTTD_x,Cap_SU_TDMA_DSTTD_f,'k','Linewidth',2)
    %plot(Cap_SU_TDMA_CL_ML_x,Cap_SU_TDMA_CL_ML_f,'k','Linewidth',2)
    hold on

    plot(Cap_4U_ZF_x,Cap_4U_ZF_f,'b','Linewidth',2)
    plot(Cap_4U_MMSE_x,Cap_4U_MMSE_f,'r','Linewidth',2)

    %plot(Cap_2U_ZF_x,Cap_2U_ZF_f,'b--','Linewidth',2)
    %plot(Cap_2U_MMSE_x,Cap_2U_MMSE_f,'r--','Linewidth',2)

%     legend('iid SUTS 4U','iid ZF 4U','iid MMSE 4U','iid ZF 2U','iid MMSE 2U',...
%         'measured SUTS 4U',...
%         'measured ZF 4U','measured MMSE 4U','measured ZF 2U','measured MMSE 2U',...
%         'Location','SouthEast')
    legend('SU-SISO 1x1 TDMA measured','SU-SIMO 1x2 TDMA measured','SU-MIMO 4x2 TDMA measured',...
        'MU-MIMO ZF 4U 4Tx measured','MU-MIMO MMSE 4U 4Tx measured',...
        'Location','SouthEast')
    %title('SU-MIMO vs. MU-MIMO with max. multipexing gain','Fontsize',14,'Fontweight','bold')
    xlabel('bits/sec/Hz')
    xlim([0 18])
    ylabel('Prob(capacity < x-axis)')
    if logscale
        set(gca,'yscale','log')
        ylim([1e-2 1])
    end
    grid on
    filename_cap = fullfile(filepath_figs,sprintf('idx_%d_capacity_4U.eps',idx));
    saveas(h_fig, filename_cap, 'epsc2');
    filename_cap_pdf = fullfile(filepath_figs,sprintf('idx_%d_capacity_4U.pdf',idx));
    system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));

%% 4U MU-MIMO ZF and 4U MU-MIMO MMSE (measured+iid)
    h_fig = figure(6);
    %load('cap_iid.mat')
    hold off
    %plot(Cap_SU_TDMA_MMSE_iid_x,Cap_SU_TDMA_MMSE_iid_f,'k-.','Linewidth',1)
    %hold on

    load('Cap_iid_N_Rx=1.mat')
    plot(Cap_4U_ZF_iid_x,Cap_4U_ZF_iid_f,'b--','Linewidth',1)
    hold on
    plot(Cap_4U_MMSE_iid_x,Cap_4U_MMSE_iid_f,'r','Linewidth',1)

    %plot(Cap_2U_ZF_iid_x,Cap_2U_ZF_iid_f,'b--','Linewidth',1)
    %plot(Cap_2U_MMSE_iid_x,Cap_2U_MMSE_iid_f,'r--','Linewidth',1)

    %plot(Cap_SU_TDMA_MMSE_x,Cap_SU_TDMA_MMSE_f,'k-.','Linewidth',2)

    plot(Cap_4U_ZF_x,Cap_4U_ZF_f,'b--','Linewidth',2)
    plot(Cap_4U_MMSE_x,Cap_4U_MMSE_f,'r','Linewidth',2)

    %plot(Cap_2U_ZF_x,Cap_2U_ZF_f,'b--','Linewidth',2)
    %plot(Cap_2U_MMSE_x,Cap_2U_MMSE_f,'r--','Linewidth',2)

%     legend('iid SUTS 4U','iid ZF 4U','iid MMSE 4U','iid ZF 2U','iid MMSE 2U',...
%         'measured SUTS 4U',...
%         'measured ZF 4U','measured MMSE 4U','measured ZF 2U','measured MMSE 2U',...
%         'Location','SouthEast')
    legend('MU-MIMO ZF 4U 4Tx iid','MU-MIMO MMSE 4U 4Tx iid',...
        'MU-MIMO ZF 4U 4Tx measured','MU-MIMO MMSE 4U 4Tx measured',...
        'Location','SouthEast')
    %title('Theoretical ideal models vs. measured channels','Fontsize',14,'Fontweight','bold')
    xlabel('bits/sec/Hz')
    xlim([0 18])
    ylabel('Prob(capacity < x-axis)')
    if logscale
        set(gca,'yscale','log')
        ylim([1e-2 1])
    end
    grid on
    filename_cap = fullfile(filepath_figs,sprintf('idx_%d_capacity_4U_iid.eps',idx));
    saveas(h_fig, filename_cap, 'epsc2');
    filename_cap_pdf = fullfile(filepath_figs,sprintf('idx_%d_capacity_4U_iid.pdf',idx));
    system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));

% %% different config SU-MIMO TDMA (measured+iid)
%     h_fig = figure(11);
%     load('cap_iid.mat')
%     hold off
%     plot(Cap_SU_TDMA_MMSE_iid_x,Cap_SU_TDMA_MMSE_iid_f,'k-.','Linewidth',1)
%     hold on
%     plot(Cap_SU1x1_TDMA_ML_iid_x,Cap_SU1x1_TDMA_ML_iid_f,'m-','Linewidth',1)
%     plot(Cap_SU2x1_TDMA_ML_iid_x,Cap_SU2x1_TDMA_ML_iid_f,'c-.','Linewidth',1)
% 
%     %load('Cap_iid_N_Rx=1.mat')
%     %plot(Cap_4U_ZF_iid_x,Cap_4U_ZF_iid_f,'b--','Linewidth',1)
%     %hold on
%     %plot(Cap_4U_MMSE_iid_x,Cap_4U_MMSE_iid_f,'r','Linewidth',1)
% 
%     %plot(Cap_2U_ZF_iid_x,Cap_2U_ZF_iid_f,'b--','Linewidth',1)
%     %plot(Cap_2U_MMSE_iid_x,Cap_2U_MMSE_iid_f,'r--','Linewidth',1)
% 
%     plot(Cap_SU_TDMA_MMSE_x,Cap_SU_TDMA_MMSE_f,'k-.','Linewidth',2)
% 
%     plot(Cap_SU1x1_TDMA_ML_x,Cap_SU1x1_TDMA_ML_f,'m-','Linewidth',2)
%     plot(Cap_SU1x2_TDMA_ML_x,Cap_SU1x2_TDMA_ML_f,'c-.','Linewidth',2)
% 
%     %plot(Cap_4U_ZF_x,Cap_4U_ZF_f,'b--','Linewidth',2)
%     %plot(Cap_4U_MMSE_x,Cap_4U_MMSE_f,'r','Linewidth',2)
% 
%     %plot(Cap_2U_ZF_x,Cap_2U_ZF_f,'b--','Linewidth',2)
%     %plot(Cap_2U_MMSE_x,Cap_2U_MMSE_f,'r--','Linewidth',2)
% 
% %     legend('iid SUTS 4U','iid ZF 4U','iid MMSE 4U','iid ZF 2U','iid MMSE 2U',...
% %         'measured SUTS 4U',...
% %         'measured ZF 4U','measured MMSE 4U','measured ZF 2U','measured MMSE 2U',...
% %         'Location','SouthEast')
%     legend('SU-MIMO 4x2 MMSE TDMA iid','SU-SISO 1x1 ML TDMA iid','SU-SIMO 1x2 ML TDMA iid',...
%         'SU-MIMO 4x2 MMSE TDMA measured','SU-SISO 1x1 ML TDMA measured','SU-SIMO 1x2 ML TDMA measured',...
%         'Location','SouthEast')
%     title('Theoretical ideal models vs. measured channels','Fontsize',14,'Fontweight','bold')
%     xlabel('bits/sec/Hz')
%     xlim([0 18])
%     ylabel('CDF')
%     if logscale
%         set(gca,'yscale','log')
%         ylim([1e-2 1])
%     end
%     grid on
%     filename_cap = fullfile(filepath_figs,sprintf('idx_%d_capacity_SU_iid.eps',idx));
%     saveas(h_fig, filename_cap, 'epsc2');
%     filename_cap_pdf = fullfile(filepath_figs,sprintf('idx_%d_capacity_SU_iid.pdf',idx));
%     system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));
    
    
%%  AS vs. no AS for 2 and 4 Users
    h_fig = figure(7);
    % AS = load('Cap_iid_N_Rx=2.mat');
    hold off
    %plot(Cap_4U_ZF_iid_x,Cap_4U_ZF_iid_f,'b','Linewidth',1)
    %hold on
    %plot(Cap_4U_MMSE_iid_x,Cap_4U_MMSE_iid_f,'r','Linewidth',1)
    %hold on
    %plot(AS.Cap_4U_MMSE_iid_x,AS.Cap_4U_MMSE_iid_f,'m','Linewidth',1)

    %plot(Cap_SU1x1_TDMA_ML_x,Cap_SU1x1_TDMA_ML_f,'c:','Linewidth',2)
    %hold on
    %plot(Cap_SU1x2_TDMA_ML_x,Cap_SU1x2_TDMA_ML_f,'c-.','Linewidth',2)
  

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

    legend('MU-MIMO MMSE 4U 4Tx measured','MU-MIMO MMSE AS 4U 4Tx measured',...
        'MU-MIMO MMSE 2U 4Tx measured','MU-MIMO MMSE AS 2U 4Tx measured',...
        'Location','SouthEast')
    %title('No antenna selection vs. antenna selection','Fontsize',14,'Fontweight','bold')
    xlabel('bits/sec/Hz')
    xlim([0 18])
    ylabel('Prob(capacity < x-axis)')
    if logscale
        set(gca,'yscale','log')
        ylim([1e-2 1])
    end
    grid on
    filename_cap = fullfile(filepath_figs,sprintf('idx_%d_capacity_AS.eps',idx));
    saveas(h_fig, filename_cap, 'epsc2');
    filename_cap_pdf = fullfile(filepath_figs,sprintf('idx_%d_capacity_AS.pdf',idx));
    system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));

%%  2 Users, 2 transmit antennas, antenna selection, 
    h_fig = figure(8);
    hold off

    plot(Cap_SU1x1_TDMA_ML_x,Cap_SU1x1_TDMA_ML_f,'c:','Linewidth',2)
    hold on
    plot(Cap_SU1x2_TDMA_ML_x,Cap_SU1x2_TDMA_ML_f,'c-.','Linewidth',2)
    plot(Cap_SU2x2_TDMA_ML_x,Cap_SU2x2_TDMA_ML_f,'c-','Linewidth',2)
 
    plot(Cap_2U2Tx_MMSE_x,Cap_2U2Tx_MMSE_f,'r','Linewidth',2)
    plot(Cap_4U_AS_MMSE_x,Cap_4U_AS_MMSE_f,'m','Linewidth',2)

    legend('SU-SISO 1x1 TDMA measured',...
        'SU-SIMO 1x2 TDMA measured',...
        'SU-MIMO 2x2 TDMA measured',...
        'MU-MIMO MMSE 2U 2Tx AS measured',...
        'MU-MIMO MMSE 4U 4Tx AS measured',...
        'Location','SouthEast')
    %title('Two Tx antennas vs. 4 Tx antennas','Fontsize',14,'Fontweight','bold')
    xlabel('bits/sec/Hz')
    xlim([0 18])
    ylabel('Prob(capacity < x-axis)')
    if logscale
        set(gca,'yscale','log')
        ylim([1e-2 1])
    end
    grid on
    filename_cap = fullfile(filepath_figs,sprintf('idx_%d_capacity_NTx2.eps',idx));
    saveas(h_fig, filename_cap, 'epsc2');
    filename_cap_pdf = fullfile(filepath_figs,sprintf('idx_%d_capacity_NTx2.pdf',idx));
    system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));

if 0
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
    filename_pdp = fullfile(filepath_figs,sprintf('idx_%d_Dprofile.eps',idx));
    saveas(h_fig, filename_pdp, 'epsc2');
    filename_pdp_pdf = fullfile(filepath_figs,sprintf('idx_%d_Dprofile.pdf',idx));
    system(sprintf('epstopdf --outfile=%s %s',filename_pdp_pdf,filename_pdp));

% %%
%     h_fig = figure(10);
%     colors = {'red','magenta','blue','black'};
%     for k = 1:2
%         plot(min(framestamp_min(:,idx)):max(framestamp_max(:,idx)),filter(ones(1,50)/50,1,gain_2U_BD(:,k)),'Linewidth',1,'Color',colors{k});
%         hold on
%     end
%     hold off
%     xlabel('Frame');
%     ylabel('Gain');
%     legend('User 1','User 2','User 3','User 4')
%     grid on
%     filename_MUgains = fullfile(filepath_figs,sprintf('idx_%d_MUgains.eps',idx));
%     saveas(h_fig, filename_MUgains, 'epsc2');
% 
%%    
end

end