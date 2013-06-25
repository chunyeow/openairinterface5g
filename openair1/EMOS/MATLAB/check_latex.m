function a=check_latex(a);

% Check for reserved Latex chars
%
% (c) Helmut Hofstetter, ftw
%     23.03.2003

b=findstr(a,'_');
for k=1:length(b)
    a=[a(1:b(k)-2+k),'\_',a(b(k)+k:end)];
end
