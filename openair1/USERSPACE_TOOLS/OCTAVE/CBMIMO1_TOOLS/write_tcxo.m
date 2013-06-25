card = input('Input card number: ', 's'); 

filename_h = ['tcxo_val_' card '.h'];      % file to store the gain table to
      
fid = fopen(filename_h,'w');

fprintf(fid,"#ifndef _TCXO_VAL_H__\n");
fprintf(fid,"#define _TCXO_VAL_H__\n");

fprintf(fid,"#define TCXO_VAL %d\n",tcxo_freq_min);

fprintf(fid,"#endif /* _TCXO_VAL_H__ */\n");

fclose(fid);

