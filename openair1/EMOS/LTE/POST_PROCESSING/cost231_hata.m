function PL = cost231_hata(d)
%  PL = cost231-hata(d)
% gives the PL for distance d (in km) for the cost231_hata model 
%
% E Damosso, LM Correia “Digital Mobile Radio Towards Future Generation 
% Systems,” COST 231 Final Report, European Commission, 1999


hMobile = 1.5;
hBase = 30;
f1 = 800; %MHz

%% COST231-Hata
a = (1.1*log10(f1) - 0.7)*hMobile - (1.56*log10(f1) - 0.8);
Qr = 4.78*log10(f1)^2 - 18.33*log10(f1) +40.94;
PL = 69.55 + 26.16*log10(f1) -13.82*log10(hBase) - a + (44.9 - 6.55*log10(hBase))*log10(d) - Qr;