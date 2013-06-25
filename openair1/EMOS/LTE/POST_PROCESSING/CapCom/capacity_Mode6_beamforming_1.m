%Channel Capacity for LTE Mode 6 Beamforming
load 'SISO.mat'

[x, y, z] = size(estimates_UE(1).channel);

z = x/4;
h = zeros(1,2*z);

%precoding matrix values q
qi = [1 -1 1i -1i];

chcap_beamforming_4Qam_1Rx = zeros(1,length(h), length(estimates_UE));
chcap_beamforming_16Qam_1Rx = zeros(1,length(h), length(estimates_UE));
chcap_beamforming_64Qam_1Rx = zeros(1,length(h), length(estimates_UE));
chcap_beamforming_supportedQam_1Rx = zeros(1,length(h), length(estimates_UE));

chcap_beamforming_4Qam_2Rx = zeros(1,length(h), length(estimates_UE));
chcap_beamforming_16Qam_2Rx = zeros(1,length(h), length(estimates_UE));
chcap_beamforming_64Qam_2Rx = zeros(1,length(h), length(estimates_UE));
chcap_beamforming_supportedQam_2Rx = zeros(1,length(h), length(estimates_UE));

SNR_eNB_1Rx = zeros(1,length(h), length(estimates_UE));
SNR_eNB_2Rx = zeros(1,length(h), length(estimates_UE));

index(1).subband = [1:8 51:58 101:108 151:158];
index(2).subband = [9:16 59:66 109:116 159:166];
index(3).subband = [17:24 67:74 117:124 167:174];
index(4).subband = [25:32 75:82 125:132 175:182];
index(5).subband = [33:40 83:90 133:140 183:190];
index(6).subband = [41:48 91:98 141:148 191:198];
index(7).subband = [49 50 99 100 149 150 199 200];

for est=1:length(estimates_UE)
    
    NO_1Rx = double(estimates_UE(est).phy_measurements(11).n0_power(1));
    NO_2ndRx = double(estimates_UE(est).phy_measurements(11).n0_power(2));
    
    % for checking the connected Sector
    H = estimates_UE(est).channel;
    Hc = double(H(1:2:end,:,:))+1j*double(H(2:2:end,:,:));
    Hs = squeeze(10*log10(sum(sum(abs(Hc).^2,1),2)));
    [val, ind] = max(Hs);
    antenna_index = estimates_UE(est).phy_measurements(11).selected_rx_antennas(:,1);
         
        h11_eNB = double(estimates_UE(est).channel(2*z+1:2:end,1,ind)) + 1j*double(estimates_UE(est).channel(2*z+2:2:end,1,ind));
        h12_eNB = double(estimates_UE(est).channel(2*z+1:2:end,2,ind)) + 1j*double(estimates_UE(est).channel(2*z+2:2:end,2,ind));
        h21_eNB = double(estimates_UE(est).channel(2*z+1:2:end,3,ind)) + 1j*double(estimates_UE(est).channel(2*z+2:2:end,3,ind));
        h22_eNB = double(estimates_UE(est).channel(2*z+1:2:end,4,ind)) + 1j*double(estimates_UE(est).channel(2*z+2:2:end,4,ind));
        
        for i=1:1:7
            
            for qq=1:4
       
            Rx1(qq).eNB =  abs(h11_eNB(index(i).subband) + qi(qq)*h21_eNB(index(i).subband)).^2;
            Rx2(qq).eNB =  abs(h12_eNB(index(i).subband) + qi(qq)*h22_eNB(index(i).subband)).^2;
                
            end
            
            
            for ii=1:4
            
                SNR_eNB_q_1Rx(ii).snr = 10*log10((Rx1(ii).eNB)/NO_1Rx);
                sum_eNB_q_1Rx(ii) = sum(SNR_eNB_q_1Rx(ii).snr);
                
                SNR_eNB_q_2Rx(ii).snr = 10*log10(((Rx1(ii).eNB)/NO_1Rx) + ((Rx2(ii).eNB)/NO_2ndRx));
                sum_eNB_q_2Rx(ii) = sum(SNR_eNB_q_2Rx(ii).snr);
        
            end
        
        [value_eNB_1Rx, index_eNB_1Rx] = max(sum_eNB_q_1Rx);
        [value_eNB_2Rx, index_eNB_2Rx] = max(sum_eNB_q_2Rx);

        %SNR calculation
        SNR_eNB_1Rx(1, (index(i).subband), est) = SNR_eNB_q_1Rx(index_eNB_1Rx).snr;
        SNR_eNB_2Rx(1, (index(i).subband), est) = SNR_eNB_q_2Rx(index_eNB_2Rx).snr;
        end
        
        
        % after feedback in 3rd subframe
        
        Rx111 = [];
        Rx222 = [];
        
       % q = double(estimates_UE(est).phy_measurements(1).subband_pmi_re((antenna_index(i)+1),i,ind))+1j*double(estimates_UE(est).phy_measurements(1).subband_pmi_im((antenna_index(i)+1),i,ind));
       % q = double(estimates_UE(est).phy_measurements(1).subband_pmi_re(ind,:,1))+1j*double(estimates_UE(est).phy_measurements(1).subband_pmi_im(ind,:,1));
        for i =1:1:7
           
           q = double(estimates_UE(est).phy_measurements(1).subband_pmi_re((antenna_index(i)+1),i,1))+1j*double(estimates_UE(est).phy_measurements(1).subband_pmi_im((antenna_index(i)+1),i,1));
            
            qq(i) = quantize_q(q);
        
            Rx11 =  abs(h11_eNB(index(i).subband) + qq(i)*h21_eNB(index(i).subband)).^2;
            Rx22 =  abs(h12_eNB(index(i).subband) + qq(i)*h22_eNB(index(i).subband)).^2;
            
            Rx111 = [Rx111 Rx11'];
            Rx222 = [Rx222 Rx22'];
        end
        
            SNR_eNB_1Rx(1, 201:end, est) = 10*log10((Rx111)/NO_1Rx);
            SNR_eNB_2Rx(1, 201:end, est) = 10*log10((Rx111/NO_1Rx) + (Rx222/NO_2ndRx));
            
    nan_in_SNR = isnan(SNR_eNB_1Rx);
    SNR_eNB_1Rx(nan_in_SNR) = 0;
    
    nan_in_SNR = isnan(SNR_eNB_2Rx);
    SNR_eNB_2Rx(nan_in_SNR) = 0;

    
      chcap_beamforming_4Qam_1Rx(1, :, est) = interp1(SNR,c_siso_4Qam,SNR_eNB_1Rx(1, :, est),'nearest','extrap');
      chcap_beamforming_4Qam_2Rx(1, :, est) = interp1(SNR,c_siso_4Qam,SNR_eNB_2Rx(1, :, est),'nearest','extrap');

      chcap_beamforming_16Qam_1Rx(1, :, est) = interp1(SNR,c_siso_16Qam,SNR_eNB_1Rx(1, :, est),'nearest','extrap');
      chcap_beamforming_16Qam_2Rx(1, :, est) = interp1(SNR,c_siso_16Qam,SNR_eNB_2Rx(1, :, est),'nearest','extrap');
      
      chcap_beamforming_64Qam_1Rx(1, :, est) = interp1(SNR,c_siso_64Qam,SNR_eNB_1Rx(1, :, est),'nearest','extrap');
      chcap_beamforming_64Qam_2Rx(1, :, est) = interp1(SNR,c_siso_64Qam,SNR_eNB_2Rx(1, :, est),'nearest','extrap');
      
      %%
       while (min(SNR_eNB_1Rx(1, :, est)) < -20)
        [value, index1]  = min(SNR_eNB_1Rx(1, :, est));
        SNR_eNB_1Rx(1, index1, est) = -20;
        continue
        
        end
    
    while (max(SNR_eNB_1Rx(1, :, est)) > 40)
        [value, index1]  = max(SNR_eNB_1Rx(1, :, est));
        SNR_eNB_1Rx(1, index1, est) = 40;
        continue
        
    end
    
    while (min(SNR_eNB_2Rx(1, :, est)) < -20)
        [value, index1]  = min(SNR_eNB_2Rx(1, :, est));
        SNR_eNB_2Rx(1, index1, est) = -20;
        continue
        
    end
    
    while (max(SNR_eNB_2Rx(1, :, est)) > 40)
        [value, index1]  = max(SNR_eNB_2Rx(1, :, est));
        SNR_eNB_2Rx(1, index1, est) = 40;
        continue
    end
      
    MI = log2(abs((1 + SNR_eNB_1Rx(1,1:200,est))));
    C1 = mean(MI);
    
    MI = log2(abs((1 + SNR_eNB_1Rx(1,201:end,est))));
    C2 = mean(MI);
    
    MI = log2(abs((1 + SNR_eNB_2Rx(1,1:200,est))));
    C3 = mean(MI);
    
    MI = log2(abs((1 + SNR_eNB_2Rx(1,201:end,est))));
    C4 = mean(MI);
    
    
%     MI = log2(abs((1 + SNR_eNB_2ndRx(1,:,est))));
%     C2 = mean(MI);
    
    
    if C1 <2
        for c=1:z
        chcap_beamforming_supportedQam_1Rx(1, c, est) = c_siso_4Qam(find(SNR == round(SNR_eNB_1Rx(1, c, est))));
        end
    else
        if C1 >2 && C1 < 4
            for c=1:z
            chcap_beamforming_supportedQam_1Rx(1, c, est) = c_siso_16Qam(find(SNR == round(SNR_eNB_1Rx(1, c, est))));
            end
        else
            for c=1:z
            chcap_beamforming_supportedQam_1Rx(1, c, est) = c_siso_64Qam(find(SNR == round(SNR_eNB_1Rx(1, c, est))));
            end
            
        end
    end
    
    if C2 <2
        for c=201:2*z
        chcap_beamforming_supportedQam_1Rx(1, c, est) = c_siso_4Qam(find(SNR == round(SNR_eNB_1Rx(1, c, est))));
        end
    else
        if C2 >2 && C2 < 4
            for c=201:2*z
            chcap_beamforming_supportedQam_1Rx(1, c, est) = c_siso_16Qam(find(SNR == round(SNR_eNB_1Rx(1, c, est))));
            end
        else
            for c=201:2*z
            chcap_beamforming_supportedQam_1Rx(1, c, est) = c_siso_64Qam(find(SNR == round(SNR_eNB_1Rx(1, c, est))));
            end
        end
    end

    if C3 <2
        for c=1:z
        chcap_beamforming_supportedQam_2Rx(1, c, est) = c_siso_4Qam(find(SNR == round(SNR_eNB_2Rx(1, c, est))));
        end
    else
        if C3 >2 && C3 < 4
            for c=1:z
            chcap_beamforming_supportedQam_2Rx(1, c, est) = c_siso_16Qam(find(SNR == round(SNR_eNB_2Rx(1, c, est))));
            end
        else
            for c=1:z
            chcap_beamforming_supportedQam_2Rx(1, c, est) = c_siso_64Qam(find(SNR == round(SNR_eNB_2Rx(1, c, est))));
            end
            
        end
    end
    
    if C4 <2
        for c=201:2*z
        chcap_beamforming_supportedQam_2Rx(1, c, est) = c_siso_4Qam(find(SNR == round(SNR_eNB_2Rx(1, c, est))));
        end
    else
        if C4 >2 && C4 < 4
            for c=201:2*z
            chcap_beamforming_supportedQam_2Rx(1, c, est) = c_siso_16Qam(find(SNR == round(SNR_eNB_2Rx(1, c, est))));
            end
        else
            for c=201:2*z
            chcap_beamforming_supportedQam_2Rx(1, c, est) = c_siso_64Qam(find(SNR == round(SNR_eNB_2Rx(1, c, est))));
            end
        end
    end
end

