% Skript: top_eval_data.m:
%
% This script checks for EMOS data files within the current directory and
% starts the eval_data.m script to extract all parameters
% for the automatic generation of a report page of the corresponding
% measurement run.
%
%
%----------------------------------------------------------------------%
%                                                                      %
%              Author: Florian Kaltenberger                            %
%             Created: 12/10/2007 by FK                                %
%   Last modification: 12/10/2007 by FK                                %
%        Organization: Institut Eurecom                                %
%                      2229 Route des Cretes                           %
%                      B.P. 193                                        %
%                      06904 Sophia Antipolis Cedex, France            %
%                                                                      %
%----------------------------------------------------------------------%

clear all
close all

if isunix
    mm = imread('/extras/kaltenbe/EMOS/2004_fd0006_250_c_0485.tif');
else
    mm = imread('\\extras\kaltenbe\EMOS\2004_fd0006_250_c_0485.tif');
end    

d=dir('*.EMOS');

for k=1:size(d,1)   % for all files of the directory
    dname=d(k).name;
    dsize=d(k).bytes;
    if dsize>0
        filename_emos = dname
        eval_data;   % extract all the parameters of a single measurement file
    end  % if b
end    % for k=3:size(d,1)

