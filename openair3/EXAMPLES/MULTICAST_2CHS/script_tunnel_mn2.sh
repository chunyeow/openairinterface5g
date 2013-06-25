source params.sh
ip -6 route add 2001:660::/4 dev eth0

modprobe ip6_tunnel
/opt/iproute2-2.6.26/ip/ip -6 tunnel add tunMN2toMN1 mode ipip6 remote $MN1_IN6_ADDR local $MN2_IN6_ADDR  dev eth0

/opt/iproute2-2.6.26/ip/ip link set dev tunMN2toMN1  up
sudo ip -6 addr add 4001:660:5502::25 dev tunMN2toMN1

echo "redirection of the ipv4 traffic to our tunnel"
ip route add 192.168.3.0/24 dev tunMN2toMN1

echo "1" >/proc/sys/net/ipv6/conf/all/forwarding
echo "1" >/proc/sys/net/ipv4/ip_forward

