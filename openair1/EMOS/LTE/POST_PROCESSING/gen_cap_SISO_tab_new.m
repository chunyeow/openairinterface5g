
%For generating table for SISO

SNR = -20:2:40;

c_siso_4Qam = zeros(size(SNR));
c_siso_16Qam = zeros(size(SNR));
c_siso_64Qam = zeros(size(SNR));


for q=1:3 % for following Constellations: QAM 4, QAM 16, QAM 64
    
    for v=1:length(SNR)
  
        if q ==1
        c_siso_4Qam(v) = capacity_siso_tab(SNR(v), 4);
        else if q ==2
                c_siso_16Qam(v) = capacity_siso_tab(SNR(v), 16);
            else
                c_siso_64Qam(v) = capacity_siso_tab(SNR(v), 64);
            end
        end
    
    end

end