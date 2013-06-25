%Channel Capacity of SISO system

load 'SISO.mat';
[x, y, z] = size(estimates_UE(1).channel);

z = x/4;

chcap_siso_single_stream_4Qam_2Rx = zeros(1,z, length(estimates_UE));
chcap_siso_single_stream_16Qam_2Rx = zeros(1,z, length(estimates_UE));
chcap_siso_single_stream_64Qam_2Rx = zeros(1,z, length(estimates_UE));
chcap_siso_single_stream_supportedQam_2Rx = zeros(1,z, length(estimates_UE));

chcap_siso_single_stream_4Qam_1stRx = zeros(1,z, length(estimates_UE));
chcap_siso_single_stream_16Qam_1stRx = zeros(1,z, length(estimates_UE));
chcap_siso_single_stream_64Qam_1stRx = zeros(1,z, length(estimates_UE));
chcap_siso_single_stream_supportedQam_1stRx = zeros(1,z, length(estimates_UE));

chcap_siso_single_stream_4Qam_2ndRx = zeros(1,z, length(estimates_UE));
chcap_siso_single_stream_16Qam_2ndRx = zeros(1,z, length(estimates_UE));
chcap_siso_single_stream_64Qam_2ndRx = zeros(1,z, length(estimates_UE));
chcap_siso_single_stream_supportedQam_2ndRx = zeros(1,z, length(estimates_UE));

SNR_eNB_1stRx = zeros(1,z, length(estimates_UE));
SNR_eNB_2ndRx = zeros(1,z, length(estimates_UE));
SNR_eNB_2Rx = zeros(1,z, length(estimates_UE));

I = [1 0;0 1];
for est=1:length(estimates_UE)
    
    NO_1stRx = double(estimates_UE(est).phy_measurements(11).n0_power(1));
    NO_2ndRx = double(estimates_UE(est).phy_measurements(11).n0_power(2));
    
    % for checking the connected Sector
    H = estimates_UE(est).channel;
    Hc = double(H(1:2:end,:,:))+1j*double(H(2:2:end,:,:));
    Hs = squeeze(10*log10(sum(sum(abs(Hc).^2,1),2)));
    [val, ind] = max(Hs);
    
         
        h11_eNB = double(estimates_UE(est).channel(2*z+1:2:end,1,ind)) + 1j*double(estimates_UE(est).channel(2*z+2:2:end,1,ind));
        h12_eNB = double(estimates_UE(est).channel(2*z+1:2:end,2,ind)) + 1j*double(estimates_UE(est).channel(2*z+2:2:end,2,ind));
        h21_eNB = double(estimates_UE(est).channel(2*z+1:2:end,3,ind)) + 1j*double(estimates_UE(est).channel(2*z+2:2:end,3,ind));
        h22_eNB = double(estimates_UE(est).channel(2*z+1:2:end,4,ind)) + 1j*double(estimates_UE(est).channel(2*z+2:2:end,4,ind));
    
     
    % version 1: using tabulated values
    SNR_eNB_1stRx(1, :, est) = 10*log10(abs(h11_eNB + h21_eNB).^2/NO_1stRx);
    
    SNR_eNB_2ndRx(1, :, est) = 10*log10(abs(h12_eNB + h22_eNB).^2/NO_2ndRx);
    
    SNR_eNB_2Rx(1, :, est) = 10*log10(((abs(h11_eNB + h21_eNB).^2)/NO_1stRx)+((abs(h12_eNB + h22_eNB).^2)/NO_2ndRx));
   
    nan_in_SNR = isnan(SNR_eNB_1stRx);
    SNR_eNB_1stRx(nan_in_SNR) = 0;
    
    nan_in_SNR = isnan(SNR_eNB_2ndRx);
    SNR_eNB_2ndRx(nan_in_SNR) = 0;
    
    nan_in_SNR = isnan(SNR_eNB_2Rx);
    SNR_eNB_2Rx(nan_in_SNR) = 0;
    
      chcap_siso_single_stream_4Qam_1stRx(1, :, est) = interp1(SNR,c_siso_4Qam,SNR_eNB_1stRx(1, :, est),'nearest','extrap');
      chcap_siso_single_stream_4Qam_2ndRx(1, :, est) = interp1(SNR,c_siso_4Qam,SNR_eNB_2ndRx(1, :, est),'nearest','extrap');
      chcap_siso_single_stream_4Qam_2Rx(1, :, est) = interp1(SNR,c_siso_4Qam,SNR_eNB_2Rx(1, :, est),'nearest','extrap');

      chcap_siso_single_stream_16Qam_1stRx(1, :, est) = interp1(SNR,c_siso_16Qam,SNR_eNB_1stRx(1, :, est),'nearest','extrap');
      chcap_siso_single_stream_16Qam_2ndRx(1, :, est) = interp1(SNR,c_siso_16Qam,SNR_eNB_2ndRx(1, :, est),'nearest','extrap');
      chcap_siso_single_stream_16Qam_2Rx(1, :, est) = interp1(SNR,c_siso_16Qam,SNR_eNB_2Rx(1, :, est),'nearest','extrap');
      
      chcap_siso_single_stream_64Qam_1stRx(1, :, est) = interp1(SNR,c_siso_64Qam,SNR_eNB_1stRx(1, :, est),'nearest','extrap');
      chcap_siso_single_stream_64Qam_2ndRx(1, :, est) = interp1(SNR,c_siso_64Qam,SNR_eNB_2ndRx(1, :, est),'nearest','extrap');
      chcap_siso_single_stream_64Qam_2Rx(1, :, est) = interp1(SNR,c_siso_64Qam,SNR_eNB_2Rx(1, :, est),'nearest','extrap');
   
   %%   
%     CH = [h11_eNB'; h12_eNB'; h21_eNB'; h22_eNB'];
%          
%     Hh = reshape(CH,2,2,200);
%     
%     %for mi=1:200
%         
% %     MI(mi) = log2(det((I + SNR_eNB_1stRx(1,mi,est)*(Hh(:,:,mi)*(conj(Hh(:,:,mi)))'))));
% 
%     %end
    
    
    while (min(SNR_eNB_1stRx(1, :, est)) < -20 )
        [value, index]  = min(SNR_eNB_1stRx(1, :, est));
        SNR_eNB_1stRx(1, index, est) = -20;
        continue
        
    end
    
    while (max(SNR_eNB_1stRx(1, :, est)) > 40)
        [value, index]  = max(SNR_eNB_1stRx(1, :, est));
        SNR_eNB_1stRx(1, index, est) = 40;
        continue
        
    end
    
    
    while (min(SNR_eNB_2ndRx(1, :, est)) < -20 )
        [value, index]  = min(SNR_eNB_2ndRx(1, :, est));
        SNR_eNB_2ndRx(1, index, est) = -20;
        continue
        
    end
    
    while (max(SNR_eNB_2ndRx(1, :, est)) > 40)
        [value, index]  = max(SNR_eNB_2ndRx(1, :, est));
        SNR_eNB_2ndRx(1, index, est) = 40;
        continue
        
    end
    
    while (min(SNR_eNB_2Rx(1, :, est)) < -20 )
        [value, index]  = min(SNR_eNB_2Rx(1, :, est));
        SNR_eNB_2Rx(1, index, est) = -20;
        continue
        
    end
    
    while (max(SNR_eNB_2Rx(1, :, est)) > 40)
        [value, index]  = max(SNR_eNB_2Rx(1, :, est));
        SNR_eNB_2Rx(1, index, est) = 40;
        continue
        
    end
   
    MI = log2(abs((1 + SNR_eNB_1stRx(1,:,est))));
    C1 = mean(MI);
    
    MI = log2(abs((1 + SNR_eNB_2ndRx(1,:,est))));
    C2 = mean(MI);
    
    MI = log2(abs((1 + SNR_eNB_2Rx(1,:,est))));
    C3 = mean(MI);
    
    
    if C1 <=2
        for c=1:z
        chcap_siso_single_stream_supportedQam_1stRx(1, c, est) = c_siso_4Qam(find(SNR == round(SNR_eNB_1stRx(1, c, est))));
        end
    else
        if C1 >2 && C1 <= 4
            for c=1:z
            chcap_siso_single_stream_supportedQam_1stRx(1, c, est) = c_siso_16Qam(find(SNR == round(SNR_eNB_1stRx(1, c, est))));
            end
        else
            for c=1:z
            chcap_siso_single_stream_supportedQam_1stRx(1, c, est) = c_siso_64Qam(find(SNR == round(SNR_eNB_1stRx(1, c, est))));
            end
            
        end
    end
    
    if C2 <=2
        for c=1:z
        chcap_siso_single_stream_supportedQam_2ndRx(1, c, est) = c_siso_4Qam(find(SNR == round(SNR_eNB_2ndRx(1, c, est))));
        end
    else
        if C2 >2 && C2 <= 4
            for c=1:z
            chcap_siso_single_stream_supportedQam_2ndRx(1, c, est) = c_siso_16Qam(find(SNR == round(SNR_eNB_2ndRx(1, c, est))));
            end
        else
            for c=1:z
            chcap_siso_single_stream_supportedQam_2ndRx(1, c, est) = c_siso_64Qam(find(SNR == round(SNR_eNB_2ndRx(1, c, est))));
            end
        end
    end
    
    if C3 <=2
        for c=1:z
        chcap_siso_single_stream_supportedQam_2Rx(1, c, est) = c_siso_4Qam(find(SNR == round(SNR_eNB_2Rx(1, c, est))));
        end
    else
        if C3 >2 && C3 <= 4
            for c=1:z
            chcap_siso_single_stream_supportedQam_2Rx(1, c, est) = c_siso_16Qam(find(SNR == round(SNR_eNB_2Rx(1, c, est))));
            end
        else
            for c=1:z
            chcap_siso_single_stream_supportedQam_2Rx(1, c, est) = c_siso_64Qam(find(SNR == round(SNR_eNB_2Rx(1, c, est))));
            end
        end
    end
    
%     MI = log2(abs((1 + SNR_eNB_2Rx(1,:,est))));
%     C3 = mean(MI);
%     nc = [NO_1stRx 0;0 NO_2ndRx];
%     
%     for mi = 1:200
%         
%     MII(mi) = log2(abs(det((I + (conj(Hh(:,:,mi)))'*nc*Hh(:,:,mi)))));
%     
%     end
%     
%     C4 = mean(MI)
%     for const=1:3
%         if const ==1
%             for c=1:z
       
      
      
%     chcap_siso_single_stream_16Qam(1, :, est) = interp1(SNR,c_siso_16Qam,SNR_eNB(1, :, est),'nearest','extrap');
%                     
%     chcap_siso_single_stream_64Qam(1, :, est) = interp1(SNR,c_siso_64Qam,SNR_eNB(1, :, est),'nearest','extrap');
%                 
%                 chcap_siso_single_stream_4Qam_1stRx(1, :, est) = c_siso_4Qam(find(SNR == round(SNR_eNB_1stRx(1, :, est))));
%                 chcap_siso_single_stream_4Qam_2ndRx(1, :, est) = c_siso_4Qam(find(SNR == round(SNR_eNB_2ndRx(1, :, est))));
%                 chcap_siso_single_stream_4Qam_2Rx(1, :, est) = c_siso_4Qam(find(SNR == round(SNR_eNB_2Rx(1, :, est))));
%                 
%             end
%         else if const==2
%                 for c=1:z
%                     
%                     chcap_siso_single_stream_16Qam_1stRx(1, c, est) = c_siso_16Qam(find(SNR == round(SNR_eNB_1stRx(1, c, est))));
%                     chcap_siso_single_stream_16Qam_2ndRx(1, c, est) = c_siso_16Qam(find(SNR == round(SNR_eNB_2ndRx(1, c, est))));
%                     chcap_siso_single_stream_16Qam_2Rx(1, c, est) = c_siso_16Qam(find(SNR == round(SNR_eNB_2Rx(1, c, est))));
%                     
%                 end
%             else
%                 for c=1:z
%                     
%                     chcap_siso_single_stream_64Qam_1stRx(1, c, est) = c_siso_64Qam(find(SNR == round(SNR_eNB_1stRx(1, c, est))));
%                     chcap_siso_single_stream_64Qam_2ndRx(1, c, est) = c_siso_64Qam(find(SNR == round(SNR_eNB_2ndRx(1, c, est))));
%                     chcap_siso_single_stream_64Qam_2Rx(1, c, est) = c_siso_64Qam(find(SNR == round(SNR_eNB_2Rx(1, c, est))));
%                     
%                 end
%             end
%         end
%     end
end

%save 'chcap_SISO_Measurements.mat' 'chcap_siso*' 'SNR_eNB*'
