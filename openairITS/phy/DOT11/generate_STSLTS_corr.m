nSubcarriers   = 48;                 % Number of data subcarriers
nPilots        = 4;                  % Number of pilot subcarriers
nPointsFFT     = 64;                 % Number of FFT points
nGuardSamples  = nPointsFFT/4;       % Samples in the guard interval

shortseq = (1.472) * complex(1,1)* ...
           [0, 0, 1, 0, 0, 0,-1, 0, 0, 0, 1, 0, 0, 0,-1, 0, 0, 0,-1, 0, 0, 0, 1, 0, 0, 0, ...
            0, 0, 0, 0,-1, 0, 0, 0,-1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0];
X=zeros(nPointsFFT,1);                 % Empty input to the IFFT
% Map subcarriers
X(39:64) = shortseq(1:26);             % Map subcarriers -26 to -1
X(1)     = shortseq(27);               % Map DC subcarrier (not really necessary)
X(2:27)  = shortseq(28:53);            % Map subcarriers 1 to 26
% IFFT and scale
x=sqrt(nPointsFFT)*ifft(X,nPointsFFT); % IFFT
% Repeat for 2.5 times
short_preamble=[x;x;x(1:32)];

longseq = [1, 1,-1,-1, 1, 1,-1, 1,-1, 1, 1, 1, 1, 1, 1,-1,-1, 1, 1,-1, 1,-1, 1, 1, 1, 1, ...
           0, 1,-1,-1, 1, 1,-1, 1,-1, 1,-1,-1,-1,-1,-1, 1, 1,-1,-1, 1,-1, 1,-1, 1, 1, 1, 1];
X=zeros(nPointsFFT,1);                 % Empty input to the IFFT
% Map subcarriers
X(39:64) = longseq(1:26);              % Map subcarriers -26 to -1
X(1)     = longseq(27);                % Map DC subcarrier (not really necessary)
X(2:27)  = longseq(28:53);             % Map subcarriers 1 to 26
% IFFT and scale
x=sqrt(nPointsFFT)*ifft(X,nPointsFFT); % IFFT
% Cyclic prefix using block-diagonal matrix T (note: double length!)
I=eye(2*nPointsFFT);                           % Identitity matrix used to construct T
T=[I(2*nPointsFFT-2*nGuardSamples+1:end,:);I]; % Matrix for cyclic prefix insertion
% Prepend the prefix once to two copies of the sequence 
long_preamble=T*[x;x];


STS_LTS = [short_preamble.' long_preamble.'];

STS_LTS_F = fft(STS_LTS,512);

STS_LTS_2 = floor(32767*STS_LTS_F/sqrt(512*9));

STS_LTS_3 = zeros(2048,1);

STS_LTS_3(1:4:end) = real(STS_LTS_2);
STS_LTS_3(2:4:end) = imag(STS_LTS_2);
STS_LTS_3(3:4:end) = -imag(STS_LTS_2);
STS_LTS_3(4:4:end) = real(STS_LTS_2);

fd = fopen("STS_LTS_F.h","w");
fprintf(fd,"int16_t STS_LTS_F[2048] __attribute__((aligned(16))) = {");
fprintf(fd,"%d,",STS_LTS_3(1:(end-1)));
fprintf(fd,"%d};\n\n",STS_LTS_3(end));


X2 = zeros(256,1);
X2(1:4:end) = X;
X2(4:4:end) = X; 
fprintf(fd,"int16_t LTS_F[256] __attribute__((aligned(16))) = {");
fprintf(fd,"%d,",X2(1:(end-1)));
fprintf(fd,"%d};\n",X2(end));

fclose(fd);
