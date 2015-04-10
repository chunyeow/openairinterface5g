#!/usr/bin/python
# -*- coding: utf-8 -*-

# The aim of this script is to collect some traces from oai stack and generate a sequence diagram image (png or jpeg).


import sys
import subprocess
import re
import socket
import datetime
import os.path
from datetime import date

MSCGEN_OUTPUT_TYPE       = "png"
MAX_MESSAGES_PER_PAGE    = 36

MSC_NEW_STR              = '[PROTO]'
MSC_MSG_STR              = '[MESSAGE]'
MSC_BOX_STR              = '[EVENT]'


# This list is filled as follow : g_proto_names[module_id_int] = (proto_name)
g_proto_names = []



# list of messages
g_messages         = {}


# Display color of messages of sending entities
g_display_color  = ['\"teal\"',
                    '\"red\"',
                    '\"red\"',
                    '\"blue\"',
                    '\"navy\"',
                    '\"navy\"',
                    '\"navy\"',
                    '\"indigo\"',
                    '\"indigo\"',
                    '\"indigo\"',
                    '\"black\"',
                    '\"black\"',
                    '\"black\"',
                    '\"black\"',
                    '\"black\"',
                    '\"black\"',
                    '\"black\"',
                    '\"black\"',
                    '\"black\"']



g_sequence_generator = 0

def sequence_number_generator():
    global g_sequence_generator
    l_seq = g_sequence_generator
    g_sequence_generator = g_sequence_generator + 1
    return l_seq



def parse_oai_log_files():
    global g_entities_dic
    global g_entities
    global g_messages
    global g_final_display_order_list
    #open TXT file that contain OAI filtered traces for mscgen
    filenames = [
        '/tmp/openair.msc.nas_ue.log',
        '/tmp/openair.msc.pdcp_ue.log',
        '/tmp/openair.msc.rrc_ue.log',
        '/tmp/openair.msc.rlc_ue.log',
        '/tmp/openair.msc.mac_ue.log',
        '/tmp/openair.msc.phy_ue.log',
        '/tmp/openair.msc.phy_enb.log',
        '/tmp/openair.msc.mac_enb.log',
        '/tmp/openair.msc.rlc_enb.log',
        '/tmp/openair.msc.pdcp_enb.log',
        '/tmp/openair.msc.rrc_enb.log',
        '/tmp/openair.msc.s1ap_enb.log',
        '/tmp/openair.msc.gtpu_enb.log',
        '/tmp/openair.msc.nas_mme.log',
        '/tmp/openair.msc.gtpu_sgw.log',
        '/tmp/openair.msc.s1ap_mme.log']

    for filename in filenames:
        fhandle  = open(filename, 'r')
        fcontent = fhandle.read()
        fhandle.close()

        # split file content in lines
        lines = fcontent.splitlines()
        for line in lines:
            if line.strip() != "":
                print ("INPUT LINE:  %s " % line)
                partition = line.split(' ',3)
                event_id = int(partition[0])
                event_type = partition[1]
                entity_id = int(partition[2])
                if MSC_NEW_STR == event_type:
                    entity_name = partition[3]
                    if len(g_proto_names) <= entity_id:
                        for i in range(len(g_proto_names),(entity_id +1)):
                            g_proto_names.append("NotDeclared")
                    g_proto_names[entity_id] = entity_name

                # if line is a trace of a message between 2 protocol entities or layers
                elif MSC_MSG_STR == event_type:
                    print ("partition[3]:%s" % partition[3])
                    sub_partition = partition[3].split(' ',4)
                    arrow   = sub_partition[0]
                    entity2_id = int(sub_partition[1])
                    mac     = int(sub_partition[2])
                    time    = sub_partition[3]
                    message = sub_partition[4]
                    Message = {}
                    Message['mac'] = mac
                    Message['time'] = time
                    Message['message'] = message
                    Message['line_color'] = g_display_color[entity_id]
                    Message['text_color'] = g_display_color[entity_id]
                    if arrow == '<-':
                      Message['type'] = "rx"
                      Message['tx'] = entity2_id
                      Message['rx'] = entity_id
                      Message['discarded'] = False
                      g_messages[event_id] = Message
                    elif arrow == '->':
                      Message['type'] = "tx"
                      Message['tx'] = entity_id
                      Message['rx'] = entity2_id
                      Message['discarded'] = False
                      g_messages[event_id] = Message
                    elif arrow == 'x-':
                      Message['type'] = "rx"
                      Message['tx'] = entity2_id
                      Message['rx'] = entity_id
                      Message['discarded'] = True
                      g_messages[event_id] = Message
                    elif arrow == '-x':
                      Message['type'] = "tx"
                      Message['tx'] = entity_id
                      Message['rx'] = entity2_id
                      Message['discarded'] = True
                      g_messages[event_id] = Message

                elif MSC_BOX_STR == event_type:
                    message = partition[3]
                    Message = {}
                    Message['type'] = "box"
                    Message['tx'] = entity_id
                    Message['rx'] = entity_id
                    Message['discarded'] = False
                    Message['mac'] = mac
                    Message['time'] = time
                    Message['message'] = message
                    Message['line_color'] = g_display_color[entity_id]
                    Message['text_color'] = g_display_color[entity_id]
                    g_messages[event_id] = Message

  
    #print("------------------------------------")
    #print ("  %s " % ( g_messages ) )

#    for event_id_int in sorted(g_messages.iterkeys()):



def msc_chart_write_header(fileP):
    global g_final_display_order_list
    fileP.write("msc {\n")
    fileP.write("width = \"2048\";\n")

    entity_line_list_str = ''
    print ("  %s " % ( g_proto_names ) )
    for entity in g_proto_names:
        entity_line_list_str = entity_line_list_str + ' ' + entity + ','

    entity_line_list_str = entity_line_list_str.rstrip().strip(',')
    fileP.write("  %s;" % (entity_line_list_str))


def msc_chart_write_footer(fileP):
    fileP.write("\n}\n")

def msc_chart_generate(file_nameP):
    global  MSCGEN_OUTPUT_TYPE
    print "Generating sequence diagram for ",file_nameP
    command_line = "mscgen -T " + MSCGEN_OUTPUT_TYPE + " -i " + file_nameP
    fi,fo,fe=os.popen3(command_line)
    for i in fe.readlines():
        print "error:",i

def get_new_file_descriptor():
    global g_base_file_name
    global g_page_index
    l_file_name = g_base_file_name + str(g_page_index)+'.txt'
    l_file = open(l_file_name, "wb")
    return l_file


###### MAIN STAR HERE #################
parse_oai_log_files()

g_page_index    = 0
g_message_index = 0
g_now = datetime.datetime.now()
g_now_formated = g_now.strftime("%Y-%m-%d_%H.%M.%S")
g_currentdir = os.curdir
g_resultdir = os.path.join(g_currentdir, g_now_formated)
os.mkdir(g_resultdir)
os.chdir(g_resultdir)

g_base_file_name = 'oai_mscgen_page_'

g_file = get_new_file_descriptor()
msc_chart_write_header(g_file)

for event_id_int in sorted(g_messages.iterkeys()):
    message = g_messages[event_id_int]
    if 'tx' in message['type']:
        g_file.write("  %s=>%s [ label = \"(%d|%s) %s\", linecolour=%s , textcolour=%s ] ;\n" % (g_proto_names[message['tx']], g_proto_names[message['rx']], event_id_int, message['time'], message['message'], message['line_color'], message['text_color']))
    elif 'rx' in message['type']:
        g_file.write("  %s<=%s [ label = \"(%d|%s) %s\", linecolour=%s , textcolour=%s ] ;\n" % (g_proto_names[message['rx']], g_proto_names[message['tx']], event_id_int, message['time'], message['message'], message['line_color'], message['text_color']))
    elif 'box' in message['type']:
        g_file.write("  %s note %s [ label = \"%s\", textcolour=%s ] ;\n" % (g_proto_names[message['tx']], g_proto_names[message['rx']], message['message'], message['text_color']))

    g_message_index = g_message_index + 1

    if ((g_message_index % MAX_MESSAGES_PER_PAGE) == 0):
        msc_chart_write_footer(g_file)
        g_file.close()
        msc_chart_generate(g_file.name)
        g_page_index = g_page_index + 1

        g_file = get_new_file_descriptor()
        msc_chart_write_header(g_file)


msc_chart_write_footer(g_file)
g_file.close()
msc_chart_generate(g_file.name)
