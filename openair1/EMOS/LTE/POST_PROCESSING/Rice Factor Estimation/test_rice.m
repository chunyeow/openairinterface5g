clear all
close all

%% these lines are for syntetic data
v = 10; s = 0.5; N = 20000;
 r = ricernd(v*ones(1, N), s);

K = v.^2/(2*s.^2)
rc = v + s*(randn(1,N)+1j*randn(1,N));
r1 = abs(rc).^2;
r2 = ricernd(v*ones(1, N), s).^2;
r = r1;

%% Rice factor estimation
c = linspace(floor(min(sqrt(r(:)))), ceil(max(sqrt(r(:)))), 100);
w = c(2)-c(1); % histogram bin-width
h = hist(sqrt(r), c); 
h = h./(sum(h)*w);

% estimate Rice factor using method of moments
[Ke,v2,s2] = estimate_rice(r)
epdf = ricepdf(c, sqrt(v2), sqrt(s2./2));
epdf(isnan(epdf))=0;
Err = norm(h-epdf);

% use Claude's LS estimator
init_lam = [sqrt(v2), sqrt(s2./2)];
[lambda,Err2]=fitcurve(c.',h.','ricepdf',init_lam);
Ke2 = lambda(1).^2/(2*lambda(2).^2)
epdf2 = ricepdf(c, lambda(1), lambda(2));
epdf2(isnan(epdf))=0;

%% plot results
figure(2)
hold off
bar(c, h); %, 'histc'); 
%xlim([c(1),c(end)])
hold on
plot(c, epdf, 'r','Linewidth',2);
plot(c, epdf2, 'g','Linewidth',2);
hold off
legend('Histogram','Method of Moments','LS estimation')
%legend('Histogram','Estimated pdf');
xlabel('Channel Gain (normalized by its mean)');
ylabel('Histogram')


%%
%mcdf = empirical_cdf(c,sqrt(r));
%figure(3)
%hold off
%semilogy(c,mcdf)
%hold on
%semilogy(c,cumsum(epdf)/sum(epdf),'r')
%hold off
