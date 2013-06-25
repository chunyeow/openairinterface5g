function [lambda,Err]=fitcurve(x,y,funct,init_lam)
% Created by: Mauro Fiacco
% Date: 25/10/97
%
% Function: Find the best coefficients for curve best fitting.
%
% Input: x: 		abscissa
%	 y: 		values
%	 funct:		name of the function
%	 init_lam: 	initial values of lambda (opt.)
%	 tol:		tollerance (opt.)
%

%-------------------------------------------------------------------

global Data Funct;
global Plothandle Err;
%-------------------------------------------------------------------
if (nargin<4 | isempty(init_lam)), init_lam = [1;0]; tol=1; 
elseif nargin<5; tol=1;
end;

Funct=funct;
Data=[x,y];

% axis auto;
% plot(x,y,'ro','EraseMode','xor');
% title('Input data')
% hold on
% 
% Plothandle = plot(x,y,'b--','LineWidth',2,'EraseMode','xor'); 

trace = 0;
lambda = fminsearch('fitfunct',init_lam,[trace tol]);


  
