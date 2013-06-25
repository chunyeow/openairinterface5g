card = input('Input card number: ', 's'); 

filename_h = ['gain_table_' card '.h'];      % file to store the gain table to
filename_mat = ['gain_table_' card '.mat'];

save('-mat',filename_mat,'ALL_power_dBm','ALL_rxrfmode','ALL_gain2391','ALL_gain9862','min_agc_level','max_agc_level','gain_levels','SpN0','SpN1','N0','N1','S0','S1','G0','G1','NF0','NF1','SNR0','SNR1');
      
fid = fopen(filename_h,'w');

fprintf(fid,'#ifndef _GAIN_TABLE_H__\n');
fprintf(fid,'#define _GAIN_TABLE_H__\n');

fprintf(fid,'#define MAX_RF_GAIN %d\n',max_agc_level);
fprintf(fid,'#define MIN_RF_GAIN %d\n',min_agc_level);

%fprintf(fid,'#define TCXO_OFFSET\n');

fprintf(fid,'struct gain_table_entry {\n');
fprintf(fid,'  unsigned char gain0;\n');
fprintf(fid,'  unsigned char gain1;\n');
fprintf(fid,'};\n');

fprintf(fid,'struct gain_table_entry gain_table[MAX_RF_GAIN-MIN_RF_GAIN+1] = {\n');
for agc_level = 1:(max_agc_level-min_agc_level)
	fprintf(fid,'%d, %d,\n',gain_levels(agc_level,:));
end
fprintf(fid,'%d, %d};\n',gain_levels(end,:));


fprintf(fid,'#endif /* _GAIN_TABLE_H__ */\n');

fclose(fid);

