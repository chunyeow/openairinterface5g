function [Rate_4Qam_1Rx_maxq,Rate_16Qam_1Rx_maxq,Rate_64Qam_1Rx_maxq, Rate_4Qam_1Rx_feedbackq,...
    Rate_16Qam_1Rx_feedbackq,Rate_64Qam_1Rx_feedbackq,bmfr_optmq_SNR_1Rx, bmfr_feedbkq_SNR_1Rx,...
    Rate_4Qam_2Rx_maxq,Rate_16Qam_2Rx_maxq,Rate_64Qam_2Rx_maxq, Rate_4Qam_2Rx_feedbackq,...
    Rate_16Qam_2Rx_feedbackq,Rate_64Qam_2Rx_feedbackq,bmfr_optmq_SNR_2Rx,...
    bmfr_feedbkq_SNR_2Rx, Rate_supportedQam_1Rx_maxq, Rate_supportedQam_1Rx_feedbackq,...
    Rate_supportedQam_2Rx_maxq,Rate_supportedQam_2Rx_feedbackq]  = calc_rps_Beamforming(estimates_UE)

capacity_Mode6_beamforming_1


bmfr_optmq = SNR_eNB_1Rx(1,1:200,:);
bmfr_optmq_SNR_1Rx = mean(bmfr_optmq(:));

bmfr_optmq = SNR_eNB_2Rx(1,1:200,:);
bmfr_optmq_SNR_2Rx = mean(bmfr_optmq(:));

bmfr_feedbkq = SNR_eNB_1Rx(1,201:end,:);
bmfr_feedbkq_SNR_1Rx = mean(bmfr_feedbkq(:));

bmfr_feedbkq = SNR_eNB_2Rx(1,201:end,:);
bmfr_feedbkq_SNR_2Rx = mean(bmfr_feedbkq(:));

%[x y z] = size(chcap_siso_single_stream_4Qam_1Rx_eNB1);

% Following will hold the data rate of one whole Frame

Frame_DL_sum_4Qam_1Rx = zeros(1, length(estimates_UE));
Frame_DL_sum_16Qam_1Rx = zeros(1,length(estimates_UE));
Frame_DL_sum_64Qam_1Rx = zeros(1,length(estimates_UE));
Frame_DL_sum_supportedQam_1Rx = zeros(1,length(estimates_UE));

Frame_DL_sum_4Qam_2Rx = zeros(1, length(estimates_UE));
Frame_DL_sum_16Qam_2Rx = zeros(1,length(estimates_UE));
Frame_DL_sum_64Qam_2Rx = zeros(1,length(estimates_UE));
Frame_DL_sum_supportedQam_2Rx = zeros(1,length(estimates_UE));
for i = 1:length(estimates_UE)
   
     %following holds accumulated data rate of 200 resource elements
    % first 200 values corresponds to q which maximizes the over all sum
    % rate, and next 200 values correspond to the calculations which are
    % done on the basis of fedback q
    RS_sum_4Qam_1Rx_maxq = sum(chcap_beamforming_4Qam_1Rx(1,1:200,i));
    RS_sum_16Qam_1Rx_maxq = sum(chcap_beamforming_16Qam_1Rx(1,1:200,i));
    RS_sum_64Qam_1Rx_maxq = sum(chcap_beamforming_64Qam_1Rx(1,1:200,i));
    RS_sum_supportedQam_1Rx_maxq = sum(chcap_beamforming_supportedQam_1Rx(1,1:200,i));
    
    RS_sum_4Qam_2Rx_maxq = sum(chcap_beamforming_4Qam_2Rx(1,1:200,i));
    RS_sum_16Qam_2Rx_maxq = sum(chcap_beamforming_16Qam_2Rx(1,1:200,i));
    RS_sum_64Qam_2Rx_maxq = sum(chcap_beamforming_64Qam_2Rx(1,1:200,i));
    RS_sum_supportedQam_2Rx_maxq = sum(chcap_beamforming_supportedQam_2Rx(1,1:200,i));
    
    RS_sum_4Qam_1Rx_feedbackq = sum(chcap_beamforming_4Qam_1Rx(1,201:end,i));
    RS_sum_16Qam_1Rx_feedbackq = sum(chcap_beamforming_16Qam_1Rx(1,201:end,i));
    RS_sum_64Qam_1Rx_feedbackq = sum(chcap_beamforming_64Qam_1Rx(1,201:end,i));
    RS_sum_supportedQam_1Rx_feedbackq = sum(chcap_beamforming_supportedQam_1Rx(1,201:end,i));
    
    RS_sum_4Qam_2Rx_feedbackq = sum(chcap_beamforming_4Qam_2Rx(1,201:end,i));
    RS_sum_16Qam_2Rx_feedbackq = sum(chcap_beamforming_16Qam_2Rx(1,201:end,i));
    RS_sum_64Qam_2Rx_feedbackq = sum(chcap_beamforming_64Qam_2Rx(1,201:end,i));
    RS_sum_supportedQam_2Rx_feedbackq = sum(chcap_beamforming_supportedQam_2Rx(1,201:end,i));
    
    % since 4200/200 = 21 groups of 200 resource elements are there in one subframe, so to get
    % the estimate of whole subframe we need to multiply RS_sum_XQam_1Rx by 21.
    
    Subframe_DL_sum_4Qam_1Rx_maxq = RS_sum_4Qam_1Rx_maxq * 12;
    Subframe_DL_sum_16Qam_1Rx_maxq = RS_sum_16Qam_1Rx_maxq * 12;
    Subframe_DL_sum_64Qam_1Rx_maxq = RS_sum_64Qam_1Rx_maxq * 12;
    Subframe_DL_sum_supportedQam_1Rx_maxq = RS_sum_supportedQam_1Rx_maxq * 12;
 
    Subframe_DL_sum_4Qam_1Rx_feedbackq = RS_sum_4Qam_1Rx_feedbackq * 12;
    Subframe_DL_sum_16Qam_1Rx_feedbackq = RS_sum_16Qam_1Rx_feedbackq * 12;
    Subframe_DL_sum_64Qam_1Rx_feedbackq = RS_sum_64Qam_1Rx_feedbackq * 12;
    Subframe_DL_sum_supportedQam_1Rx_feedbackq = RS_sum_supportedQam_1Rx_feedbackq * 12;
    
    Subframe_DL_sum_4Qam_2Rx_maxq = RS_sum_4Qam_2Rx_maxq * 12;
    Subframe_DL_sum_16Qam_2Rx_maxq = RS_sum_16Qam_2Rx_maxq * 12;
    Subframe_DL_sum_64Qam_2Rx_maxq = RS_sum_64Qam_2Rx_maxq * 12;
    Subframe_DL_sum_supportedQam_2Rx_maxq = RS_sum_supportedQam_2Rx_maxq * 12;
 
    Subframe_DL_sum_4Qam_2Rx_feedbackq = RS_sum_4Qam_2Rx_feedbackq * 12;
    Subframe_DL_sum_16Qam_2Rx_feedbackq = RS_sum_16Qam_2Rx_feedbackq * 12;
    Subframe_DL_sum_64Qam_2Rx_feedbackq = RS_sum_64Qam_2Rx_feedbackq * 12;
    Subframe_DL_sum_supportedQam_2Rx_feedbackq = RS_sum_supportedQam_2Rx_feedbackq * 12;
    
    %In LTE Confguration 3 we have 6 downlink subframes in one frame so
    
    Frame_DL_sum_4Qam_1Rx_feedbackq(i) = Subframe_DL_sum_4Qam_1Rx_feedbackq * 6;
    Frame_DL_sum_16Qam_1Rx_feedbackq(i) = Subframe_DL_sum_16Qam_1Rx_feedbackq * 6;
    Frame_DL_sum_64Qam_1Rx_feedbackq(i) = Subframe_DL_sum_64Qam_1Rx_feedbackq * 6;
    Frame_DL_sum_supportedQam_1Rx_feedbackq(i) = Subframe_DL_sum_supportedQam_1Rx_feedbackq * 6;
    
    Frame_DL_sum_4Qam_1Rx_maxq(i) = Subframe_DL_sum_4Qam_1Rx_maxq * 6;
    Frame_DL_sum_16Qam_1Rx_maxq(i) = Subframe_DL_sum_16Qam_1Rx_maxq * 6;
    Frame_DL_sum_64Qam_1Rx_maxq(i) = Subframe_DL_sum_64Qam_1Rx_maxq * 6;
    Frame_DL_sum_supportedQam_1Rx_maxq(i) = Subframe_DL_sum_supportedQam_1Rx_maxq * 6;
    
    Frame_DL_sum_4Qam_2Rx_feedbackq(i) = Subframe_DL_sum_4Qam_2Rx_feedbackq * 6;
    Frame_DL_sum_16Qam_2Rx_feedbackq(i) = Subframe_DL_sum_16Qam_2Rx_feedbackq * 6;
    Frame_DL_sum_64Qam_2Rx_feedbackq(i) = Subframe_DL_sum_64Qam_2Rx_feedbackq * 6;
    Frame_DL_sum_supportedQam_2Rx_feedbackq(i) = Subframe_DL_sum_supportedQam_2Rx_feedbackq * 6;
    
    Frame_DL_sum_4Qam_2Rx_maxq(i) = Subframe_DL_sum_4Qam_2Rx_maxq * 6;
    Frame_DL_sum_16Qam_2Rx_maxq(i) = Subframe_DL_sum_16Qam_2Rx_maxq * 6;
    Frame_DL_sum_64Qam_2Rx_maxq(i) = Subframe_DL_sum_64Qam_2Rx_maxq * 6;
    Frame_DL_sum_supportedQam_2Rx_maxq(i) = Subframe_DL_sum_supportedQam_2Rx_maxq * 6;
end

        % add rate of all 100 frames each of 10ms length to compute data rate/sec
        Rate_4Qam_1Rx_maxq = sum(Frame_DL_sum_4Qam_1Rx_maxq(:));
        Rate_16Qam_1Rx_maxq = sum(Frame_DL_sum_16Qam_1Rx_maxq(:));
        Rate_64Qam_1Rx_maxq = sum(Frame_DL_sum_64Qam_1Rx_maxq(:));
        Rate_supportedQam_1Rx_maxq = sum(Frame_DL_sum_supportedQam_1Rx_maxq(:));
        
        Rate_4Qam_1Rx_feedbackq = sum(Frame_DL_sum_4Qam_1Rx_feedbackq(:));
        Rate_16Qam_1Rx_feedbackq = sum(Frame_DL_sum_16Qam_1Rx_feedbackq(:));
        Rate_64Qam_1Rx_feedbackq = sum(Frame_DL_sum_64Qam_1Rx_feedbackq(:));
        Rate_supportedQam_1Rx_feedbackq = sum(Frame_DL_sum_supportedQam_1Rx_feedbackq(:));
        
        Rate_4Qam_2Rx_maxq = sum(Frame_DL_sum_4Qam_2Rx_maxq(:));
        Rate_16Qam_2Rx_maxq = sum(Frame_DL_sum_16Qam_2Rx_maxq(:));
        Rate_64Qam_2Rx_maxq = sum(Frame_DL_sum_64Qam_2Rx_maxq(:));
        Rate_supportedQam_2Rx_maxq = sum(Frame_DL_sum_supportedQam_2Rx_maxq(:));
         
        Rate_4Qam_2Rx_feedbackq = sum(Frame_DL_sum_4Qam_2Rx_feedbackq(:));
        Rate_16Qam_2Rx_feedbackq = sum(Frame_DL_sum_16Qam_2Rx_feedbackq(:));
        Rate_64Qam_2Rx_feedbackq = sum(Frame_DL_sum_64Qam_2Rx_feedbackq(:));
        Rate_supportedQam_2Rx_feedbackq = sum(Frame_DL_sum_supportedQam_2Rx_feedbackq(:));
 
        
    
    