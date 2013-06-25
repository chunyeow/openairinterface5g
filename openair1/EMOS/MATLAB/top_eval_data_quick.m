% Skript: top_eval_data.m:
%
% This script checks for EMOS data files within the current directory,
% extracts all the metadata out of the files, and stores them to a .mat
% file.
%
%
%----------------------------------------------------------------------%
%                                                                      %
%              Author: Florian Kaltenberger                            %
%             Created: 21/12/2007 by FK                               %
%   Last modification: 21/12/2007 by FK                                %
%        Organization: Institut Eurecom                                %
%                      2229 Route des Cretes                           %
%                      B.P. 193                                        %
%                      06904 Sophia Antipolis Cedex, France            %
%                                                                      %
%----------------------------------------------------------------------%

clear all
close all

addpath('/homes/kaltenbe/Devel/openair1/EMOS/MATLAB/import_filter/')
cd('/extras/kaltenbe/EMOS/data/20081023_CHORIST_BARCELONA/Campus2/');
IFversion = 2.0;
NTx=2;

for term = 1:4;
    
    clear est gps_data NFrames

    d=dir(sprintf('data_term%d_*.EMOS',term));
    if isempty(d)
        continue
    end

    for k=1:size(d,1)   % for all files of the directory
        dname=d(k).name;
        dsize=d(k).bytes;
        if (dsize>0) % if it is a measuremnt file
            filename_emos = dname;
            disp(filename_emos);
            %[path,name] = fileparts(filename_emos);
            [est(k),gps_data(k),NFrames(k)] = load_estimates2_no_channel(filename_emos,NTx,Inf,IFversion);
        end  % if b
    end    % for k=3:size(d,1)

    save(sprintf('data_term%d.mat',term));

end


