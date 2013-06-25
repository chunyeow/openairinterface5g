function [qq] = map_q(qqq)

if (qqq == -1)
               qq = 3;
           else
               if(qqq == 1i)
                   qq = 2;
               else
                   if(qqq == -1i)
                       qq = 1;
                   else
                       qq = 0;
                   end
               end
end

