function [m1r, m2r, stdr] = mrice(sig, c)

q = c.^2/4./sig^2;

m1r = sqrt(pi*sig^2/2).*exp(-q).*(besseli(0, q).*(1+2*q)+besseli(1,q)*2.*q);

m2r = 2*sig.^2+c.^2;

stdr = sqrt(m2r-m1r.^2);


