function [DISTANCE] = Dist_Calc_from_GPS(LAT1, LONG1, LAT2, LONG2)
%% Distance Calculator between two GPS coordinates
% [DISTANCE] = Dist_Calc_from_GPS(LAT1, LONG1, LAT2, LONG2) 
% DISTANCE is in Kilometers. 
% LAT1 is the latitude value of first point in decimal degrees
% LONG1 is the longitude value of first point in decimal degrees
% LAT2 is the latitude value of second point in decimal degrees
% LONG2 is the longitude value of second point in decimal degrees
%
% For Example 
%
% DISTANCE = Dist_Calc_from_GPS(43.62785, 7.052418, 43.629224, 7.040884);
%
%  Date:    20-04-2010
%  Author:  Imran Latif (imran.latif@eurecom.fr)
%  Source: http://www.mathforum.com/library/drmath/view/51711.html

% all converted to radians: degree/57.29577951

   A = LAT1/57.29577951;
   B = LONG1/57.29577951;
   
   C = LAT2/57.29577951;
   D = LONG2/57.29577951;
   

   if (A == C && B == D)
       DISTANCE = 0; 
   else
       if ((sin(A) * sin(C) + cos(A) * cos(C) * cos(B-D)) > 1)
           DISTANCE = 6378 * acos(1);                    
       else
           DISTANCE = 6378 * acos(sin(A) * sin(C) + cos(A) * cos(C) * cos(B-D));
       end
   end
   
%    disp('distance in Kilometers: ');
%    DISTANCE
if isnan(DISTANCE)
    DISTANCE = 0;
end
 