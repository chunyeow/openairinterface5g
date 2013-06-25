
k=0;
for i=0:5,
  for n=0:(2^i)-1,

      twiddleFFT_64(1+(4*k))  = round(16384*cos(2*pi*n/(2^(i+1))));
      twiddleIFFT_64(1+(4*k))  = twiddleFFT_64(1+(4*k));

      twiddleFFT_64(2+(4*k))  = round(16384*sin(2*pi*n/(2^(i+1))));
      twiddleIFFT_64(2+(4*k))  = -twiddleFFT_64(2+(4*k));

      twiddleFFT_64(3+(4*k))  = -twiddleFFT_64(2+(4*k));
      twiddleIFFT_64(3+(4*k))  = twiddleFFT_64(2+(4*k));

      twiddleFFT_64(4+(4*k))  = twiddleFFT_64(1+(4*k));
      twiddleIFFT_64(4+(4*k))  = twiddleFFT_64(1+(4*k)); 

      k=k+1;
  end
end

[fid, msg] = fopen("twiddle64.h","w","ieee-le");

fprintf(fid,"short twiddle_fft64[63*4] = {");
fprintf(fid,"%d,%d,%d,%d,\n",twiddleFFT_64(1:248));
fprintf(fid,"%d,%d,%d,%d};\n\n",twiddleFFT_64(249:252));
fprintf(fid,"short twiddle_ifft64[63*4] = {");
fprintf(fid,"%d,%d,%d,%d,\n",twiddleIFFT_64(1:248));
fprintf(fid,"%d,%d,%d,%d};\n\n",twiddleIFFT_64(249:252));

fclose(fid);


k=0;
for i=0:6,
  for n=0:(2^i)-1,

      twiddleFFT_128(1+(4*k))  = round(16384*cos(2*pi*n/(2^(i+1))));
      twiddleIFFT_128(1+(4*k))  = twiddleFFT_128(1+(4*k));

      twiddleFFT_128(2+(4*k))  = round(16384*sin(2*pi*n/(2^(i+1))));
      twiddleIFFT_128(2+(4*k))  = -twiddleFFT_128(2+(4*k));

      twiddleFFT_128(3+(4*k))  = -twiddleFFT_128(2+(4*k));
      twiddleIFFT_128(3+(4*k))  = twiddleFFT_128(2+(4*k));

      twiddleFFT_128(4+(4*k))  = twiddleFFT_128(1+(4*k));
      twiddleIFFT_128(4+(4*k))  = twiddleFFT_128(1+(4*k));

      k=k+1;
  end
end

[fid, msg] = fopen("twiddle128.h","w","ieee-le");

fprintf(fid,"short twiddle_fft128[127*4] = {");
fprintf(fid,"%d,%d,%d,%d,\n",twiddleFFT_128(1:504));
fprintf(fid,"%d,%d,%d,%d};\n\n",twiddleFFT_128(505:508));

fprintf(fid,"short twiddle_ifft128[127*4] = {");
fprintf(fid,"%d,%d,%d,%d,\n",twiddleIFFT_128(1:504));
fprintf(fid,"%d,%d,%d,%d};\n\n",twiddleIFFT_128(505:508));
fclose(fid);
k=0;

for i=0:7,
  for n=0:(2^i)-1,

      twiddleFFT_256(1+(4*k))  = round(16384*cos(2*pi*n/(2^(i+1))));
      twiddleIFFT_256(1+(4*k))  = twiddleFFT_256(1+(4*k));

      twiddleFFT_256(2+(4*k))  = round(16384*sin(2*pi*n/(2^(i+1))));
      twiddleIFFT_256(2+(4*k))  = -twiddleFFT_256(2+(4*k));

      twiddleFFT_256(3+(4*k))  = -twiddleFFT_256(2+(4*k));
      twiddleIFFT_256(3+(4*k))  = twiddleFFT_256(2+(4*k));

      twiddleFFT_256(4+(4*k))  = twiddleFFT_256(1+(4*k));
      twiddleIFFT_256(4+(4*k))  = twiddleFFT_256(1+(4*k));



      k=k+1;
  end
end

[fid, msg] = fopen("twiddle256.h","w","ieee-le");

      fprintf(fid,"short twiddle_fft256[255*4] = {");
      fprintf(fid,"%d,%d,%d,%d,\n",twiddleFFT_256(1:1016));
      fprintf(fid,"%d,%d,%d,%d};\n\n",twiddleFFT_256(1017:1020));

      fprintf(fid,"short twiddle_ifft256[255*4] = {");
      fprintf(fid,"%d,%d,%d,%d,\n",twiddleIFFT_256(1:1016));
      fprintf(fid,"%d,%d,%d,%d};\n\n",twiddleIFFT_256(1017:1020));

fclose(fid)

k=0;

for i=0:8,
  for n=0:(2^i)-1,

      twiddleFFT_512(1+(4*k))  = round(16384*cos(2*pi*n/(2^(i+1))));
      twiddleIFFT_512(1+(4*k))  = twiddleFFT_512(1+(4*k));

      twiddleFFT_512(2+(4*k))  = round(16384*sin(2*pi*n/(2^(i+1))));
      twiddleIFFT_512(2+(4*k))  = -twiddleFFT_512(2+(4*k));

      twiddleFFT_512(3+(4*k))  = -twiddleFFT_512(2+(4*k));
      twiddleIFFT_512(3+(4*k))  = twiddleFFT_512(2+(4*k));

      twiddleFFT_512(4+(4*k))  = twiddleFFT_512(1+(4*k));
      twiddleIFFT_512(4+(4*k))  = twiddleFFT_512(1+(4*k));



      k=k+1;
  end
end

[fid, msg] = fopen("twiddle512.h","w","ieee-le");

      fprintf(fid,"short twiddle_fft512[511*4] = {");
      fprintf(fid,"%d,%d,%d,%d,\n",twiddleFFT_512(1:2040));
      fprintf(fid,"%d,%d,%d,%d};\n\n",twiddleFFT_512(2041:2044));

      fprintf(fid,"short twiddle_ifft512[511*4] = {");
      fprintf(fid,"%d,%d,%d,%d,\n",twiddleIFFT_512(1:2040));
      fprintf(fid,"%d,%d,%d,%d};\n\n",twiddleIFFT_512(2041:2044));


fclose(fid)

k=0;

for i=0:9,
  for n=0:(2^i)-1,

      twiddleFFT_1024(1+(4*k))  = fix(16384*cos(2*pi*n/(2^(i+1))));
      twiddleIFFT_1024(1+(4*k))  = twiddleFFT_1024(1+(4*k));

      twiddleFFT_1024(2+(4*k))  = fix(16384*sin(2*pi*n/(2^(i+1))));
      twiddleIFFT_1024(2+(4*k))  = -twiddleFFT_1024(2+(4*k));

      twiddleFFT_1024(3+(4*k))  = -twiddleFFT_1024(2+(4*k));
      twiddleIFFT_1024(3+(4*k))  = twiddleFFT_1024(2+(4*k));

      twiddleFFT_1024(4+(4*k))  = twiddleFFT_1024(1+(4*k));
      twiddleIFFT_1024(4+(4*k))  = twiddleFFT_1024(1+(4*k));



      k=k+1;
  end
end

[fid, msg] = fopen("twiddle1024.h","w","ieee-le");

fprintf(fid,"short twiddle_fft1024[1023*4] = {");
fprintf(fid,"%d,%d,%d,%d,\n",twiddleFFT_1024(1:4088));
fprintf(fid,"%d,%d,%d,%d};\n\n",twiddleFFT_1024(4089:4092));

fprintf(fid,"short twiddle_ifft1024[1023*4] = {");
fprintf(fid,"%d,%d,%d,%d,\n",twiddleIFFT_1024(1:4088));
fprintf(fid,"%d,%d,%d,%d};\n\n",twiddleIFFT_1024(4089:4092));


%fprintf(fid,"unsigned short rev1024[1024];\n")
fclose(fid)


k=0;

for i=0:10,
  for n=0:(2^i)-1,

      twiddleFFT_2048(1+(4*k))  = fix(16384*cos(2*pi*n/(2^(i+1))));
      twiddleIFFT_2048(1+(4*k))  = twiddleFFT_2048(1+(4*k));

      twiddleFFT_2048(2+(4*k))  = fix(16384*sin(2*pi*n/(2^(i+1))));
      twiddleIFFT_2048(2+(4*k))  = -twiddleFFT_2048(2+(4*k));

      twiddleFFT_2048(3+(4*k))  = -twiddleFFT_2048(2+(4*k));
      twiddleIFFT_2048(3+(4*k))  = twiddleFFT_2048(2+(4*k));

      twiddleFFT_2048(4+(4*k))  = twiddleFFT_2048(1+(4*k));
      twiddleIFFT_2048(4+(4*k))  = twiddleFFT_2048(1+(4*k));



      k=k+1;
  end
end

[fid, msg] = fopen("twiddle2048.h","w","ieee-le");

      fprintf(fid,"short twiddle_fft2048[2047*4] = {");
      fprintf(fid,"%d,%d,%d,%d,\n",twiddleFFT_2048(1:8184));
      fprintf(fid,"%d,%d,%d,%d};\n\n",twiddleFFT_2048(8185:8188));

      fprintf(fid,"short twiddle_ifft2048[2047*4] = {");
      fprintf(fid,"%d,%d,%d,%d,\n",twiddleIFFT_2048(1:8184));
      fprintf(fid,"%d,%d,%d,%d};\n\n",twiddleIFFT_2048(8185:8188));




fclose(fid)

k=0;

for i=0:11,
  for n=0:(2^i)-1,

      twiddleFFT_4096(1+(4*k))  = fix(16384*cos(2*pi*n/(2^(i+1))));
      twiddleIFFT_4096(1+(4*k))  = twiddleFFT_4096(1+(4*k));

      twiddleFFT_4096(2+(4*k))  = fix(16384*sin(2*pi*n/(2^(i+1))));
      twiddleIFFT_4096(2+(4*k))  = -twiddleFFT_4096(2+(4*k));

      twiddleFFT_4096(3+(4*k))  = -twiddleFFT_4096(2+(4*k));
      twiddleIFFT_4096(3+(4*k))  = twiddleFFT_4096(2+(4*k));

      twiddleFFT_4096(4+(4*k))  = twiddleFFT_4096(1+(4*k));
      twiddleIFFT_4096(4+(4*k))  = twiddleFFT_4096(1+(4*k));



      k=k+1;
  end
end

[fid, msg] = fopen("twiddle4096.h","w","ieee-le");

      fprintf(fid,"short twiddle_fft4096[4095*4] = {");
      fprintf(fid,"%d,%d,%d,%d,\n",twiddleFFT_4096(1:16376));
      fprintf(fid,"%d,%d,%d,%d};\n\n",twiddleFFT_4096(16377:16380));

      fprintf(fid,"short twiddle_ifft4096[4095*4] = {");
      fprintf(fid,"%d,%d,%d,%d,\n",twiddleIFFT_4096(1:16376));
      fprintf(fid,"%d,%d,%d,%d};\n\n",twiddleIFFT_4096(16377:16380));
      



%fprintf(fid,"unsigned short rev4096[4096];\n")
fclose(fid)

k=0;

for i=0:12,
  for n=0:(2^i)-1,

      twiddleFFT_8192(1+(4*k))  = fix(16384*cos(2*pi*n/(2^(i+1))));
      twiddleIFFT_8192(1+(4*k))  = twiddleFFT_8192(1+(4*k));

      twiddleFFT_8192(2+(4*k))  = fix(16384*sin(2*pi*n/(2^(i+1))));
      twiddleIFFT_8192(2+(4*k))  = -twiddleFFT_8192(2+(4*k));

      twiddleFFT_8192(3+(4*k))  = -twiddleFFT_8192(2+(4*k));
      twiddleIFFT_8192(3+(4*k))  = twiddleFFT_8192(2+(4*k));

      twiddleFFT_8192(4+(4*k))  = twiddleFFT_8192(1+(4*k));
      twiddleIFFT_8192(4+(4*k))  = twiddleFFT_8192(1+(4*k));



      k=k+1;
  end
end

[fid, msg] = fopen("twiddle8192.h","w","ieee-le");

      fprintf(fid,"short twiddle_fft8192[8191*4] = {");
      fprintf(fid,"%d,%d,%d,%d,\n",twiddleFFT_8192(1:16376));
      fprintf(fid,"%d,%d,%d,%d};\n\n",twiddleFFT_8192(16377:16380));

      fprintf(fid,"short twiddle_ifft8192[8191*4] = {");
      fprintf(fid,"%d,%d,%d,%d,\n",twiddleIFFT_8192(1:16376));
      fprintf(fid,"%d,%d,%d,%d};\n\n",twiddleIFFT_8192(16377:16380));
      



%fprintf(fid,"unsigned short rev8192[8192];\n")
fclose(fid)
