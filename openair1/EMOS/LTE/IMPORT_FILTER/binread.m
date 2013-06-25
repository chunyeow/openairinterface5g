function s=binread(fid,fmt,count,varargin)
% BINREAD - Read formatted binary data from a file
%    S=BINREAD(FID,TEMPLATE) reads binary data from an open file handle and
%    formats it according to TEMPLATE. TEMPLATE is typically a structure, but
%    it may be any data or aggregate type. Refer to CSTRUCT for more details
%    on formatting.
%
%    S=BINREAD(...,COUNT) reads sucessive records from the file. If COUNT is
%    INF, then all data possible is read from the file. If omitted, or empty,
%    only one data record will be read.
%
%    S=BINREAD(...,COUNT,OPTS) passes the OPTS along to the call to CSTRUCT
%    which allows the byte alignment or order to be changed from default.
%
%    Example: (modeled from C structures)
%       header_template = struct('DataType',uint32(0),'DataCount',uint32(0));
%       data_template = struct('SampleTime',double(0),'Data',int16(zeros(1,16)));
%       fid = fopen('myfile.bin','rb');
%       header = binread(fid,header_template);
%       data = binread(fid,data_template,double(header.DataCount));
%
%    See also BINREAD, FOPEN, CSTRUCT

% 11/14/2003 fixed OBOB thanks to Olaf Bousche

if nargin < 3
  count = 1;
end

attrib = cstruct(fmt,[],varargin{:});
if isinf(count)
  s=cell(0,0);
else
  s = cell(count,1); % Preallocate cell structure
end
i=1;
while i <= count % 11/14/2003 '<' changed to '<='
  b = uint8(fread(fid,attrib.size,'uint8'));
  if length(b) < attrib.size
    if length(b) > 0
      warning('partial data record at end of file');
    end
    s(i:end) = []; % Out of data, remove extraneous elements
    break;
  end
  s{i} = cstruct(fmt,b,varargin{:});
  i = i+1;
end

try
  % Try de-celling the data, if possible
  s = [s{:}];
end
