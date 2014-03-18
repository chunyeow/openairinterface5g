function La = LanczosKernel(a,x0,samples)


min = floor(x0) - a;
max = ceil(x0) + a;

min(min<1)=1;
max(max>size(samples,2))=size(samples,2);

La=zeros(size(x0,2),1);
for i=1:size(x0,2)
La(i)=sinc((-x0(i)+(min(i):max(i)))).*sinc(((-x0(i)+(min(i):max(i)))/a))*samples(min(i):max(i))';
end