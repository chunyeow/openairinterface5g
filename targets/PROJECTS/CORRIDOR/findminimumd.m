function [MSEsubsp,minind]=findminimumd(sigma2, nM, nD, Sb)

% Changes:
% Author: Thomas Zemen
% Copyright (c) by Forschungszentrum Telekommunikation Wien (ftw.)

for i=1:nD
  MSEsubsp(i)=i/nM*sigma2+1/nM*sum(Sb(i+1:nD));
end

[MSEmin,minind]=min(MSEsubsp);