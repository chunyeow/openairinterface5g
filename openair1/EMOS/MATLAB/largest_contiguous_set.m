function [max_start, max_len] = largest_contiguous_set(array,x)

if nargin==1 
    x=1;
end

max_start = 1;
max_len = 0;
start = 1;
len = 0;
for i=2:length(array)
    if (array(i)-array(i-1)<=x) %we have a contigous region (allow for holes of x)
        if len==0 % it is the start of the region
            start = i-1;
        end
        len = len+1;
        if len>max_len % is it longer than the one we have?
            max_start = start;
            max_len = len;
        end
    else % reset counter
        start = 1;
        len = 0;
    end
end