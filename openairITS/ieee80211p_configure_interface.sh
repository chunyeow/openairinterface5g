## INSTRUCTIONS
## - check the coherence of the OPENAIRITS_DIR and MOD_DIR with YOUR platform
## - configure the module dependencies by running the following command:
##       sudo depmod -a 
## - install 'iw' by the following command:
##      sudo apt-get install iw
## - allow Ubuntu to reply to a PING in Broadcast by the following command:
##      echo 0 | sudo tee /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
## run the following shell
## voila !!

# Interface configuration (interface type, MAC address, IP address, disable ARP)
sudo iw phy phy0 interface add wlan0 type ibss 4addr off
sudo ifconfig wlan0 hw ether 10:11:12:13:14:15
sudo ifconfig wlan0 192.168.1.1 up -arp

# Static ARP table
sudo arp -i wlan0 -s 192.168.1.2 10:21:22:23:24:25
sudo arp -i wlan0 -s 192.168.1.255 FF:FF:FF:FF:FF:FF

