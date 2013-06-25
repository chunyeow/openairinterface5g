function tp2 = scale_ideal_tp(tp1)
% tp2 = scale_ideal_tp(tp1)
%  this function scales the tp1 obtained with the MI criterion to the one
%  obtained by simulation.

load SISO_scaling.mat

% find the snr of tp in tp_mi
snr = interp1(tp_mi,snr_mi,tp1,'nearest','extrap');

% apply the offset of the nomadic antennas
% snr = snr - 4.7;

% lookup the new throughput
tp2 = interp1(snr_sim,tp_sim,snr,'nearest','extrap');

tp2(isnan(tp1))=nan;
