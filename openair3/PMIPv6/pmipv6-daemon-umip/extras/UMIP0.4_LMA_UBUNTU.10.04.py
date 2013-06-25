#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os
import subprocess
import getopt

from subprocess  import *
from netaddr import *

g_path = os.getcwd()
g_path = os.path.dirname(sys.argv[0]) + "/.."
g_path = os.path.abspath(g_path)

try:
    opts, args = getopt.getopt(sys.argv[1:], "prd", ["pcap=", "runversion=", "pmipdir=", "cfile="])
except getopt.GetoptError, err:
    # print help information and exit:
    print str(err) # will print something like "option -a not recognized"
    sys.exit(2)

g_pcap = "no"
g_run_version = "1"
g_config_file = "example-ha-lma.conf"

for o,p in opts:
  if o in ('-p','--pcap'):
     g_pcap = p
  elif o in ['-r','--runversion']:
     g_run_version = str(p)
  elif o in ['-d','--pmipdir']:
     g_path = p
  elif o in ['-c','--cfile']:
     g_config_file = p

############################################################################################
g_file_config=g_path+"/extras/"+g_config_file
############################################################################################

print "Config file is : " + g_file_config

g_RFC5213FixedMAGLinkLocalAddressOnAllAccessLinks = IPAddress('0::0')
g_RFC5213FixedMAGLinkLayerAddressOnAllAccessLinks = " "
g_LmaAddress                                      = IPAddress('0::0')
g_LmaPmipNetworkDevice                            = ""
g_LmaCoreNetworkAddress                           = IPAddress('0::0')
g_LmaCoreNetworkDevice                            = ""
g_MagAddressIngress                               = []
g_MagAddressEgress                                = []
g_num_mags                                        = 0

g_fhandle = open(g_file_config, 'r')
g_fcontent = g_fhandle.read()
g_fhandle.close()

lines = g_fcontent.splitlines()
for line in lines:
    line = line.rstrip().lstrip()
    line = line.rstrip(';')
    split = line.split(' ')
    element = split[-1]
    element = element.strip('"')
    if line.startswith("#"):
        continue
    if 'RFC5213FixedMAGLinkLocalAddressOnAllAccessLinks' in line:
        print line
        g_RFC5213FixedMAGLinkLocalAddressOnAllAccessLinks = IPAddress(element)

    elif 'RFC5213FixedMAGLinkLayerAddressOnAllAccessLinks' in line:
        print line
        g_RFC5213FixedMAGLinkLayerAddressOnAllAccessLinks = element
    elif 'LmaPmipNetworkAddress' in line:
        print line
        g_LmaAddress = IPAddress(element)
    elif 'LmaPmipNetworkDevice' in line:
        print line
        g_LmaPmipNetworkDevice = element
    elif 'LmaCoreNetworkAddress' in line:
        print line
        g_LmaCoreNetworkAddress = IPAddress(element)
    elif 'LmaCoreNetworkDevice' in line:
        print line
        g_LmaCoreNetworkDevice = element
    elif 'MagAddressIngress' in line:
        print line
        g_MagAddressIngress.append(IPAddress(element))
    elif 'MagAddressEgress' in line:
        print line
        g_MagAddressEgress.append(IPAddress(element))

for ip in g_MagAddressIngress:
    if ip.format() != IPAddress('0::0').format():
        command = "ip -6 route del " + ip.format() + "/64"
        print command
        os.system(command)
        g_num_mags = g_num_mags + 1


for i in range (1 , 255):
    command = "ip -6 tunnel del ip6tnl" + str(i) + " >/dev/null 2>&1"
    os.system(command)

command = "ip -6 addr del " + g_LmaAddress.format() + "/64 dev " + g_LmaPmipNetworkDevice
print command
os.system(command)

command = "rmmod ip6_tunnel"
print command
os.system(command)
command = "rmmod tunnel6"
print command
os.system(command)



command = "echo \"0\" > /proc/sys/net/ipv6/conf/all/accept_ra"
print command
os.system(command)
command = "echo \"0\" > /proc/sys/net/ipv6/conf/" + g_LmaPmipNetworkDevice + "/accept_ra"
print command
os.system(command)
command = "echo \"0\" > /proc/sys/net/ipv6/conf/" + g_LmaCoreNetworkDevice + "/accept_ra"
print command
os.system(command)
command = "echo \"1\" > /proc/sys/net/ipv6/conf/all/forwarding"
print command
os.system(command)



command = "ip -6 addr add " + g_LmaAddress.format() + "/64 dev " + g_LmaPmipNetworkDevice
print command
os.system(command)
command = "ip -6 addr add " + g_LmaCoreNetworkAddress.format()+"/64 dev "+ g_LmaCoreNetworkDevice
print command
os.system(command)

index = 0
for ip_ingress in g_MagAddressIngress:
    ip_egress = g_MagAddressEgress[index]
    if ip_ingress.format() != IPAddress('0::0').format() and ip_egress.format() != IPAddress('0::0').format():
        command = "ip -6 route add " + ip_ingress.format() + "/64 via " + ip_egress.format() + " dev " + g_LmaPmipNetworkDevice
        print command
        os.system(command)
    index += 1


command = "modprobe ip6_tunnel"
print command
os.system(command)
command = "modprobe tunnel6"
print command
os.system(command)

command = "pkill -9 mip6d"
print command
os.system(command)

if g_pcap == "yes":
	command = "xhost + ; export DISPLAY=:0.0 ; sync; wireshark -i "+g_LmaPmipNetworkDevice+" -k -n -w  "+ g_path + "/logs/lma2mags."+g_run_version+".pcap &"
	value = os.system(command)
	print value

	command = "xhost + ; export DISPLAY=:0.0 ; sync; wireshark -i "+g_LmaCoreNetworkDevice+" -k -n -w  "+ g_path + "/logs/lma2cn."+g_run_version+".pcap  &"
	value = os.system(command)
	print value


# LD_LIBRARY_PATH for freeradius libs
command = 'export LD_LIBRARY_PATH=/usr/local/lib;/usr/local/sbin/mip6d -c ' + g_file_config
print command
subprocess.call(command, shell=True)

