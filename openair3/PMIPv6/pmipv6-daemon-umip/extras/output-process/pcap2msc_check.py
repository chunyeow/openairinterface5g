#!/usr/bin/python
# -*- coding: utf-8 -*-
# Program pcap2msc_check.py

import sys
import re
import datetime

g_echo_reply_last_seq   = -1
g_echo_request_last_seq = -1
g_line_number           =  0

fhandle = open(sys.argv[1], 'r')
fcontent = fhandle.read()
fhandle.close()
# split file content in lines
lines = fcontent.splitlines()
for line in lines:
	g_line_number = g_line_number + 1
	if 'ICMPv6 Echo reply seq ' in line:
		message_string = line.split('\"')[1]
		g_echo_reply_last_seq = int(message_string.split(' ')[-1])
		#sys.stdout.write("g_echo_reply_last_seq   = %d\n" % (g_echo_reply_last_seq))
		if g_echo_reply_last_seq > g_echo_request_last_seq:
			sys.stdout.write("Line %d: %s\n" % (g_line_number, line))
			sys.stdout.write("ERROR found ECHO REPLY SEQ %d AFTER ECHO REQUEST SEQ %d\n" % (g_echo_reply_last_seq, g_echo_request_last_seq))
			s = raw_input('Press any key to exit...')
			sys.exit(-1)
	if 'ICMPv6 Echo request seq ' in line:
		message_string = line.split('\"')[1]
		g_echo_request_last_seq = int(message_string.split(' ')[-1])
		#sys.stdout.write("g_echo_request_last_seq = %d\n" % (g_echo_request_last_seq))
		if g_echo_reply_last_seq >= g_echo_request_last_seq:
			sys.stdout.write("Line %d: %s\n" % (g_line_number, line))
			sys.stdout.write("ERROR found ECHO REQUEST SEQ %d AFTER ECHO REPLY SEQ %d\n" % (g_echo_request_last_seq, g_echo_reply_last_seq))
			s = raw_input('Press any key to exit...')
			sys.exit(-1)

sys.stdout.write("File %s is OK\n" % sys.argv[1])


