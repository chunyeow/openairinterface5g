# Clear the iptables mangle table
iptables -t mangle -F

# Remove GTPU KLM
rmmod xt_GTPU

# Insert the GTPU KLM
insmod ./Bin/xt_GTPU.ko

# Copy the userland iptables extenstion library
if [ -d /lib/xtables ]; then
    /bin/cp -f ./Bin/libxt_GTPU.so /lib/xtables/
fi

if [ -d /lib/iptables ]; then
    /bin/cp -f ./Bin/libxt_GTPU.so /lib/iptables/libipt_GTPU.so
fi

# Some sample commands for demonstration
iptables -t mangle -A PREROUTING -d 10.10.10.1 -j GTPU --own-ip 192.168.0.98 --own-tun 100 --peer-ip 192.168.0.109 --peer-tun 101 --action add
iptables -t mangle -A PREROUTING -s 192.168.0.109 -d 192.168.0.98 -p udp --dport 2152 -j GTPU --action remove
