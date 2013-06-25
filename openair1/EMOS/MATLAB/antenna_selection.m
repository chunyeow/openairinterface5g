function [H,antenna_index] = antenna_selection(HS)

t = size(HS);
subcarr_chunk = 5; % number of subcarriers per chunk
antenna_index = zeros(t(3)/subcarr_chunk,t(5));
H = zeros(1,t(2),t(3),t(4),t(5));

if (t(1) == 2) %%%selection
    for chunk = 1:t(3)/subcarr_chunk
        for usr =1:t(5) 
            chunks = (chunk-1)*subcarr_chunk+1:chunk*subcarr_chunk;
            if (norm(squeeze(HS(1,:,chunks,:,usr)),'fro') > norm(squeeze(HS(2,:,chunks,:,usr)),'fro'))
                antenna_index(chunk,usr) = 1;
                H(:,:,chunks,:,usr) = HS(1,:,chunks,:,usr);
            else
                antenna_index(chunk,usr) = 2;
                H(:,:,chunks,:,usr) = HS(2,:,chunks,:,usr);
            end 
        end
    end
else
    %warning('[antenna selection] No antennas to select from :(');
    H = HS;
    antenna_index = [];
end


