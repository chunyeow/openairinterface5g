function H_dpss = dpss_smooth(H,V,Dopt,f_start)

alpha = V(f_start:4:end,1:Dopt)'*H.';
H_dpss = V(:,1:Dopt)*alpha;