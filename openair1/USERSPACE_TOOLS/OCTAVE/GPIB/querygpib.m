function [qresult]=QueryGPIB(addr,cmdstr);
% Pass address of GPIB instrument and the command string to 
% trigger the query.
% example querygpib(20, FREQ?)

A = system('/local/gpibquery ' sprintf('%i ',addr) cmdstr);
qresult = sscanf(A, "%f")
