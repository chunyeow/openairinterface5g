function power = IWF_4users(s,totalenergy)

j=length(s);
if nargin ==1
    totalenergy = 1;
end

betas = 1./s;
[betas_sort,index] = sort(betas);

mu_star = 0;
for k=j:-1:1
    mu = (totalenergy+sum(betas_sort(1:k)))/k;

    if ((mu - betas_sort(k))>0)
        mu_star = mu;
        break
    end
end 
    

power = max(mu_star - betas,0);


%(sum(power)>(totalenergy+0.000000000001))
