# Clear the iptables mangle table
iptables -t mangle -F

# Remove GTPU KLM
rmmod xt_GTPUAH

# Insert the GTPUAH KLM
insmod ./Bin/xt_GTPUAH.ko

# Copy the userland iptables extenstion library
if [ -d /lib/xtables ]; then
    /bin/cp -f ./Bin/libxt_GTPUAH.so /lib/xtables/
fi

if [ -d /lib/iptables ]; then
    /bin/cp -f ./Bin/libxt_GTPUAH.so /lib/iptables/libipt_GTPUAH.so
fi

