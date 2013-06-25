system(sprintf('make clean && make dump_size EMOS_CHANNEL=%d',EMOS_CHANNEL));
[status,result] = system('./dump_size');
eval(result);
