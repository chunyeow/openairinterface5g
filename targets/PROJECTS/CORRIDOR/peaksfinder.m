function [m,ind]=peaksfinder(corr,frame_length)



threshold=max(abs(corr))*0.75;
consecutivePos=[];
highCorrVal=find(abs(corr)>threshold);
num_peak=0;
k=1;
consecutivePos(1)=highCorrVal(1);
for i_high=1:size(highCorrVal,1)-1
    if highCorrVal(i_high+1)-highCorrVal(i_high)==1 
        consecutivePos(k+1)=highCorrVal(i_high+1);
        k=k+1;
    else
        num_peak=num_peak+1;
        [m(num_peak),temp_idx]=max(abs(corr(min(consecutivePos):max(consecutivePos))));
        ind(num_peak)=min(consecutivePos)-1+temp_idx;
        consecutivePos=[];
        consecutivePos(1)=highCorrVal(i_high+1);
        k=1;
    end
end

num_peak=num_peak+1;
[m(num_peak),temp_idx]=max(abs(corr(min(consecutivePos):max(consecutivePos))));
ind(num_peak)=min(consecutivePos)-1+temp_idx;


% a bigining for make code which clculate the best peak in respect to whole frlames

% corrMatrix=vec2mat(corr, frame_length);
% corrMatrix(end,:)=[];
% sumCorrMatrix=sum(abs(corrMatrix));
% [Value Pos]=max(sumCorrMatrix);





