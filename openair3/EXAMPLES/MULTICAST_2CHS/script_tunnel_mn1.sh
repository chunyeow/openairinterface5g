source params.sh


ip -6 route add 2001:660::/4 dev eth0

modprobe ip6_tunnel
ip -6 tunnel add tunMN1toMN2 mode ipip6 remote $MN2_IN6_ADDR local $MN1_IN6_ADDR dev eth0
ip link set dev tunMN1toMN2  up
sudo ip -6 addr add 4001:660:5502::20 dev tunMN1toMN2

echo "redirection of the ipv4 traffic to the tunMN1toMN2 tunnel" 
ip route add 192.168.1.0/24 dev tunMN1toMN2
echo "1" >/proc/sys/net/ipv6/conf/all/forwarding
echo "1" >/proc/sys/net/ipv4/ip_forward

