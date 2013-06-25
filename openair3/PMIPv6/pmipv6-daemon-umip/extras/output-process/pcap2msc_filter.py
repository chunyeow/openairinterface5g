#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import re
import datetime

g_echo_reply_last_seq   = -2
g_echo_request_last_seq = -2

g_buffer = []

g_icmp_buffer = {}
g_icmp_buffer_count = {}
#g_icmp_buffer_first_seq = -2

def filter_write(lines):
	global g_fwhandle
	for line in lines:
		g_fwhandle.write(line+'\n')


def flush_icmp_buffer():
	global g_icmp_buffer
	global g_icmp_buffer_count
	global g_fwhandle

	print >>sys.stderr, "g_icmp_buffer: " , g_icmp_buffer
	print >>sys.stderr, "g_icmp_buffer_count: " , g_icmp_buffer_count
	last_written_seq = -2
	item_index = 0
	num_keys = len(g_icmp_buffer)
	for key_seq in sorted(g_icmp_buffer.iterkeys()):
		item_index = item_index + 1
		lines      =  g_icmp_buffer[key_seq]
		icmp_count =  g_icmp_buffer_count[key_seq]
		if last_written_seq < 0:
			last_written_seq = key_seq
			filter_write(lines)
		elif item_index == num_keys:
			last_written_seq = key_seq
			filter_write(lines)
		elif icmp_count != 0:
			last_written_seq = key_seq
			filter_write(lines)
		else:
			if (last_written_seq + 1) == key_seq:
				g_fwhandle.write('  ---  [ label = "ICMP echo traffic"];\n')
	g_icmp_buffer       = {}
	g_icmp_buffer_count = {}
			

g_fwhandle = open(sys.argv[1]+'.icmp_filtered', 'w')
g_fhandle = open(sys.argv[1], 'r')
g_fcontent = g_fhandle.read()
g_fhandle.close()
# split file content in lines
lines = g_fcontent.splitlines()
for line in lines:
	if 'ICMPv6 Echo reply seq ' in line:
		message_string = line.split('\"')[1]
		g_echo_reply_last_seq = int(message_string.split(' ')[-1])
		sys.stdout.write("g_echo_reply_last_seq   = %d\n" % (g_echo_reply_last_seq))

		# buffer line
		if g_echo_reply_last_seq not in g_icmp_buffer:
			g_icmp_buffer[g_echo_reply_last_seq] = []
		g_icmp_buffer[g_echo_reply_last_seq].append(line)

		# count num echo req, num echo reply
		if g_echo_reply_last_seq not in g_icmp_buffer_count:
			g_icmp_buffer_count[g_echo_reply_last_seq] = -1
			flush_icmp_buffer()
		else:
			g_icmp_buffer_count[g_echo_reply_last_seq] = g_icmp_buffer_count[g_echo_reply_last_seq] - 1


		if g_echo_reply_last_seq > g_echo_request_last_seq:
			flush_icmp_buffer()

	elif 'ICMPv6 Echo request seq ' in line:
		message_string = line.split('\"')[1]
		g_echo_request_last_seq = int(message_string.split(' ')[-1])
		sys.stdout.write("g_echo_request_last_seq = %d\n" % (g_echo_request_last_seq))

		if g_echo_request_last_seq not in g_icmp_buffer:
			g_icmp_buffer[g_echo_request_last_seq] = []
		g_icmp_buffer[g_echo_request_last_seq].append(line)

		# count num echo req, num echo reply
		if g_echo_request_last_seq not in g_icmp_buffer_count:
			g_icmp_buffer_count[g_echo_request_last_seq] = 1
		else:
			g_icmp_buffer_count[g_echo_request_last_seq] = g_icmp_buffer_count[g_echo_request_last_seq] + 1


		if g_echo_reply_last_seq >= g_echo_request_last_seq:
			flush_icmp_buffer()
	else:
		flush_icmp_buffer()
		g_fwhandle.write(line+'\n')
           
g_fwhandle.close()
