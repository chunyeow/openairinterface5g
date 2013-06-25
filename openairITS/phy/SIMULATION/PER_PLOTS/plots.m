SNR_0_256
errors_0_256
trials_0_256
signal_errors_0_256

SNR_1_256
errors_1_256
trials_1_256

SNR_2_256
errors_2_256
trials_2_256

SNR_3_256
errors_3_256
trials_3_256

SNR_4_256
errors_4_256
trials_4_256

SNR_5_256
errors_5_256
trials_5_256

SNR_0_1024
errors_0_1024
trials_0_1024
signal_errors_0_1024

SNR_1_1024
errors_1_1024
trials_1_1024

SNR_2_1024
errors_2_1024
trials_2_1024

SNR_3_1024
errors_3_1024
trials_3_1024

SNR_4_1024
errors_4_1024
trials_4_1024

SNR_5_1024
errors_5_1024
trials_5_1024

Pe_signal = signal_errors_0_256_v./trials_0_256_v;
Pe_data_0   = errors_0_256_v./trials_0_256_v;
Pe_data_1   = errors_1_256_v./trials_1_256_v;
Pe_data_2   = errors_2_256_v./trials_2_256_v;
Pe_data_3   = errors_3_256_v./trials_3_256_v;
Pe_data_4   = errors_4_256_v./trials_4_256_v;
Pe_data_5   = errors_5_256_v./trials_5_256_v;

Pe_data_0_1024   = errors_0_1024_v./trials_0_1024_v;
Pe_data_1_1024   = errors_1_1024_v./trials_1_1024_v;
Pe_data_2_1024   = errors_2_1024_v./trials_2_1024_v;
Pe_data_3_1024   = errors_3_1024_v./trials_3_1024_v;
Pe_data_4_1024   = errors_4_1024_v./trials_4_1024_v;
Pe_data_5_1024   = errors_5_1024_v./trials_5_1024_v;

semilogy(SNR_0_256_v,Pe_signal,'b',SNR_0_256_v,Pe_data_0,'r',SNR_0_1024_v,Pe_data_0_1024,'k',SNR_1_256_v,Pe_data_1,'r',SNR_2_256_v,Pe_data_2,'r',SNR_3_256_v,Pe_data_3,'r',SNR_4_256_v,Pe_data_4,'r',SNR_5_256_v,Pe_data_5,'r',SNR_1_1024_v,Pe_data_1_1024,'k',SNR_2_1024_v,Pe_data_2_1024,'k',SNR_3_1024_v,Pe_data_3_1024,'k',SNR_4_1024_v,Pe_data_4_1024,'k',SNR_5_1024_v,Pe_data_5_1024,'k');

axis([1 20 5e-3 .5])
grid
legend('SIGNAL','256 byte SDU','1024 byte SDU');
title('openairITS Block Error Rates for SIGNAL, 802.11p rates 0-5, for SDU lengths 256 and 1024 bytes')
xlabel('SNR dB')
ylabel('PER')
