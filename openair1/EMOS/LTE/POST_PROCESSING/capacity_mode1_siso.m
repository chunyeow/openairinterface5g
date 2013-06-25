%load 'table_LTE_MODE_4_5.mat';
load 'SISO.mat';

[x, y, z] = size(estimates_UE_1(1).channel);

estimates_UE1 = estimates_UE_1(1,:);
estimates_UE2 = estimates_UE_1(2,:);

chcap_siso = zeros(1,1200, length(estimates_UE1));
chcap_siso_16 = zeros(1,1200, length(estimates_UE1));
chcap_siso_64 = zeros(1,1200, length(estimates_UE1));

subband_indices = 1:48:288;
subband_indices(7) = 289;
subband_indices(8) = 301;
for est=1:length(estimates_UE1)
    
        N0_1stRx = (double(estimates_UE1(est).phy_measurements(11).n0_power(1)));
        N0_2ndRx = (double(estimates_UE2(est).phy_measurements(11).n0_power(1)));
        
    
    H = estimates_UE1(est).channel;
    Hc = double(H(1:2:end,:,:))+1j*double(H(2:2:end,:,:));
    Hs = squeeze(10*log10(sum(sum(abs(Hc).^2,1),2)));
    [val, ind1] = max(Hs);
    antenna_index1 = estimates_UE1(est).phy_measurements(11).selected_rx_antennas(:,1);
    
    H = estimates_UE2(est).channel;
    Hc = double(H(1:2:end,:,:))+1j*double(H(2:2:end,:,:));
    Hs = squeeze(10*log10(sum(sum(abs(Hc).^2,1),2)));
    [val, ind2] = max(Hs);
    antenna_index2 = estimates_UE2(est).phy_measurements(11).selected_rx_antennas(:,1);
    
    %h11_eNB1 = double([estimates_UE1(est).channel(z+1:end,1,ind1).real]) + 1j*double([estimates_UE1(est).channel(z+1:end,1,ind1).imag]);
    channel_tmp = typecast(estimates_UE1(est).channel(201:end,1,ind1),'int16');
    h11_eNB1 = double(channel_tmp(1:2:end,1)) + 1j*double(channel_tmp(2:2:end,1));
    %% h11_eNB1 = double(channel_tmp(2:2:end,1)) + 1j*double(channel_tmp(1:2:end,1));
    
    h11_eNB1_srs1 = interp1(1:6:300,h11_eNB1(1:50),1:1:300,'linear','extrap');
    h11_eNB1_srs2 = interp1(4:6:300,h11_eNB1(51:100),1:1:300,'linear','extrap');
    h11_eNB1_srs3 = interp1(1:6:300,h11_eNB1(101:150),1:1:300,'linear','extrap');
    h11_eNB1_srs4 = interp1(4:6:300,h11_eNB1(151:200),1:1:300,'linear','extrap');
    
    h11_eNB1_comp = [h11_eNB1_srs1' h11_eNB1_srs2' h11_eNB1_srs3' h11_eNB1_srs4']';
    
    channel_tmp = typecast(estimates_UE1(est).channel(201:end,2,ind1),'int16');
    
    h12_eNB1 = double(channel_tmp(1:2:end,1)) + 1j*double(channel_tmp(2:2:end,1));
    %%h12_eNB1 = double(channel_tmp(2:2:end,1)) + 1j*double(channel_tmp(1:2:end,1));
    
    h12_eNB1_srs1 = interp1(1:6:300,h12_eNB1(1:50),1:1:300,'linear','extrap');
    h12_eNB1_srs2 = interp1(4:6:300,h12_eNB1(51:100),1:1:300,'linear','extrap');
    h12_eNB1_srs3 = interp1(1:6:300,h12_eNB1(101:150),1:1:300,'linear','extrap');
    h12_eNB1_srs4 = interp1(4:6:300,h12_eNB1(151:200),1:1:300,'linear','extrap');
    
    h12_eNB1_comp = [h12_eNB1_srs1' h12_eNB1_srs2' h12_eNB1_srs3' h12_eNB1_srs4']';
    
    channel_tmp = typecast(estimates_UE1(est).channel(201:end,3,ind1),'int16');
    h21_eNB1 = double(channel_tmp(1:2:end,1)) + 1j*double(channel_tmp(2:2:end,1));
    %%h21_eNB1 = double(channel_tmp(2:2:end,1)) + 1j*double(channel_tmp(1:2:end,1));
    
    h21_eNB1_srs1 = interp1(4:6:300,h21_eNB1(1:50),1:1:300,'linear','extrap');
    h21_eNB1_srs2 = interp1(1:6:300,h21_eNB1(51:100),1:1:300,'linear','extrap');
    h21_eNB1_srs3 = interp1(4:6:300,h21_eNB1(101:150),1:1:300,'linear','extrap');
    h21_eNB1_srs4 = interp1(1:6:300,h21_eNB1(151:200),1:1:300,'linear','extrap');
    
    h21_eNB1_comp = [h21_eNB1_srs1' h21_eNB1_srs2' h21_eNB1_srs3' h21_eNB1_srs4']';
    
    channel_tmp = typecast(estimates_UE1(est).channel(201:end,4,ind1),'int16');
    h22_eNB1 = double(channel_tmp(1:2:end,1)) + 1j*double(channel_tmp(2:2:end,1));
    %%h22_eNB1 = double(channel_tmp(2:2:end,1)) + 1j*double(channel_tmp(1:2:end,1));
    
    h22_eNB1_srs1 = interp1(4:6:300,h22_eNB1(1:50),1:1:300,'linear','extrap');
    h22_eNB1_srs2 = interp1(1:6:300,h22_eNB1(51:100),1:1:300,'linear','extrap');
    h22_eNB1_srs3 = interp1(4:6:300,h22_eNB1(101:150),1:1:300,'linear','extrap');
    h22_eNB1_srs4 = interp1(1:6:300,h22_eNB1(151:200),1:1:300,'linear','extrap');
    
    h22_eNB1_comp = [h22_eNB1_srs1' h22_eNB1_srs2' h22_eNB1_srs3' h22_eNB1_srs4']';
    
    
    channel_tmp = typecast(estimates_UE2(est).channel(201:end,1,ind2),'int16');
    
    h11_eNB2 = double(channel_tmp(1:2:end,1)) + 1j*double(channel_tmp(2:2:end,1));
    %%h11_eNB2 = double(channel_tmp(2:2:end,1)) + 1j*double(channel_tmp(1:2:end,1));
    
    h11_eNB2_srs1 = interp1(1:6:300,h11_eNB2(1:50),1:1:300,'linear','extrap');
    h11_eNB2_srs2 = interp1(4:6:300,h11_eNB2(51:100),1:1:300,'linear','extrap');
    h11_eNB2_srs3 = interp1(1:6:300,h11_eNB2(101:150),1:1:300,'linear','extrap');
    h11_eNB2_srs4 = interp1(4:6:300,h11_eNB2(151:200),1:1:300,'linear','extrap');
    
    h11_eNB2_comp = [h11_eNB2_srs1' h11_eNB2_srs2' h11_eNB2_srs3' h11_eNB2_srs4']';
    
    channel_tmp = typecast(estimates_UE2(est).channel(201:end,2,ind2),'int16');
    h12_eNB2 = double(channel_tmp(1:2:end,1)) + 1j*double(channel_tmp(2:2:end,1));
    %% h12_eNB2 = double(channel_tmp(2:2:end,1)) + 1j*double(channel_tmp(1:2:end,1));
    
    h12_eNB2_srs1 = interp1(1:6:300,h12_eNB2(1:50),1:1:300,'linear','extrap');
    h12_eNB2_srs2 = interp1(4:6:300,h12_eNB2(51:100),1:1:300,'linear','extrap');
    h12_eNB2_srs3 = interp1(1:6:300,h12_eNB2(101:150),1:1:300,'linear','extrap');
    h12_eNB2_srs4 = interp1(4:6:300,h12_eNB2(151:200),1:1:300,'linear','extrap');
    
    h12_eNB2_comp = [h12_eNB2_srs1' h12_eNB2_srs2' h12_eNB2_srs3' h12_eNB2_srs4']';
    
    channel_tmp = typecast(estimates_UE2(est).channel(201:end,3,ind2),'int16');
    h21_eNB2 = double(channel_tmp(1:2:end,1)) + 1j*double(channel_tmp(2:2:end,1));
    %% h21_eNB2 = double(channel_tmp(2:2:end,1)) + 1j*double(channel_tmp(1:2:end,1));
    h21_eNB2_srs1 = interp1(4:6:300,h21_eNB2(1:50),1:1:300,'linear','extrap');
    h21_eNB2_srs2 = interp1(1:6:300,h21_eNB2(51:100),1:1:300,'linear','extrap');
    h21_eNB2_srs3 = interp1(4:6:300,h21_eNB2(101:150),1:1:300,'linear','extrap');
    h21_eNB2_srs4 = interp1(1:6:300,h21_eNB2(151:200),1:1:300,'linear','extrap');
    
    h21_eNB2_comp = [h21_eNB2_srs1' h21_eNB2_srs2' h21_eNB2_srs3' h21_eNB2_srs4']';
    
    
    channel_tmp = typecast(estimates_UE2(est).channel(201:end,4,ind2),'int16');
    h22_eNB2 = double(channel_tmp(1:2:end,1)) + 1j*double(channel_tmp(2:2:end,1));
    %% h22_eNB2 = double(channel_tmp(2:2:end,1)) + 1j*double(channel_tmp(1:2:end,1));
    
    h22_eNB2_srs1 = interp1(4:6:300,h22_eNB2(1:50),1:1:300,'linear','extrap');
    h22_eNB2_srs2 = interp1(1:6:300,h22_eNB2(51:100),1:1:300,'linear','extrap');
    h22_eNB2_srs3 = interp1(4:6:300,h22_eNB2(101:150),1:1:300,'linear','extrap');
    h22_eNB2_srs4 = interp1(1:6:300,h22_eNB2(151:200),1:1:300,'linear','extrap');
    
    h22_eNB2_comp = [h22_eNB2_srs1' h22_eNB2_srs2' h22_eNB2_srs3' h22_eNB2_srs4']';
    
    
    SNR_eNB1_1stRx = 10*log10(((abs(h11_eNB1_comp + h21_eNB1_comp).^2))/N0_1stRx);
    SNR_eNB1_2ndRx = 10*log10(((abs(h12_eNB1_comp + h22_eNB1_comp).^2))/N0_2ndRx);
    %SNR_eNB1_2Rx = 10*log10((((abs(h11_eNB1_comp + h12_eNB1_comp).^2))/NO_1stRx) + (((abs(h21_eNB1_comp + h22_eNB1_comp).^2))/NO_2ndRx));
    
    SNR_eNB2_1stRx = 10*log10(((abs(h11_eNB2_comp + h21_eNB2_comp).^2))/N0_1stRx);
    SNR_eNB2_2ndRx = 10*log10(((abs(h12_eNB2_comp + h22_eNB2_comp).^2))/N0_2ndRx);
    %SNR_eNB2_2Rx = 10*log10((((abs(h11_eNB2_comp + h12_eNB2_comp).^2))/NO_1stRx) + (((abs(h21_eNB2_comp + h22_eNB2_comp).^2))/NO_2ndRx));
   
    SNR_eNB1_1stRx((SNR_eNB1_1stRx <= -20)) = -20;
    SNR_eNB1_1stRx((SNR_eNB1_1stRx >= 40)) = 40;
    
    SNR_eNB2_1stRx((SNR_eNB2_1stRx <= -20)) = -20;
    SNR_eNB2_1stRx((SNR_eNB2_1stRx >= 40)) = 40;
    
    SNR_eNB1_2ndRx((SNR_eNB1_2ndRx <= -20)) = -20;
    SNR_eNB1_2ndRx((SNR_eNB1_2ndRx >= 40)) = 40;
    
    SNR_eNB2_2ndRx((SNR_eNB2_2ndRx <= -20)) = -20;
    SNR_eNB2_2ndRx((SNR_eNB2_2ndRx >= 40)) = 40;
    
       
    chcap_siso_1 = [];
    chcap_siso_2 = [];
    chcap_siso_3 = [];
    
       for i=1:1:7
        if (antenna_index1(i) == 0)
            SNR_eNB1 = SNR_eNB1_1stRx(:,subband_indices(i):subband_indices(i+1)-1);
        else
            SNR_eNB1 = SNR_eNB1_2ndRx(:,subband_indices(i):subband_indices(i+1)-1);
        end
        
        if (antenna_index2(i) == 0)
            SNR_eNB2 = SNR_eNB2_1stRx(:,subband_indices(i):subband_indices(i+1)-1);
        else
            SNR_eNB2 = SNR_eNB2_2ndRx(:,subband_indices(i):subband_indices(i+1)-1);
        end
        if(sum(SNR_eNB2(:)) < sum(SNR_eNB1(:)))
            chcap_siso_1 = [chcap_siso_1 (interp1(SNR,c_siso_4Qam,SNR_eNB1(:),'nearest','extrap'))'];
            chcap_siso_2 = [chcap_siso_2 (interp1(SNR,c_siso_16Qam,SNR_eNB1(:),'nearest','extrap'))'];
            chcap_siso_3 = [chcap_siso_3 (interp1(SNR,c_siso_64Qam,SNR_eNB1(:),'nearest','extrap'))'];
            
        else
            chcap_siso_1 = [chcap_siso_1 (interp1(SNR,c_siso_4Qam,SNR_eNB2(:),'nearest','extrap'))'];
            chcap_siso_2 = [chcap_siso_2 (interp1(SNR,c_siso_16Qam,SNR_eNB2(:),'nearest','extrap'))'];
            chcap_siso_3 = [chcap_siso_3 (interp1(SNR,c_siso_64Qam,SNR_eNB2(:),'nearest','extrap'))'];
        
        end
       end
    
    chcap_siso(1,:,est) = chcap_siso_1;
    chcap_siso_16(1,:,est) = chcap_siso_2;
    chcap_siso_64(1,:,est) = chcap_siso_3;
end