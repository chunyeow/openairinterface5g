function [x1]=mapping(mod1)

%  mod1 = modulation on tx ant
%  x1 = symbol stream by tx antenna

%QAM 4 constellation
x_4=[-1  -1   1  1 ];
y_4=[-1   1  -1  1 ];
Qam4=x_4+y_4*j;
Qam4=Qam4/sqrt(sum(abs(Qam4).^2)/4);

% QAM 16 constellation
x_16=[-1  -1 -1  -1     -3  -3 -3  -3      1  1  1  1      3  3  3  3 ];
y_16=[-1  -3  1   3     -1  -3  1   3     -1 -3  1  3     -1 -3  1  3 ];
Qam16=x_16+y_16*j;
Qam16=Qam16/sqrt(sum(abs(Qam16).^2)/16);

% QAM 64 constellation
x_64=[-1 -1 -1 -1 -1 -1 -1 -1    -3 -3 -3 -3 -3 -3 -3 -3   -5 -5 -5 -5 -5 -5 -5 -5    -7 -7 -7 -7 -7 -7 -7 -7    1   1  1  1  1  1  1  1     3  3  3  3  3  3  3  3     5  5  5  5  5  5  5  5     7  7  7  7  7  7  7  7];
y_64=[-1 -3 -5 -7  1  3  5  7    -1 -3 -5 -7  1  3  5  7   -1 -3 -5 -7  1  3  5  7    -1 -3 -5 -7  1  3  5  7    -1 -3 -5 -7  1  3  5  7    -1 -3 -5 -7  1  3  5  7    -1 -3 -5 -7  1  3  5  7    -1 -3 -5 -7  1  3  5  7];
Qam64=x_64+y_64*sqrt(-1);
Qam64=Qam64/sqrt(sum(abs(Qam64).^2)/64);

% Modulating on antenna 1
if mod1==4
    x1=Qam4;
elseif mod1==16
    x1=Qam16;
elseif mod1==64
    x1=Qam64;
else
    display('Modulation selected is not available in the library');
end



