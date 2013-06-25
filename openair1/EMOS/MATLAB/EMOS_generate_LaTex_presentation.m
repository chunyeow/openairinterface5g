filename_rssi = sprintf('figs/idx_%d_rx_rssi',idx);
filename_gps = sprintf('figs/idx_%d_map_rx_rssi',idx);
filename_cap = sprintf('figs/idx_%d_capacity',idx);
filename_cap2 = sprintf('figs/idx_%d_capacity_AS',idx);
filename_pdp = sprintf('figs/idx_%d_Dprofile',idx);
filename_align = sprintf('figs/idx_%d_align',idx);
filename_MUgains = sprintf('figs/idx_%d_MUgains',idx);

%    system(sprintf('epstopdf --outfile=%s %s',filename_cap_pdf,filename_cap));


fprintf(fdp,'\\frame {\n');
fprintf(fdp,'  \\frametitle{Measurement %d}\n', idx);
fprintf(fdp,'  \\includegraphics[height=8cm]{%s}\n',fullfile(pwd,filename_gps));
fprintf(fdp,'}\n');

fprintf(fdp,'\\frame {\n');
fprintf(fdp,'  \\frametitle{Measurement %d}\n', idx);
fprintf(fdp,'  \\includegraphics[height=8cm]{%s}\n',fullfile(pwd,filename_pdp));
fprintf(fdp,'}\n');

fprintf(fdp,'\\frame {\n');
fprintf(fdp,'  \\frametitle{Measurement %d}\n', idx);
fprintf(fdp,'  \\includegraphics[height=8cm]{%s}\n',fullfile(pwd,filename_cap));
fprintf(fdp,'}\n');

fprintf(fdp,'\\frame {\n');
fprintf(fdp,'  \\frametitle{Measurement %d}\n', idx);
fprintf(fdp,'  \\includegraphics[height=8cm]{%s}\n',fullfile(pwd,filename_cap2));
fprintf(fdp,'}\n');

fprintf(fdp,'\\frame {\n');
fprintf(fdp,'  \\frametitle{Measurement %d}\n', idx);
fprintf(fdp,'  \\includegraphics[height=8cm]{%s}\n',fullfile(pwd,filename_MUgains));
fprintf(fdp,'}\n');

