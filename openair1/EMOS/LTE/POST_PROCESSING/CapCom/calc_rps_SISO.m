function [Rate_4Qam_1stRx,Rate_16Qam_1stRx,Rate_64Qam_1stRx,Rate_4Qam_2ndRx,...
    Rate_16Qam_2ndRx,Rate_64Qam_2ndRx,siso_SNR_1stRx_persecond,...
    siso_SNR_2ndRx_persecond,Rate_4Qam_2Rx,Rate_16Qam_2Rx,Rate_64Qam_2Rx,...
    siso_SNR_2Rx_persecond,Rate_supportedQam_1stRx,Rate_supportedQam_2ndRx, Rate_supportedQam_2Rx]  = calc_rps_SISO(estimates_UE)

capacity_siso_1 

[x y z] = size(chcap_siso_single_stream_4Qam_1stRx);

siso_SNR_1stRx_persecond = mean(SNR_eNB_1stRx(:));
siso_SNR_2ndRx_persecond = mean(SNR_eNB_2ndRx(:));
siso_SNR_2Rx_persecond = mean(SNR_eNB_2Rx(:));
% figure
% hold on
% plot(SNR_eNB1, 'b-x');

% Following will hold the data rate of one whole Frame

Frame_DL_sum_4Qam_1stRx = zeros(1, length(estimates_UE));
Frame_DL_sum_16Qam_1stRx = zeros(1,length(estimates_UE));
Frame_DL_sum_64Qam_1stRx = zeros(1,length(estimates_UE));
Frame_DL_sum_supportedQam_1stRx = zeros(1,length(estimates_UE));

Frame_DL_sum_4Qam_2ndRx = zeros(1, length(estimates_UE));
Frame_DL_sum_16Qam_2ndRx = zeros(1,length(estimates_UE));
Frame_DL_sum_64Qam_2ndRx = zeros(1,length(estimates_UE));
Frame_DL_sum_supportedQam_2ndRx = zeros(1,length(estimates_UE));

Frame_DL_sum_4Qam_2Rx = zeros(1, length(estimates_UE));
Frame_DL_sum_16Qam_2Rx = zeros(1,length(estimates_UE));
Frame_DL_sum_64Qam_2Rx = zeros(1,length(estimates_UE));
Frame_DL_sum_supportedQam_2Rx = zeros(1,length(estimates_UE));

for i = 1:length(estimates_UE)
    %following holds accumulated data rate of 200 resource elements
    
    RS_sum_4Qam_1stRx = sum(chcap_siso_single_stream_4Qam_1stRx(1,:,i));
    RS_sum_16Qam_1stRx = sum(chcap_siso_single_stream_16Qam_1stRx(1,:,i));
    RS_sum_64Qam_1stRx = sum(chcap_siso_single_stream_64Qam_1stRx(1,:,i));
    RS_sum_supportedQam_1stRx = sum(chcap_siso_single_stream_supportedQam_1stRx(1,:,i));
    
    RS_sum_4Qam_2ndRx = sum(chcap_siso_single_stream_4Qam_2ndRx(1,:,i));
    RS_sum_16Qam_2ndRx = sum(chcap_siso_single_stream_16Qam_2ndRx(1,:,i));
    RS_sum_64Qam_2ndRx = sum(chcap_siso_single_stream_64Qam_2ndRx(1,:,i));
    RS_sum_supportedQam_2ndRx = sum(chcap_siso_single_stream_supportedQam_2ndRx(1,:,i));

    RS_sum_4Qam_2Rx = sum(chcap_siso_single_stream_4Qam_2Rx(1,:,i));
    RS_sum_16Qam_2Rx = sum(chcap_siso_single_stream_16Qam_2Rx(1,:,i));
    RS_sum_64Qam_2Rx = sum(chcap_siso_single_stream_64Qam_2Rx(1,:,i));
    RS_sum_supportedQam_2Rx = sum(chcap_siso_single_stream_supportedQam_2Rx(1,:,i));
    % The number of RE availiable for the DLSCH is 9 OFDM symbols * 300
    % subcarriers - 300 pilots = 2400.
    % since 2400/200 = 12 groups of 200 resource elements are there in one subframe, so to get
    % the estimate of whole subframe we need to multiply RS_sum_XQam by 12.
    
    Subframe_DL_sum_4Qam_1stRx = RS_sum_4Qam_1stRx * 12;
    Subframe_DL_sum_16Qam_1stRx = RS_sum_16Qam_1stRx * 12;
    Subframe_DL_sum_64Qam_1stRx = RS_sum_64Qam_1stRx * 12;
    Subframe_DL_sum_supportedQam_1stRx = RS_sum_supportedQam_1stRx * 12;
    
    Subframe_DL_sum_4Qam_2ndRx = RS_sum_4Qam_2ndRx * 12;
    Subframe_DL_sum_16Qam_2ndRx = RS_sum_16Qam_2ndRx * 12;
    Subframe_DL_sum_64Qam_2ndRx = RS_sum_64Qam_2ndRx * 12;
    Subframe_DL_sum_supportedQam_2ndRx = RS_sum_supportedQam_2ndRx * 12;
    
    Subframe_DL_sum_4Qam_2Rx = RS_sum_4Qam_2Rx * 12;
    Subframe_DL_sum_16Qam_2Rx = RS_sum_16Qam_2Rx * 12;
    Subframe_DL_sum_64Qam_2Rx = RS_sum_64Qam_2Rx * 12;
    Subframe_DL_sum_supportedQam_2Rx = RS_sum_supportedQam_2Rx * 12;
    
    %In LTE Confguration 3 we have 6 downlink subframes in one frame so
    
    Frame_DL_sum_4Qam_1stRx(i) = Subframe_DL_sum_4Qam_1stRx * 6;
    Frame_DL_sum_16Qam_1stRx(i) = Subframe_DL_sum_16Qam_1stRx * 6;
    Frame_DL_sum_64Qam_1stRx(i) = Subframe_DL_sum_64Qam_1stRx * 6;
    
    Frame_DL_sum_supportedQam_1stRx(i) = Subframe_DL_sum_supportedQam_1stRx * 6;
    
    Frame_DL_sum_4Qam_2ndRx(i) = Subframe_DL_sum_4Qam_2ndRx * 6;
    Frame_DL_sum_16Qam_2ndRx(i) = Subframe_DL_sum_16Qam_2ndRx * 6;
    Frame_DL_sum_64Qam_2ndRx(i) = Subframe_DL_sum_64Qam_2ndRx * 6;
    Frame_DL_sum_supportedQam_2ndRx(i) = Subframe_DL_sum_supportedQam_2ndRx * 6;
    
     Frame_DL_sum_4Qam_2Rx(i) = Subframe_DL_sum_4Qam_2Rx * 6;
    Frame_DL_sum_16Qam_2Rx(i) = Subframe_DL_sum_16Qam_2Rx * 6;
    Frame_DL_sum_64Qam_2Rx(i) = Subframe_DL_sum_64Qam_2Rx * 6;
    Frame_DL_sum_supportedQam_2Rx(i) = Subframe_DL_sum_supportedQam_2Rx * 6;
end

        % add rate of all 100 frames each of 10ms length to compute data rate/sec
        Rate_4Qam_1stRx = sum(Frame_DL_sum_4Qam_1stRx(:));
        Rate_16Qam_1stRx = sum(Frame_DL_sum_16Qam_1stRx(:));
        Rate_64Qam_1stRx = sum(Frame_DL_sum_64Qam_1stRx(:));
        Rate_supportedQam_1stRx = sum(Frame_DL_sum_supportedQam_1stRx(:));
        
        Rate_4Qam_2ndRx = sum(Frame_DL_sum_4Qam_2ndRx(:));
        Rate_16Qam_2ndRx = sum(Frame_DL_sum_16Qam_2ndRx(:));
        Rate_64Qam_2ndRx = sum(Frame_DL_sum_64Qam_2ndRx(:));
        Rate_supportedQam_2ndRx = sum(Frame_DL_sum_supportedQam_2ndRx(:));
        
        Rate_4Qam_2Rx = sum(Frame_DL_sum_4Qam_2Rx(:));
        Rate_16Qam_2Rx = sum(Frame_DL_sum_16Qam_2Rx(:));
        Rate_64Qam_2Rx = sum(Frame_DL_sum_64Qam_2Rx(:));
        Rate_supportedQam_2Rx = sum(Frame_DL_sum_supportedQam_2Rx(:));
 
        
    
    