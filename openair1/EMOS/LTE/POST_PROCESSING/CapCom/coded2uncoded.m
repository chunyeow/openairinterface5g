function uncoded_rps = coded2uncoded(coded_rps,direction)
%   uncoded_rps = coded2uncoded(coded_rps);
%
% This function computes the uncoded rate per second for a coded rate. It
% basically caps the rate to the one given in the table below.
%
% The maximum uncoded throughput is derived as follows. Each DL subframe
% contains 3600 REs out of which 400 REs are used for pilots and 800 for
% the PDCCH, leaving 2400 REs for the DLSCH. Each UL subframe has 3600 RE,
% out of which 600 are used for pilots and 300 for the SRS, leving 2700 REs
% for the ULSCH. Assuming TDD frame configuration 3 (6 DL subframes, 3 UL
% subframes, 1 special subframe), this results in a maximum throughput of
% 
%           DL          UL
% QPSK: 	2.88 Mbps	1.62 Mbps
% 16QAM: 	5.76 Mbps	3.24 Mbps
% 64QAM: 	8.64 Mbps	4.86 Mbps

uncoded_rps = coded_rps;

switch (direction)
    case 'DL'
       uncoded_rps(coded_rps>0 & coded_rps<=2.88e6) = 2.88e6;
       uncoded_rps(coded_rps>2.88e6 & coded_rps<=5.76e6) = 5.76e6;
       uncoded_rps(coded_rps>5.76e6) = 8.64e6;
    case 'UL'
       uncoded_rps(coded_rps>0 & coded_rps<=1.62e6) = 1.62e6;
       uncoded_rps(coded_rps>1.62e6 & coded_rps<=3.24e6) = 3.24e6;
       uncoded_rps(coded_rps>3.24e6) = 4.86e6;
    otherwise
        error('Unknown direction')
end
        
        