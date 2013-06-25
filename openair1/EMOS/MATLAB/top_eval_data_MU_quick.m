% Skript: top_eval_data_MU_quick.m:
%
% This script checks for EMOS data files within the current directory,
% loads their metadata and stores the first and last frameindex of the file
% into a table. It then find the frame indices, where all users have a
% signal. Further it finds the 10 largest contiguous sets of this region.
% Finally, the filenames of these regions are stored.
%
%
%----------------------------------------------------------------------%
%                                                                      %
%              Author: Florian Kaltenberger                            %
%             Created: 12/10/2007 by FK                                %
%   Last modification: 16/11/2007 by FK                                %
%        Organization: Institut Eurecom                                %
%                      2229 Route des Cretes                           %
%                      B.P. 193                                        %
%                      06904 Sophia Antipolis Cedex, France            %
%                                                                      %
%----------------------------------------------------------------------%

clear all
close all

addpath('/homes/kaltenbe/Devel/openair_trunk/openair1/EMOS/MATLAB/import_filter/')
%cd('/extras/kaltenbe/EMOS/data/20081110_MU_Outdoor/');
%cd('/extras/kaltenbe/EMOS/data/20081023_CHORIST_BARCELONA/Underground_Parking4');
%IFversion = 2.0;
cd('/extras/kaltenbe/EMOS/data/20090506_DualStream_Lab');
IFversion = 2.1;
NTx=2;

dd = dir('*.EMOS');
for d=1:length(dd)
    dname = dd(d).name;
    dsize = dd(d).bytes;
    if dsize > 0 % if it is a measuremnt file
        tmp = sscanf(dname,'data_term%d_idx%d_[...].EMOS');
        u = tmp(1);
        k = tmp(2)+1;
        filename_emos{u,k} = dname;
        disp(dname);

        [est(u,k),gps_data(u,k),NFrames(u,k)] = load_estimates2_no_channel(filename_emos{u,k},4,Inf,IFversion);

        framenoerror = est(u,k).framestamp_tx(~est(u,k).crc_status);
        if ~isempty(framenoerror)
            framestamp_min(u,k) = framenoerror(1);
            framestamp_max(u,k) = framenoerror(end);
        end
    end
end

% for u=1:4
% 
%     d=dir(sprintf('data_term%d*.EMOS',u));
%     
%     for k=1:size(d,1)   % for all files of the directory
%         filename_emos{u,k} = d(k).name;
%         disp(filename_emos{u,k});
%         % [path,name] = fileparts(filename_emos{u,k});
%         [est(u,k),gps_data(u,k),NFrames(u,k)] = load_estimates_no_channel(filename_emos{u,k},4,Inf,IFversion);
% 
%         framenoerror = est(u,k).framestamp_tx(~est(u,k).err_ind);
%         if ~isempty(framenoerror)
%             framestamp_min(u,k) = framenoerror(1);
%             framestamp_max(u,k) = framenoerror(end);
%         end
% 
%     end    % for k=1:size(d,1)
% 
% end

%%
if strfind(version,'7.3')
    save('metadata_MU.mat','-V7','framestamp_min','framestamp_max','filename_emos','est','gps_data','NFrames');
else
    save('metadata_MU.mat','framestamp_min','framestamp_max','filename_emos','est','gps_data','NFrames');
end


