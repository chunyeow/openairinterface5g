function[x]=basictls(A,B);

%TLS de base voir Overview...
%[U D V]=svd([A B]);
%m=size(A,1);d=size(B,2);n=size(A,2);

%Vdd=V((n+1):(n+d),(n+1):(n+d));
%if det(Vdd)~=0 %non singular
   % x=-V(1:size(A,2),(size(A,2)+1):(n+d)) * inv(Vdd);
%    x=-V(1:n,(n+1):(n+d)) / Vdd;
%else
%     fprintf('\n*******Pas de solution, matrice singuliere Vdd********\n')
%end

 px =[A B]' * [A B];
 ar=real(px(1,1));
 ai=imag(px(1,1));
 br=real(px(1,2));
 bi=imag(px(1,2));
 cr=real(px(2,1));
 ci=imag(px(2,1));
 dr=real(px(2,2));
 di=imag(px(2,2));
 
 if((ar-dr+sqrt((ar-dr)*(ar-dr)+((br*br+bi*bi)*4)))~=0)
     x=2*br/(ar-dr+sqrt((ar-dr)*(ar-dr)+((br*br+bi*bi)*4))) + 2i*bi/(ar-dr+sqrt((ar-dr)*(ar-dr)+((br*br+bi*bi)*4)));
 else
     x=0+1i*0;
 end
