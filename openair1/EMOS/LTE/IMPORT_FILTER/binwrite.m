function n=binwrite(fid,data,varargin)
%BINWRITE - Write data to a formatted binary file
%    BINWRITE(FID,DATA) writes data to an open file handle in a binary
%    format, according to the structure of DATA, and returns the number of
%    bytes written to the file. DATA is typically a structure, but it may
%    be any data or aggregate type. Refer to CSTRUCT for more details on
%    formatting.
%
%    S=BINWRITE(...,,OPTS) passes the OPTS along to the call to CSTRUCT
%    which allows the byte alignment or order to be changed from default.
%
%    Example: (modeled from C structures)
%       data(1) = struct('SampleTime',double(8),'Data',int16(1:16));
%       data(2) = struct('SampleTime',double(9),'Data',int16(101:116));
%       data(3) = struct('SampleTime',double(10),'Data',int16(201:216));
%       header = struct('DataType',uint32(1),'DataCount',uint32(length(data)));
%       fid = fopen('myfile.bin','wb');
%       binwrite(fid,header);
%       binwrite(fid,data);
%       fclose(fid);
%
%    See also BINREAD, FOPEN, CSTRUCT
[attrib,bytes] = cstruct(data);
n = fwrite(fid,bytes,'uint8');

