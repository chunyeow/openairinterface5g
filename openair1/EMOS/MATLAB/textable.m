function textable(FileName, Matrix, varargin)
% TEXTABLE  writes a TeX formatted table to file.
%   TEXTABLE(FILENAME, MATRIX) writes MATRIX in TeX tabular formatted style 
%       to the file "FILENAME.tbl"
%
%   TEXTABLE(..., OPTIONAL ARGUMENTS)
%      'folder','FOLDERNAME' outputfile is saved in FOLDERNAME\FILENAME.tbl
%
%      'line','LINECOMMAND' optional line argument that is placed in
%       between two lines of numbers. Standard is no line.
%
%      Example: TEXTABLE(...,'line','\hline')      --> puts "\hline" between 
%       every line of numbers.
%
%      'format', 'FORMATARGUMENT' optional argument to change float format
%       to e.g. exponential or integer format. Can be one argument, or one
%       continuous string for single columns. The format 'e' is translated 
%       into TeX code as "number\cdot 10^{exponential}"
%
%      Example 1: TEXTABLE(...,'format', 'i')      --> all columns will  
%       be integer format
%      Example 2: TEXTABLE(...,'format', 'iefff')  --> for every single  
%       column the format is defined.
%
%      'floatformat', [floatformat] is used to point the function how many
%       digits should be used. This works only in combination with 'format'
%       set to float, exponential or no setting (float 3.2 (###.##) is default).
%
%      Example 1: TEXTABLE(...,'floatformat', 1.4)        
%      Example 2: TEXTABLE(...,'floatformat', [1.4 3.2 4.5]) digits defined per
%       column
%
%      'environment', 'ENV' optional argument to point in what TeX environment
%       the table is to be placed: text (TXT) or math (MATH) environment. 
%       Default is 'TXT'. This has an effect on the placement of '$' around 
%       each element.
%
%      Example: TEXTABLE(...,'environment','MATH')
%       
%
%   [JdB] February 27, 2007
%

%   [JdB]            October  16, 2006 - first version
%   [JdB]            October  20, 2006 - exponential notation 'LaTeX style'
%                                      - 'line' argument bug fixed
%   [MarkDuppe]      November  3, 2006 - 'line' argument backslash bug fixed
%   [JdB]            November 24, 2006 - 'floatformat' also works for scientific
%                                        (exponential) notation. 
%                                      - minor improvements (commenting)
%   [BartekGrychtol] December 10, 2006 - exponential notation algorithm
%                                        improved
%   [JdB]            February 27, 2007 - 'environment' option added -
%                                        enables the generated tables to be 
%                                        used within math environment


% Define standard options -------------------------------------------------
Folder           = '';
Argument         = '';
FileAndExtension = [FileName, '.tbl'];                                     % .tbl extension is added
WidthMatrix      = size(Matrix,2);
HeightMatrix     = size(Matrix,1);
Format           = 'f';                                                    % standard format is float ('f')
FloatFormat      = 3.2;
Environment      = 'TXT';

% Analyse optional input --------------------------------------------------
ExtraInput  = (nargin-3);
if(ExtraInput>0)                % loop through all optional arguments
    for j = 1:2:ExtraInput
        switch lower(varargin{j})
            case 'folder'       % folder is specified
                Folder = varargin{j+1};
                FileAndExtension = [Folder,'/',FileAndExtension];
            case 'line'         % there is a between line argument specified
                Argument = varargin{j+1};
            case 'format'       % format is specified for:
                switch length(varargin{j+1}) % all columns            
                    case 1
                       Format = varargin{j+1};
                       for k=2:WidthMatrix
                            Format = [Format, varargin{j+1}];
                       end
                    otherwise   % each column specified
                        Format = varargin{j+1};
                end
            case 'floatformat'  % numeric format is specified
                 FloatFormat = varargin{j+1};
            case 'environment'  % environment is specified
                 Environment = varargin{j+1};
        end
                
    end
end

% Process optional input --------------------------------------------------
switch(Environment)
    case 'TXT'                  % when TeX text environment, a $ is placed
        EnvOp = '$';            % around each table entry
    case 'MATH'                 % when TeX math environment, this $ is
        EnvOp = '';             % omitted
end

if(WidthMatrix>1)
    if(length(Format)==1)       % if format scalar,
       for k=2:WidthMatrix      % then make array of that scalar (width = columnwidth)
            Format = [Format, Format];
       end 
    end
    if(length(FloatFormat)==1)  % if floatformat scalar, 
        FloatFormat = FloatFormat * ones(1, WidthMatrix); % then make array for every column
    end
end

%--------------------------------------------------------------------------
% Module to enable exponential notation in TeX
%--------------------------------------------------------------------------
WriteMatrix         = Matrix;
WriteFloatFormat    = FloatFormat;
WriteFormat         = Format;
WriteWidthMatrix    = WidthMatrix;
place               = 0;
for n = 1 : WidthMatrix                                                    % loop to insert exponential notation
    place = place+1;
    if(Format(n)=='e')
        for m=1:HeightMatrix
            ExponentialString = sprintf('%e',Matrix(m,n));                 % first let matlab put the column in exponential notation
            pos = strfind(ExponentialString,'e');                          % find position of 'e'
            Exp = str2double(ExponentialString((pos+1):end));              % extract exponential from this string
            Flo = str2double(ExponentialString(1:pos-1));                  % extract float from this string
            ColumnFloatExponential(m,:) = [Flo, Exp];                      % element in the matrix is now split into float and exponential
        end
        Etakeout = ColumnFloatExponential;                                 % Etakeout is now a 2 column vector containing 
                                                                           % on the left side the float and on the right side
                                                                           % exponential number.
        % place extra columns
        if(place>1)                                 
            if(place < WriteWidthMatrix)
                WriteMatrix         = [ WriteMatrix(:,1:place-1),      Etakeout, WriteMatrix(:, place+1:WriteWidthMatrix)       ];
                WriteFloatFormat    = [ WriteFloatFormat(:,1:place),   1,        WriteFloatFormat(:, place+1:WriteWidthMatrix)  ];
                WriteFormat         = [ WriteFormat(:,1:place-1),      'bx',     WriteFormat(:, place+1:WriteWidthMatrix)       ];
            else
                WriteMatrix         = [ WriteMatrix(:,1:place-1),      Etakeout  ];
                WriteFloatFormat    = [ WriteFloatFormat(:,1:place),   1         ];
                WriteFormat         = [ WriteFormat(:,1:place-1),      'bx'      ];
            end
        else
            if(place < WriteWidthMatrix)
                WriteMatrix         = [Etakeout,                        WriteMatrix(:,place+1:WriteWidthMatrix)     ];
                WriteFloatFormat    = [WriteFloatFormat(:,place), 1,    WriteFloatFormat(:,place+1:WriteWidthMatrix)];
                WriteFormat         = ['bx',                            WriteFormat(:,place+1:WriteWidthMatrix)     ];
            else
                WriteMatrix         = Etakeout;
                WriteFloatFormat    = [WriteFloatFormat(:,place), 1];
                WriteFormat         = 'bx';
            end
        end
        WriteWidthMatrix = size(WriteMatrix,2);
        place = place + 1;
    end  
end                                         


%--------------------------------------------------------------------------
%  Create String that writes the matrix in the table file
%--------------------------------------------------------------------------
switch(WriteFormat(1))                                                     % first element of WriteFormat 
    case 'b'
        PrintFormat = [ EnvOp, '%', num2str(WriteFloatFormat(1)), 'f'                   ];
    otherwise
        PrintFormat = [ EnvOp, '%', num2str(WriteFloatFormat(1)), WriteFormat(1), EnvOp ];
end
if(WriteWidthMatrix>1)
    for i=2:WriteWidthMatrix                                               % rest of elements of WriteFormat
        switch(WriteFormat(i))
            case 'b'  % float of exponential    
                PrintFormat = [PrintFormat, ' & ', EnvOp, '%', num2str(WriteFloatFormat(i)), 'f'                 ];            
            case 'x'  % exponential number
                PrintFormat = [PrintFormat, '\\cdot 10^{%i}', EnvOp                                              ];
            otherwise % other format
                PrintFormat = [PrintFormat, ' & ', EnvOp, '%',num2str(WriteFloatFormat(i)), WriteFormat(i), EnvOp];
        end
    end
end

%------------------------- Include optional argument & finish 'PrintFormat'
if(isempty(Argument)==0)                                                   % argument not empty
    tw=1;
    for t=1:length(Argument)                                               % loop through Argument string
        if(Argument(t)=='\')
            WriteArgument(tw:tw+1)='\\';                                   % replace single backslash with double since 
            tw=tw+1;                                                       % matlab fprintf needs that
        else
            WriteArgument(tw) = Argument(t);
        end
        tw=tw+1;
    end
    PrintFormat = [PrintFormat,' \\\\',WriteArgument, '\n'];               % finish format line with \n nextline command
else                                                                       % argument empty
    PrintFormat = [PrintFormat,' \\\\\n'];
end



%--------------------------------------------------------------------------
%  Write to file 
%--------------------------------------------------------------------------
try
   fid = fopen(FileAndExtension,'wt');                                     % open file
   fprintf (fid, PrintFormat, WriteMatrix.');                              % write to file
   fclose(fid);                                                            % close file
catch                                   
   disp 'Error while writing to file:'
   disp 'Folder could be nonexisting or file specified could be write-protected'
end

