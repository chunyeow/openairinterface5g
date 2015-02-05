#******************************************************************************

#  Eurecom OpenAirInterface
#  Copyright(c) 1999 - 2013 Eurecom

#  This program is free software; you can redistribute it and/or modify it
#  under the terms and conditions of the GNU General Public License,
#  version 2, as published by the Free Software Foundation.

#  This program is distributed in the hope it will be useful, but WITHOUT
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
#  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
#  more details.

#  You should have received a copy of the GNU General Public License along with
#  this program; if not, write to the Free Software Foundation, Inc.,
#  51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.

#  The full GNU General Public License is included in this distribution in
#  the file called "COPYING".

#  Contact Information
#  Openair Admin: openair_admin@eurecom.fr
#  Openair Tech : openair_tech@eurecom.fr
#  Forums       : http://forums.eurecom.fsr/openairinterface
#  Address      : Eurecom, Compus SophiaTech 450, route des chappes, 06451 Biot, France

#*****************************************************************************

# \file test01.py
# \brief test 02 for OAI: downlink and uplink performance and profiler
# \author Navid Nikaein
# \date 2014
# \version 0.1
# @ingroup _test


import sys
import wave
import os
import time
import datetime
import getpass
import math #from time import clock 

import log
import case11
import case12
import case13


from  openair import *

debug = 0
prompt = '$'
pw =''
i = 0
clean = 0 
start_case  = 0
cpu = -1 
localshell=0

for arg in sys.argv:
    if arg == '-d':
        debug = 1
    elif arg == '-dd':
        debug = 2
    elif arg == '-p' :
        prompt = sys.argv[i+1]
    elif arg == '-w' :
        pw = sys.argv[i+1]
    elif arg == '-c' :
        clean = 1
    elif arg == '-t' :
        cpu = sys.argv[i+1]
    elif arg == '-s' :
        start_case = sys.argv[i+1]
    elif arg == '-l' :
        localshell = 1
    elif arg == '-h' :
        print "-d:  low debug level"
        print "-dd: high debug level"
        print "-p:  set the prompt"
        print "-w:  set the password for ssh to localhost"
        print "-c: clean the log directory " 
        print "-t: set the cpu "
        print "-l:  use local shell instead of ssh connection"
        sys.exit()
    i= i + 1     

try:  
   os.environ["OPENAIR1_DIR"]
except KeyError: 
   print "Please set the environment variable OPENAIR1_DIR in the .bashrc"
   sys.exit(1)

try:  
   os.environ["OPENAIR2_DIR"]
except KeyError: 
   print "Please set the environment variable OPENAIR2_DIR in the .bashrc"
   sys.exit(1)

try:  
   os.environ["OPENAIR_TARGETS"]
except KeyError: 
   print "Please set the environment variable OPENAIR_TARGETS in the .bashrc"
   sys.exit(1)

host = os.uname()[1]
# get the oai object
oai = openair('localdomain','localhost')
#start_time = time.time()  # datetime.datetime.now()
user = getpass.getuser()
if localshell == 0:
    try: 
        print '\n******* Note that the user <'+user+'> should be a sudoer *******\n'
        if cpu > -1 :
            print '******* Connecting to the localhost <'+host+'> to perform the test on CPU '+str(cpu)+' *******\n'
        else :
            print '******* Connecting to the localhost <'+host+'> to perform the test *******\n'
    
        if not pw :
            print "username: " + user 
            pw = getpass.getpass() 
        else :
            print "username: " + user 
            #print "password: " + pw 
        print "prompt:   " + prompt
        
        oai.connect(user,pw,prompt)
        #oai.get_shell()
    except :
        print 'Fail to connect to the local host'
        sys.exit(1)
else:
    pw = ''
    print "prompt:   " + prompt
    oai.connect_localshell(prompt)

test = 'test02'
ctime=datetime.datetime.utcnow().strftime("%Y-%m-%d.%Hh%M")
logdir = os.getcwd() + '/PERF_'+host;
logfile = logdir+'/'+user+'.'+test+'.'+ctime+'.txt'  
#oai.send_nowait('mkdir -p -m 755' + logdir + ';')
oai.create_dir(logdir,debug)  
#print '=================start the ' + test + ' at ' + ctime + '=================\n'
#print 'Results will be reported in log file : ' + logfile
log.writefile(logfile,'====================start'+test+' at ' + ctime + '=======================\n')
log.set_debug_level(debug)

oai.kill(user, pw)
if clean == 1 :
    oai.cleandir(logdir,debug)   

#oai.rm_driver(oai,user,pw)

# start te test cases 
#compile 

rv=case11.execute(oai, user, pw, host,logfile,logdir,debug)
if rv == 1 :
    case12.execute(oai, user, pw, host,logfile,logdir,debug,cpu)
    case13.execute(oai, user, pw, host,logfile,logdir,debug,cpu)
else :
    print 'Compilation error: skip case 12 and 13'

oai.kill(user, pw) 
#oai.rm_driver(oai,user,pw)

# perform the stats
log.statistics(logfile)


oai.disconnect()

ctime=datetime.datetime.utcnow().strftime("%Y-%m-%d_%Hh%M")
log.writefile(logfile,'====================end the '+ test + ' at ' + ctime +'====================')
print 'Test results can be found in : ' + logfile 
#print '\nThis test took %f minutes\n' % math.ceil((time.time() - start_time)/60) 

#print '\n=====================end the '+ test + ' at ' + ctime + '====================='
