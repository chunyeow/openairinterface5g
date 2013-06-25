load Z:\EMOS\data\20071126_MU_Outdoor\data_term3_idx30_20071126_153448.mat
[Cap4x2_meas_f,Cap4x2_meas_x] = ecdf(CAP1(:));

load Z:\EMOS\data\20071128_SU_Indoor_RxCorrTest\data_term1_idx1_20071128_190238.mat
[Cap4x2_meas2_f,Cap4x2_meas2_x] = ecdf(CAP1(:));

load Z:\EMOS\data\20071126_MU_Outdoor\results_idx_32.mat
[Cap_4U_SUTS_f,Cap_4U_SUTS_x] = ecdf(CAP_4U_SUTS(:));

load cap_iid.mat

%% plot results
h_fig = figure(78)
hold off
plot(Cap4x2_iid_x,Cap4x2_iid_f,'b')
hold on
plot(Cap4x2_meas_x,Cap4x2_meas_f,'b--','Linewidth',2)
plot(Cap4x2_meas2_x,Cap4x2_meas2_f,'b','Linewidth',2)
plot(Cap_MU_SUTS_iid_x,Cap_MU_SUTS_iid_f,'k')
plot(Cap_4U_SUTS_x,Cap_4U_SUTS_f,'k','Linewidth',2)
xlabel('bits/sec/Hz')
%xlim([0 18])
ylabel('CDF')
title('Capacity') 
legend('SU 4x2 iid','SU 4x2 meas outdoor','SU 4x2 meas indoor','SUTS 4x2 iid','SUTS 4x2 measured','Location','SouthEast')
grid on
filename_cap = 'capacity_comp_SU_MU.eps';
saveas(h_fig, filename_cap, 'epsc2');
system(['epstopdf ' filename_cap]);
