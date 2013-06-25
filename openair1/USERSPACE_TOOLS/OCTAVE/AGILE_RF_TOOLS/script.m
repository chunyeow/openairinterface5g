clear all

size_data = 20480; % in ech
band = 7*10^6; %in Hz

Samples = zeros(size_data,1);
prefixe_in = '/tmp/'; % c'estle repertoire des fichiers .dat

signal_1 = [prefixe_in '953_4.dat'];
fid_1 = fopen(signal_1,'rb');
signal = fread(fid_1, size_data);
fclose(fid_1);
Samples(:,1) = signal;

data = (fft(Samples))';

mes_norm = sqrt(real(data).^2+imag(data).^2);

[n,m]= size(mes_norm);

decal_freq = 180*10^3; % in Hz
decal = ceil((decal_freq*size_data)/band); % in ech


for q = 1:(m - decal);
    % Parameters
    mu_norm = sum(mes_norm(q:q+decal))/(decal+1);
    sigma2_norm = sum((mes_norm(q:q+decal)-mu_norm).^2)./(decal+1);
    sigma_ray = sqrt((1/(2*(decal+1)))*sum(mes_norm(q:q+decal).^2));

    % Distributions %%%%%%%%%
    rayleigh = mes_norm(q:q+decal)./(sigma_ray^2).*exp(-(mes_norm(q:q+decal).^2)./(2*(sigma_ray^2)));
    aic_matrix (1,q) = sum(log10(rayleigh))+2;
    nr = exp(-(1/2)*(mes_norm(q:q+decal)-mu_norm).^2./(sigma2_norm))./(sqrt(sigma2_norm*2*pi));
    aic_matrix (2,q) = sum(log10(nr))+8;
end


min_vect = min(aic_matrix(:,:));

for i=1:m-decal,
    D(1,i) = aic_matrix(1,i) - min_vect(i);
    D(2,i) = aic_matrix(2,i) - min_vect(i);

    den = sum(exp(-0.5*D(:,i)));
    w_ray(i) = exp(-0.5*D(1,i))/den;
    w_nr(i) = exp(-0.5*D(2,i))/den;
end

for i= 1:ceil(decal/2),
    bande(i) = 0;
end

for i=ceil(decal/2) + 1:length(w_ray)+ceil(decal/2),
    if w_ray(i-ceil(decal/2)) < 0.009,
        bande(i) = 1;
    else
        bande(i) = 0;
    end
end

for i= length(w_ray)+ceil(decal/2)+1:length(w_ray)+decal,
    bande(i) = 0;
end

bande_1 = zeros(1,length(bande));

for i=1:decal:length(bande)-decal,
    nb_zeros = 0;
    for k = 1:decal,
        if (bande(i+k-1)==0),
            nb_zeros = nb_zeros+1;
        end
    end
    if (nb_zeros > decal/2),
        for k = 1:decal,
            bande_1(i+k-1)=0;
        end
    else
        for k = 1:decal,
            bande_1(i+k-1)=1;
        end
    end
end

for i=1:length(bande)/2-72,
    bande_1(length(bande) - i - 1) = bande_1(i + 72);
end

for i=1:length(bande),
    ban(i) = 0;
end

for i=1:decal:length(bande),
    ban(i) = 1;
end

if 1,
    size_data = size_data - decal;
    fr = -band/2:band/size_data:band/2-band/size_data;
    figure(1),
    subplot(2,1,1),plot(fr,w_ray,'k.'), axis([-band/2, band/2, 0, 1])
    title('')
    size_data = 20480;
    band = 7*10^6; %in kHz
    fr = -band/2:band/size_data:band/2-band/size_data;
    subplot(2,1,2),plot(fr, real(data(1,:)), 'k-'), axis([-band/2, band/2, -150000, 150000])
    subplot(2,1,2),hold on, plot(fr, 10^5.*ban,'k--'), plot(fr, 10^5.*bande_1,'r')
    subplot(2,1,1),hold on, plot(fr, ban,'k--'),
    title('')
end