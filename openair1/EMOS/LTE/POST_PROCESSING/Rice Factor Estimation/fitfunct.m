function err = fitfunct(lambda)
%FITFUN Used by FITDEMO.
%   FITFUN(lambda) returns the error between the data and the
%   values computed by the current function of lambda.
%   FITFUN assumes a function of the form
%
%     y =  c(1)*exp(-lambda(1)*t) + ... + c(n)*exp(-lambda(n)*t)
%
%   with n linear parameters and n nonlinear parameters.
 
%   Copyright (c) 1984-97 by The MathWorks, Inc.
%   $Revision: 5.2 $  $Date: 1997/04/08 05:33:07 $
 
%-------------------------------------------------------------------

global Data Plothandle Funct Err;
 
%-------------------------------------------------------------------
x = Data(:,1);
y = Data(:,2);

z=feval(Funct,x,lambda(1), lambda(2));

%set(Plothandle,'ydata',z)
%drawnow

err = norm(z-y);
Err=err;
