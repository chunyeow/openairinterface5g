% Skript: EMOS_generate_Latex_file
%
% This script generates the Latex source code for each measurement file.
%
%----------------------------------------------------------------------%
%                                                                      %
%              Author: Helmut Hofstetter <hofstett@eurecom.fr>  
%                      Florian Kaltenberger <kaltenbe@eurecom.fr> 
%             Created: 09/08/2006 by HH                                %
%   Last modification: 26/11/2007 by FK                                %
%        Organization: Institut Eurecom                                %
%                      2229 Route des Cretes                           %
%                      B.P. 193                                        %
%                      06904 Sophia Antipolis Cedex, France            %
%                                                                      %
%             Project: EMOS sounder                                    %
%    Deliverable Type: This M-file is part of the EMOS sounder         %
%         Source Code: MATLAB v6.5                                     %
%             History:                                                 %
%             Created: 1.00, 09/08/2006 by HH (Eurecom)
%                      2.00, 26/11/2007 adapted to new OFDM EMOS (FK) 
%                                                                      %
%----------------------------------------------------------------------%


texname  = sprintf('idx_%d.tex',idx);         % name to write the Tex file
fid      = fopen(fullfile(filepath_tex,texname),'w');

fname   = filename_emos{1,idx};
finfo   = sscanf(fname,'data_term%d_idx%d_%d_%d.EMOS');

for k=1:NUser
    fer(k) = sum(est(k,idx).err_ind)/length(est(k,idx).err_ind);
end

file_scenario=['Measurement:     & ',num2str(idx),'\\'];
file_date=['Date:                & ',num2str(finfo(3)),'\\'];
file_time=['Time:                & ',num2str(finfo(4)),'\\'];
% file_date=['Date:                & ',num2str(fname(24:25)),'.',num2str(fname(22:23)),'.',num2str(fname(18:21)),'\\'];
% file_time=['Time:                & ',num2str(fname(27:28)),':',num2str(fname(29:30)),'\\'];
file_freq=['Frequency:           & $',num2str(1.9176,'%3.4f'),'\,$GHz\\'];
file_bandwidth=['Bandwidth:           & $',num2str(4.8,'%3.2f'),'\,$MHz\\'];
file_numresp=['Number of frames: & $',num2str(max(framestamp_max(:,idx))-min(framestamp_min(:,idx))+1),'$\\'];
file_fer=['Frame Error Rate: & $',num2str(fer,'%1.2f, '),'$\\'];
%file_timebtwresp=['Time between Responses: & $',num2str(S.timestampE(3)-S.timestampE(2),'%1.3f'),'\,$sec\\'];
file_IRlength=['IR length:           & $',num2str(1),'\,$samples \\'];
file_Txpower=['Tx power:            & $30\,$dBm\\'];
file_Txdirection=['Tx antenna & \\'];
file_Txdirection2=['direction: &  towards Garbejaire\\'];
file_Rxheight=['Rx antenna position: & inside the car  \\'];

filename_rssi = fullfile(pwd,sprintf('figs/idx_%d_rx_rssi.eps',idx));
filename_gps = fullfile(pwd,sprintf('figs/idx_%d_map_rx_rssi.eps',idx));
filename_cap_4U_iid = fullfile(pwd,sprintf('figs/idx_%d_capacity_4U_iid.eps',idx));
filename_cap_4U = fullfile(pwd,sprintf('figs/idx_%d_capacity_4U.eps',idx));
filename_cap_2U = fullfile(pwd,sprintf('figs/idx_%d_capacity_2U.eps',idx));
filename_cap_AS = fullfile(pwd,sprintf('figs/idx_%d_capacity_AS.eps',idx));
filename_cap_2Tx = fullfile(pwd,sprintf('figs/idx_%d_capacity_NTx2.eps',idx));
filename_pdp = fullfile(pwd,sprintf('figs/idx_%d_Dprofile.eps',idx));
filename_align = fullfile(pwd,sprintf('figs/idx_%d_align.eps',idx));
filename_corr = fullfile(pwd,sprintf('figs/idx_%d_corr.eps',idx));
filename_gains = fullfile(pwd,sprintf('figs/idx_%d_MUgains.eps',idx));

fprintf(fid,'%s\r\n','\setlength{\unitlength}{1cm}');
fprintf(fid,'%s\r\n','\begin{picture}(16,21.3)');
fprintf(fid,'%s\r\n','\scriptsize{');

fprintf(fid,'%s\r\n','\put(7,21){\makebox(7,0.5)[tl]{');
fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
fprintf(fid,'%s\r\n',file_scenario);
fprintf(fid,'%s\r\n','\end{tabularx}}} \\');

fprintf(fid,'%s\r\n','\put(7,20.5){\makebox(7,0.5)[tl]{');
fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
fprintf(fid,'%s\r\n',file_date);
fprintf(fid,'%s\r\n','\end{tabularx}}} \\');

fprintf(fid,'%s\r\n','\put(7,20){\makebox(7,0.5)[tl]{');
fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
fprintf(fid,'%s\r\n',file_time);
fprintf(fid,'%s\r\n','\end{tabularx}}} \\');

% fprintf(fid,'%s\r\n','\put(7,19.5){\makebox(7,0.5)[tl]{');
% fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% fprintf(fid,'%s\r\n',file_freq);
% fprintf(fid,'%s\r\n','\end{tabularx}}} \\');
% 
% fprintf(fid,'%s\r\n','\put(7,19){\makebox(7,0.5)[tl]{');
% fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% fprintf(fid,'%s\r\n',file_bandwidth);
% fprintf(fid,'%s\r\n','\end{tabularx}}} \\');

fprintf(fid,'%s\r\n','\put(7,19.5){\makebox(7,0.5)[tl]{');
fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
fprintf(fid,'%s\r\n',file_numresp);
fprintf(fid,'%s\r\n','\end{tabularx}}} \\');

fprintf(fid,'%s\r\n','\put(7,19){\makebox(7,0.5)[tl]{');
fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
fprintf(fid,'%s\r\n',file_fer);
fprintf(fid,'%s\r\n','\end{tabularx}}} \\');

% fprintf(fid,'%s\r\n','\put(8,18.5){\makebox(7,0.5)[tl]{');
% fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% fprintf(fid,'%s\r\n',file_timebtwresp);
% fprintf(fid,'%s\r\n','\end{tabularx}}} \\');

% fprintf(fid,'%s\r\n','\put(7,17.5){\makebox(7,0.5)[tl]{');
% fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% fprintf(fid,'%s\r\n',file_IRlength);
% fprintf(fid,'%s\r\n','\end{tabularx}}} \\');

% fprintf(fid,'%s\r\n','\put(7,17){\makebox(7,0.5)[tl]{');
% fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% fprintf(fid,'%s\r\n',file_Txpower);
% fprintf(fid,'%s\r\n','\end{tabularx}}} \\');

% fprintf(fid,'%s\r\n','\put(7,16.5){\makebox(7,0.5)[tl]{');
% fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% fprintf(fid,'%s\r\n','Measurement duration & $',num2str(1,'%3.1f'),'\,$s\\');
% fprintf(fid,'%s\r\n','\end{tabularx}}} \\');

% fprintf(fid,'%s\r\n','\put(7,16){\makebox(7,0.5)[tl]{');
% fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% fprintf(fid,'%s\r\n','Tx antenna:          & Powerwave 7760.00\\');
% fprintf(fid,'%s\r\n','\end{tabularx}}} \\');
% 
% fprintf(fid,'%s\r\n','\put(7,15.5){\makebox(7,0.5)[tl]{');
% fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% fprintf(fid,'%s\r\n','Tx polarization:     & Slanted\\');
% fprintf(fid,'%s\r\n','\end{tabularx}}} \\');
% 
% %fprintf(fid,'%s\r\n','\put(8,16.5){\makebox(7,0.5)[tl]{');
% %fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% %fprintf(fid,'%s\r\n','Tx number of active elements:& $4$\\');
% %fprintf(fid,'%s\r\n','\end{tabularx}}} \\');
% 
% % fprintf(fid,'%s\r\n','\put(7,15){\makebox(7,0.5)[tl]{');
% % fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% % fprintf(fid,'%s\r\n','Tx element spacing:   & xxxmm\\');
% % fprintf(fid,'%s\r\n','\end{tabularx}}} \\');
% 
% fprintf(fid,'%s\r\n','\put(7,15){\makebox(7,0.5)[tl]{');
% fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% fprintf(fid,'%s\r\n','Tx antenna height above rooftop: & $3$m\\');
% fprintf(fid,'%s\r\n','\end{tabularx}}} \\');
% 
% fprintf(fid,'%s\r\n','\put(7,14,5){\makebox(7,0.5)[tl]{');
% fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% fprintf(fid,'%s\r\n',file_Txdirection);
% fprintf(fid,'%s\r\n','\end{tabularx}}} \\');
% 
% fprintf(fid,'%s\r\n','\put(7,14){\makebox(7,0.5)[tl]{');
% fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% fprintf(fid,'%s\r\n',file_Txdirection2);
% fprintf(fid,'%s\r\n','\end{tabularx}}} \\');
% 
% fprintf(fid,'%s\r\n','\put(7,13.5){\makebox(7,0.5)[tl]{');
% fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% fprintf(fid,'%s\r\n','Rx antenna:         & TCLIP-DE3G\\');
% fprintf(fid,'%s\r\n','\end{tabularx}}} \\');
% 
% fprintf(fid,'%s\r\n','\put(7,13){\makebox(7,0.5)[tl]{');
% fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% fprintf(fid,'%s\r\n','Rx polarization:     & vertical\\');
% fprintf(fid,'%s\r\n','\end{tabularx}}} \\');
% 
% fprintf(fid,'%s\r\n','\put(7,12.5){\makebox(7,0.5)[tl]{');
% fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% fprintf(fid,'%s\r\n','Rx number of active elements: & $2$\\');
% fprintf(fid,'%s\r\n','\end{tabularx}}} \\');
% 
% fprintf(fid,'%s\r\n','\put(7,12){\makebox(7,0.5)[tl]{');
% fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% fprintf(fid,'%s\r\n','Rx element spacing:  & $0.5\lambda - 75$mm\\');
% fprintf(fid,'%s\r\n','\end{tabularx}}} \\');
% 
% fprintf(fid,'%s\r\n','\put(7,11.5){\makebox(7,0.5)[tl]{');
% fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% fprintf(fid,'%s\r\n',file_Rxheight);
% fprintf(fid,'%s\r\n','\end{tabularx}}} \\');
% 
% fprintf(fid,'%s\r\n','\put(7,11){\makebox(7,0.5)[tl]{');
% fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% fprintf(fid,'%s\r\n','SNR for the multiuser capacity & $10\,$dB \\');
% fprintf(fid,'%s\r\n','\end{tabularx}}}');

% fprintf(fid,'%s\r\n','\put(7,10.7){\makebox(7,0.5)[tl]{');
% fprintf(fid,'%s\r\n','\begin{tabularx}{7cm}{Xr}');
% fprintf(fid,'%s\r\n','\bf{Rice factor} & \bf{',num2str(kfact,'%3.1f'),'}\\');
% fprintf(fid,'%s\r\n','\end{tabularx}}}');
% fprintf(fid,'%s\r\n','\normalsize \\');
 
% file_align=['\put(7.2,16.3){\makebox (7,1.5) {\epsfig{file=' filename_align ', height=1.3cm} } }'];
% fprintf(fid,'%s\r\n',file_align);
% fprintf(fid,'%s\r\n','\put(7.35,18){\scriptsize{Align matrix:}} \\');

% file_corr=['\put(7.2,16.3){\makebox (7,2) {\epsfig{file=' filename_corr ', height=1.8cm} } }'];
% fprintf(fid,'%s\r\n',file_corr);
% fprintf(fid,'%s\r\n','\put(7.35,18.5){\scriptsize{Correlation matrix:}} \\');

file_gps=['\put(0,16.5){\makebox (7,4.5) {\epsfig{file=' filename_gps ', width=6cm} } }'];
fprintf(fid,'%s\r\n',file_gps);
fprintf(fid,'%s\r\n','\put(0,21.3){\scriptsize{Plot 1: Map of the measurement run}} \\');

% file_gains=['\put(7.35,11){\makebox (7,4.5) {\epsfig{file=' filename_gains ', height=5cm} } }'];
% fprintf(fid,'%s\r\n',file_gains);
% fprintf(fid,'%s\r\n','\put(7.35,15.8){\scriptsize{Effective User Gains:}} \\');

file_pdp=['\put(0,11){\makebox (7,4.5) {\epsfig{file=' filename_pdp ', height=5cm} } }'];
fprintf(fid,'%s\r\n',file_pdp);
fprintf(fid,'%s\r\n','\put(0,15.8){\scriptsize{Plot 2: Power delay profile}} \\');

file_rssi=['\put(7.35,11){\makebox (7,4.5) {\epsfig{file=' filename_rssi ', height=5cm} } }'];
fprintf(fid,'%s\r\n',file_rssi);
fprintf(fid,'%s\r\n','\put(7.35,15.8){\scriptsize{Plot 3: Receive power over time}} \\');

% file_cap_4U_iid=['\put(0,5.5){\makebox (7,4.5) {\epsfig{file=' filename_cap_4U_iid ', height=5cm} } }'];
% fprintf(fid,'%s\r\n',file_cap_4U_iid);
% fprintf(fid,'%s\r\n','\put(0,10.3){\scriptsize{CDF of multiuser capacity (10dB SNR):}} \\');

file_cap_4U=['\put(0,5.5){\makebox (7,4.5) {\epsfig{file=' filename_cap_4U ', height=5cm} } }'];
fprintf(fid,'%s\r\n',file_cap_4U);
fprintf(fid,'%s\r\n','\put(0,10.3){\scriptsize{Plot 4: SU vs.\ MU with max.\ multipl.\ gain}} \\');

file_cap_2U=['\put(7.35,5.5){\makebox (7,4.5) {\epsfig{file=' filename_cap_2U ', height=5cm} } }'];
fprintf(fid,'%s\r\n',file_cap_2U);
fprintf(fid,'%s\r\n','\put(7.35,10.3){\scriptsize{Plot 5: SU vs.\ MU with equal multipl.\ gain}} \\');

file_cap_2Tx=['\put(0,0){\makebox (7,4.5) {\epsfig{file=' filename_cap_2Tx ', height=5cm} } }'];
fprintf(fid,'%s\r\n',file_cap_2Tx);
fprintf(fid,'%s\r\n','\put(0,4.8){\scriptsize{Plot 6: Two Tx antennas vs.\ four Tx antennas}} \\');

file_cap_AS=['\put(7.35,0){\makebox (7,4.5) {\epsfig{file=' filename_cap_AS ', height=5cm} } }'];
fprintf(fid,'%s\r\n',file_cap_AS);
fprintf(fid,'%s\r\n','\put(7.35,4.8){\scriptsize{Plot 7: No antenna selection vs.\ antenna selection}} \\');

fprintf(fid,'%s\r\n','}');
fprintf(fid,'%s\r\n','\end{picture}');

err=fclose(fid);

